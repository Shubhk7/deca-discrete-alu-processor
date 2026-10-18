#pragma once
#include <Arduino.h>

// ============================================================================
// DECA Minor Project - Discrete 74HCT Gate-Level ALU Testbench
// Microcontroller: ESP32-S3 N16R8 (16MB Flash, 8MB Octal PSRAM)
// ============================================================================
// Architecture Notes:
// 1. The hardware ALU is constructed entirely from discrete 74HCT series ICs:
//    - 74HCT86 (Quad XOR) - Full adder XORs, subtraction inversion, XOR logic
//    - 74HCT08 (Quad AND) - Full adder carry generate, AND logic, mode gating
//    - 74HCT32 (Quad OR)  - Full adder carry propagate, OR logic
//    - 74HCT04 (Hex NOT)  - Bitwise NOT logic
//    - 74HCT151 (8:1 MUX) - Operation output selection per bit slice
// 2. Breadboard ALU operates at 5V logic powered from Arduino UNO R4 5V rail.
// 3. ESP32-S3 GPIO pins are 3.3V max!
//    - ALL 5V outputs from breadboard (ALU Output Bus Y[7:0]) MUST pass through
//      1.0 kΩ / 2.2 kΩ resistor voltage dividers before entering ESP32 GPIOs:
//      V_in (5.0V) ---> [ 1.0 kΩ ] ---> ESP32 GPIO ---> [ 2.2 kΩ ] ---> GND
//      V_gpio = 5.0V * (2.2k / (1.0k + 2.2k)) = 3.43V (safe for 3.3V logic)
// 4. Hybrid Input Injection:
//    - Physical DIP switches have 10 kΩ pull-down resistors to GND.
//    - ESP32 GPIOs connect to the DIP switch nodes through 4.7 kΩ series resistors.
//    - When ESP32 GPIO is set to INPUT (INPUT_PULLDOWN), the physical DIP switch
//      controls the logic level (ESP32 acts as passive telemetry monitor).
//    - When ESP32 GPIO is set to OUTPUT, it safely overpowers the DIP switch
//      node through the 4.7 kΩ resistor to inject remote WebSocket test vectors.
// 5. Pin Safety on ESP32-S3 N16R8:
//    - Strapping pins AVOIDED: GPIO 0, GPIO 3, GPIO 45, GPIO 46
//    - Native USB D-/D+ AVOIDED: GPIO 19, GPIO 20
//    - Octal Flash/PSRAM bus pins reserved: GPIO 26-34
// ============================================================================

// ----------------------------------------------------------------------------
// GROUP 1: Operand A Bus (A0..A7) - Hybrid Bidirectional (8 Pins)
// INPUT mode = monitor physical DIP switches | OUTPUT mode = remote override
// ----------------------------------------------------------------------------
#define PIN_A0              1   // Bit 0 (LSB) - Lower Nibble
#define PIN_A1              2   // Bit 1
#define PIN_A2              4   // Bit 2
#define PIN_A3              5   // Bit 3       - Lower Nibble MSB
#define PIN_A4              6   // Bit 4       - Upper Nibble LSB
#define PIN_A5              7   // Bit 5
#define PIN_A6              8   // Bit 6
#define PIN_A7              9   // Bit 7 (MSB) - Upper Nibble MSB

// ----------------------------------------------------------------------------
// GROUP 2: Operand B Bus (B0..B7) - Hybrid Bidirectional (8 Pins)
// INPUT mode = monitor physical DIP switches | OUTPUT mode = remote override
// ----------------------------------------------------------------------------
#define PIN_B0              10  // Bit 0 (LSB) - Lower Nibble
#define PIN_B1              11  // Bit 1
#define PIN_B2              12  // Bit 2
#define PIN_B3              13  // Bit 3       - Lower Nibble MSB
#define PIN_B4              14  // Bit 4       - Upper Nibble LSB
#define PIN_B5              15  // Bit 5
#define PIN_B6              16  // Bit 6
#define PIN_B7              17  // Bit 7 (MSB) - Upper Nibble MSB

// ----------------------------------------------------------------------------
// GROUP 3: Opcode Bus (S0..S2) & Mode Control - Hybrid Bidirectional (4 Pins)
// INPUT mode = monitor physical switches | OUTPUT mode = remote override
// Opcode Table (74HCT151 Select lines):
//   000 = ADD  (A + B via Gate-Level Ripple-Carry Adder)
//   001 = SUB  (A - B via 2's Complement Inversion & Carry-In)
//   010 = AND  (A & B via 74HCT08)
//   011 = OR   (A | B via 74HCT32)
//   100 = XOR  (A ^ B via 74HCT86)
//   101 = NOT  (~A via 74HCT04)
//   110 = SHL  (A << 1 Left Shift)
//   111 = SHR  (A >> 1 Right Shift)
// Mode Control:
//   MODE = 1 : 8-Bit Cascaded ALU Mode (nibble carry ripple enabled)
//   MODE = 0 : 4-Bit Dual Independent Mode (nibble carry gate disabled)
// ----------------------------------------------------------------------------
#define PIN_OPCODE_S0       18  // Opcode Select S0 (LSB)
#define PIN_OPCODE_S1       35  // Opcode Select S1
#define PIN_OPCODE_S2       36  // Opcode Select S2 (MSB)
#define PIN_MODE_SELECT     37  // Mode: 1 = 8-Bit Cascaded, 0 = 4-Bit Dual

// ----------------------------------------------------------------------------
// GROUP 4: 8-Bit ALU Hardware Output Bus (Y0..Y7) - Dedicated Inputs (8 Pins)
// Receives 5V logic from 74HCT151 multiplexer outputs via 1k/2.2k dividers.
// ----------------------------------------------------------------------------
#define PIN_ALU_Y0          38  // Bit 0 (LSB) - Lower Nibble MUX 0 Output
#define PIN_ALU_Y1          39  // Bit 1       - Lower Nibble MUX 1 Output
#define PIN_ALU_Y2          40  // Bit 2       - Lower Nibble MUX 2 Output
#define PIN_ALU_Y3          41  // Bit 3       - Lower Nibble MUX 3 Output
#define PIN_ALU_Y4          42  // Bit 4       - Upper Nibble MUX 4 Output
#define PIN_ALU_Y5          47  // Bit 5       - Upper Nibble MUX 5 Output
#define PIN_ALU_Y6          48  // Bit 6       - Upper Nibble MUX 6 Output
#define PIN_ALU_Y7          21  // Bit 7 (MSB) - Upper Nibble MUX 7 Output

// ----------------------------------------------------------------------------
// Bus Arrays for Clean Loop Iterations
// ----------------------------------------------------------------------------
#define BUS_WIDTH_DATA      8
#define BUS_WIDTH_OPCODE    3

static const uint8_t PINS_BUS_A[BUS_WIDTH_DATA] = {
    PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5, PIN_A6, PIN_A7
};

static const uint8_t PINS_BUS_B[BUS_WIDTH_DATA] = {
    PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_B4, PIN_B5, PIN_B6, PIN_B7
};

static const uint8_t PINS_BUS_Y[BUS_WIDTH_DATA] = {
    PIN_ALU_Y0, PIN_ALU_Y1, PIN_ALU_Y2, PIN_ALU_Y3,
    PIN_ALU_Y4, PIN_ALU_Y5, PIN_ALU_Y6, PIN_ALU_Y7
};

static const uint8_t PINS_BUS_OPCODE[BUS_WIDTH_OPCODE] = {
    PIN_OPCODE_S0, PIN_OPCODE_S1, PIN_OPCODE_S2
};
