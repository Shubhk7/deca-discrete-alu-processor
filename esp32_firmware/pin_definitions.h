#pragma once
#include <Arduino.h>

// ============================================================================
// DECA Minor Project: Dual-Mode Hybrid Discrete ALU Processor Testbench
// Microcontroller: ESP32-S3 N16R8 ("The Brain" / Web Dashboard / Remote Injector)
// Author: Shubh Kashyap | Course: Digital Electronics & Computer Architecture
// ============================================================================
// Dual-Mode Hybrid Architecture Specification:
// 1. Hardware ALU Under Test:
//    - MODE 0: 8-Bit Cascaded Compute
//      * Lower Nibble Y[3:0]: Custom 4-Bit ALU synthesized from 74xx283 4-bit binary
//        full adder, 74xx86 XOR, 74xx08 AND, 74xx32 OR, 74xx04 NOT, and 4x 74xx151 8:1 MUXes.
//      * Upper Nibble Y[7:4]: Commercial 74LS181N 4-Bit High-Functionality ALU.
//      * Carry Cascade: IC1 (74xx283) Carry Out (C4) connects to 74LS181N Carry In (/Cn).
//    - MODE 1: Real-Time Silicon Verification
//      * Both Custom 4-Bit ALU and 74LS181N receive IDENTICAL 4-bit inputs A[3:0], B[3:0]
//        and opcode control signals.
//      * IC12 (74xx85 4-Bit Magnitude Comparator) performs real-time hardware equivalence
//        checking between Custom ALU Y[3:0] and 74LS181N F[3:0].
//      * 74xx85 Pin 6 (A=B Output) goes HIGH on exact hardware match, lighting an external
//        Green PASS LED and signaling both Arduino UNO R4 WiFi and ESP32-S3.
//
// 2. Heterogeneous 3-Board Infrastructure:
//    - ESP32-S3 N16R8 ("The Brain"):
//      * SoftAP Wi-Fi ('DECA-ALU', 192.168.4.1), HTTP Web Server (Port 80), WebSockets (Port 81).
//      * Remote input injection via 4.7 kΩ override resistors over physical 10 kΩ pull-downs.
//    - Arduino UNO R4 WiFi ("The Verifier"):
//      * Native 5.0V bus sniffer and Software Golden Model reference engine.
//      * 12x8 LED Matrix HUD (Checkmark ✓ / Cross ✗ / Scrolling Opcode) & Status LEDs.
//    - Arduino UNO R4 Minima ("The Power Station"):
//      * Dedicated clean 5.0V / 1.0A USB-C power rail supplying all breadboard TTL/CMOS ICs.
//      * Common ground topology shared across all three microcontroller boards.
//
// 3. Logic Level Translation (5V Breadboard to 3.3V ESP32-S3):
//    - Breadboard ICs operate on +5.0V VCC powered from Arduino UNO R4 Minima power rail.
//    - ESP32-S3 GPIO pins tolerate max 3.3V (3.6V absolute maximum).
//    - ALL 5V signals entering ESP32 GPIOs (Y0..Y7, 7485 PASS flag) pass through
//      1.0 kΩ (series) / 2.2 kΩ (shunt to GND) precision resistor dividers:
//      V_in (5.0V) ---> [ 1.0 kΩ ] ---> ESP32 GPIO ---> [ 2.2 kΩ ] ---> GND
//      V_gpio = 5.0V * (2200 / (1000 + 2200)) = 3.43V (safe, robust 3.3V CMOS HIGH).
//
// 4. Safe GPIO Allocation on ESP32-S3 N16R8:
//    - Strapping pins preserved/avoided: GPIO 0, 45, 46
//    - Native USB D-/D+ preserved: GPIO 19, 20
//    - Octal Flash/PSRAM bus reserved: GPIO 26 through GPIO 34 (DO NOT USE!)
//    - JTAG / UART pins preserved: GPIO 43 (TXD0), GPIO 44 (RXD0)
// ============================================================================

// ----------------------------------------------------------------------------
// GROUP 1: Operand A Bus (A0..A7) - Hybrid Bidirectional (8 Pins)
// INPUT mode = monitor physical DIP switches | OUTPUT mode = remote override
// In Mode 0: A[7:0] = full 8-bit operand (A[3:0] -> Custom ALU, A[7:4] -> 74LS181)
// In Mode 1: A[3:0] is duplicated to A[7:4] for parallel silicon comparison
// ----------------------------------------------------------------------------
#define PIN_A0              1   // Bit 0 (LSB) - Lower Nibble Custom ALU
#define PIN_A1              2   // Bit 1
#define PIN_A2              4   // Bit 2
#define PIN_A3              5   // Bit 3       - Lower Nibble Custom ALU MSB
#define PIN_A4              6   // Bit 4       - Upper Nibble 74LS181 LSB
#define PIN_A5              7   // Bit 5
#define PIN_A6              8   // Bit 6
#define PIN_A7              9   // Bit 7 (MSB) - Upper Nibble 74LS181 MSB

// ----------------------------------------------------------------------------
// GROUP 2: Operand B Bus (B0..B7) - Hybrid Bidirectional (8 Pins)
// INPUT mode = monitor physical DIP switches | OUTPUT mode = remote override
// In Mode 0: B[7:0] = full 8-bit operand (B[3:0] -> Custom ALU, B[7:4] -> 74LS181)
// In Mode 1: B[3:0] is duplicated to B[7:4] for parallel silicon comparison
// ----------------------------------------------------------------------------
#define PIN_B0              10  // Bit 0 (LSB) - Lower Nibble Custom ALU
#define PIN_B1              11  // Bit 1
#define PIN_B2              12  // Bit 2
#define PIN_B3              13  // Bit 3       - Lower Nibble Custom ALU MSB
#define PIN_B4              14  // Bit 4       - Upper Nibble 74LS181 LSB
#define PIN_B5              15  // Bit 5
#define PIN_B6              16  // Bit 6
#define PIN_B7              17  // Bit 7 (MSB) - Upper Nibble 74LS181 MSB

// ----------------------------------------------------------------------------
// GROUP 3: Opcode Bus (S0..S2) & Mode Control - Hybrid Bidirectional (4 Pins)
// INPUT mode = monitor physical switches | OUTPUT mode = remote override
// Opcode Selection Table (74xx151 8:1 MUX Select & 74LS181 Function Mapping):
//   000 = ADD  (Mode 0: 8-Bit Cascaded Add | Mode 1: 4-Bit Silicon Add Verify)
//   001 = SUB  (Mode 0: 8-Bit Cascaded Sub | Mode 1: 4-Bit Silicon Sub Verify)
//   010 = AND  (Mode 0: 8-Bit Bitwise AND  | Mode 1: 4-Bit Silicon AND Verify)
//   011 = OR   (Mode 0: 8-Bit Bitwise OR   | Mode 1: 4-Bit Silicon OR Verify)
//   100 = XOR  (Mode 0: 8-Bit Bitwise XOR  | Mode 1: 4-Bit Silicon XOR Verify)
//   101 = NOT  (Mode 0: 8-Bit Bitwise NOT  | Mode 1: 4-Bit Silicon NOT Verify)
//   110 = SHL  (Mode 0: 8-Bit Logical Shift Left)
//   111 = SHR  (Mode 0: 8-Bit Logical Shift Right)
// Mode Selection:
//   MODE = 0 : 8-Bit Cascaded Compute (Lower Custom ALU + Upper 74LS181)
//   MODE = 1 : Real-Time Silicon Verification (Custom ALU vs 74LS181 via 74xx85)
// ----------------------------------------------------------------------------
#define PIN_OPCODE_S0       18  // Opcode Select S0 (LSB)
#define PIN_OPCODE_S1       35  // Opcode Select S1
#define PIN_OPCODE_S2       36  // Opcode Select S2 (MSB)
#define PIN_MODE_SELECT     37  // Datapath Mode: 0 = 8-Bit Cascaded, 1 = Silicon Verification

// ----------------------------------------------------------------------------
// GROUP 4: 8-Bit ALU Hardware Output Bus (Y0..Y7) - Dedicated Inputs (8 Pins)
// Receives 5V logic via 1.0kΩ / 2.2kΩ dividers.
// Bits 0..3 (Y0..Y3) = Custom 4-Bit ALU Output (from 4x 74xx151 MUX outputs)
// Bits 4..7 (Y4..Y7) = Commercial 74LS181N 4-Bit ALU Output (F0..F3)
// ----------------------------------------------------------------------------
#define PIN_ALU_Y0          38  // Bit 0 (LSB) - Custom 4-Bit ALU MUX 0 Output
#define PIN_ALU_Y1          39  // Bit 1       - Custom 4-Bit ALU MUX 1 Output
#define PIN_ALU_Y2          40  // Bit 2       - Custom 4-Bit ALU MUX 2 Output
#define PIN_ALU_Y3          41  // Bit 3       - Custom 4-Bit ALU MUX 3 Output
#define PIN_ALU_Y4          42  // Bit 4       - 74LS181N Output F0
#define PIN_ALU_Y5          47  // Bit 5       - 74LS181N Output F1
#define PIN_ALU_Y6          48  // Bit 6       - 74LS181N Output F2
#define PIN_ALU_Y7          21  // Bit 7 (MSB) - 74LS181N Output F3

// ----------------------------------------------------------------------------
// GROUP 5: 74xx85 Hardware Silicon Match Monitor Pin
// Receives HIGH (1) from IC12 (74xx85) Pin 6 (A=B Output) via 1.0kΩ / 2.2kΩ divider
// HIGH = Hardware Silicon Equivalence Confirmed between Custom ALU and 74LS181N
// ----------------------------------------------------------------------------
#define PIN_7485_PASS       3   // Monitor 74xx85 Pin 6 (A=B Match Output)
#define PIN_VERIFIER_PASS   3   // Alias for backwards compatibility

// ----------------------------------------------------------------------------
// Bus Arrays for Iterative Loop Operations
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
