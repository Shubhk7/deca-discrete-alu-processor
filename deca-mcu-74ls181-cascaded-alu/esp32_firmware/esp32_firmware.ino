/*
 * =====================================================================================
 * DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor
 * ESP32-S3 Firmware — SoftAP Web Dashboard, WebSockets & 4.7k Override Injector
 * =====================================================================================
 * Target: ESP32-S3 N16R8 (Dual Xtensa LX7 @ 240 MHz)
 * Logic Level: 3.3V CMOS (with 1.0k / 2.2k dividers on 5V inputs, 4.7k on outputs)
 *
 * Responsibilities:
 *  1. Hosts standalone Wi-Fi Access Point: SSID "DECA-ALU-HYBRID", Password "decaalu2024".
 *  2. Serves embedded HTML5/CSS3/JS dark dashboard on HTTP Port 80.
 *  3. Operates bi-directional WebSockets server on Port 81 for 100ms real-time telemetry.
 *  4. Hybrid Input System:
 *     - When Override is OFF: Pins set to INPUT, sensing manual DIP switches.
 *     - When Override is ON: Pins set to OUTPUT, driving logic through 4.7kΩ resistors.
 *  5. Reads 8-bit hybrid output bus Y[7:0] and carry C8 via voltage dividers.
 * =====================================================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "pin_definitions.h"
#include "web_page.h"

// =====================================================================================
// OPTIONAL 0.96" I2C OLED DISPLAY (SSD1306 128x64)
// Set to 1 if you have an OLED plugged into SDA=GPIO 8, SCL=GPIO 9. Set to 0 to disable.
// =====================================================================================
#define USE_OLED 1

#if USE_OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oled_found = false;
#endif

// Wi-Fi Access Point Credentials
const char* AP_SSID = "DECA-ALU-HYBRID";
const char* AP_PASS = "decaalu2024";

// Servers
WebServer server(80);
WebSocketsServer webSocket(81);

// System State
bool remote_override_active = false;
uint8_t override_a = 0;
uint8_t override_b = 0;
uint8_t override_op = 0;
uint8_t override_mode = 0;

unsigned long last_telemetry_broadcast = 0;

// =====================================================================================
// GPIO PIN CONFIGURATION
// =====================================================================================
void configureInjectionPins(bool as_outputs) {
    uint8_t mode = as_outputs ? OUTPUT : INPUT_PULLDOWN;

    // Operand A pins
    pinMode(PIN_INJ_A0, mode); pinMode(PIN_INJ_A1, mode);
    pinMode(PIN_INJ_A2, mode); pinMode(PIN_INJ_A3, mode);
    pinMode(PIN_INJ_A4, mode); pinMode(PIN_INJ_A5, mode);
    pinMode(PIN_INJ_A6, mode); pinMode(PIN_INJ_A7, mode);

    // Operand B pins
    pinMode(PIN_INJ_B0, mode); pinMode(PIN_INJ_B1, mode);
    pinMode(PIN_INJ_B2, mode); pinMode(PIN_INJ_B3, mode);
    pinMode(PIN_INJ_B4, mode); pinMode(PIN_INJ_B5, mode);
    pinMode(PIN_INJ_B6, mode); pinMode(PIN_INJ_B7, mode);

    // Opcode & Mode pins
    pinMode(PIN_INJ_S0, mode);
    pinMode(PIN_INJ_S1, mode);
    pinMode(PIN_INJ_S2, mode);
    pinMode(PIN_INJ_MODE, mode);
}

void configureTelemetryPins() {
    // 5V bus lines connected through 1.0k / 2.2k voltage dividers (Safe 3.43V)
    pinMode(PIN_SENSE_Y0, INPUT);
    pinMode(PIN_SENSE_Y1, INPUT);
    pinMode(PIN_SENSE_Y2, INPUT);
    pinMode(PIN_SENSE_Y3, INPUT);

    pinMode(PIN_SENSE_Y4, INPUT);
    pinMode(PIN_SENSE_Y5, INPUT);
    pinMode(PIN_SENSE_Y6, INPUT);
    pinMode(PIN_SENSE_Y7, INPUT);

    pinMode(PIN_SENSE_C8, INPUT);
}

// =====================================================================================
// DRIVE REMOTE OVERRIDE VALUES (via 4.7kΩ Override Resistors)
// =====================================================================================
void driveOverrideOutputs() {
    if (!remote_override_active) return;

    // Drive Operand A
    digitalWrite(PIN_INJ_A0, (override_a >> 0) & 1);
    digitalWrite(PIN_INJ_A1, (override_a >> 1) & 1);
    digitalWrite(PIN_INJ_A2, (override_a >> 2) & 1);
    digitalWrite(PIN_INJ_A3, (override_a >> 3) & 1);
    digitalWrite(PIN_INJ_A4, (override_a >> 4) & 1);
    digitalWrite(PIN_INJ_A5, (override_a >> 5) & 1);
    digitalWrite(PIN_INJ_A6, (override_a >> 6) & 1);
    digitalWrite(PIN_INJ_A7, (override_a >> 7) & 1);

    // Drive Operand B
    digitalWrite(PIN_INJ_B0, (override_b >> 0) & 1);
    digitalWrite(PIN_INJ_B1, (override_b >> 1) & 1);
    digitalWrite(PIN_INJ_B2, (override_b >> 2) & 1);
    digitalWrite(PIN_INJ_B3, (override_b >> 3) & 1);
    digitalWrite(PIN_INJ_B4, (override_b >> 4) & 1);
    digitalWrite(PIN_INJ_B5, (override_b >> 5) & 1);
    digitalWrite(PIN_INJ_B6, (override_b >> 6) & 1);
    digitalWrite(PIN_INJ_B7, (override_b >> 7) & 1);

    // Drive Opcode & Mode
    digitalWrite(PIN_INJ_S0, (override_op >> 0) & 1);
    digitalWrite(PIN_INJ_S1, (override_op >> 1) & 1);
    digitalWrite(PIN_INJ_S2, (override_op >> 2) & 1);
    digitalWrite(PIN_INJ_MODE, override_mode & 1);
}

// =====================================================================================
// READ CURRENT BUS VALUES
// =====================================================================================
void readCurrentBusValues(uint8_t &a_out, uint8_t &b_out, uint8_t &op_out, uint8_t &m_out, uint8_t &y_out, uint8_t &c8_out) {
    if (remote_override_active) {
        a_out = override_a;
        b_out = override_b;
        op_out = override_op;
        m_out = override_mode;
    } else {
        // Read manual DIP switch states
        a_out = (digitalRead(PIN_INJ_A7) << 7) | (digitalRead(PIN_INJ_A6) << 6) |
                (digitalRead(PIN_INJ_A5) << 5) | (digitalRead(PIN_INJ_A4) << 4) |
                (digitalRead(PIN_INJ_A3) << 3) | (digitalRead(PIN_INJ_A2) << 2) |
                (digitalRead(PIN_INJ_A1) << 1) | digitalRead(PIN_INJ_A0);

        b_out = (digitalRead(PIN_INJ_B7) << 7) | (digitalRead(PIN_INJ_B6) << 6) |
                (digitalRead(PIN_INJ_B5) << 5) | (digitalRead(PIN_INJ_B4) << 4) |
                (digitalRead(PIN_INJ_B3) << 3) | (digitalRead(PIN_INJ_B2) << 2) |
                (digitalRead(PIN_INJ_B1) << 1) | digitalRead(PIN_INJ_B0);

        op_out = (digitalRead(PIN_INJ_S2) << 2) | (digitalRead(PIN_INJ_S1) << 1) | digitalRead(PIN_INJ_S0);
        m_out  = digitalRead(PIN_INJ_MODE);
    }

    // Read 8-bit output bus Y[7:0] via voltage dividers
    uint8_t y_lo = (digitalRead(PIN_SENSE_Y3) << 3) | (digitalRead(PIN_SENSE_Y2) << 2) |
                   (digitalRead(PIN_SENSE_Y1) << 1) | digitalRead(PIN_SENSE_Y0);

    uint8_t y_hi = (digitalRead(PIN_SENSE_Y7) << 3) | (digitalRead(PIN_SENSE_Y6) << 2) |
                   (digitalRead(PIN_SENSE_Y5) << 1) | digitalRead(PIN_SENSE_Y4);

    y_out = (y_hi << 4) | (y_lo & 0x0F);
    c8_out = digitalRead(PIN_SENSE_C8);
}

// =====================================================================================
// WEBSOCKET MESSAGE HANDLER
// =====================================================================================
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_TEXT) {
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, payload);
        if (err) return;

        const char* cmd = doc["cmd"];
        if (strcmp(cmd, "set_input") == 0) {
            if (!remote_override_active) {
                remote_override_active = true;
                configureInjectionPins(true); // Switch GPIOs to OUTPUT
            }
            override_a = doc["a"] | 0;
            override_b = doc["b"] | 0;
            override_op = doc["op"] | 0;
            override_mode = doc["m"] | 0;
            driveOverrideOutputs();
        } else if (strcmp(cmd, "release_override") == 0) {
            remote_override_active = false;
            configureInjectionPins(false); // Switch GPIOs back to INPUT_PULLDOWN
        }
    }
}

// =====================================================================================
// SETUP
// =====================================================================================
void setup() {
    Serial.begin(115200);

    // 1. Initialize pins
    configureInjectionPins(false); // Default to INPUT mode (DIP switch monitoring)
    configureTelemetryPins();

    // 2. Start Wi-Fi SoftAP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress IP = WiFi.softAPIP();
    Serial.println(F("\n======================================================="));
    Serial.println(F(" DECA Dual 4-Bit Cascaded 8-Bit ALU — ESP32 Dashboard"));
    Serial.print(F(" SoftAP Started: ")); Serial.println(AP_SSID);
    Serial.print(F(" Dashboard URL:  http://")); Serial.println(IP);
    Serial.println(F("======================================================="));

#if USE_OLED
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        oled_found = true;
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 4);
        display.println(F("DECA HYBRID ALU"));
        display.println(F("74LS181 + R4 WiFi"));
        display.println(F("AP: DECA-ALU-HYBRID"));
        display.println(F("IP: 192.168.4.1"));
        display.display();
        delay(800);
    }
#endif

    // 3. Start Web Server
    server.on("/", []() {
        server.send_P(200, "text/html", index_html);
    });
    server.begin();

    // 4. Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

// =====================================================================================
// BROADCAST TELEMETRY JSON (Every 100 ms) + RENDER OLED
// =====================================================================================
void broadcastTelemetry() {
    uint8_t a, b, op, m, y, c8;
    readCurrentBusValues(a, b, op, m, y, c8);

    // Compute intermediate cascade C4 approximation for dashboard
    uint8_t a_lo = a & 0x0F;
    uint8_t b_lo = b & 0x0F;
    uint8_t c4_calc = 0;
    if (op == 0 && m == 0) {
        c4_calc = ((a_lo + b_lo) > 15) ? 1 : 0;
    } else if (op == 1 && m == 0) {
        c4_calc = (a_lo >= b_lo) ? 1 : 0;
    }

#if USE_OLED
    if (oled_found) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        const char* op_names[] = {"ADD", "SUB", "AND", "OR ", "XOR", "NOT", "SHL", "SHR"};
        display.print(F("OP:")); display.print(op_names[op & 0x07]);
        display.print(m ? F(" [LOG]") : F(" [ARI]"));
        display.print(F(" C4:")); display.println(c4_calc);

        display.print(F("A:0x")); if(a<16)display.print('0'); display.print(a, HEX);
        display.print(F(" (")); display.print(a); display.println(F(")"));

        display.print(F("B:0x")); if(b<16)display.print('0'); display.print(b, HEX);
        display.print(F(" (")); display.print(b); display.println(F(")"));

        display.print(F("Y:0x")); if(y<16)display.print('0'); display.print(y, HEX);
        display.print(F(" (")); display.print(y); display.println(F(")"));

        display.print(F("C8:")); display.print(c8);
        display.print(F(" IP:192.168.4.1"));
        display.display();
    }
#endif

    StaticJsonDocument<192> doc;
    doc["a"] = a;
    doc["b"] = b;
    doc["op"] = op;
    doc["m"] = m;
    doc["y"] = y;
    doc["c4"] = c4_calc;
    doc["c8"] = c8;
    doc["ts"] = millis();

    String json_str;
    serializeJson(doc, json_str);
    webSocket.broadcastTXT(json_str);
}

// =====================================================================================
// MAIN LOOP
// =====================================================================================
void loop() {
    server.handleClient();
    webSocket.loop();

    unsigned long current_time = millis();
    if (current_time - last_telemetry_broadcast >= 100) {
        last_telemetry_broadcast = current_time;
        broadcastTelemetry();
    }
}
