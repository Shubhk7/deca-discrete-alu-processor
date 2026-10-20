#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <esp_task_wdt.h>
#include "pin_definitions.h"
#include "web_page.h"

// ============================================================================
// DECA Minor Project: Dual-Mode Hybrid Discrete ALU Processor Testbench
// Microcontroller: ESP32-S3 N16R8 ("The Brain" / Web Dashboard / Remote Injector)
// Author: Shubh Kashyap | Course: Digital Electronics & Computer Architecture
// ============================================================================
// System Responsibilities:
// 1. Dual-Mode Hybrid Architecture Management:
//    - MODE 0: 8-Bit Cascaded Compute
//      * Lower Nibble Y[3:0]: Custom 4-Bit ALU (74LS83N + 74HC86N/08/32/04 + 74HCT151 MUXes)
//      * Upper Nibble Y[7:4]: Commercial 74LS181N 4-Bit ALU
//      * Carry Cascade: IC1 (74LS83N) C4 cascades directly into 74LS181N /Cn
//    - MODE 1: Real-Time Silicon Verification
//      * Custom 4-Bit ALU and 74LS181N receive IDENTICAL 4-bit inputs A[3:0] and B[3:0]
//      * IC12 (74HCT85 4-Bit Magnitude Comparator) verifies Custom Y[3:0] vs 74LS181 F[3:0]
//      * 74HCT85 Pin 6 (A=B Output) asserts HIGH on hardware match
// 2. Heterogeneous 3-Board Infrastructure:
//    - ESP32-S3 N16R8 ("The Brain"): SoftAP Wi-Fi ('DECA-ALU'), WebSocket telemetry, UI
//    - Arduino UNO R4 WiFi ("The Verifier"): 5V bus sniffer, Golden Model, 12x8 Matrix HUD
//    - Arduino UNO R4 Minima ("The Power Station"): Clean 5V/1A USB-C logic power supply
// 3. Wi-Fi SoftAP & WebSocket Server:
//    - SoftAP SSID: 'DECA-ALU' | Pass: 'decaalu2024' (IP: 192.168.4.1)
//    - Port 80 HTTP Web Server, Port 81 WebSockets (100ms real-time broadcast)
// 4. Hybrid Cyber-Physical Vector Injection:
//    - Passive Mode: High-Z monitoring of physical DIP switches
//    - Remote Override: Actively drives 4.7kΩ resistors over 10kΩ pull-downs
// 5. Hardware Watchdog Protection:
//    - 5-second Task WDT ensures crash-proof operation
// ============================================================================

// --- Network Configuration ---
const char* AP_SSID = "DECA-ALU";
const char* AP_PASS = "decaalu2024";

// --- Server Instances ---
WebServer server(80);
WebSocketsServer webSocket(81);

// --- Watchdog Configuration ---
#define WDT_TIMEOUT_SECONDS 5

// --- Opcode Lookup Table (3-bit select S2, S1, S0) ---
const char* const OP_NAMES[8] = {
    "ADD",   // 000: Addition
    "SUB",   // 001: Subtraction (2's complement)
    "AND",   // 010: Bitwise AND
    "OR",    // 011: Bitwise OR
    "XOR",   // 100: Bitwise XOR
    "NOT",   // 101: Bitwise NOT
    "SHL",   // 110: Shift Left
    "SHR"    // 111: Shift Right
};

// --- System Telemetry State Variables ---
uint8_t currentA = 0;           // Operand A (8-bit)
uint8_t currentB = 0;           // Operand B (8-bit)
uint8_t currentOpcode = 0;      // Opcode Select (3-bit: 0..7)
uint8_t currentMode = 0;        // Datapath Mode: 0 = 8-Bit Cascaded, 1 = Silicon Verification
uint8_t currentY = 0;           // ALU Hardware Output Bus (8-bit)
uint8_t customAluY = 0;         // Custom 4-Bit ALU Output (Y[3:0])
uint8_t ls181AluY = 0;          // 74LS181N 4-Bit ALU Output (Y[7:4])
uint8_t expectedY = 0;          // Software Golden Model Expected Output
bool hw7485Pass = false;        // 74HCT85 Pin 6 (A=B Output) hardware match flag
bool goldenMatch = false;       // Software Golden Model match flag
bool siliconMatch = false;      // Custom ALU vs 74LS181 direct silicon match flag
bool overallPass = false;       // Comprehensive system verification pass flag
bool overrideActive = false;    // Hybrid Remote Override Status Flag

// Telemetry interval timer
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_INTERVAL_MS = 100;

// ============================================================================
// Software Golden Model (Reference Architecture Engine)
// ============================================================================

uint8_t computeGoldenModel(uint8_t a, uint8_t b, uint8_t opcode, uint8_t mode) {
    uint8_t result = 0;
    opcode &= 0x07;

    if (mode == 0) {
        // Mode 0: 8-Bit Cascaded ALU Mode (Custom Lower + 74LS181 Upper)
        switch (opcode) {
            case 0: result = (uint8_t)((a + b) & 0xFF); break;  // ADD
            case 1: result = (uint8_t)((a - b) & 0xFF); break;  // SUB (2's complement)
            case 2: result = (uint8_t)(a & b); break;           // AND
            case 3: result = (uint8_t)(a | b); break;           // OR
            case 4: result = (uint8_t)(a ^ b); break;           // XOR
            case 5: result = (uint8_t)(~a & 0xFF); break;       // NOT
            case 6: result = (uint8_t)((a << 1) & 0xFF); break; // SHL
            case 7: result = (uint8_t)(a >> 1); break;          // SHR
            default: result = 0; break;
        }
    } else {
        // Mode 1: Real-Time Silicon Verification Mode (Custom 4-Bit vs 74LS181)
        // Both units receive identical 4-bit inputs A[3:0] and B[3:0]
        uint8_t a4 = a & 0x0F;
        uint8_t b4 = b & 0x0F;
        uint8_t res4 = 0;

        switch (opcode) {
            case 0: res4 = (a4 + b4) & 0x0F; break;         // ADD
            case 1: res4 = (a4 - b4) & 0x0F; break;         // SUB (2's complement)
            case 2: res4 = (a4 & b4) & 0x0F; break;         // AND
            case 3: res4 = (a4 | b4) & 0x0F; break;         // OR
            case 4: res4 = (a4 ^ b4) & 0x0F; break;         // XOR
            case 5: res4 = (~a4) & 0x0F; break;             // NOT
            case 6: res4 = (a4 << 1) & 0x0F; break;         // SHL
            case 7: res4 = (a4 >> 1) & 0x0F; break;         // SHR
            default: res4 = 0; break;
        }
        // In Mode 1, both nibbles are expected to yield the identical 4-bit result
        result = (res4 << 4) | (res4 & 0x0F);
    }
    return result;
}

// ============================================================================
// Hardware Abstraction Functions
// ============================================================================

// Read 8-bit bus from arbitrary pin array
uint8_t readBus8(const uint8_t pins[]) {
    uint8_t val = 0;
    for (int i = 0; i < BUS_WIDTH_DATA; i++) {
        if (digitalRead(pins[i])) {
            val |= (1 << i);
        }
    }
    return val;
}

// Read 3-bit opcode from pin array
uint8_t readOpcode3(const uint8_t pins[]) {
    uint8_t val = 0;
    for (int i = 0; i < BUS_WIDTH_OPCODE; i++) {
        if (digitalRead(pins[i])) {
            val |= (1 << i);
        }
    }
    return val;
}

// Read all hardware states and evaluate verification equivalence
void updateHardwareStates() {
    if (!overrideActive) {
        // Physical DIP switches dictate the inputs
        currentA = readBus8(PINS_BUS_A);
        currentB = readBus8(PINS_BUS_B);
        currentOpcode = readOpcode3(PINS_BUS_OPCODE);
        currentMode = (digitalRead(PIN_MODE_SELECT) == HIGH) ? 1 : 0;
    }

    // Read the 8-bit Hardware ALU Output Bus Y (from 5V breadboard via dividers)
    currentY = readBus8(PINS_BUS_Y);
    customAluY = currentY & 0x0F;        // Lower Nibble: Custom 4-Bit ALU
    ls181AluY = (currentY >> 4) & 0x0F;  // Upper Nibble: Commercial 74LS181N ALU

    // Read 74HCT85 4-Bit Magnitude Comparator Pin 6 (A=B Output)
    hw7485Pass = (digitalRead(PIN_7485_PASS) == HIGH);

    // Compute expected result via Software Golden Model
    expectedY = computeGoldenModel(currentA, currentB, currentOpcode, currentMode);

    if (currentMode == 0) {
        // Mode 0: 8-Bit Cascaded Compute
        // Full 8-bit bus must match the 8-bit Golden Model
        goldenMatch = (currentY == expectedY);
        siliconMatch = true; // In Mode 0, nibbles are cascaded, not compared directly
        overallPass = goldenMatch;
    } else {
        // Mode 1: Real-Time Silicon Verification
        // 1. Direct Silicon Comparison: Custom ALU Y[3:0] must equal 74LS181 F[3:0]
        siliconMatch = (customAluY == ls181AluY);

        // 2. Software Golden Comparison: Both must equal 4-bit expected
        uint8_t exp4 = expectedY & 0x0F;
        goldenMatch = (customAluY == exp4 && ls181AluY == exp4);

        // 3. Hardware Comparator: IC12 (74HCT85) Pin 6 must be HIGH
        overallPass = siliconMatch && goldenMatch && hw7485Pass;
    }
}

// Apply or release hybrid override mode
void setOverride(bool enable, uint8_t a, uint8_t b, uint8_t op, uint8_t mode) {
    overrideActive = enable;

    if (enable) {
        currentOpcode = op & 0x07;
        currentMode = (mode != 0) ? 1 : 0;

        if (currentMode == 1) {
            // In Mode 1 (Silicon Verification), replicate lower 4 bits to upper 4 bits
            // so both Custom ALU and 74LS181 receive identical A[3:0] and B[3:0]
            uint8_t a4 = a & 0x0F;
            uint8_t b4 = b & 0x0F;
            currentA = (a4 << 4) | a4;
            currentB = (b4 << 4) | b4;
        } else {
            currentA = a;
            currentB = b;
        }

        // Drive Operand A pins via 4.7kΩ override resistors
        for (int i = 0; i < BUS_WIDTH_DATA; i++) {
            pinMode(PINS_BUS_A[i], OUTPUT);
            digitalWrite(PINS_BUS_A[i], (currentA >> i) & 1);
        }

        // Drive Operand B pins via 4.7kΩ override resistors
        for (int i = 0; i < BUS_WIDTH_DATA; i++) {
            pinMode(PINS_BUS_B[i], OUTPUT);
            digitalWrite(PINS_BUS_B[i], (currentB >> i) & 1);
        }

        // Drive Opcode select pins
        for (int i = 0; i < BUS_WIDTH_OPCODE; i++) {
            pinMode(PINS_BUS_OPCODE[i], OUTPUT);
            digitalWrite(PINS_BUS_OPCODE[i], (currentOpcode >> i) & 1);
        }

        // Drive Mode select pin
        pinMode(PIN_MODE_SELECT, OUTPUT);
        digitalWrite(PIN_MODE_SELECT, currentMode ? HIGH : LOW);

        Serial.printf("[OVERRIDE ON]: A=0x%02X | B=0x%02X | Op=%d (%s) | Mode=%d (%s)\n",
                      currentA, currentB, currentOpcode, OP_NAMES[currentOpcode],
                      currentMode, currentMode ? "Silicon Verify" : "8-Bit Cascaded");
    } else {
        // Revert all bidirectional pins to inputs to restore physical DIP switch control
        for (int i = 0; i < BUS_WIDTH_DATA; i++) {
            pinMode(PINS_BUS_A[i], INPUT_PULLDOWN);
            pinMode(PINS_BUS_B[i], INPUT_PULLDOWN);
        }

        for (int i = 0; i < BUS_WIDTH_OPCODE; i++) {
            pinMode(PINS_BUS_OPCODE[i], INPUT_PULLDOWN);
        }

        pinMode(PIN_MODE_SELECT, INPUT_PULLDOWN);

        Serial.println("[OVERRIDE OFF]: Restored physical breadboard DIP switch control.");
    }
}

// ============================================================================
// Telemetry Serialization
// ============================================================================

String createTelemetryJson() {
    String json = "{";
    json += "\"type\":\"telemetry\",";
    json += "\"a\":" + String(currentA) + ",";
    json += "\"b\":" + String(currentB) + ",";
    json += "\"opcode\":" + String(currentOpcode) + ",";
    json += "\"op_name\":\"" + String(OP_NAMES[currentOpcode]) + "\",";
    json += "\"mode\":" + String(currentMode) + ",";
    json += "\"mode_str\":\"" + String(currentMode ? "Mode 1: Silicon Verification" : "Mode 0: 8-Bit Cascaded") + "\",";
    json += "\"y\":" + String(currentY) + ",";
    json += "\"custom_y\":" + String(customAluY) + ",";
    json += "\"ls181_y\":" + String(ls181AluY) + ",";
    json += "\"expected\":" + String(expectedY) + ",";
    json += "\"hw_7485_pass\":" + String(hw7485Pass ? "true" : "false") + ",";
    json += "\"silicon_match\":" + String(siliconMatch ? "true" : "false") + ",";
    json += "\"golden_match\":" + String(goldenMatch ? "true" : "false") + ",";
    json += "\"pass\":" + String(overallPass ? "true" : "false") + ",";
    json += "\"override\":" + String(overrideActive ? "true" : "false") + ",";
    json += "\"timestamp\":" + String(millis());
    json += "}";
    return json;
}

// ============================================================================
// WebSocket Protocol & Event Handler
// ============================================================================

void handleWebSocketCommand(uint8_t num, const char* msg) {
    // Command: {"cmd":"set_input","a":255,"b":128,"opcode":5,"mode":0}
    if (strstr(msg, "\"set_input\"") != NULL) {
        uint8_t a = currentA;
        uint8_t b = currentB;
        uint8_t op = currentOpcode;
        uint8_t mode = currentMode;

        char* pA = strstr(msg, "\"a\":");
        if (pA) a = (uint8_t)atoi(pA + 4);

        char* pB = strstr(msg, "\"b\":");
        if (pB) b = (uint8_t)atoi(pB + 4);

        char* pOp = strstr(msg, "\"opcode\":");
        if (pOp) op = (uint8_t)atoi(pOp + 9);

        char* pMode = strstr(msg, "\"mode\":");
        if (pMode) mode = (uint8_t)atoi(pMode + 7);

        setOverride(true, a, b, op, mode);

        // Immediate telemetry echo with updated states
        updateHardwareStates();
        String response = createTelemetryJson();
        webSocket.broadcastTXT(response);
    }
    // Release override: {"cmd":"release_override"}
    else if (strstr(msg, "\"release_override\"") != NULL ||
             (strstr(msg, "\"set_override\"") != NULL && strstr(msg, "\"active\":false") != NULL)) {
        setOverride(false, 0, 0, 0, 0);

        // Immediate telemetry echo with physical values
        updateHardwareStates();
        String response = createTelemetryJson();
        webSocket.broadcastTXT(response);
    }
    // Ping/Heartbeat
    else if (strstr(msg, "\"ping\"") != NULL) {
        webSocket.sendTXT(num, "{\"type\":\"pong\"}");
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WebSocket]: Client [%u] Disconnected\n", num);
            break;

        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[WebSocket]: Client [%u] Connected from %d.%d.%d.%d\n",
                          num, ip[0], ip[1], ip[2], ip[3]);
            // Send current state immediately upon client connection
            updateHardwareStates();
            webSocket.sendTXT(num, createTelemetryJson());
            break;
        }

        case WStype_TEXT: {
            char buffer[256];
            size_t copyLen = (length < sizeof(buffer) - 1) ? length : sizeof(buffer) - 1;
            memcpy(buffer, payload, copyLen);
            buffer[copyLen] = '\0';
            handleWebSocketCommand(num, buffer);
            break;
        }

        case WStype_BIN:
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
        default:
            break;
    }
}

// ============================================================================
// HTTP Web Server Route Handlers
// ============================================================================

void handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleApiTelemetry() {
    updateHardwareStates();
    server.send(200, "application/json", createTelemetryJson());
}

void handleApiSetInput() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");
        handleWebSocketCommand(0, body.c_str());
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"Missing body\"}");
    }
}

// ============================================================================
// Watchdog Initialization
// ============================================================================

void initWatchdog() {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = WDT_TIMEOUT_SECONDS * 1000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true
    };
    esp_task_wdt_reconfigure(&twdt_config);
    esp_task_wdt_add(NULL);
#else
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
    esp_task_wdt_add(NULL);
#endif
    Serial.println("[WATCHDOG]: Configured for 5-second timeout.");
}

// ============================================================================
// Setup & Main Loop
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(250);

    Serial.println("\n========================================================");
    Serial.println("  DECA Dual-Mode Hybrid ALU Processor Testbench");
    Serial.println("  Author: Shubh Kashyap | Architecture: Custom 4-Bit + 74LS181");
    Serial.println("  Hardware: ESP32-S3 N16R8 | Golden Model: Arduino UNO R4 WiFi");
    Serial.println("========================================================");

    // 1. Configure Hardware ALU Output Bus Pins (Dedicated Inputs from 5V bus via dividers)
    for (int i = 0; i < BUS_WIDTH_DATA; i++) {
        pinMode(PINS_BUS_Y[i], INPUT_PULLDOWN);
    }

    // 2. Configure 74HCT85 Hardware Silicon Match Monitor Pin (Pin 6 A=B via divider)
    pinMode(PIN_7485_PASS, INPUT_PULLDOWN);

    // 3. Configure Hybrid Bidirectional Pins as Inputs by default (Passive physical DIP mode)
    for (int i = 0; i < BUS_WIDTH_DATA; i++) {
        pinMode(PINS_BUS_A[i], INPUT_PULLDOWN);
        pinMode(PINS_BUS_B[i], INPUT_PULLDOWN);
    }

    for (int i = 0; i < BUS_WIDTH_OPCODE; i++) {
        pinMode(PINS_BUS_OPCODE[i], INPUT_PULLDOWN);
    }

    pinMode(PIN_MODE_SELECT, INPUT_PULLDOWN);

    Serial.println("[GPIO]: All testbench monitoring pins initialized successfully.");

    // 4. Start Wi-Fi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress myIP = WiFi.softAPIP();

    Serial.printf("[Wi-Fi]: SoftAP started. SSID: '%s' | Pass: '%s'\n", AP_SSID, AP_PASS);
    Serial.printf("[Wi-Fi]: Web Dashboard URL: http://%s\n", myIP.toString().c_str());

    // 5. Start HTTP Web Server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/telemetry", HTTP_GET, handleApiTelemetry);
    server.on("/api/set_input", HTTP_POST, handleApiSetInput);
    server.begin();
    Serial.println("[HTTP]: Web Server listening on Port 80.");

    // 6. Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("[WebSocket]: Live Telemetry Server listening on Port 81.");

    // 7. Start Hardware Watchdog
    initWatchdog();

    Serial.println("[SYSTEM]: Dual-Mode Hybrid ALU Testbench Ready.\n");
}

void loop() {
    // Reset hardware watchdog on every loop iteration
    esp_task_wdt_reset();

    // Service active network protocols
    server.handleClient();
    webSocket.loop();

    // Periodic live telemetry broadcast (every 100ms)
    unsigned long now = millis();
    if (now - lastTelemetryTime >= TELEMETRY_INTERVAL_MS) {
        lastTelemetryTime = now;
        updateHardwareStates();

        // Broadcast to all connected WebSocket dashboard clients
        String telemetry = createTelemetryJson();
        webSocket.broadcastTXT(telemetry);
    }
}
