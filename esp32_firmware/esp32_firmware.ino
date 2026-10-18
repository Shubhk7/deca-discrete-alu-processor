#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <esp_task_wdt.h>
#include "pin_definitions.h"
#include "web_page.h"

// ============================================================================
// DECA Minor Project: Discrete 74HCT Gate-Level ALU Testbench & Dashboard
// Microcontroller: ESP32-S3 N16R8
// ============================================================================
// Functions:
// 1. Hosts Wi-Fi SoftAP ('DECA-ALU' / 'decaalu2024')
// 2. HTTP Web Server on port 80 (serves interactive dashboard)
// 3. WebSocket Server on port 81 (sub-100ms real-time telemetry & remote injection)
// 4. Hybrid Input Control:
//    - Passively monitors physical DIP switches (GPIOs in INPUT mode)
//    - Actively overrides inputs via WebSocket injection (GPIOs in OUTPUT mode)
// 5. Hardware-in-the-Loop Telemetry Bus Monitor:
//    - Reads 8-bit output bus Y[7:0] directly from 74HCT151 MUX outputs via dividers
// 6. Hardware Watchdog Timer (5-second timeout)
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
    "ADD",   // 000: Gate-level Ripple-Carry Adder (A + B)
    "SUB",   // 001: Gate-level 2's Complement Subtraction (A - B)
    "AND",   // 010: Bitwise AND via 74HCT08 (A & B)
    "OR",    // 011: Bitwise OR via 74HCT32 (A | B)
    "XOR",   // 100: Bitwise XOR via 74HCT86 (A ^ B)
    "NOT",   // 101: Bitwise NOT via 74HCT04 (~A)
    "SHL",   // 110: Shift Left (A << 1)
    "SHR"    // 111: Shift Right (A >> 1)
};

// --- System State Variables ---
uint8_t currentA = 0;          // Operand A (8-bit)
uint8_t currentB = 0;          // Operand B (8-bit)
uint8_t currentOpcode = 0;     // Opcode Select (3-bit: 0..7)
uint8_t currentMode = 1;       // Mode: 1 = 8-Bit Cascaded, 0 = 4-Bit Dual
uint8_t currentY = 0;          // ALU Hardware Output Bus (8-bit)
bool overrideActive = false;   // Hybrid mode flag

// Telemetry timing
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_INTERVAL_MS = 100;

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

// Read all hardware states according to current hybrid mode
void updateHardwareStates() {
    if (!overrideActive) {
        // Physical DIP switches dictate the inputs
        currentA = readBus8(PINS_BUS_A);
        currentB = readBus8(PINS_BUS_B);
        currentOpcode = readOpcode3(PINS_BUS_OPCODE);
        currentMode = digitalRead(PIN_MODE_SELECT) ? 1 : 0;
    }
    // The ALU Output Bus is ALWAYS read from hardware pins (from 74HCT breadboard)
    currentY = readBus8(PINS_BUS_Y);
}

// Apply or release hybrid override mode
void setOverride(bool enable, uint8_t a, uint8_t b, uint8_t op, uint8_t mode) {
    overrideActive = enable;

    if (enable) {
        currentA = a;
        currentB = b;
        currentOpcode = op & 0x07;
        currentMode = (mode != 0) ? 1 : 0;

        // Drive Operand A pins
        for (int i = 0; i < BUS_WIDTH_DATA; i++) {
            pinMode(PINS_BUS_A[i], OUTPUT);
            digitalWrite(PINS_BUS_A[i], (currentA >> i) & 1);
        }

        // Drive Operand B pins
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

        Serial.printf("[OVERRIDE ON]: A=0x%02X (%d) | B=0x%02X (%d) | Op=%d (%s) | Mode=%d\n",
                      currentA, currentA, currentB, currentB, currentOpcode, OP_NAMES[currentOpcode], currentMode);
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

        Serial.println("[OVERRIDE OFF]: Restored physical DIP switch control.");
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
    json += "\"mode_str\":\"" + String(currentMode ? "8-Bit Cascaded" : "4-Bit Dual") + "\",";
    json += "\"y\":" + String(currentY) + ",";
    json += "\"override\":" + String(overrideActive ? "true" : "false") + ",";
    json += "\"timestamp\":" + String(millis());
    json += "}";
    return json;
}

// ============================================================================
// WebSocket Protocol & Event Handler
// ============================================================================

void handleWebSocketCommand(uint8_t num, const char* msg) {
    // Expected format: {"cmd":"set_input","a":255,"b":128,"opcode":5,"mode":1}
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

        // Immediate telemetry echo
        String response = createTelemetryJson();
        webSocket.broadcastTXT(response);
    }
    // Release override: {"cmd":"release_override"} or {"cmd":"set_override","active":false}
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
            // Send current state immediately on connection
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
// Arduino Setup & Main Loop
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(250);

    Serial.println("\n========================================================");
    Serial.println("  DECA Discrete 74HCT Gate-Level ALU Testbench Controller");
    Serial.println("  Hardware: ESP32-S3 N16R8 | Golden Model: Arduino UNO R4");
    Serial.println("========================================================");

    // 1. Configure Hardware ALU Output Bus Pins (Dedicated Inputs from 74HCT151 MUX)
    for (int i = 0; i < BUS_WIDTH_DATA; i++) {
        pinMode(PINS_BUS_Y[i], INPUT_PULLDOWN);
    }

    // 2. Configure Hybrid Bidirectional Pins as Inputs by default (Physical DIP mode)
    for (int i = 0; i < BUS_WIDTH_DATA; i++) {
        pinMode(PINS_BUS_A[i], INPUT_PULLDOWN);
        pinMode(PINS_BUS_B[i], INPUT_PULLDOWN);
    }

    for (int i = 0; i < BUS_WIDTH_OPCODE; i++) {
        pinMode(PINS_BUS_OPCODE[i], INPUT_PULLDOWN);
    }

    pinMode(PIN_MODE_SELECT, INPUT_PULLDOWN);

    Serial.println("[GPIO]: All 28 pins initialized successfully.");

    // 3. Start Wi-Fi Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress myIP = WiFi.softAPIP();

    Serial.printf("[Wi-Fi]: SoftAP started. SSID: '%s' | Pass: '%s'\n", AP_SSID, AP_PASS);
    Serial.printf("[Wi-Fi]: Web Dashboard URL: http://%s\n", myIP.toString().c_str());

    // 4. Start HTTP Web Server
    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/telemetry", HTTP_GET, handleApiTelemetry);
    server.on("/api/set_input", HTTP_POST, handleApiSetInput);
    server.begin();
    Serial.println("[HTTP]: Web Server listening on Port 80.");

    // 5. Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("[WebSocket]: Live Telemetry Server listening on Port 81.");

    // 6. Start Hardware Watchdog
    initWatchdog();

    Serial.println("[SYSTEM]: Testbench Ready.\n");
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
