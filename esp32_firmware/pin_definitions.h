#pragma once

// ============================================================================
// ESP32-S3 N16R8 Pin Definitions for Dual-Mode 8-Bit ALU + HIL Verifier
// Note: GPIO 33-37 are reserved for Octal Flash / PSRAM on N16R8.
//       GPIO 19, 20 are used for Native USB D-/D+.
// ============================================================================

// --- 8-Bit Result Bus Inputs (From Breadboard ALU via 1k/2k Resistor Dividers) ---
#define PIN_RESULT_Y0       40  // Custom ALU Bit 0 (LSB)
#define PIN_RESULT_Y1       41  // Custom ALU Bit 1
#define PIN_RESULT_Y2       42  // Custom ALU Bit 2
#define PIN_RESULT_Y3       43  // Custom ALU Bit 3
#define PIN_RESULT_Y4       44  // 74LS181 Bit 4
#define PIN_RESULT_Y5       47  // 74LS181 Bit 5
#define PIN_RESULT_Y6       48  // 74LS181 Bit 6
#define PIN_RESULT_Y7       3   // 74LS181 Bit 7 (MSB)

// --- Telemetry & Status Flags (via Resistor Dividers) ---
#define PIN_FLAG_CARRY      26  // Carry Out (C8 in Compute / C4 in Verify)
#define PIN_FLAG_PASS       38  // 74LS85 Comparator Pin 6 (HIGH = PASS / MATCH)

// --- Monitored Control & Mode Pins ---
#define PIN_MODE_SELECT     39  // Mode Switch (0 = 8-Bit Compute, 1 = 4-Bit Verify)
#define PIN_OPCODE_S0       10  // Opcode Bit 0
#define PIN_OPCODE_S1       11  // Opcode Bit 1
#define PIN_OPCODE_S2       12  // Opcode Bit 2

// --- Monitored Inputs (Operands A & B) ---
#define PIN_OPERAND_A0      1
#define PIN_OPERAND_A1      2
#define PIN_OPERAND_A2      4
#define PIN_OPERAND_A3      5

#define PIN_OPERAND_B0      6
#define PIN_OPERAND_B1      7
#define PIN_OPERAND_B2      8
#define PIN_OPERAND_B3      9
