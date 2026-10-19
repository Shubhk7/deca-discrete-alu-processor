#include "Arduino_LED_Matrix.h"

// ============================================================================
// DECA Minor Project: Hardware-in-the-Loop Software Golden Model Verifier
// Target Microcontroller: Arduino UNO R4 WiFi (Renesas RA4M1 32-bit ARM Cortex-M4)
// Native Operating Voltage: 5.0V Logic (Direct Interface without Level Shifters)
// Author: Shubh Kashyap | Course: Digital Electronics & Computer Architecture
// ============================================================================
// Dual-Mode Hybrid Architecture Specification:
// 1. MODE 0: 8-Bit Cascaded Compute
//    - Lower Nibble Y[3:0]: Custom 4-Bit ALU (74xx283 + XOR/AND/OR/NOT + 74xx151 MUXes)
//    - Upper Nibble Y[7:4]: Commercial 74LS181N 4-Bit High-Functionality ALU
//    - Carry Cascade: IC1 (74xx283) C4 cascades directly into 74LS181N /Cn
//    - Verifier reads full 8-bit Y[7:0] bus and checks against 8-bit Software Golden Model.
// 2. MODE 1: Real-Time Silicon Verification
//    - Both Custom 4-Bit ALU and 74LS181N receive IDENTICAL 4-bit inputs A[3:0], B[3:0]
//    - IC12 (74xx85 4-Bit Magnitude Comparator) verifies Custom Y[3:0] vs 74LS181 F[3:0] in hardware
//    - Verifier reads:
//      * Custom ALU Y[3:0] (Pins 2..5)
//      * Commercial 74LS181 F[3:0] (Pins 6..9)
//      * IC12 (74xx85) Pin 6 (A=B Output on Pin A0)
//    - Verifies 3-way equivalence: Custom ALU == 74LS181 == Software Golden Model == 74xx85 Match
// 3. 12x8 LED Matrix HUD Scoreboard:
//    - Displays Checkmark icon (✓) on PASS, X-Mark icon (✗) on FAIL
//    - Scrolls Mode and Opcode name with PASS/FAIL diagnostic message
// 4. Physical Status Annunciators:
//    - Green PASS LED (Pin A4) and Red FAIL LED (Pin A5)
// ============================================================================

ArduinoLEDMatrix matrix;

// ----------------------------------------------------------------------------
// Pin Definitions (Arduino UNO R4 WiFi 5.0V Native Logic Pins)
// ----------------------------------------------------------------------------

// 8-Bit Hardware ALU Output Bus (Y0..Y7) - Digital Inputs from 5.0V Breadboard
// Y[3:0] (Pins 2..5) = Custom 4-Bit ALU (IC1 74xx283 + 74xx151 MUXes)
// Y[7:4] (Pins 6..9) = Commercial 74LS181N ALU (F0..F3)
const uint8_t PIN_Y[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

// Opcode Bus (S0..S2) & Datapath Mode Control Pin
const uint8_t PIN_OPCODE[3] = { 10, 11, 12 }; // S0, S1, S2
const uint8_t PIN_MODE = 13;                  // Datapath Mode: 0 = 8-Bit Cascaded, 1 = Silicon Verification

// IC12 (74xx85 4-Bit Magnitude Comparator) Hardware Match Pin
// Connects directly to 74xx85 Pin 6 (A=B Output)
const uint8_t PIN_7485_MATCH = A0;

// Dedicated Physical Status LED Indicators
const uint8_t PIN_LED_PASS = A4; // External Green PASS LED (via 220Ω resistor)
const uint8_t PIN_LED_FAIL = A5; // External Red FAIL LED (via 220Ω resistor)

// Modular Monitored Input Pins (Pins A1..A3 for input vector sampling)
const uint8_t PIN_MONITOR_A[3] = { A1, A2, A3 };

// ----------------------------------------------------------------------------
// 12x8 LED Matrix Bitmaps (3 words of 32 bits = 96 LEDs)
// ----------------------------------------------------------------------------

// PASS Checkmark Bitmap (Centered, bold checkmark on 12x8 matrix)
const uint32_t CHECKMARK[3] = {
    0x00000400,
    0xC0184306,
    0x603C0180
};

// FAIL 'X' Mark Bitmap (Centered, high-contrast X on 12x8 matrix)
const uint32_t X_MARK[3] = {
    0x60630C19,
    0x80F00F01,
    0x9830C606
};

// Boot Logo / Standby Diamond Frame
const uint32_t LOGO_FRAME[3] = {
    0x0200700F,
    0x81FC3FE0,
    0xF8070020
};

// ----------------------------------------------------------------------------
// Operation Names Lookup Table
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// State Variables & Sampling
// ----------------------------------------------------------------------------
uint8_t lastSampledOp = 0xFF;
uint8_t lastSampledMode = 0xFF;
uint8_t stableA = 0x55;
uint8_t stableB = 0x23;
uint8_t stableOp = 0;
uint8_t stableMode = 0; // 0 = 8-Bit Cascaded, 1 = Silicon Verification
uint8_t stableHW = 0;
uint8_t stableCustomY = 0;
uint8_t stableLS181Y = 0;
bool stable7485Pass = false;

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL_MS = 150; // 150ms verification cycle

// ============================================================================
// Hardware Bus Reading Helpers
// ============================================================================

uint8_t readBus(const uint8_t pins[], uint8_t count) {
    uint8_t val = 0;
    for (uint8_t i = 0; i < count; i++) {
        if (digitalRead(pins[i]) == HIGH) {
            val |= (1 << i);
        }
    }
    return val;
}

// ============================================================================
// Software Golden Model (Reference Architecture Engine)
// ============================================================================

uint8_t computeGoldenModel(uint8_t a, uint8_t b, uint8_t opcode, uint8_t mode) {
    uint8_t expected = 0;
    opcode &= 0x07;

    if (mode == 0) {
        // Mode 0: Full 8-Bit Cascaded Datapath (Custom ALU lower + 74LS181 upper)
        switch (opcode) {
            case 0: expected = (uint8_t)((a + b) & 0xFF); break;  // ADD
            case 1: expected = (uint8_t)((a - b) & 0xFF); break;  // SUB (2's complement)
            case 2: expected = (uint8_t)(a & b); break;           // AND
            case 3: expected = (uint8_t)(a | b); break;           // OR
            case 4: expected = (uint8_t)(a ^ b); break;           // XOR
            case 5: expected = (uint8_t)(~a & 0xFF); break;       // NOT
            case 6: expected = (uint8_t)((a << 1) & 0xFF); break; // SHL
            case 7: expected = (uint8_t)(a >> 1); break;          // SHR
            default: expected = 0; break;
        }
    } else {
        // Mode 1: Real-Time Silicon Verification Mode (Custom 4-Bit vs 74LS181)
        // Both units receive identical 4-bit operands A[3:0] and B[3:0]
        uint8_t a4 = a & 0x0F;
        uint8_t b4 = b & 0x0F;
        uint8_t exp4 = 0;

        switch (opcode) {
            case 0: exp4 = (a4 + b4) & 0x0F; break;        // ADD
            case 1: exp4 = (a4 - b4) & 0x0F; break;        // SUB (2's complement)
            case 2: exp4 = (a4 & b4) & 0x0F; break;        // AND
            case 3: exp4 = (a4 | b4) & 0x0F; break;        // OR
            case 4: exp4 = (a4 ^ b4) & 0x0F; break;        // XOR
            case 5: exp4 = (~a4) & 0x0F; break;            // NOT
            case 6: exp4 = (a4 << 1) & 0x0F; break;        // SHL
            case 7: exp4 = (a4 >> 1) & 0x0F; break;        // SHR
            default: exp4 = 0; break;
        }
        // In Mode 1, both nibbles should match exp4
        expected = (exp4 << 4) | exp4;
    }
    return expected;
}

// ============================================================================
// Matrix Text & Pattern Rendering Engine
// ============================================================================

// Lightweight 3x5 ASCII Font Table for numbers, uppercase letters, symbols
static const uint8_t FONT_3X5[][3] = {
    { 0x00, 0x00, 0x00 }, // Space (32)
    { 0x00, 0x17, 0x00 }, // !
    { 0x03, 0x00, 0x03 }, // "
    { 0x1F, 0x0A, 0x1F }, // #
    { 0x12, 0x1F, 0x09 }, // $
    { 0x19, 0x04, 0x13 }, // %
    { 0x0A, 0x15, 0x12 }, // &
    { 0x00, 0x03, 0x00 }, // '
    { 0x00, 0x0E, 0x11 }, // (
    { 0x11, 0x0E, 0x00 }, // )
    { 0x05, 0x02, 0x05 }, // *
    { 0x04, 0x0E, 0x04 }, // +
    { 0x00, 0x10, 0x08 }, // ,
    { 0x04, 0x04, 0x04 }, // -
    { 0x00, 0x10, 0x00 }, // .
    { 0x10, 0x08, 0x04 }, // /
    // 0..9 (ASCII 48..57)
    { 0x1F, 0x11, 0x1F }, // 0
    { 0x00, 0x1F, 0x00 }, // 1
    { 0x1D, 0x15, 0x17 }, // 2
    { 0x15, 0x15, 0x1F }, // 3
    { 0x07, 0x04, 0x1F }, // 4
    { 0x17, 0x15, 0x1D }, // 5
    { 0x1F, 0x15, 0x1D }, // 6
    { 0x01, 0x01, 0x1F }, // 7
    { 0x1F, 0x15, 0x1F }, // 8
    { 0x17, 0x15, 0x1F }, // 9
    { 0x00, 0x0A, 0x00 }, // :
    { 0x00, 0x14, 0x08 }, // ;
    { 0x04, 0x0A, 0x11 }, // <
    { 0x0A, 0x0A, 0x0A }, // =
    { 0x11, 0x0A, 0x04 }, // >
    { 0x01, 0x15, 0x03 }, // ?
    { 0x1E, 0x15, 0x1D }, // @
    // A..Z (ASCII 65..90)
    { 0x1E, 0x05, 0x1E }, // A
    { 0x1F, 0x15, 0x0A }, // B
    { 0x0E, 0x11, 0x11 }, // C
    { 0x1F, 0x11, 0x0E }, // D
    { 0x1F, 0x15, 0x15 }, // E
    { 0x1F, 0x05, 0x01 }, // F
    { 0x0E, 0x11, 0x1D }, // G
    { 0x1F, 0x04, 0x1F }, // H
    { 0x11, 0x1F, 0x11 }, // I
    { 0x08, 0x10, 0x0F }, // J
    { 0x1F, 0x04, 0x1B }, // K
    { 0x1F, 0x10, 0x10 }, // L
    { 0x1F, 0x02, 0x1F }, // M
    { 0x1F, 0x06, 0x1F }, // N
    { 0x0E, 0x11, 0x0E }, // O
    { 0x1F, 0x05, 0x02 }, // P
    { 0x0E, 0x11, 0x1E }, // Q
    { 0x1F, 0x05, 0x1A }, // R
    { 0x12, 0x15, 0x09 }, // S
    { 0x01, 0x1F, 0x01 }, // T
    { 0x0F, 0x10, 0x0F }, // U
    { 0x07, 0x18, 0x07 }, // V
    { 0x1F, 0x08, 0x1F }, // W
    { 0x1B, 0x04, 0x1B }, // X
    { 0x03, 0x1C, 0x03 }, // Y
    { 0x19, 0x15, 0x13 }  // Z
};

// Render an 8x12 pixel grid to 3 uint32_t words and load into the matrix
void renderGrid(const uint8_t grid[8][12]) {
    uint32_t frame[3] = { 0, 0, 0 };
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 12; c++) {
            if (grid[r][c]) {
                int ledIndex = r * 12 + c; // 0..95
                int wordIdx = ledIndex / 32;
                int bitIdx = 31 - (ledIndex % 32);
                frame[wordIdx] |= (1UL << bitIdx);
            }
        }
    }
    matrix.loadFrame(frame);
}

// Scroll a string across the 12x8 LED Matrix
void scrollMessage(const char* text, int speedMs) {
    int len = strlen(text);
    if (len == 0) return;

    int totalCols = len * 4 + 12;
    uint8_t cols[totalCols];
    memset(cols, 0, sizeof(cols));

    int colPtr = 12; // Start after screen width
    for (int i = 0; i < len; i++) {
        char ch = text[i];
        if (ch >= 'a' && ch <= 'z') ch -= 32; // Uppercase

        if (ch >= 32 && ch <= 90) {
            int fontIdx = ch - 32;
            cols[colPtr++] = FONT_3X5[fontIdx][0];
            cols[colPtr++] = FONT_3X5[fontIdx][1];
            cols[colPtr++] = FONT_3X5[fontIdx][2];
            cols[colPtr++] = 0; // Spacing column
        } else {
            colPtr += 4;
        }
    }

    // Scroll across 12-column display
    uint8_t displayGrid[8][12];
    for (int offset = 0; offset <= totalCols - 12; offset++) {
        memset(displayGrid, 0, sizeof(displayGrid));
        for (int c = 0; c < 12; c++) {
            uint8_t colByte = cols[offset + c];
            for (int r = 0; r < 5; r++) {
                if ((colByte >> r) & 1) {
                    displayGrid[r + 1][c] = 1; // Center vertically on 8-pixel height
                }
            }
        }
        renderGrid(displayGrid);
        delay(speedMs);
    }
}

// ============================================================================
// Setup & Initialization
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(300);

    Serial.println("\n========================================================");
    Serial.println("  Arduino UNO R4 WiFi - DECA ALU Software Golden Verifier");
    Serial.println("  Author: Shubh Kashyap | Architecture: Dual-Mode Hybrid");
    Serial.println("  Custom 4-Bit ALU + 74LS181N + 74xx85 Hardware Comparator");
    Serial.println("========================================================");

    // Initialize 12x8 LED Matrix
    matrix.begin();
    matrix.loadFrame(LOGO_FRAME);

    // Configure ALU Output Bus Pins (5V Inputs with Pull-Down)
    for (int i = 0; i < 8; i++) {
        pinMode(PIN_Y[i], INPUT_PULLDOWN);
    }

    // Configure Opcode & Mode Pins
    for (int i = 0; i < 3; i++) {
        pinMode(PIN_OPCODE[i], INPUT_PULLDOWN);
    }
    pinMode(PIN_MODE, INPUT_PULLDOWN);

    // Configure 74xx85 Hardware Match Pin (Pin 6 A=B Output)
    pinMode(PIN_7485_MATCH, INPUT_PULLDOWN);

    // Configure Modular Input Monitoring Pins
    for (int i = 0; i < 3; i++) {
        pinMode(PIN_MONITOR_A[i], INPUT_PULLDOWN);
    }

    // Configure Status LED Outputs
    pinMode(PIN_LED_PASS, OUTPUT);
    pinMode(PIN_LED_FAIL, OUTPUT);
    digitalWrite(PIN_LED_PASS, LOW);
    digitalWrite(PIN_LED_FAIL, LOW);

    // Initial Self-Test LED Blink Sequence
    digitalWrite(PIN_LED_PASS, HIGH);
    delay(150);
    digitalWrite(PIN_LED_PASS, LOW);
    digitalWrite(PIN_LED_FAIL, HIGH);
    delay(150);
    digitalWrite(PIN_LED_FAIL, LOW);

    Serial.println("[SETUP]: Dual-Mode Verification Engine Ready.\n");
}

// ============================================================================
// Main Execution Loop
// ============================================================================

void loop() {
    unsigned long now = millis();

    if (now - lastPollTime >= POLL_INTERVAL_MS) {
        lastPollTime = now;

        // 1. Sample all hardware pins directly at 5.0V native logic
        uint8_t sampleOp = readBus(PIN_OPCODE, 3) & 0x07;
        uint8_t sampleMode = (digitalRead(PIN_MODE) == HIGH) ? 1 : 0;
        uint8_t sampleHW = readBus(PIN_Y, 8);
        bool sample7485 = (digitalRead(PIN_7485_MATCH) == HIGH);

        // Debounce: require matching consecutive readings
        if (sampleOp == lastSampledOp && sampleMode == lastSampledMode) {
            stableOp = sampleOp;
            stableMode = sampleMode;
            stableHW = sampleHW;
            stableCustomY = sampleHW & 0x0F;        // Lower Nibble: Custom 4-Bit ALU
            stableLS181Y = (sampleHW >> 4) & 0x0F;  // Upper Nibble: Commercial 74LS181N ALU
            stable7485Pass = sample7485;

            // 2. Compute Software Golden Model expected result
            uint8_t expected = computeGoldenModel(stableA, stableB, stableOp, stableMode);

            // 3. Dual-Mode Verification Logic
            bool pass = false;
            if (stableMode == 0) {
                // Mode 0: 8-Bit Cascaded Compute
                // Verifies full 8-bit output against 8-bit software golden reference
                pass = (stableHW == expected);
            } else {
                // Mode 1: Real-Time Silicon Verification
                // Verifies Custom ALU vs 74LS181 vs Software Golden Model & 74xx85 Match
                uint8_t exp4 = expected & 0x0F;
                bool siliconMatch = (stableCustomY == stableLS181Y);
                bool goldenMatch = (stableCustomY == exp4 && stableLS181Y == exp4);
                pass = siliconMatch && goldenMatch && stable7485Pass;
            }

            // 4. Update External Physical Status LEDs
            if (pass) {
                digitalWrite(PIN_LED_PASS, HIGH);
                digitalWrite(PIN_LED_FAIL, LOW);
            } else {
                digitalWrite(PIN_LED_PASS, LOW);
                digitalWrite(PIN_LED_FAIL, HIGH);
            }

            // 5. Serial Telemetry Log
            Serial.print("[SCOREBOARD]: Mode=");
            Serial.print(stableMode ? "M1 (Silicon Verify)" : "M0 (8-Bit Cascaded)");
            Serial.print(" | Op=");
            Serial.print(OP_NAMES[stableOp]);
            Serial.print(" | CustomY=0x");
            Serial.print(stableCustomY, HEX);
            Serial.print(" | 74LS181=0x");
            Serial.print(stableLS181Y, HEX);
            Serial.print(" | HW_Y=0x");
            if (stableHW < 16) Serial.print("0");
            Serial.print(stableHW, HEX);
            Serial.print(" | Exp=0x");
            if (expected < 16) Serial.print("0");
            Serial.print(expected, HEX);
            Serial.print(" | 7485_Pass=");
            Serial.print(stable7485Pass ? "1 (MATCH)" : "0 (NO)");
            Serial.println(pass ? " -> PASS [VERIFIED]" : " -> FAIL [MISMATCH] !");

            // 6. Update 12x8 LED Matrix Display
            if (pass) {
                // Show Checkmark bitmap frame
                matrix.loadFrame(CHECKMARK);
                delay(350);

                // Scroll Mode & Operation PASS text
                char passMsg[32];
                if (stableMode == 0) {
                    snprintf(passMsg, sizeof(passMsg), "M0 8B %s PASS", OP_NAMES[stableOp]);
                } else {
                    snprintf(passMsg, sizeof(passMsg), "M1 SILICON %s PASS", OP_NAMES[stableOp]);
                }
                scrollMessage(passMsg, 50);
            } else {
                // Show X-Mark bitmap frame
                matrix.loadFrame(X_MARK);
                delay(450);

                // Scroll Diagnostic FAIL text
                char failMsg[36];
                if (stableMode == 0) {
                    snprintf(failMsg, sizeof(failMsg), "M0 FAIL EXP %02X HW %02X", expected, stableHW);
                } else {
                    snprintf(failMsg, sizeof(failMsg), "M1 FAIL CUST %X 181 %X", stableCustomY, stableLS181Y);
                }
                scrollMessage(failMsg, 45);
            }
        }

        // Store last sample for debounce
        lastSampledOp = sampleOp;
        lastSampledMode = sampleMode;
    }
}
