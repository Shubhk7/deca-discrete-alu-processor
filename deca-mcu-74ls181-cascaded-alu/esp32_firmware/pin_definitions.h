#pragma once
#include <Arduino.h>

/*
 * =====================================================================================
 * DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor
 * ESP32-S3 N16R8 Pin Definitions — Telemetry Sensing & Remote Input Injection
 * =====================================================================================
 * Pin Mapping Strategy:
 *  - 19x Contiguous GPIOs for driving/monitoring inputs via 4.7kΩ series override resistors.
 *  - 9x GPIOs for sensing 5V bus lines via 1.0kΩ / 2.2kΩ voltage dividers (Safe 3.43V).
 *  - Strapping pins (GPIO 0, 45, 46) completely avoided.
 * =====================================================================================
 */

// =====================================================================================
// 1. HYBRID REMOTE INJECTION GPIO PINS (Connected via 19x 4.7kΩ Override Resistors)
// =====================================================================================
// In "Manual Mode" (DIP switches active): Set as INPUT_PULLDOWN to monitor physical switches.
// In "Remote Override Mode": Set as OUTPUT to drive logical values over the 10kΩ pull-downs.

// Operand A Lower Nibble (A3..A0) -> Feeds 74LS181N Pins 19, 21, 23, 2
const uint8_t PIN_INJ_A0 = 1;
const uint8_t PIN_INJ_A1 = 2;
const uint8_t PIN_INJ_A2 = 4;
const uint8_t PIN_INJ_A3 = 5;

// Operand A Upper Nibble (A7..A4) -> Feeds Arduino UNO R4 Pins D10, D9, D8, D7
const uint8_t PIN_INJ_A4 = 6;
const uint8_t PIN_INJ_A5 = 7;
const uint8_t PIN_INJ_A6 = 8;
const uint8_t PIN_INJ_A7 = 9;

// Operand B Lower Nibble (B3..B0) -> Feeds 74LS181N Pins 18, 20, 22, 1
const uint8_t PIN_INJ_B0 = 10;
const uint8_t PIN_INJ_B1 = 11;
const uint8_t PIN_INJ_B2 = 12;
const uint8_t PIN_INJ_B3 = 13;

// Operand B Upper Nibble (B7..B4) -> Feeds Arduino UNO R4 Pins A0, D13, D12, D11
const uint8_t PIN_INJ_B4 = 14;
const uint8_t PIN_INJ_B5 = 15;
const uint8_t PIN_INJ_B6 = 16;
const uint8_t PIN_INJ_B7 = 17;

// Opcode S[2:0] & Mode M -> Feeds both 74LS181N and Arduino R4
const uint8_t PIN_INJ_S0 = 18;  // Opcode bit 0 (SUB in arithmetic mode)
const uint8_t PIN_INJ_S1 = 35;  // Opcode bit 1
const uint8_t PIN_INJ_S2 = 36;  // Opcode bit 2
const uint8_t PIN_INJ_MODE = 37;// Mode Select (0 = Arithmetic, 1 = Logic)

// =====================================================================================
// 2. BUS TELEMETRY SENSE GPIO PINS (Connected via 9x 1.0kΩ / 2.2kΩ Voltage Dividers)
// =====================================================================================
// Voltage divider transfers 5V TTL levels down to 3.43V CMOS-safe logic:
// V_ESP32 = 5.0V * (2.2k / (1.0k + 2.2k)) = 3.4375V

// Lower Nibble Y[3:0] (Monitors 74LS181N outputs via divider)
const uint8_t PIN_SENSE_Y0 = 38;
const uint8_t PIN_SENSE_Y1 = 39;
const uint8_t PIN_SENSE_Y2 = 40;
const uint8_t PIN_SENSE_Y3 = 41;

// Upper Nibble Y[7:4] (Monitors Arduino R4 outputs via divider)
const uint8_t PIN_SENSE_Y4 = 42;
const uint8_t PIN_SENSE_Y5 = 47;
const uint8_t PIN_SENSE_Y6 = 48;
const uint8_t PIN_SENSE_Y7 = 21;

// Final 8-Bit Carry-Out C8 Telemetry (Monitored via divider)
const uint8_t PIN_SENSE_C8 = 3;

// =====================================================================================
// 3. POWER ISOLATION RULE
// =====================================================================================
// ESP32-S3 5V/VIN pin must remain DISCONNECTED from the breadboard +5V rail.
// ESP32-S3 GND pin MUST be tied to the breadboard Common Star Ground hub.
