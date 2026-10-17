#pragma once

// ============================================================================
// ESP32-S3 N16R8 Pin Definitions for 4-Bit / 8-Bit ALU Hardware Testbench
// Note: GPIO 33-37 are reserved for Octal Flash / PSRAM on N16R8 modules.
//       GPIO 19, 20 are used for Native USB D-/D+.
// ============================================================================

// --- Inputs from ALU Outputs (5V -> 3.3V via Resistor Dividers: 1k / 2k) ---
#define PIN_RESULT_Y0       40  // ALU Output Bit 0 (LSB)
#define PIN_RESULT_Y1       41  // ALU Output Bit 1
#define PIN_RESULT_Y2       42  // ALU Output Bit 2
#define PIN_RESULT_Y3       43  // ALU Output Bit 3 (MSB)

#define PIN_FLAG_CARRY      26  // Adder Carry Out (Pin 9 of 74LS283)
#define PIN_FLAG_MATCH      38  // 74LS85 Comparator Match Pin (Optional)

// --- Monitored or Driven Inputs (A, B, and Opcode) ---
#define PIN_OPERAND_A0      1   // Operand A Bit 0
#define PIN_OPERAND_A1      2   // Operand A Bit 1
#define PIN_OPERAND_A2      4   // Operand A Bit 2
#define PIN_OPERAND_A3      5   // Operand A Bit 3

#define PIN_OPERAND_B0      6   // Operand B Bit 0
#define PIN_OPERAND_B1      7   // Operand B Bit 1
#define PIN_OPERAND_B2      8   // Operand B Bit 2
#define PIN_OPERAND_B3      9   // Operand B Bit 3

#define PIN_OPCODE_S0       10  // Opcode Bit 0
#define PIN_OPCODE_S1       11  // Opcode Bit 1
#define PIN_OPCODE_S2       12  // Opcode Bit 2

// Operating Mode (0 = Passive Monitor Mode, 1 = Active Injection / Testbench Mode)
extern bool activeInjectionMode;
