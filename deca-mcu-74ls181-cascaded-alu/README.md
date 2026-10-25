# DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor
### *Silicon 74LS181N Bipolar ALU × 32-Bit ARM MCU Cascaded Computing Architecture*

[![Architecture](https://img.shields.io/badge/Architecture-Hybrid%20Silicon%20%2B%20MCU%20Cascade-orange.svg)]()
[![Hardware Silicon](https://img.shields.io/badge/Hardware-74LS181N%20DIP--24%20ALU-blue.svg)]()
[![MCU Engine](https://img.shields.io/badge/MCU-Arduino%20UNO%20R4%20WiFi%20(RA4M1)-red.svg)]()
[![Dashboard](https://img.shields.io/badge/Dashboard-ESP32--S3%20WebSockets%20HUD-green.svg)]()
[![Display](https://img.shields.io/badge/Display-12x8%20LED%20Matrix%20(No%20Discrete%20LEDs)-critical.svg)]()
[![Course](https://img.shields.io/badge/Course-DECA%20Minor%20Project-purple.svg)]()

---

## 1. Executive Summary

The **DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU** is an alternative, streamlined hardware-software architecture for the DECA (Digital Electronics & Computer Architecture) project. Rather than requiring 10 discrete TTL/HCT logic chips (adders, XORs, ANDs, ORs, inverters, and 8:1 MUXes) for the lower nibble, this design leverages the vintage **74LS181N 4-bit bipolar silicon ALU** cascaded with a **high-speed 32-bit ARM Cortex-M4 microcontroller (Arduino UNO R4 WiFi)** to form a complete, real-time **8-bit Arithmetic Logic Unit**.

Furthermore, addressing builds that lack discrete indicator LEDs, this architecture eliminates all loose LEDs and routes all telemetry, 8-bit output bus states ($Y_7..Y_0$), intermediate carry ($C_4$), and final carry ($C_8$) directly to the Arduino UNO R4 WiFi's onboard **12×8 LED Matrix HUD** and the ESP32-S3's **real-time WebSocket web dashboard**.

```
========================================================================================================
                              SYSTEM BLOCK DIAGRAM (8-BIT HYBRID CASCADE)
========================================================================================================

  +-----------------------+     +-----------------------+     +-----------------------+
  |  DIP SW1: Operand A   |     |  DIP SW2: Operand B   |     | DIP SW3: Opcode/Mode  |
  |  A[7:0] (8-Pos Switch)|     |  B[7:0] (8-Pos Switch)|     | S[2:0], M (4-Pos SW)  |
  +-----------+-----------+     +-----------+-----------+     +-----------+-----------+
              |                             |                             |
              | 20x 10k Pull-Downs          | 20x 10k Pull-Downs          | 20x 10k Pull-Downs
              v                             v                             v
  ======================================================================================
     BREADBOARD MASTER INPUT BUSES:  A[7:0]  |  B[7:0]  |  S[2:0] (Opcode)  |  MODE
  ======================================================================================
        ^                               ^                             ^
        | 19x 4.7k Override Resistors   |                             |
  +-----+-------------------------------+-----------------------------+-----------------+
  |                  ESP32-S3 N16R8 BRAIN & WEB DASHBOARD INJECTOR                   |
  |  SoftAP "DECA-ALU-HYBRID" (192.168.4.1) | WebSocket 100ms Telemetry | Remote Sliders|
  +-----------------------------------------------------------------------------------+
        |
        | Lower Nibble: A[3:0], B[3:0], S[2:0], M
        v
  +---------------------------------------------------+
  |      IC1: 74LS181N 4-BIT SILICON ALU (DIP-24)     |
  |  - Computes Lower Nibble: Y[3:0] in ~15 ns        |
  |  - Pin 16: Active-LOW Carry-Out (/Cn+4)           |
  +-------------------------+-------------------------+
                            |
                            | Hardware Carry Cascade:
                            | IC1 Pin 16 (/Cn+4) ---> Arduino Pin D6 (C4 In)
                            v
  +-----------------------------------------------------------------------------------+
  |               MCU2: ARDUINO UNO R4 WIFI (ARM Cortex-M4 @ 48 MHz)                  |
  |  - High-Speed 4-Bit Software ALU Engine (Computes Upper Nibble Y[7:4])            |
  |  - Ingests Hardware Carry C4 from 74LS181N Pin 16 in Real Time                    |
  |  - Assembles Complete 8-Bit Word: Y[7:0] = { Y_MCU[7:4], Y_181[3:0] }            |
  |  - Evaluates Golden Model Verification & Computes Final Carry-Out C8               |
  |  - DYNAMIC VISUAL HUD: Onboard 12x8 LED Matrix displays bit states & scrolling HUD|
  +-----------------------------------------------------------------------------------+
        |
        | Output Bus Y[7:0] (via 9x 1.0k / 2.2k dividers)
        v
  +-----------------------------------------------------------------------------------+
  |               ESP32-S3 LIVE WEB DASHBOARD & BIT-LEVEL VISUALIZER                  |
  =====================================================================================
```

---

## 2. Key Architectural Features

1. **True Hybrid Silicon + MCU Cascading**:
   * **Nibble 0 (Bits 3..0)**: Executed physically in pure bipolar TTL silicon by the classic **74LS181N** arithmetic logic unit in $< 15\text{ ns}$.
   * **Nibble 1 (Bits 7..4)**: Executed concurrently by the **Arduino UNO R4 WiFi's 32-bit Renesas RA4M1 ARM processor**.
   * **Physical Carry Interconnect**: The 74LS181's hardware Carry-Out ($\overline{C}_{n+4}$, Pin 16) physically connects to digital pin `D6` of the Arduino UNO R4, propagating arithmetic carries across the 4-bit nibble boundary in real time.
2. **Built-in 12×8 LED Matrix Virtual Display**:
   * Eliminates the need for discrete LEDs, current-limiting resistors, and extensive breadboard LED wiring.
   * Real-time 8-bit binary dot visualization.
   * Dynamic mathematical opcode symbols (`+`, `-`, `&`, `|`, `^`, `~`, `<<`, `>>`).
   * Carry status indicators ($C_{in}, C_4, C_8$).
   * Full text scrolling for decimal results and PASS/FAIL verification.
3. **Drastically Simplified Breadboard Wiring**:
   * Requires only **one single DIP IC** (74LS181N) on the breadboard instead of twelve ICs.
   * Zero loose breadboard wire clutter.
   * Virtually zero risk of loose contact or gate routing errors.
4. **Tri-Board Heterogeneous Processing & Clean Power Distribution**:
   * **Arduino UNO R4 Minima**: Serves as the dedicated, clean $+5\text{V} / 1.2\text{A}$ USB-C power station for the 74LS181N and DIP switch pull-downs.
   * **Arduino UNO R4 WiFi**: Native $5\text{V}$ I/O logic, executes the upper 4-bit ALU engine, reads 74LS181N outputs directly, and drives the onboard 12×8 matrix.
   * **ESP32-S3 N16R8**: Hosts the Wi-Fi AP (`DECA-ALU-HYBRID`), serves the embedded web dashboard, injects operands via $4.7\text{ k}\Omega$ override resistors, and samples bus telemetry.

---

## 3. Bill of Materials (BOM)

| Item | Component | Designation | Quantity | Purpose |
| :--- | :--- | :--- | :---: | :--- |
| 1 | **74LS181N** | IC1 | 1 | 4-Bit Silicon Arithmetic Logic Unit (DIP-24) |
| 2 | **Arduino UNO R4 WiFi** | MCU2 | 1 | 5V Upper 4-Bit ALU Engine & 12×8 LED Matrix HUD |
| 3 | **ESP32-S3 N16R8** | MCU3 | 1 | Wi-Fi AP, WebSocket Telemetry & Remote DIP Override |
| 4 | **Arduino UNO R4 Minima** | MCU1 | 1 | Dedicated Clean +5V / 1.2A USB-C Power Station |
| 5 | **8-Position DIP Switch** | SW1, SW2 | 2 | Operand A ($A_0..A_7$) and Operand B ($B_0..B_7$) |
| 6 | **4-Position DIP Switch** | SW3 | 1 | Opcode ($S_0, S_1, S_2$) and Mode ($M$) |
| 7 | **$10\text{ k}\Omega$ Resistors** | R_pull | 20 | Pull-down resistors for DIP switch inputs |
| 8 | **$4.7\text{ k}\Omega$ Resistors** | R_over | 19 | Series override resistors for ESP32 injection |
| 9 | **$1.0\text{ k}\Omega$ Resistors** | R_div1 | 9 | 5V-to-3.3V voltage divider series resistors |
| 10 | **$2.2\text{ k}\Omega$ Resistors** | R_div2 | 9 | 5V-to-3.3V voltage divider shunt resistors |
| 11 | **$0.1\ \mu\text{F}$ Ceramic Cap** | C_bypass | 2 | Decoupling caps (104) across IC1 and power rail |
| 12 | **$100\ \mu\text{F}$ Electrolytic Cap**| C_bulk | 1 | Bulk filter capacitor across +5V master rail |
| 13 | **Solderless Breadboard** | MB-102 | 1 | Standard 830-tie-point solderless breadboard |
| 14 | **Jumper Wires** | Wires | 1 pack | Male-to-Male and Male-to-Female jumper wires |

> [!NOTE]
> **No Discrete LEDs Required**: All status indicators, bus values, and carry flags are rendered on the Arduino UNO R4 WiFi's onboard $12\times 8$ matrix.

---

## 4. Operation Truth Table

The processor supports 8 standard arithmetic and logic operations selected via Opcode $S_2, S_1, S_0$:

| Opcode ($S_2 S_1 S_0$) | Mode ($M$) | Operation Name | Mathematical Definition | 74LS181 Function ($S_3..S_0, M$) |
| :---: | :---: | :--- | :--- | :--- |
| `000` | `0` (Arith) | **ADD** | $Y = A + B$ | $S = 1001\ (9),\ M = 0,\ \overline{C}_n = 1$ |
| `001` | `0` (Arith) | **SUB** | $Y = A - B$ | $S = 0110\ (6),\ M = 0,\ \overline{C}_n = 0$ |
| `010` | `1` (Logic) | **AND** | $Y = A \ \& \ B$ | $S = 1011\ (11),\ M = 1$ |
| `011` | `1` (Logic) | **OR**  | $Y = A \mid B$ | $S = 1110\ (14),\ M = 1$ |
| `100` | `1` (Logic) | **XOR** | $Y = A \oplus B$ | $S = 0110\ (6),\ M = 1$ |
| `101` | `1` (Logic) | **NOT** | $Y = \sim A$ | $S = 0000\ (0),\ M = 1$ |
| `110` | `1` (Logic) | **SHL** | $Y = A \ll 1$ | Hardwired / MCU Bit Shift ($A_{i-1} \to Y_i$) |
| `111` | `1` (Logic) | **SHR** | $Y = A \gg 1$ | Hardwired / MCU Bit Shift ($A_{i+1} \to Y_i$) |

---

## 5. Arduino UNO R4 12×8 LED Matrix HUD

The onboard 96-pixel matrix functions as a dynamic dashboard:

```
    Col: 0  1   2   3  4  5  6   7   8  9 10 11
  Row 0 [C][4] [ ] [  OPCODE ]  [ ] [STATUS HUD]
  Row 1 [ ][ ] [ ] [  SYMBOL ]  [ ] [          ]
  Row 2 [ ][ ] [ ] [ ][ ][ ][ ] [ ] [ ][ ][ ][ ]
  Row 3 [C][8] [ ] [ Y7 Y6 Y5 Y4] [ ] [ Y3 Y2 Y1 Y0]  <-- 8-BIT OUTPUT BUS
  Row 4 [ ][ ] [ ] [ UPPER ALU ] [ ] [ LOWER ALU ]
  Row 5 [ ][ ] [ ] [ (MCU R4)  ] [ ] [ (74LS181) ]
  Row 6 [ ][ ] [ ] [ ][ ][ ][ ] [ ] [ ][ ][ ][ ]
  Row 7 [P][A][S][S]  or  [F][A][I][L]  STATUS
```

* **Columns 8–11**: Display the 4 bits of the Lower Nibble ($Y_3..Y_0$) computed by the 74LS181N silicon ALU.
* **Columns 3–6**: Display the 4 bits of the Upper Nibble ($Y_7..Y_4$) computed by the Arduino R4 ARM MCU.
* **Column 0**: Displays Carry status ($C_{in}, C_4\text{ cascade}, C_8\text{ out}$).
* **Row 0–1**: Visualizes the active opcode symbol (`+`, `-`, `&`, `|`, etc.).
* **Automatic Text Scrolling**: Whenever the result changes, the matrix scrolls `Y: 0xXX (= N)` across the screen.

---

## 6. Directory Structure

```
deca-mcu-74ls181-cascaded-alu/
├── README.md                                  # Master Architecture & Overview
├── hardware_docs/
│   ├── PIN_TO_PIN_WIRING_GUIDE.md             # Complete Pin-by-Pin Wiring Guide
│   ├── CIRCUIT_DIAGRAM.md                     # Full Schematic & Netlist
│   ├── BREADBOARD_ASSEMBLY_GUIDE.md           # Step-by-Step Breadboard Instructions
│   ├── PINOUTS_AND_SCHEMATICS.md              # ASCII Pinout Diagrams & Power Isolation
│   └── Circuit_Diag.py                        # Complete Schemdraw Vector Generator (.svg)
├── arduino_r4_firmware/
│   └── arduino_r4_cascaded_alu.ino            # 4-Bit ALU Engine + 12x8 LED Matrix HUD
├── esp32_firmware/
│   ├── esp32_firmware.ino                     # Wi-Fi AP + WebSocket Telemetry & Injection
│   ├── pin_definitions.h                      # ESP32-S3 GPIO Mapping
│   └── web_page.h                             # Embedded Responsive Web Dashboard
└── project_report/
    └── DECA_HYBRID_ALU_PROJECT_REPORT.md      # Academic University Project Report
```

---

## 7. Quick Start Guide

1. **Breadboard Assembly**:
   * Insert the **74LS181N** (DIP-24) across the breadboard center trough.
   * Wire Pin 24 to $+5\text{V}$ and Pin 12 to $\text{GND}$. Add a $0.1\ \mu\text{F}$ bypass capacitor directly between Pins 24 and 12.
   * Place DIP switches SW1 (Operand A), SW2 (Operand B), and SW3 (Opcode/Mode) with $10\text{ k}\Omega$ pull-down resistors to GND.
2. **Connect the Microcontrollers**:
   * Connect **Arduino UNO R4 Minima** Pin 5V to the Breadboard $+5\text{V}$ rail, and Pin GND to Common GND.
   * Connect **Arduino UNO R4 WiFi** Pin D6 to 74LS181N Pin 16 (Carry cascade), Pins D2..D5 to 74LS181N Pins 9, 10, 11, 13 ($Y_0..Y_3$), and Pins D7..A4 to input buses.
   * Connect **ESP32-S3** GPIOs to input buses via $4.7\text{ k}\Omega$ resistors and telemetry pins via $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage dividers.
   * **DO NOT** connect the 5V pins of the WiFi R4 or ESP32-S3 to the breadboard!
3. **Flash Firmware**:
   * Flash `arduino_r4_firmware/arduino_r4_cascaded_alu.ino` to the Arduino UNO R4 WiFi via Arduino IDE.
   * Flash `esp32_firmware/esp32_firmware.ino` to the ESP32-S3.
4. **Operate & Test**:
   * Power all boards via their independent USB-C ports.
   * Watch the onboard $12\times 8$ matrix light up immediately with the ALU scoreboard.
   * Connect your phone or laptop to Wi-Fi AP `DECA-ALU-HYBRID` (password: `decaalu2024`) and browse to `http://192.168.4.1` for the live telemetry dashboard.
