/*
 * =====================================================================================
 * DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor
 * Arduino UNO R4 WiFi — Upper 4-Bit ALU Engine & 12x8 LED Matrix Virtual Scoreboard HUD
 * =====================================================================================
 * Target: Arduino UNO R4 WiFi (Renesas RA4M1 32-bit ARM Cortex-M4 @ 48 MHz)
 * Logic Level: Native 5V TTL
 *
 * Responsibilities:
 *  1. Ingests hardware Carry-Out (C4) from 74LS181N Pin 16 in real time via digital Pin D6.
 *  2. Reads Lower Nibble outputs Y[3:0] directly from 74LS181N Pins 9, 10, 11, 13 (Pins D2..D5).
 *  3. Ingests Upper Nibble inputs A[7:4] (Pins D7..D10) & B[7:4] (Pins D11, D12, D13, A0).
 *  4. Ingests Opcode S[2:0] (Pins A1, A2, A3), Mode M (Pin A4), and Carry-In C0 (Pin A5).
 *  5. Computes Upper 4-bit ALU operations in real-time firmware, cascading C4 into C8.
 *  6. Assembles full 8-bit word Y[7:0] = { Y_MCU[7:4], Y_181[3:0] }.
 *  7. Software Golden Model verification engine: Compares hybrid hardware against math model.
 *  8. Dynamic 12x8 LED Matrix HUD: Renders 8-bit binary dots, carry bits, opcode symbols,
 *     and scrolling result telemetry without requiring ANY discrete LEDs!
 * =====================================================================================
 */

#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// =====================================================================================
// PIN ASSIGNMENTS (Native 5V Logic)
// =====================================================================================

// Lower 4-Bit Results read directly from 74LS181N Outputs (Active-HIGH after polarity read)
const uint8_t PIN_Y0_181 = 2;   // 74LS181N Pin 9  (/F0)
const uint8_t PIN_Y1_181 = 3;   // 74LS181N Pin 10 (/F1)
const uint8_t PIN_Y2_181 = 4;   // 74LS181N Pin 11 (/F2)
const uint8_t PIN_Y3_181 = 5;   // 74LS181N Pin 13 (/F3)

// Hardware Carry Cascade Line from 74LS181N Pin 16 (/Cn+4)
// Note: 74LS181 Pin 16 is active-LOW for arithmetic carry out.
const uint8_t PIN_C4_CASCADE = 6; // 74LS181N Pin 16 (/Cn+4)

// Upper Nibble Operand A Inputs (A4..A7)
const uint8_t PIN_A4 = 7;
const uint8_t PIN_A5 = 8;
const uint8_t PIN_A6 = 9;
const uint8_t PIN_A7 = 10;

// Upper Nibble Operand B Inputs (B4..B7)
const uint8_t PIN_B4 = 11;
const uint8_t PIN_B5 = 12;
const uint8_t PIN_B6 = 13;
const uint8_t PIN_B7 = A0;  // Analog pin A0 used as Digital GPIO D14

// Opcode S[2:0] & Mode Select M Inputs
const uint8_t PIN_S0 = A1;  // Opcode bit 0 (SUB toggle in arithmetic mode)
const uint8_t PIN_S1 = A2;  // Opcode bit 1
const uint8_t PIN_S2 = A3;  // Opcode bit 2
const uint8_t PIN_MODE = A4;// Mode Select (0 = Arithmetic, 1 = Logic)
const uint8_t PIN_C0   = A5;// Carry-In bit C0

// =====================================================================================
// GLOBAL STATE & TELEMETRY REGISTERS
// =====================================================================================
uint8_t a_val = 0;           // Full 8-bit Operand A
uint8_t b_val = 0;           // Full 8-bit Operand B
uint8_t opcode = 0;          // 3-bit Opcode (0..7)
uint8_t mode = 0;            // 1-bit Mode (0=Arith, 1=Logic)
uint8_t c0_in = 0;           // Initial Carry-In
uint8_t c4_cascade = 0;      // Intermediate Carry from 74LS181N
uint8_t c8_out = 0;          // Final 8-bit Carry-Out from Upper ALU

uint8_t y_181_lo = 0;        // Lower 4 bits from 74LS181N
uint8_t y_mcu_hi = 0;        // Upper 4 bits computed by Arduino
uint8_t y_hybrid_8bit = 0;   // Assembled 8-bit hybrid output
uint8_t y_expected_8bit = 0; // Software golden model prediction
bool verification_pass = true;

// LED Matrix Frame Buffer (12 columns x 8 rows = 96 pixels, packed as 3 x uint32_t)
uint32_t matrix_frame[3] = {0, 0, 0};

unsigned long last_display_update = 0;
unsigned long last_serial_telemetry = 0;
uint8_t prev_hybrid_y = 0xFF;
uint8_t prev_opcode = 0xFF;

// =====================================================================================
// 12x8 MATRIX BITMAP ICONS FOR ALU OPCODES
// =====================================================================================
// Each frame represents a 12-column by 8-row grid encoded into three 32-bit words
// [Row 0..2 (32b), Row 2..5 (32b), Row 5..7 (32b)]

// Checkmark Icon (PASS)
const uint32_t ICON_PASS[3] = {
    0x00000000,
    0x00200060,
    0x0c401880
};

// X-Mark Icon (FAIL)
const uint32_t ICON_FAIL[3] = {
    0x00020400,
    0x01800180,
    0x04000200
};

// Set pixel at (x, y) where x is 0..11 and y is 0..7
void setMatrixPixel(uint8_t x, uint8_t y, bool on) {
    if (x >= 12 || y >= 8) return;
    uint8_t pixel_index = y * 12 + x; // 0 to 95
    uint8_t word_idx = pixel_index / 32;
    uint8_t bit_idx = 31 - (pixel_index % 32);

    if (on) {
        matrix_frame[word_idx] |= (1UL << bit_idx);
    } else {
        matrix_frame[word_idx] &= ~(1UL << bit_idx);
    }
}

void clearMatrixFrame() {
    matrix_frame[0] = 0;
    matrix_frame[1] = 0;
    matrix_frame[2] = 0;
}

// =====================================================================================
// SETUP
// =====================================================================================
void setup() {
    Serial.begin(115200);

    // Initialize 12x8 LED Matrix
    matrix.begin();

    // Configure 74LS181N input sensing pins
    pinMode(PIN_Y0_181, INPUT);
    pinMode(PIN_Y1_181, INPUT);
    pinMode(PIN_Y2_181, INPUT);
    pinMode(PIN_Y3_181, INPUT);

    // Configure Hardware Carry Cascade line (Pin 16 of 74LS181)
    pinMode(PIN_C4_CASCADE, INPUT);

    // Configure Upper Nibble inputs with internal pull-downs (No external resistors needed!)
    pinMode(PIN_A4, INPUT_PULLDOWN);
    pinMode(PIN_A5, INPUT_PULLDOWN);
    pinMode(PIN_A6, INPUT_PULLDOWN);
    pinMode(PIN_A7, INPUT_PULLDOWN);

    pinMode(PIN_B4, INPUT_PULLDOWN);
    pinMode(PIN_B5, INPUT_PULLDOWN);
    pinMode(PIN_B6, INPUT_PULLDOWN);
    pinMode(PIN_B7, INPUT_PULLDOWN);

    // Configure Opcode, Mode, and C0 inputs with internal pull-downs
    pinMode(PIN_S0, INPUT_PULLDOWN);
    pinMode(PIN_S1, INPUT_PULLDOWN);
    pinMode(PIN_S2, INPUT_PULLDOWN);
    pinMode(PIN_MODE, INPUT_PULLDOWN);
    pinMode(PIN_C0, INPUT_PULLDOWN);

    Serial.println(F("====================================================================="));
    Serial.println(F(" DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor"));
    Serial.println(F(" Upper 4-Bit ALU Engine & 12x8 Matrix Virtual Scoreboard HUD Started"));
    Serial.println(F("====================================================================="));
}

// =====================================================================================
// STEP 1: READ MASTER BUS INPUTS & LOWER 74LS181 SILICON OUTPUTS
// =====================================================================================
void readHardwareInputs() {
    // Read Opcode S[2:0] and Mode M
    uint8_t s0 = digitalRead(PIN_S0);
    uint8_t s1 = digitalRead(PIN_S1);
    uint8_t s2 = digitalRead(PIN_S2);
    opcode = (s2 << 2) | (s1 << 1) | s0;
    mode   = digitalRead(PIN_MODE);
    c0_in  = digitalRead(PIN_C0);

    // Read Upper Nibbles of A and B
    uint8_t a4 = digitalRead(PIN_A4);
    uint8_t a5 = digitalRead(PIN_A5);
    uint8_t a6 = digitalRead(PIN_A6);
    uint8_t a7 = digitalRead(PIN_A7);
    uint8_t a_hi = (a7 << 3) | (a6 << 2) | (a5 << 1) | a4;

    uint8_t b4 = digitalRead(PIN_B4);
    uint8_t b5 = digitalRead(PIN_B5);
    uint8_t b6 = digitalRead(PIN_B6);
    uint8_t b7 = digitalRead(PIN_B7);
    uint8_t b_hi = (b7 << 3) | (b6 << 2) | (b5 << 1) | b4;

    // Read Lower Nibble from 74LS181N outputs (F0..F3)
    // Note: 74LS181 outputs active-HIGH result when active-HIGH table is followed
    uint8_t y0 = digitalRead(PIN_Y0_181);
    uint8_t y1 = digitalRead(PIN_Y1_181);
    uint8_t y2 = digitalRead(PIN_Y2_181);
    uint8_t y3 = digitalRead(PIN_Y3_181);
    y_181_lo = (y3 << 3) | (y2 << 2) | (y1 << 1) | y0;

    // Read Carry Cascade line (74LS181 Pin 16: /Cn+4)
    // In 74LS181 arithmetic mode, /Cn+4 is active-LOW (0 = Carry Generated, 1 = No Carry).
    // Invert to get standard active-HIGH Carry C4 for upper addition.
    uint8_t raw_c4 = digitalRead(PIN_C4_CASCADE);
    c4_cascade = (raw_c4 == LOW) ? 1 : 0;

    // In logic operations (mode == 1), carry does not propagate
    if (mode == 1) {
        c4_cascade = 0;
    }
}

// =====================================================================================
// STEP 2: UPPER 4-BIT ALU FIRMWARE ENGINE (Cascaded Computing)
// =====================================================================================
void computeUpperNibbleALU() {
    uint8_t a_hi = (a_val >> 4) & 0x0F;
    uint8_t b_hi = (b_val >> 4) & 0x0F;
    c8_out = 0;

    switch (opcode) {
        case 0: // ADD: A + B + C4
            {
                uint16_t sum = (uint16_t)a_hi + (uint16_t)b_hi + (uint16_t)c4_cascade;
                y_mcu_hi = sum & 0x0F;
                c8_out = (sum > 15) ? 1 : 0;
            }
            break;

        case 1: // SUB: A - B with borrow
            {
                // In two's complement cascading, borrow-in = 1 - c4_cascade
                int16_t diff = (int16_t)a_hi - (int16_t)b_hi - (int16_t)(1 - c4_cascade);
                if (diff < 0) {
                    y_mcu_hi = (diff + 16) & 0x0F;
                    c8_out = 0; // Borrow occurred
                } else {
                    y_mcu_hi = diff & 0x0F;
                    c8_out = 1; // No borrow (Standard unsigned carry)
                }
            }
            break;

        case 2: // AND: A & B
            y_mcu_hi = a_hi & b_hi;
            c8_out = 0;
            break;

        case 3: // OR: A | B
            y_mcu_hi = a_hi | b_hi;
            c8_out = 0;
            break;

        case 4: // XOR: A ^ B
            y_mcu_hi = a_hi ^ b_hi;
            c8_out = 0;
            break;

        case 5: // NOT: ~A
            y_mcu_hi = (~a_hi) & 0x0F;
            c8_out = 0;
            break;

        case 6: // SHL: Shift Left (Bit 3 of lower nibble shifted into Bit 0 of upper nibble)
            {
                uint8_t bit3_lower = (y_181_lo >> 3) & 0x01;
                uint8_t shifted = (a_hi << 1) | bit3_lower;
                y_mcu_hi = shifted & 0x0F;
                c8_out = (a_hi >> 3) & 0x01; // Outgoing MSB is Carry-Out
            }
            break;

        case 7: // SHR: Shift Right (Bit 4 of upper nibble shifted into MSB)
            {
                y_mcu_hi = (a_hi >> 1) & 0x07;
                c8_out = a_hi & 0x01;
            }
            break;

        default:
            y_mcu_hi = 0;
            c8_out = 0;
            break;
    }

    // Assemble complete 8-bit hybrid output word
    y_hybrid_8bit = ((y_mcu_hi & 0x0F) << 4) | (y_181_lo & 0x0F);
}

// =====================================================================================
// STEP 3: SOFTWARE GOLDEN MODEL ORACLE VERIFICATION
// =====================================================================================
void evaluateGoldenModel() {
    uint16_t expected_math = 0;

    switch (opcode) {
        case 0: // ADD
            expected_math = (uint16_t)a_val + (uint16_t)b_val + (uint16_t)c0_in;
            y_expected_8bit = expected_math & 0xFF;
            break;

        case 1: // SUB
            expected_math = (uint16_t)((int16_t)a_val - (int16_t)b_val);
            y_expected_8bit = expected_math & 0xFF;
            break;

        case 2: // AND
            y_expected_8bit = a_val & b_val;
            break;

        case 3: // OR
            y_expected_8bit = a_val | b_val;
            break;

        case 4: // XOR
            y_expected_8bit = a_val ^ b_val;
            break;

        case 5: // NOT
            y_expected_8bit = (~a_val) & 0xFF;
            break;

        case 6: // SHL
            y_expected_8bit = (a_val << 1) & 0xFF;
            break;

        case 7: // SHR
            y_expected_8bit = (a_val >> 1) & 0x7F;
            break;
    }

    verification_pass = (y_hybrid_8bit == y_expected_8bit);
}

// =====================================================================================
// STEP 4: 12x8 LED MATRIX VIRTUAL SCOREBOARD HUD (No Discrete LEDs Needed!)
// =====================================================================================
void renderLedMatrixHUD() {
    clearMatrixFrame();

    // ---------------------------------------------------------------------------------
    // COLUMNS 8..11: LOWER 4-BIT SILICON RESULT Y[3:0] (from 74LS181N)
    // ---------------------------------------------------------------------------------
    for (uint8_t bit = 0; bit < 4; bit++) {
        uint8_t col = 11 - bit; // Col 11=Y0, Col 10=Y1, Col 9=Y2, Col 8=Y3
        bool bit_state = (y_181_lo >> bit) & 0x01;
        if (bit_state) {
            // Light 3 vertical dots for visual impact
            setMatrixPixel(col, 3, true);
            setMatrixPixel(col, 4, true);
            setMatrixPixel(col, 5, true);
        }
    }

    // ---------------------------------------------------------------------------------
    // COLUMN 7: NIBBLE BOUNDARY SEPARATOR (Hardware vs Software Interface)
    // ---------------------------------------------------------------------------------
    // A subtle dashed dot line indicating the boundary between 74LS181 and MCU
    setMatrixPixel(7, 2, true);
    setMatrixPixel(7, 5, true);

    // ---------------------------------------------------------------------------------
    // COLUMNS 3..6: UPPER 4-BIT MCU RESULT Y[7:4] (from Arduino R4)
    // ---------------------------------------------------------------------------------
    for (uint8_t bit = 0; bit < 4; bit++) {
        uint8_t col = 6 - bit; // Col 6=Y4, Col 5=Y5, Col 4=Y6, Col 3=Y7
        bool bit_state = (y_mcu_hi >> bit) & 0x01;
        if (bit_state) {
            setMatrixPixel(col, 3, true);
            setMatrixPixel(col, 4, true);
            setMatrixPixel(col, 5, true);
        }
    }

    // ---------------------------------------------------------------------------------
    // COLUMNS 0..1: CARRY CHAIN TELEMETRY HUD
    // ---------------------------------------------------------------------------------
    // Col 0, Row 7: Carry-In C0
    if (c0_in) {
        setMatrixPixel(0, 7, true);
    }
    // Col 0, Row 4: Intermediate Cascade Carry C4 (from 74LS181 Pin 16)
    if (c4_cascade) {
        setMatrixPixel(0, 4, true);
        setMatrixPixel(1, 4, true);
    }
    // Col 0, Row 1: Final Carry-Out C8 (from Upper 4-bit ALU)
    if (c8_out) {
        setMatrixPixel(0, 1, true);
        setMatrixPixel(1, 1, true);
    }

    // ---------------------------------------------------------------------------------
    // ROW 0: OPCODE SYMBOL INDICATOR
    // ---------------------------------------------------------------------------------
    switch (opcode) {
        case 0: // ADD '+'
            setMatrixPixel(4, 0, true);
            setMatrixPixel(5, 0, true);
            setMatrixPixel(4, 1, true);
            setMatrixPixel(5, 1, true);
            break;
        case 1: // SUB '-'
            setMatrixPixel(3, 0, true);
            setMatrixPixel(4, 0, true);
            setMatrixPixel(5, 0, true);
            setMatrixPixel(6, 0, true);
            break;
        case 2: // AND '&'
            setMatrixPixel(4, 0, true);
            setMatrixPixel(5, 1, true);
            break;
        case 3: // OR '|'
            setMatrixPixel(5, 0, true);
            setMatrixPixel(5, 1, true);
            break;
        case 4: // XOR '^'
            setMatrixPixel(4, 1, true);
            setMatrixPixel(5, 0, true);
            setMatrixPixel(6, 1, true);
            break;
        case 5: // NOT '~'
            setMatrixPixel(3, 1, true);
            setMatrixPixel(4, 0, true);
            setMatrixPixel(5, 1, true);
            setMatrixPixel(6, 0, true);
            break;
        case 6: // SHL '<<'
            setMatrixPixel(4, 0, true);
            setMatrixPixel(3, 1, true);
            setMatrixPixel(6, 0, true);
            setMatrixPixel(5, 1, true);
            break;
        case 7: // SHR '>>'
            setMatrixPixel(3, 0, true);
            setMatrixPixel(4, 1, true);
            setMatrixPixel(5, 0, true);
            setMatrixPixel(6, 1, true);
            break;
    }

    // ---------------------------------------------------------------------------------
    // ROW 7 (Bottom Right): HARDWARE EQUIVALENCE PASS / FAIL BADGE
    // ---------------------------------------------------------------------------------
    if (verification_pass) {
        // Dot for PASS
        setMatrixPixel(11, 7, true);
        setMatrixPixel(10, 7, true);
    } else {
        // Flash entire bottom line on verification fail
        for (uint8_t c = 8; c < 12; c++) {
            setMatrixPixel(c, 7, true);
        }
    }

    // Push the rendered frame to the hardware LED matrix
    matrix.loadFrame(matrix_frame);
}

// =====================================================================================
// STEP 5: SERIAL TELEMETRY LOGGING & DEBUG HUD
// =====================================================================================
void printSerialTelemetry() {
    const char* op_names[] = {"ADD", "SUB", "AND", "OR ", "XOR", "NOT", "SHL", "SHR"};

    Serial.print(F("[DECA-ALU] OP: "));
    Serial.print(op_names[opcode & 0x07]);
    Serial.print(F(" | A: 0x"));
    if (a_val < 0x10) Serial.print('0');
    Serial.print(a_val, HEX);
    Serial.print(F(" (")); Serial.print(a_val); Serial.print(F(")"));

    Serial.print(F(" | B: 0x"));
    if (b_val < 0x10) Serial.print('0');
    Serial.print(b_val, HEX);
    Serial.print(F(" (")); Serial.print(b_val); Serial.print(F(")"));

    Serial.print(F(" | C4_Cascade: "));
    Serial.print(c4_cascade);

    Serial.print(F(" | Y_Hybrid: 0x"));
    if (y_hybrid_8bit < 0x10) Serial.print('0');
    Serial.print(y_hybrid_8bit, HEX);
    Serial.print(F(" [181:0x"));
    Serial.print(y_181_lo, HEX);
    Serial.print(F(", MCU:0x"));
    Serial.print(y_mcu_hi, HEX);
    Serial.print(F("]"));

    Serial.print(F(" | C8: "));
    Serial.print(c8_out);

    Serial.print(F(" | Status: "));
    if (verification_pass) {
        Serial.println(F("PASSED [100% Match]"));
    } else {
        Serial.print(F("MISMATCH! Expected: 0x"));
        Serial.println(y_expected_8bit, HEX);
    }
}

// =====================================================================================
// MAIN LOOP
// =====================================================================================
void loop() {
    // 1. Ingest hardware inputs & 74LS181 lower nibble result
    readHardwareInputs();

    // Reconstruct full A and B for software models
    // (Lower nibble read from 74LS181 A & B buses or simulated if shared)
    uint8_t a_hi = (digitalRead(PIN_A7) << 3) | (digitalRead(PIN_A6) << 2) |
                   (digitalRead(PIN_A5) << 1) | digitalRead(PIN_A4);
    uint8_t b_hi = (digitalRead(PIN_B7) << 3) | (digitalRead(PIN_B6) << 2) |
                   (digitalRead(PIN_B5) << 1) | digitalRead(PIN_B4);

    // If lower inputs are monitored externally:
    a_val = (a_hi << 4) | (a_val & 0x0F);
    b_val = (b_hi << 4) | (b_val & 0x0F);

    // 2. Compute Upper Nibble in real time using 74LS181 carry-out
    computeUpperNibbleALU();

    // 3. Verify against mathematical golden model
    evaluateGoldenModel();

    // 4. Update dynamic 12x8 LED matrix display HUD (every 40 ms = 25 FPS)
    unsigned long current_time = millis();
    if (current_time - last_display_update >= 40) {
        last_display_update = current_time;
        renderLedMatrixHUD();
    }

    // 5. Emit serial telemetry on state change or every 500 ms heartbeat
    if ((y_hybrid_8bit != prev_hybrid_y) || (opcode != prev_opcode) || (current_time - last_serial_telemetry >= 500)) {
        prev_hybrid_y = y_hybrid_8bit;
        prev_opcode = opcode;
        last_serial_telemetry = current_time;
        printSerialTelemetry();
    }
}
