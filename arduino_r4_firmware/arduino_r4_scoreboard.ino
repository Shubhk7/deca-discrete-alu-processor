#include "Arduino_LED_Matrix.h"

// ============================================================================
// DECA Minor Project: Hardware-in-the-Loop Software Golden Model Verifier
// Target Board: Arduino UNO R4 WiFi (Renesas RA4M1 32-bit ARM Cortex-M4)
// ============================================================================
// Architecture Overview:
// 1. The hardware under test is an 8-bit/4-bit configurable ALU built entirely
//    from discrete 74HCT logic gate ICs on breadboard:
//    - 74HCT86 (Quad XOR) - Adder sum XORs, subtraction inversion, XOR logic
//    - 74HCT08 (Quad AND) - Adder carry generate, AND logic, mode carry gating
//    - 74HCT32 (Quad OR)  - Adder carry propagate, OR logic
//    - 74HCT04 (Hex NOT)  - Bitwise NOT logic
//    - 74HCT151 (8:1 MUX) - Operation selection per bit slice
// 2. Arduino UNO R4 WiFi serves as the real-time Golden Model Verifier:
//    - Reads hardware ALU Output Bus Y[7:0] (5V native logic directly into pins)
//    - Reads Operands A[7:0], B[7:0], Opcode S[2:0], and Mode Switch
//    - Evaluates exact mathematical expected result in software
//    - Compares Hardware vs Software Golden Model in real time
//    - Displays PASS (Green checkmark + scroll) or FAIL (Red X + expected vs actual)
//      on the onboard 12×8 LED Matrix
//    - Drives external physical Green PASS LED and Red FAIL LED
// ============================================================================

ArduinoLEDMatrix matrix;

// ----------------------------------------------------------------------------
// Pin Definitions (Arduino UNO R4 WiFi 5V Native Pins)
// ----------------------------------------------------------------------------

// 8-Bit Hardware ALU Output Bus (Y0..Y7)
const uint8_t PIN_Y[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

// Dedicated 8-Bit Operand A Bus (A0..A7)
// Note: In modular testbenches, pins 10-13, A0-A3 connect to A0..A7
const uint8_t PIN_A[8] = { 10, 11, 12, 13, A0, A1, A2, A3 };

// Dedicated 8-Bit Operand B Bus (B0..B7)
// (Configurable mapping for expanded headers or parallel harness)
const uint8_t PIN_B[8] = { 10, 11, 12, 13, A0, A1, A2, A3 }; 

// Opcode Bus (S0..S2) & Mode Switch
const uint8_t PIN_OPCODE[3] = { A0, A1, A2 };
const uint8_t PIN_MODE = A3; // 1 = 8-Bit Cascaded, 0 = 4-Bit Dual

// Status LEDs
const uint8_t PIN_LED_PASS = A4; // External Green Status LED
const uint8_t PIN_LED_FAIL = A5; // External Red Status LED

// ----------------------------------------------------------------------------
// 12x8 LED Matrix Bitmaps (3 words of 32 bits = 96 LEDs)
// ----------------------------------------------------------------------------

// PASS Checkmark Bitmap (Centered, bold checkmark on 12x8 grid)
const uint32_t CHECKMARK[3] = {
    0x00000400,
    0xC0184306,
    0x603C0180
};

// FAIL 'X' Mark Bitmap (Centered, high-contrast X on 12x8 grid)
const uint32_t X_MARK[3] = {
    0x60630C19,
    0x80F00F01,
    0x9830C606
};

// Idle Diamond / Logo Frame
const uint32_t LOGO_FRAME[3] = {
    0x0200700F,
    0x81FC3FE0,
    0xF8070020
};

// ----------------------------------------------------------------------------
// Operation Lookup Names
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
// State Variables & Debounce
// ----------------------------------------------------------------------------
uint8_t lastSampledA = 0xFF;
uint8_t lastSampledB = 0xFF;
uint8_t lastSampledOp = 0xFF;
uint8_t lastSampledMode = 0xFF;
uint8_t stableA = 0;
uint8_t stableB = 0;
uint8_t stableOp = 0;
uint8_t stableMode = 1;
uint8_t stableHW = 0;

unsigned long lastPollTime = 0;
const unsigned long POLL_INTERVAL_MS = 200; // 200ms input poll & verification cycle

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
// Software Golden Model (Reference Architecture)
// ============================================================================

uint8_t computeGoldenModel(uint8_t a, uint8_t b, uint8_t opcode, uint8_t mode) {
    uint8_t expected = 0;

    if (mode == 0) {
        // 4-Bit Mode: Lower nibble arithmetic & logic (independent nibbles)
        switch (opcode) {
            case 0: // ADD
                expected = (a + b) & 0x0F;
                break;
            case 1: // SUB (2's complement)
                expected = (a - b) & 0x0F;
                break;
            case 2: // AND
                expected = (a & b) & 0x0F;
                break;
            case 3: // OR
                expected = (a | b) & 0x0F;
                break;
            case 4: // XOR
                expected = (a ^ b) & 0x0F;
                break;
            case 5: // NOT
                expected = (~a) & 0x0F;
                break;
            case 6: // SHL
                expected = (a << 1) & 0x0F;
                break;
            case 7: // SHR
                expected = (a >> 1) & 0x0F;
                break;
            default:
                expected = 0;
                break;
        }
    } else {
        // 8-Bit Mode: Full cascaded 8-bit datapath
        switch (opcode) {
            case 0: // ADD: (A + B) & 0xFF
                expected = (uint8_t)((a + b) & 0xFF);
                break;
            case 1: // SUB: (A - B) & 0xFF (2's complement subtraction)
                expected = (uint8_t)((a - b) & 0xFF);
                break;
            case 2: // AND: A & B
                expected = (uint8_t)(a & b);
                break;
            case 3: // OR: A | B
                expected = (uint8_t)(a | b);
                break;
            case 4: // XOR: A ^ B
                expected = (uint8_t)(a ^ b);
                break;
            case 5: // NOT: ~A & 0xFF
                expected = (uint8_t)(~a & 0xFF);
                break;
            case 6: // SHL: (A << 1) & 0xFF
                expected = (uint8_t)((a << 1) & 0xFF);
                break;
            case 7: // SHR: A >> 1
                expected = (uint8_t)(a >> 1);
                break;
            default:
                expected = 0;
                break;
        }
    }
    return expected;
}

// ============================================================================
// Matrix Text & Pattern Rendering Engine
// ============================================================================

// Lightweight 3x5 ASCII Font Table for numbers, uppercase letters, symbols
static const uint8_t FONT_3X5[][3] = {
    // Space (ASCII 32)
    { 0x00, 0x00, 0x00 },
    // !
    { 0x00, 0x17, 0x00 },
    // "
    { 0x03, 0x00, 0x03 },
    // #
    { 0x1F, 0x0A, 0x1F },
    // $
    { 0x12, 0x1F, 0x09 },
    // %
    { 0x19, 0x04, 0x13 },
    // &
    { 0x0A, 0x15, 0x12 },
    // '
    { 0x00, 0x03, 0x00 },
    // (
    { 0x00, 0x0E, 0x11 },
    // )
    { 0x11, 0x0E, 0x00 },
    // *
    { 0x05, 0x02, 0x05 },
    // +
    { 0x04, 0x0E, 0x04 },
    // ,
    { 0x00, 0x10, 0x08 },
    // -
    { 0x04, 0x04, 0x04 },
    // .
    { 0x00, 0x10, 0x00 },
    // /
    { 0x10, 0x08, 0x04 },
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
    // :
    { 0x00, 0x0A, 0x00 },
    // ;
    { 0x00, 0x14, 0x08 },
    // <
    { 0x04, 0x0A, 0x11 },
    // =
    { 0x0A, 0x0A, 0x0A },
    // >
    { 0x11, 0x0A, 0x04 },
    // ?
    { 0x01, 0x15, 0x03 },
    // @
    { 0x1E, 0x15, 0x1D },
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

    // Build wide column buffer: each char is 3 cols + 1 space col
    int totalCols = len * 4 + 12;
    uint8_t cols[totalCols];
    memset(cols, 0, sizeof(cols));

    int colPtr = 12; // Start after screen width
    for (int i = 0; i < len; i++) {
        char ch = text[i];
        if (ch >= 'a' && ch <= 'z') ch -= 32; // To uppercase

        if (ch >= 32 && ch <= 90) {
            int fontIdx = ch - 32;
            cols[colPtr++] = FONT_3X5[fontIdx][0];
            cols[colPtr++] = FONT_3X5[fontIdx][1];
            cols[colPtr++] = FONT_3X5[fontIdx][2];
            cols[colPtr++] = 0; // Space
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
    Serial.println("  Real-time Hardware-in-the-Loop Scoreboard Active");
    Serial.println("========================================================");

    // Initialize LED Matrix
    matrix.begin();
    matrix.loadFrame(LOGO_FRAME);

    // Configure ALU Output Bus Pins (Digital Inputs with Pull-Down)
    for (int i = 0; i < 8; i++) {
        pinMode(PIN_Y[i], INPUT_PULLDOWN);
    }

    // Configure Operand A & B Bus Pins
    for (int i = 0; i < 8; i++) {
        pinMode(PIN_A[i], INPUT_PULLDOWN);
    }

    // Configure Opcode & Mode Pins
    for (int i = 0; i < 3; i++) {
        pinMode(PIN_OPCODE[i], INPUT_PULLDOWN);
    }
    pinMode(PIN_MODE, INPUT_PULLDOWN);

    // Configure Status LED Outputs
    pinMode(PIN_LED_PASS, OUTPUT);
    pinMode(PIN_LED_FAIL, OUTPUT);
    digitalWrite(PIN_LED_PASS, LOW);
    digitalWrite(PIN_LED_FAIL, LOW);

    // Initial Self-Test Sequence
    digitalWrite(PIN_LED_PASS, HIGH);
    delay(200);
    digitalWrite(PIN_LED_PASS, LOW);
    digitalWrite(PIN_LED_FAIL, HIGH);
    delay(200);
    digitalWrite(PIN_LED_FAIL, LOW);

    Serial.println("[SETUP]: Verification Engine Ready.");
}

// ============================================================================
// Main Execution Loop
// ============================================================================

void loop() {
    unsigned long now = millis();

    if (now - lastPollTime >= POLL_INTERVAL_MS) {
        lastPollTime = now;

        // 1. Sample all hardware input pins
        uint8_t sampleA = readBus(PIN_A, 8);
        uint8_t sampleB = readBus(PIN_B, 8);
        uint8_t sampleOp = readBus(PIN_OPCODE, 3) & 0x07;
        uint8_t sampleMode = (digitalRead(PIN_MODE) == HIGH) ? 1 : 0;
        uint8_t sampleHW = readBus(PIN_Y, 8);

        // 2. Debounce: require matching consecutive readings
        if (sampleA == lastSampledA && sampleB == lastSampledB &&
            sampleOp == lastSampledOp && sampleMode == lastSampledMode) {
            
            stableA = sampleA;
            stableB = sampleB;
            stableOp = sampleOp;
            stableMode = sampleMode;
            stableHW = sampleHW;

            // 3. Compute Golden Model expected result
            uint8_t expected = computeGoldenModel(stableA, stableB, stableOp, stableMode);

            // 4. Compare Hardware Output against Golden Reference
            bool pass = false;
            if (stableMode == 0) {
                // 4-Bit Mode: compare lower nibble
                pass = ((stableHW & 0x0F) == (expected & 0x0F));
            } else {
                // 8-Bit Mode: compare full byte
                pass = (stableHW == expected);
            }

            // 5. Update Status LEDs
            if (pass) {
                digitalWrite(PIN_LED_PASS, HIGH);
                digitalWrite(PIN_LED_FAIL, LOW);
            } else {
                digitalWrite(PIN_LED_PASS, LOW);
                digitalWrite(PIN_LED_FAIL, HIGH);
            }

            // 6. Serial Telemetry Log
            Serial.print("[VERIFIER]: Op=");
            Serial.print(OP_NAMES[stableOp]);
            Serial.print(" | Mode=");
            Serial.print(stableMode ? "8-Bit" : "4-Bit");
            Serial.print(" | A=0x");
            if (stableA < 16) Serial.print("0");
            Serial.print(stableA, HEX);
            Serial.print(" | B=0x");
            if (stableB < 16) Serial.print("0");
            Serial.print(stableB, HEX);
            Serial.print(" | Exp=0x");
            if (expected < 16) Serial.print("0");
            Serial.print(expected, HEX);
            Serial.print(" | HW=0x");
            if (stableHW < 16) Serial.print("0");
            Serial.print(stableHW, HEX);
            Serial.println(pass ? " -> PASS [MATCH]" : " -> FAIL [MISMATCH] !");

            // 7. Update 12x8 LED Matrix Display
            if (pass) {
                // Show Green checkmark frame
                matrix.loadFrame(CHECKMARK);
                delay(600);
                // Scroll 'PASS'
                scrollMessage("PASS", 60);
            } else {
                // Show Red X frame
                matrix.loadFrame(X_MARK);
                delay(700);
                // Scroll 'FAIL' and diagnostic values
                char failMsg[32];
                snprintf(failMsg, sizeof(failMsg), "FAIL EXP %02X HW %02X", expected, stableHW);
                scrollMessage(failMsg, 50);
            }
        }

        // Store last sample for debouncer
        lastSampledA = sampleA;
        lastSampledB = sampleB;
        lastSampledOp = sampleOp;
        lastSampledMode = sampleMode;
    }
}
