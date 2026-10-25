# Circuit Diagram & Master Netlist
## DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU (74LS181N × Arduino R4 WiFi)

---

## 1. Complete Architectural Schematic (ASCII Art)

```
========================================================================================================================
                      DECA DUAL 4-BIT CASCADED 8-BIT HYBRID ALU PROCESSOR SCHEMATIC
========================================================================================================================

+-------------------------------------+
| MCU1: ARDUINO UNO R4 MINIMA         |
| Dedicated +5V / 1.2A Power Station  |
|                                     |
| [USB-C 5V In]                       |
|   Pin 5V  --------------------------+----------> BREADBOARD +5V MASTER RAIL (Powers 74LS181N & DIP switches)
|   Pin GND --------------------------+----+-----> BREADBOARD COMMON STAR GROUND (0V Reference Hub)
+-------------------------------------+    |
                                           |
+-------------------------------------+    |
| HYBRID INPUT SWITCH STAGES          |    |
| SW1 (A0..A7) 8-Pos DIP Switch       |    |
| SW2 (B0..B7) 8-Pos DIP Switch       |    |
| SW3 (S0..S2, M) 4-Pos DIP Switch    |    |
|                                     |    |
| (+5V Rail) ---[ DIP Switch ]--------+----+-----> MASTER INPUT NODES (A0..A7, B0..B7, S0..S2, M)
|                                     |
|                                    [ 20x 10k Pull-Downs ]
|                                     |
|                                    (GND)
+-------------------------------------+
        ^
        | 19x 4.7k Override Resistors
+-------+-----------------------------+
| MCU3: ESP32-S3 N16R8                |
| SoftAP "DECA-ALU-HYBRID"            |
| WebSocket Telemetry & Injector      |
|                                     |
| [USB-C 5V In (Independent)]         |
|   Pin GND --------------------------+
|   GPIO 1,2,4,5   ---> [4.7k] -------> A[3:0] (Lower Nibble)
|   GPIO 6,7,8,9   ---> [4.7k] -------> A[7:4] (Upper Nibble)
|   GPIO 10..13    ---> [4.7k] -------> B[3:0] (Lower Nibble)
|   GPIO 14..17    ---> [4.7k] -------> B[7:4] (Upper Nibble)
|   GPIO 18,35,36  ---> [4.7k] -------> S[2:0] (Opcode)
|   GPIO 37        ---> [4.7k] -------> M (Mode Select)
|                                     |
|   GPIO 38..41    <--- [1k / 2.2k] <-- Y[3:0] (74LS181 Lower Output Telemetry)
|   GPIO 42,47,48,21 <- [1k / 2.2k] <-- Y[7:4] (Arduino Upper Output Telemetry)
|   GPIO 3         <--- [1k / 2.2k] <-- C8 (Final Carry-Out Telemetry)
+-------------------------------------+
        |
        | Lower Inputs: A[3:0], B[3:0], S[2:0], M
        v
+-------------------------------------------------------------+
| IC1: 74LS181N 4-BIT SILICON ALU (DIP-24)                    |
| Real-time Bipolar Arithmetic Logic Unit (< 15ns)            |
|                                                             |
|   Pin 24: VCC (+5V Master Rail) | Pin 12: GND (Star Ground) |
|   Pins 2,23,21,19: /A[3:0] <--- Bus A[3:0]                  |
|   Pins 1,22,20,18: /B[3:0] <--- Bus B[3:0]                  |
|   Pins 6,5,4,3:    S[0:3]  <--- Bus S[2:0] (S3 tied/mapped) |
|   Pin 8:           M       <--- Bus M                       |
|   Pin 7:           /Cn     <--- Tie +5V (No Carry-In)       |
|                                                             |
|   Pins 9,10,11,13: /F[3:0] ---> Lower Nibble Result Y[3:0]  |
|                                 (To Arduino D2..D5 & ESP32) |
|                                                             |
|   Pin 16:          /Cn+4   ---> HARDWARE CARRY CASCADE C4   |
+---------------------------------------------+---------------+
                                              |
                                              | Hardware Wire Link
                                              | 74LS181 Pin 16 (/Cn+4) ---> Arduino Pin D6
                                              v
+-------------------------------------------------------------+
| MCU2: ARDUINO UNO R4 WIFI (Renesas RA4M1 32-Bit ARM @ 48MHz)|
| 5V Native Upper 4-Bit ALU Engine & 12x8 Matrix HUD          |
|                                                             |
| [USB-C 5V In (Independent)]                                 |
|   Pin GND  -------------------------> Common Star Ground    |
|   Pin 5V   -------------------------> LEFT UNCONNECTED!     |
|                                                             |
|   Pins D2..D5: Ingest Y[3:0] from 74LS181N Pins 9,10,11,13  |
|   Pin D6:      Ingest Hardware Carry C4 from 74LS181 Pin 16 |
|   Pins D7..D10:Ingest Upper Bus A[7:4]                      |
|   Pins D11..A0:Ingest Upper Bus B[7:4]                      |
|   Pins A1..A3: Ingest Opcode S[2:0]                         |
|   Pin A4:      Ingest Mode M                                |
|                                                             |
|   ALU Engine:  Computes Y[7:4] = f(A[7:4], B[7:4], C4)      |
|                Assembles Y[7:0] = { Y[7:4], Y[3:0] }        |
|                Evaluates Golden Model Verification          |
|                                                             |
|   ONBOARD 12x8 LED MATRIX HUD:                              |
|   - Col 8..11: Lower 4 bits Y[3:0] (74LS181 Silicon Dots)   |
|   - Col 7:     Hardware/Software Nibble Divider Dot Line    |
|   - Col 3..6:  Upper 4 bits Y[7:4] (Arduino MCU Dots)       |
|   - Col 0..1:  Carry Chain Dots (C0 in, C4 cascade, C8 out) |
|   - Row 0..1:  Mathematical Opcode Symbol (+, -, &, |, etc)|
|   - Row 7:     Hardware Equivalence Match Dot / Flash       |
+-------------------------------------------------------------+
```

---

## 2. Complete System Netlist

| Net Name | Signal Type | Source Pin | Destination Pins | Voltage Level | Notes |
| :--- | :--- | :--- | :--- | :---: | :--- |
| **+5V_MASTER** | Power | Arduino Minima Pin 5V | 74LS181 Pin 24, SW1..3 Common, Bulk Caps | +5.0V | Main logic supply |
| **COMMON_GND** | Ground | Minima GND | WiFi R4 GND, ESP32 GND, 74LS181:12, Pull-downs | 0.0V | Star ground hub |
| **NET_A0** | Input | SW1 Pin 1 / ESP32 G1 | 74LS181 Pin 2 ($\overline{A}_0$) | 5V TTL | Operand A bit 0 |
| **NET_A1** | Input | SW1 Pin 2 / ESP32 G2 | 74LS181 Pin 23 ($\overline{A}_1$) | 5V TTL | Operand A bit 1 |
| **NET_A2** | Input | SW1 Pin 3 / ESP32 G4 | 74LS181 Pin 21 ($\overline{A}_2$) | 5V TTL | Operand A bit 2 |
| **NET_A3** | Input | SW1 Pin 4 / ESP32 G5 | 74LS181 Pin 19 ($\overline{A}_3$) | 5V TTL | Operand A bit 3 |
| **NET_A4** | Input | SW1 Pin 5 / ESP32 G6 | Arduino R4 Pin D7 | 5V TTL | Operand A bit 4 |
| **NET_A5** | Input | SW1 Pin 6 / ESP32 G7 | Arduino R4 Pin D8 | 5V TTL | Operand A bit 5 |
| **NET_A6** | Input | SW1 Pin 7 / ESP32 G8 | Arduino R4 Pin D9 | 5V TTL | Operand A bit 6 |
| **NET_A7** | Input | SW1 Pin 8 / ESP32 G9 | Arduino R4 Pin D10 | 5V TTL | Operand A bit 7 |
| **NET_B0** | Input | SW2 Pin 1 / ESP32 G10 | 74LS181 Pin 1 ($\overline{B}_0$) | 5V TTL | Operand B bit 0 |
| **NET_B1** | Input | SW2 Pin 2 / ESP32 G11 | 74LS181 Pin 22 ($\overline{B}_1$) | 5V TTL | Operand B bit 1 |
| **NET_B2** | Input | SW2 Pin 3 / ESP32 G12 | 74LS181 Pin 20 ($\overline{B}_2$) | 5V TTL | Operand B bit 2 |
| **NET_B3** | Input | SW2 Pin 4 / ESP32 G13 | 74LS181 Pin 18 ($\overline{B}_3$) | 5V TTL | Operand B bit 3 |
| **NET_B4** | Input | SW2 Pin 5 / ESP32 G14 | Arduino R4 Pin D11 | 5V TTL | Operand B bit 4 |
| **NET_B5** | Input | SW2 Pin 6 / ESP32 G15 | Arduino R4 Pin D12 | 5V TTL | Operand B bit 5 |
| **NET_B6** | Input | SW2 Pin 7 / ESP32 G16 | Arduino R4 Pin D13 | 5V TTL | Operand B bit 6 |
| **NET_B7** | Input | SW2 Pin 8 / ESP32 G17 | Arduino R4 Pin A0 (D14) | 5V TTL | Operand B bit 7 |
| **NET_S0** | Control | SW3 Pin 1 / ESP32 G18 | 74LS181 Pin 6, Arduino R4 Pin A1 | 5V TTL | Opcode bit 0 |
| **NET_S1** | Control | SW3 Pin 2 / ESP32 G35 | 74LS181 Pin 5, Arduino R4 Pin A2 | 5V TTL | Opcode bit 1 |
| **NET_S2** | Control | SW3 Pin 3 / ESP32 G36 | 74LS181 Pin 4, Arduino R4 Pin A3 | 5V TTL | Opcode bit 2 |
| **NET_MODE** | Control | SW3 Pin 4 / ESP32 G37 | 74LS181 Pin 8, Arduino R4 Pin A4 | 5V TTL | Mode (0=Arith, 1=Logic) |
| **NET_C4_CASCADE**| **Carry** | **74LS181 Pin 16 (/Cn+4)**| **Arduino R4 Pin D6** | **5V TTL** | **Hardware Carry Link** |
| **NET_Y0** | Output | 74LS181 Pin 9 ($\overline{F}_0$)| Arduino R4 Pin D2, Divider 1 $\to$ ESP32 G38| 5V TTL | ALU Result bit 0 |
| **NET_Y1** | Output | 74LS181 Pin 10 ($\overline{F}_1$)| Arduino R4 Pin D3, Divider 2 $\to$ ESP32 G39| 5V TTL | ALU Result bit 1 |
| **NET_Y2** | Output | 74LS181 Pin 11 ($\overline{F}_2$)| Arduino R4 Pin D4, Divider 3 $\to$ ESP32 G40| 5V TTL | ALU Result bit 2 |
| **NET_Y3** | Output | 74LS181 Pin 13 ($\overline{F}_3$)| Arduino R4 Pin D5, Divider 4 $\to$ ESP32 G41| 5V TTL | ALU Result bit 3 |
| **NET_Y4** | Output | Arduino R4 Software Upper ALU | Divider 5 $\to$ ESP32 G42, 12x8 Matrix HUD | 5V / Virtual | ALU Result bit 4 |
| **NET_Y5** | Output | Arduino R4 Software Upper ALU | Divider 6 $\to$ ESP32 G47, 12x8 Matrix HUD | 5V / Virtual | ALU Result bit 5 |
| **NET_Y6** | Output | Arduino R4 Software Upper ALU | Divider 7 $\to$ ESP32 G48, 12x8 Matrix HUD | 5V / Virtual | ALU Result bit 6 |
| **NET_Y7** | Output | Arduino R4 Software Upper ALU | Divider 8 $\to$ ESP32 G21, 12x8 Matrix HUD | 5V / Virtual | ALU Result bit 7 |
| **NET_C8** | Output | Arduino R4 Software Upper ALU | Divider 9 $\to$ ESP32 G3, 12x8 Matrix HUD | 5V / Virtual | Final 8-Bit Carry-Out |
