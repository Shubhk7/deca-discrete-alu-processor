# ⚡ Dual-Mode Discrete & Silicon ALU Processor with Dual-Layer Verification & IoT Dashboard

[![DECA Minor Project](https://img.shields.io/badge/Course-DECA%20Minor%20Project-blue.svg?style=for-the-badge)](https://github.com/)
[![Architecture](https://img.shields.io/badge/Architecture-Hybrid%20Discrete%20%2B%2074LS181-orange.svg?style=for-the-badge)](https://github.com/)
[![Verifier](https://img.shields.io/badge/Verifier-Arduino%20UNO%20R4%20WiFi-teal.svg?style=for-the-badge)](https://github.com/)
[![Power Station](https://img.shields.io/badge/Power%20Station-Arduino%20UNO%20R4%20Minima-blueviolet.svg?style=for-the-badge)](https://github.com/)
[![Dashboard](https://img.shields.io/badge/Dashboard-ESP32--S3%20N16R8-red.svg?style=for-the-badge)](https://github.com/)
[![Hardware Verification](https://img.shields.io/badge/Hardware%20Equivalence-74HCT85%20Comparator-brightgreen.svg?style=for-the-badge)](https://github.com/)
[![Software Verification](https://img.shields.io/badge/Software%20HIL-Golden%20Model%20RA4M1-green.svg?style=for-the-badge)](https://github.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

**Course**: Digital Electronics & Computer Architecture (DECA) Minor Project  
**Author**: Shubh Kashyap  
**Institution**: Department of Electronics & Computer Engineering  

---

## 📖 Executive Summary

This project presents the design, physical breadboard implementation, and dual-layer verification of an advanced **Dual-Mode Hybrid Arithmetic Logic Unit (ALU)**. Combining classical gate-level logic design with historical integrated silicon, the system bridges fundamental digital computer architecture with modern embedded instrumentation:

1. **Dual-Mode Operation**:
   - **Mode 0 (8-Bit Cascaded ALU)**: A 4-bit Custom Discrete ALU (constructed from a `74LS83N` binary full adder, `74HC86N/08/32/04` logic gates, and four `74HCT151` 8:1 multiplexers) serves as the lower nibble ($Y[3:0]$) and cascades its carry-out ($C_4$) into a vintage `74LS181N` 4-bit ALU serving as the upper nibble ($Y[7:4]$), realizing an authentic **8-bit parallel processor slice**.
   - **Mode 1 (Real-Time Silicon Verification)**: Both the Custom Discrete ALU and the historical `74LS181N` ALU compute simultaneously on identical 4-bit input vectors ($A[3:0], B[3:0]$). A dedicated **`74HCT85` 4-bit Magnitude Comparator** performs nanosecond-level physical hardware equivalence checking ($A=B$), verifying the custom gate architecture directly against the industry-standard silicon benchmark.
2. **Heterogeneous 3-Board Infrastructure**:
   - **ESP32-S3 N16R8**: Hosts an autonomous Wi-Fi Access Point and WebSocket server, running a responsive real-time web telemetry dashboard and driving a non-destructive **hybrid input injection stage** (tactile DIP switches paired with $4.7\text{ k}\Omega$ GPIO override resistors).
   - **Arduino UNO R4 WiFi**: Functions as a native 5V **Software Golden Model HIL Oracle** (32-bit ARM Cortex-M4 @ 48 MHz) that samples the physical ALU output bus, executes an algorithmic C++ reference model, renders real-time PASS/FAIL diagnostics and scoreboard animations on its onboard **12×8 LED Matrix HUD**, and drives external status LEDs.
   - **Arduino UNO R4 Minima**: Deployed as a **Dedicated Clean 5V/1A USB-C Power Station**, providing isolated, regulated power rails and decoupling for the high-density TTL/CMOS breadboard array and power-hungry bipolar `74LS181N`.

---

## 🏛️ Master System Architecture Diagram

```
 ============================================================================================================
                                     3-BOARD HETEROGENEOUS CONTROL & POWER
 ============================================================================================================
  [ Arduino UNO R4 Minima ]             [ ESP32-S3 N16R8 (Dual-Core LX7) ]        [ Arduino UNO R4 WiFi ]
   Clean 5V/1A USB-C Power Station       Wi-Fi AP ("ALU-Testbench-AP")             5V Software Golden Model Oracle
   - Dedicated Breadboard 5V Rail        - Responsive Web Dashboard Server         - Real-time Output Bus Sampling
   - High-current isolation              - WebSocket Telemetry Engine              - C++ Algorithmic Verification
   - Common Ground Reference             - Remote Input Injection (4.7k Override)  - Onboard 12x8 LED Matrix HUD
              |                                            |                       - Green/Red Hardware Flags
              | (+5V / GND)                                | (Inject GPIOs)                   ^
              v                                            v                                  | (Sample Y[7:0])
 ============================================================================================================
                                          HYBRID INPUT SELECTION STAGE
 ============================================================================================================
    [ Physical DIP Switches ] 
      - Operand A [7:0]   ----(10k Pull-downs)----+
      - Operand B [7:0]   ----(10k Pull-downs)----+
      - Opcode S [2:0]    ----(10k Pull-downs)----+
      - Mode Select (0/1) ----(10k Pull-downs)----+
                                                  |
                         ESP32 GPIOs -----[ 4.7k Override ]-----+
                                                                |
                                                                v
 ============================================================================================================
                                 DUAL-MODE ARITHMETIC LOGIC UNIT DATAPATH
 ============================================================================================================
                           |                                                    |
         [ Inputs: A[3:0], B[3:0], S[2:0] ]                  [ Inputs: A[7:4] / A[3:0], B[7:4] / B[3:0] ]
                           |                                                    |
                           v                                                    v
     +------------------------------------------+         +------------------------------------------+
     |     4-BIT CUSTOM DISCRETE ALU SLICE      |         |          74LS181N 4-BIT SILICON ALU      |
     |------------------------------------------|         |------------------------------------------|
     | - Adder/Subtractor: 74LS83N 4-Bit Adder  |         | - Industry-Standard Bipolar ALU IC       |
     | - B-Invert (SUB) & XOR: 74HC86N (Quad XOR)|         | - Active-Low / Active-High Dual Tables   |
     | - Bitwise AND: 74HCT08 (Quad AND)         |         | - Internal Lookahead Carry Generation    |
     | - Bitwise OR:  74HCT32 (Quad OR)          |         | - 16 Arithmetic + 16 Logic Functions     |
     | - Bitwise NOT: 74HCT04 (Hex Inverter)     |         | - Function Mapped via S[3:0], M, Cn      |
     | - Bit Shifters: SHL (<< 1), SHR (>> 1)   |         |                                          |
     | - Operation Multiplexers:                |         |                                          |
     |   4x 74HCT151 8:1 Multiplexers            |         |                                          |
     +------------------------------------------+         +------------------------------------------+
                 |                     |                                       |
          Custom Carry C4         Custom Result                          74LS181 Result
                 |                 Y_custom[3:0]                           Y_181[3:0]
                 |                     |                                       |
                 |                     +-----------------------+               |
                 v                                             v               v
  ============================================================================================================
                             MODE-CONTROLLED STEERING & HARDWARE VERIFICATION
  ============================================================================================================
                 |
                 +-------------------> [ MODE SWITCH ] <-------------------+
                                              |
                     +------------------------+------------------------+
                     | (MODE = 0: 8-Bit Cascaded ALU)                  | (MODE = 1: Real-time Silicon Equivalence)
                     v                                                 v
         [ Carry C4 Cascaded to 74LS181 ]                [ Identical Inputs: A[3:0], B[3:0] ]
         - C4 routed to 74LS181 carry-in                  - Both ALUs compute same operation concurrently
         - Combined 8-Bit Output Bus:                     - Outputs fed into Hardware Comparator:
           Y[7:0] = { Y_181[3:0], Y_custom[3:0] }                      |
                     |                                                 v
                     |                                   +------------------------------------+
                     |                                   |  74HCT85 4-Bit Magnitude Comparator |
                     |                                   |------------------------------------|
                     |                                   | Input A: Y_custom[3:0]             |
                     |                                   | Input B: Y_181[3:0]                |
                     |                                   | Output:  A = B Pin (Pin 6)         |
                     |                                   +------------------------------------+
                     |                                                 |
                     |                                                 v
                     |                                   [ Hardware Equivalence LED (Pin 6) ]
                     |                                   * Lights up instantly when gate logic
                     |                                     matches 74LS181 silicon!
                     v                                                 v
  ============================================================================================================
                                      DUAL-MCU HIL TELEMETRY & VERIFICATION
  ============================================================================================================
                     +-------------------------------------------------+
                     |                                                 |
                     v (Direct 5V Sampling)                            v (1k / 2.2k Voltage Dividers)
     +------------------------------------------+      +------------------------------------------+
     |           Arduino UNO R4 WiFi            |      |              ESP32-S3 N16R8              |
     |         Software Golden Model HIL        |      |         IoT Telemetry & Web Dashboard    |
     |------------------------------------------|      |------------------------------------------|
     | - Reads Y[7:0], A, B, Opcode, Mode       |      | - Broadcasts "ALU-Testbench-AP" Wi-Fi    |
     | - Calculates theoretical Y_expected      |      | - Serves Embedded HTML5/JS Dashboard     |
     | - Compares: (Y_hw == Y_expected)         |      | - Streams WebSocket live bus packets     |
     | - Renders Onboard 12x8 LED Matrix HUD:   |      | - Live Radix HUD: Hex, Dec, Bin          |
     |     '✓' (PASS) / '✗' (FAIL)              |      | - Executes Automated Test Sweeps         |
     | - Controls Green (PASS) & Red (FAIL) LEDs|      | - Overrides inputs via 4.7k resistors    |
     +------------------------------------------+      +------------------------------------------+
```

---

## 🔀 Detailed Dual-Mode Architecture

The core innovation of this architecture is the **hardware-reconfigurable topology**, commanded by a physical Mode Switch:

```
  MODE SWITCH = 0 : 8-Bit Cascaded ALU Mode
  MODE SWITCH = 1 : Real-Time Silicon Verification Mode (Custom 74xx vs 74LS181)
```

### Mode 0: 8-Bit Cascaded ALU Processor

In **Mode 0**, the circuit operates as a unified **8-bit parallel arithmetic logic processor**:
- **Lower Nibble ($A[3:0], B[3:0]$)** is processed by the **Custom 4-Bit Discrete ALU**:
  - Binary addition/subtraction is executed by the `74LS83N` 4-bit binary adder.
  - Bitwise logic operations (AND, OR, XOR, NOT) and logical shifts (SHL, SHR) are generated across `74HCT08`, `74HCT32`, `74HC86N`, and `74HCT04` arrays.
  - Four `74HCT151` 8:1 multiplexers select the active operation for bits $Y_0, Y_1, Y_2, Y_3$.
  - The carry output ($C_4$) of the `74LS83N` is buffered and routed to the carry-in of the upper stage.
- **Upper Nibble ($A[7:4], B[7:4]$)** is processed by the **74LS181N 4-Bit ALU**:
  - Receives the upper 4 bits of Operands A and B.
  - Receives the inter-nibble carry bit ($C_4$) from the lower custom stage.
  - Generates the upper 4 bits of the result ($Y_7, Y_6, Y_5, Y_4$) and the final 8-bit carry-out flag ($C_{out8}$).
- **Result**: The two ALUs seamlessly fuse to produce an **authentic 8-bit output bus**:
  $$Y[7:0] = \{ Y_{181}[3:0], Y_{custom}[3:0] \}$$

```
  [A7..A4]  [B7..B4]                       [A3..A0]  [B3..B0]
     |         |                              |         |
     v         v                              v         v
  +---------------+                      +---------------+
  |   74LS181N    | <------ Carry C4 --- | 74LS83N Custom|
  | (Upper 4-Bit) |                      | (Lower 4-Bit) |
  +---------------+                      +---------------+
         |                                      |
         v [Y7..Y4]                             v [Y3..Y0]
         +------------------+-------------------+
                            |
                            v
                   Combined Output Y[7:0]
```

### Mode 1: Real-Time Hardware Silicon Verification

In **Mode 1**, the architecture transforms into a high-speed **Hardware Equivalence Checking Engine**:
- Both the Custom Discrete ALU and the `74LS181N` receive the **exact same 4-bit inputs**:
  $$A_{custom}[3:0] = A_{181}[3:0] = A[3:0], \quad B_{custom}[3:0] = B_{181}[3:0] = B[3:0]$$
- Both ALUs execute the **exact same operation** determined by Opcode $S[2:0]$.
- The 4-bit output of the Custom ALU ($Y_{custom}[3:0]$) and the 4-bit output of the `74LS181N` ($Y_{181}[3:0]$) are routed directly into the inputs of a hardware **`74HCT85` 4-Bit Magnitude Comparator**:
  - $A_3..A_0 \leftarrow Y_{custom}[3:0]$
  - $B_3..B_0 \leftarrow Y_{181}[3:0]$
- **Instantaneous Equivalence Assertion**:
  - When $Y_{custom}[3:0] == Y_{181}[3:0]$, Pin 6 of the `74HCT85` ($A=B$ Out) asserts **Logic HIGH** (+5V), immediately illuminating the **Hardware Equivalence LED**.
  - Any architectural flaw, gate propagation glitch, or wiring error in the custom discrete datapath breaks equivalence within nanoseconds ($t_{pd} \approx 15\text{ ns}$), driving Pin 6 LOW and extinguishing the LED.

```
       Operand A[3:0]             Operand B[3:0]
             |                          |
             +------------+-------------+
             |            |
             v            v
     +---------------+   +---------------+
     | 4-Bit Custom  |   |   74LS181N    |
     | Discrete ALU  |   |  Silicon ALU  |
     +---------------+   +---------------+
             |                   |
             v Y_custom[3:0]     v Y_181[3:0]
         (Pins A3..A0)       (Pins B3..B0)
             |                   |
             +---------> +-------v-------+
                         |    74HCT85     |
                         | 4-Bit Compar. |
                         +---------------+
                                 | (Pin 6: A=B)
                                 v
                     [ 🟢 SILICON MATCH LED ]
```

---

## ⚡ 3-Board System Responsibilities

| Board | Microcontroller / Platform | Primary Role | Key Hardware Connections |
| :--- | :--- | :--- | :--- |
| **Board 1** | **ESP32-S3 N16R8**<br>(Dual-Core Xtensa LX7 @ 240MHz, 16MB Flash, 8MB PSRAM) | **IoT Dashboard, AP & Remote Injection**<br>Broadcasts standalone Wi-Fi, serves interactive web UI, streams WebSocket bus telemetry, and remotely injects vectors via current-limiting override resistors. | • $4.7\text{ k}\Omega$ series resistors to ALU inputs ($A, B, Op, Mode$)<br>• $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage dividers on $Y[7:0]$ and flags (5V $\to$ 3.3V safe) |
| **Board 2** | **Arduino UNO R4 WiFi**<br>(Renesas RA4M1 32-bit ARM Cortex-M4 @ 48MHz + ESP32-S3) | **Software Golden Model Verifier & HUD**<br>Native 5V tolerant logic analyzer. Runs microsecond C++ reference model, verifies hardware bus against theory, and renders real-time HUD animations on onboard 12×8 LED matrix. | • Direct 5V connection to $Y[7:0]$, $A, B$, Opcode lines<br>• Direct read of `74HCT85` $A=B$ pin<br>• Drives external Green (PASS) and Red (FAIL) LEDs |
| **Board 3** | **Arduino UNO R4 Minima**<br>(Renesas RA4M1 32-bit ARM Cortex-M4 @ 48MHz) | **Clean 5V/1A Power Distribution Station**<br>Provides an isolated, high-current 5V USB-C power supply and stable ground plane exclusively for the breadboard logic ICs and `74LS181N`, eliminating brownouts. | • Direct 5V regulated pin to breadboard $+5\text{V}$ power rails<br>• Solid ground tying to breadboard $\text{GND}$ and common references |

---

## 📦 Complete Bill of Materials (Exact Physical Inventory)

This bill of materials reflects the **exact physical inventory** and component count of the implemented laboratory setup:

### Integrated Circuits (ICs)

| Component | Part Description | Logic Family | Package | In Use | Spares | Total In Hand | Circuit Role |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :--- |
| **74LS181N** | 4-Bit High-Speed Arithmetic Logic Unit | Bipolar / LS | DIP-24 | **1** | **0** | **1** | Upper 4-bit ALU in Mode 0; Silicon verification target in Mode 1 |
| **74LS83N** | 4-Bit Binary Full Adder with Fast Carry | HC / HCT / LS | DIP-16 | **1** | **2** | **3** | Core addition & subtraction engine for Custom 4-Bit ALU |
| **74HCT85** | 4-Bit Magnitude Comparator | HC / HCT / LS | DIP-16 | **1** | **2** | **3** | Real-time hardware equivalence verification ($A=B$) |
| **74HC86N** | Quad 2-Input Exclusive-OR (XOR) Gate | HC / HCT / LS | DIP-14 | **2** | **1** | **3** | IC 1: Subtraction B-inversion; IC 2: Bitwise XOR logic array |
| **74HCT08** | Quad 2-Input AND Gate | HC / HCT / LS | DIP-14 | **1** | **2** | **3** | Bitwise AND logic array + inter-nibble mode gating |
| **74HCT32** | Quad 2-Input OR Gate | HC / HCT / LS | DIP-14 | **1** | **2** | **3** | Bitwise OR logic array |
| **74HCT04** | Hex Inverter (NOT) | HC / HCT / LS | DIP-14 | **1** | **2** | **3** | Bitwise NOT array ($A_3..A_0$) + control signal inversion |
| **74HCT151** | 8-Channel / 8:1 Data Selector / Multiplexer| HC / HCT / LS | DIP-16 | **4** | **5** | **9** | Bit-slice operation multiplexers for custom ALU ($Y_0..Y_3$) |
| **74xx245** | Octal Bus Transceiver (Optional Buffer) | HC / HCT / LS | DIP-20 | **1** | **0** | **1** | Output bus isolation and LED drive buffering |

### Microcontrollers & Development Boards

| Board | Microprocessor / Core | Clock | Memory | Operating Voltage | System Role |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **Arduino UNO R4 WiFi** | Renesas RA4M1 (ARM Cortex-M4) | 48 MHz | 256KB Flash, 32KB SRAM | 5V Native | Software Golden Model Verifier + 12×8 LED Matrix HUD |
| **ESP32-S3 N16R8** | Espressif Xtensa 32-bit LX7 Dual-Core | 240 MHz | 16MB Flash, 8MB PSRAM | 3.3V Logic | Standalone Wi-Fi AP + Web Dashboard + Injection Engine |
| **Arduino UNO R4 Minima**| Renesas RA4M1 (ARM Cortex-M4) | 48 MHz | 256KB Flash, 32KB SRAM | 5V Native | Dedicated Clean 5V/1A USB-C Power Supply Station |

### Passives, Switches & Interconnects

| Component | Specifications | Package | Qty | Functional Purpose |
| :--- | :--- | :---: | :---: | :--- |
| **DIP Switches** | 8-Position SPST DIP Switch | DIP-16 | **2** | Manual entry for Operand $A[7:0]$ and Operand $B[7:0]$ |
| **DIP Switch** | 4-Position SPST DIP Switch | DIP-8 | **1** | Opcode select ($S_2, S_1, S_0$) and Mode Switch (0/1) |
| **Pull-down Resistors**| 10 kΩ, 1/4W, 1% Metal Film | Axial | **24** | Bus termination for DIP switches and control lines |
| **Override Resistors** | 4.7 kΩ, 1/4W, 1% Metal Film | Axial | **16** | Non-destructive series override resistors for ESP32 GPIOs |
| **Voltage Dividers** | 1.0 kΩ & 2.2 kΩ, 1/4W, 1% | Axial | **12+12** | 5V $\to$ 3.3V step-down network for ESP32 input protection |
| **LED Resistors** | 220 Ω / 330 Ω, 1/4W | Axial | **12** | Current limiters for bus LEDs and status indicators |
| **Decoupling Caps** | 0.1 µF (104) Multilayer Ceramic | Radial | **16** | Power supply bypass placed directly across $V_{CC}$/GND of every IC |
| **Bulk Capacitors** | 100 µF, 25V Electrolytic | Radial | **3** | Breadboard power rail smoothing & transient suppression |
| **Data Bus LEDs** | 3mm / 5mm High-Efficiency (Blue) | Radial | **8** | Output data bus visualizer ($Y_7..Y_0$) |
| **Status LEDs** | 5mm Diffused (Green, Red, Amber) | Radial | **3** | Green = PASS, Red = FAIL, Amber = Hardware Equivalence |
| **Breadboards** | 830 Tie-Point Solderless Boards | Module | **4** | Interlocked breadboard matrix for logic array |
| **Jumper Wires** | 22 AWG Solid-Core + Dupont Jumper | Wires | **Assorted** | Color-coded interconnects (Red=+5V, Black=GND, Blue=Data, Yellow=Ctrl) |

---

## 📊 Arithmetic & Logic Operation Truth Table

The system supports **8 fundamental operations** selected by the 3-bit Opcode $S[2:0]$. The following table details the Custom 4-Bit ALU multiplexer routing, the corresponding control lines for the `74LS181N`, and the resulting mathematical functions:

| Opcode ($S_2 S_1 S_0$) | Hex | Operation | Custom ALU 74HCT151 Input ($D_0..D_7$) | 74LS181 Controls ($M, S_3 S_2 S_1 S_0, C_n$) | Algebraic / Logical Function | Arithmetic Details & Flags |
| :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `000` | `0x0` | **ADD** | Channel $D_0$: `74LS83N` Sum | $M=0$, $S=\text{1001}$, $C_n=1$ | $Y = A + B$ | Full binary addition. Carry propagates into $C_4$ ($C_{out8}$ in Mode 0). |
| `001` | `0x1` | **SUB** | Channel $D_1$: `74LS83N` Diff | $M=0$, $S=\text{0110}$, $C_n=0$ | $Y = A - B$ | Two's complement subtraction: $A + \overline{B} + 1$. Carry indicates borrow. |
| `010` | `0x2` | **AND** | Channel $D_2$: `74HCT08` AND | $M=1$, $S=\text{1011}$, $C_n=\text{X}$ | $Y = A \cdot B$ | Bitwise logical AND. Carry output forced LOW. |
| `011` | `0x3` | **OR** | Channel $D_3$: `74HCT32` OR | $M=1$, $S=\text{1110}$, $C_n=\text{X}$ | $Y = A + B$ | Bitwise logical OR. Carry output forced LOW. |
| `100` | `0x4` | **XOR** | Channel $D_4$: `74HC86N` XOR | $M=1$, $S=\text{0110}$, $C_n=\text{X}$ | $Y = A \oplus B$ | Bitwise logical Exclusive-OR. Carry output forced LOW. |
| `101` | `0x5` | **NOT** | Channel $D_5$: `74HCT04` NOT | $M=1$, $S=\text{0000}$, $C_n=\text{X}$ | $Y = \overline{A}$ | Bitwise logical inversion of Operand A. B is ignored. |
| `110` | `0x6` | **SHL** | Channel $D_6$: Shift Left Bus | Mode 0: Hardware wired / Mode 1: Equiv logic | $Y = A \ll 1$ | Logical shift left: $Y_i = A_{i-1}$, $Y_0 = 0$. Bit $A_3$ ($A_7$) $\to$ Carry. |
| `111` | `0x7` | **SHR** | Channel $D_7$: Shift Right Bus | Mode 0: Hardware wired / Mode 1: Equiv logic | $Y = A \gg 1$ | Logical shift right: $Y_i = A_{i+1}$, $Y_3$ ($Y_7$) $= 0$. Bit $A_0 \to$ Carry. |

### Subtraction via Two's Complement in Custom 4-Bit ALU

In the Custom Discrete ALU, subtraction ($A - B$) is executed without requiring a dedicated subtractor chip. When Opcode = `001` (SUB active):
1. The SUB control line asserts HIGH.
2. The second input of the `74HC86N` XOR gate is driven HIGH, inverting every bit of Operand B:
   $$B'_i = B_i \oplus 1 = \overline{B}_i$$
3. The SUB control line is simultaneously connected to the carry-in ($C_0$) of the `74LS83N` adder ($C_0 = 1$).
4. The adder evaluates:
   $$Y = A + B' + C_0 = A + \overline{B} + 1 = A - B$$

---

## 🎛️ Hybrid Input Injection Architecture

The system features a **non-destructive resistive priority network** that allows tactile manipulation of physical DIP switches concurrently with automated software test vectors driven by the ESP32-S3:

```
  +5V Rail ----------------------------+
                                       |
                                [ DIP Switch ]
                                       |
  ALU Input Pin <----------------------+---------------------<[ 4.7k Override ]<--- ESP32 GPIO
                                       |
                                [ 10k Resistor ]
                                       |
  GND Rail ----------------------------+
```

### Circuit Analysis & Electrical Proof

1. **Manual Tactile Mode (`pinMode(GPIO, INPUT)`)**:
   - The ESP32 pin is placed in high-impedance tri-state mode ($Z_{in} > 10\text{ M}\Omega$).
   - When DIP switch is **ON**: ALU input connects directly to $+5\text{V}$ ($\text{Logic HIGH} = 5.0\text{V} > V_{IH,\min} = 2.0\text{V}$).
   - When DIP switch is **OFF**: ALU input connects through $10\text{ k}\Omega$ to $\text{GND}$ ($\text{Logic LOW} = 0.0\text{V} < V_{IL,\max} = 0.8\text{V}$).
2. **Automated Remote Injection Mode (`pinMode(GPIO, OUTPUT)`)**:
   - The ESP32 actively drives its 3.3V CMOS output:
     - **Injecting HIGH (3.3V)**: With DIP switch OFF, the $4.7\text{ k}\Omega$ override resistor forms a voltage divider with the $10\text{ k}\Omega$ pull-down:
       $$V_{in} = 3.3\text{ V} \times \frac{10\text{ k}\Omega}{4.7\text{ k}\Omega + 10\text{ k}\Omega} = 3.3\text{ V} \times \frac{10}{14.7} \approx 2.245\text{ V}$$
       Because $2.245\text{ V} > V_{IH,\min} (2.0\text{ V})$ for TTL/HCT logic, this reliably registers as an unambiguous **Logic HIGH**.
     - **Contention Safety (Injecting LOW while DIP is ON)**: If the user leaves the physical DIP switch ON (+5V) while the ESP32 drives LOW (0V), current flows from +5V to GND through the $4.7\text{ k}\Omega$ resistor:
       $$I_{contention} = \frac{5.0\text{ V} - 0.0\text{ V}}{4.7\text{ k}\Omega} \approx 1.06\text{ mA}$$
       This current is well within the ESP32 GPIO safe limit ($40\text{ mA}$) and causes zero damage to switches or silicon!

---

## 🔬 Dual-Layer Verification Methodology

The architecture incorporates two independent, complementary verification layers:

```
                            [ ALU Computation ]
                                     |
               +---------------------+---------------------+
               |                                           |
               v                                           v
     [ LAYER 1: SILICON LEVEL ]                 [ LAYER 2: SOFTWARE HIL LEVEL ]
     - Hardware: 74HCT85 Magnitude Comp          - Hardware: Arduino UNO R4 WiFi
     - Benchmark: Vintage 74LS181N Silicon      - Benchmark: C++ Golden Model Oracle
     - Metric: Pure Gate Equivalence (A=B)      - Metric: Algorithmic Correctness
     - Feedback: Instant Hardware LED           - Feedback: 12x8 LED Matrix HUD + Serial
```

### Layer 1: Hardware Equivalence Verification (74HCT85)
- Operates in Mode 1.
- Evaluates equivalence at physical gate switching speeds without any microcontroller software latency.
- Compares the four discrete output lines ($Y_{custom}[3:0]$) against the four `74LS181N` output lines ($Y_{181}[3:0]$).
- When all 4 bits match, Pin 6 ($A=B$) of `74HCT85` asserts HIGH (+5V) and lights the **Silicon Match LED**.

### Layer 2: Software Golden Model Oracle (Arduino UNO R4 WiFi)
- Operates across both Mode 0 and Mode 1.
- Samples the full 8-bit output bus $Y[7:0]$, Operand inputs, Opcode, and Mode switch in real-time.
- Executes an exact software specification (golden model) in C++:
  ```cpp
  uint8_t computeExpected(uint8_t a, uint8_t b, uint8_t op, bool mode, bool &c_out) {
      if (mode == 0) { // 8-Bit Cascaded ALU Mode
          switch (op) {
              case 0: { uint16_t s = a + b; c_out = (s > 0xFF); return (uint8_t)s; }
              case 1: { uint16_t d = a - b; c_out = (a >= b); return (uint8_t)d; }
              case 2: c_out = 0; return a & b;
              case 3: c_out = 0; return a | b;
              case 4: c_out = 0; return a ^ b;
              case 5: c_out = 0; return ~a;
              case 6: c_out = (a & 0x80) != 0; return a << 1;
              case 7: c_out = (a & 0x01) != 0; return a >> 1;
              default: return 0;
          }
      } else { // Mode 1: 4-Bit Equivalence Verification Mode
          uint8_t a4 = a & 0x0F, b4 = b & 0x0F;
          switch (op) {
              case 0: { uint8_t s = a4 + b4; c_out = (s > 0x0F); return s & 0x0F; }
              case 1: { uint8_t d = a4 - b4; c_out = (a4 >= b4); return d & 0x0F; }
              case 2: c_out = 0; return (a4 & b4) & 0x0F;
              case 3: c_out = 0; return (a4 | b4) & 0x0F;
              case 4: c_out = 0; return (a4 ^ b4) & 0x0F;
              case 5: c_out = 0; return (~a4) & 0x0F;
              case 6: c_out = (a4 & 0x08) != 0; return (a4 << 1) & 0x0F;
              case 7: c_out = (a4 & 0x01) != 0; return (a4 >> 1) & 0x0F;
              default: return 0;
          }
      }
  }
  ```
- **Dynamic 12×8 LED Matrix HUD Displays**:
  - `✓` Animated Checkmark when $Y_{hw} == Y_{exp}$ (PASS).
  - `✗` Blinking Fault Cross when $Y_{hw} \neq Y_{exp}$ (FAIL).
  - Real-time rolling test score counter and hexadecimal value readouts.
- Drives external high-brightness **Green PASS LED** and **Red FAIL LED**.

---

## ⚡ Power Subsystem & Clean Distribution Station

TTL ICs (specifically bipolar chips like the `74LS181N`) draw significant static current ($\approx 100\text{ mA}$ typical for `74LS181` alone, plus switching transients across multiple CMOS gate packages). Powering the entire system from a single development board's 5V LDO causes power rail droop, ground bounce, and MCU resets.

To guarantee rock-solid electrical stability, the system uses a **dedicated power topology**:

```
 [ USB-C Host (5V / 3A) ]
           |
           v
 [ Arduino UNO R4 Minima ] -------- Clean Regulated +5V @ 1.2A
   (Dedicated Power Station)       Common Solid Ground Plane
           |                                  |
           +----------------------------------+
           |                                  |
           v                                  v
 [ Breadboard Logic Rail ]          [ 74LS181N + 74xx IC Array ]
   - 100uF Bulk Electrolytic          - 0.1uF Decoupling per IC
   - 0.1uF High-Frequency Caps        - Low-impedance bus bars
```

- **Arduino UNO R4 Minima**: Serves as the primary 5V power supply source directly from a high-current USB-C port, delivering up to 1.2A of ripple-free 5V power to the breadboards.
- **Ground Referencing**: Grounds of the Arduino UNO R4 Minima, Arduino UNO R4 WiFi, ESP32-S3, and breadboards are tied together into a single star-ground configuration to eliminate ground loops.

---

## 📁 Repository Directory Structure

```
.
├── README.md                           # Master Architecture & Technical Documentation
├── hardware_docs/                      # Breadboard Schematics & Wiring Guides
│   ├── PIN_TO_PIN_WIRING_GUIDE.md      # Pin-by-pin benchtop assembly lookup
│   ├── CIRCUIT_DIAGRAM.svg             # Full-scale vector schematic diagram
│   ├── CIRCUIT_DIAGRAM.md              # Detailed schematic netlists & subsystem theory
│   ├── PINOUTS_AND_SCHEMATICS.md       # Pinout reference, Boolean tables & MUX maps
│   ├── BREADBOARD_ASSEMBLY_GUIDE.md    # Stage-by-stage physical assembly walkthrough
│   └── PERFBOARD_SOLDERING_GUIDE.md    # Dot-matrix perfboard migration manual
├── esp32_firmware/                     # ESP32-S3 N16R8 Firmware & Web Dashboard
│   ├── esp32_firmware.ino              # Wi-Fi AP, WebSockets, telemetry & override logic
│   ├── web_page.h                      # HTML5/CSS3/JavaScript responsive web interface
│   └── pin_definitions.h               # ESP32-S3 GPIO pin mapping & level shifting
├── arduino_uno_r4_firmware/            # Arduino UNO R4 WiFi Firmware
│   └── arduino_uno_r4_firmware.ino     # Golden Model HIL verifier & 12x8 LED matrix HUD
├── simulation/                         # Digital Logic Simulation
│   └── dual_mode_8bit_alu_processor.circ # Logisim Evolution behavioral & gate-level model
└── project_report/                     # University Academic Documentation
    └── DECA_ALU_PROJECT_REPORT.md      # Formal minor project defense report
```

---

## 🚀 Step-by-Step Quick Start & Deployment Guide

### Step 1: Breadboard Power & IC Distribution
1. Interlock four 830-point breadboards. Bridge the top and bottom $+5\text{V}$ and $\text{GND}$ power rails across all boards using heavy-gauge wire.
2. Connect the **Arduino UNO R4 Minima** `5V` and `GND` pins to the primary breadboard power rails.
3. Place a $100\ \mu\text{F}$ electrolytic capacitor across the main power entry and a $0.1\ \mu\text{F}$ ceramic capacitor directly across pins $V_{CC}$ and $\text{GND}$ of every IC.
4. Mount the ICs in logical order:
   - **Board 1**: Input DIP switches, 10k pull-downs, and 4.7k override resistors.
   - **Board 2**: Custom 4-Bit ALU (`74LS83N`, `74HC86N`, `74HCT08`, `74HCT32`, `74HCT04`).
   - **Board 3**: Four `74HCT151` 8:1 Multiplexers + output buffer (`74xx245`).
   - **Board 4**: `74LS181N` ALU + `74HCT85` Magnitude Comparator + Mode Switch.

### Step 2: Wiring the Custom 4-Bit ALU & MUX Bank
1. Connect $A[3:0]$ and $B[3:0]$ to the `74LS83N` adder:
   - Route $B[3:0]$ through `74HC86N` XOR gates with the SUB signal for two's complement inversion.
   - Connect the SUB signal to $C_0$ of the `74LS83N`.
2. Connect $A[3:0]$ and $B[3:0]$ in parallel to the logic arrays (`74HCT08` for AND, `74HCT32` for OR, `74HC86N` for XOR, `74HCT04` for NOT).
3. Connect the 8 operation results to the four `74HCT151` multiplexer inputs ($D_0..D_7$):
   - $D_0 \to$ ADD (`74LS83N` Sum)
   - $D_1 \to$ SUB (`74LS83N` Diff)
   - $D_2 \to$ AND (`74HCT08`)
   - $D_3 \to$ OR (`74HCT32`)
   - $D_4 \to$ XOR (`74HC86N`)
   - $D_5 \to$ NOT (`74HCT04`)
   - $D_6 \to$ SHL (Left-shifted bus)
   - $D_7 \to$ SHR (Right-shifted bus)
4. Tie the select pins ($S_2, S_1, S_0$) of all four `74HCT151` ICs in parallel to the Opcode bus.

### Step 3: Wiring 74LS181N & 74HCT85 Hardware Verification
1. Route the Opcode translation lines to the `74LS181N` control pins ($S_3..S_0, M$).
2. Connect Mode Switch:
   - **Mode = 0**: Connect `74LS83N` carry-out ($C_4$) to `74LS181N` carry-in; route $A[7:4]$ and $B[7:4]$ to `74LS181N` inputs.
   - **Mode = 1**: Connect $A[3:0]$ and $B[3:0]$ to `74LS181N` inputs; route $Y_{custom}[3:0]$ to `74HCT85` $A$-inputs and $Y_{181}[3:0]$ to `74HCT85` $B$-inputs.
3. Connect Pin 6 of the `74HCT85` to the **Silicon Match LED** via a $330\ \Omega$ resistor.

### Step 4: Interfacing Microcontrollers
1. **Arduino UNO R4 WiFi**:
   - Connect inputs `D2–D9` to the 8-bit output bus ($Y[7:0]$).
   - Connect inputs `A0–A3` to Opcode $S[2:0]$ and Mode lines.
   - Connect `D10` to Green PASS LED and `D11` to Red FAIL LED.
   - Connect common ground to breadboard ground rail.
2. **ESP32-S3 N16R8**:
   - Connect GPIO injection pins to the ALU input lines through $4.7\text{ k}\Omega$ series resistors.
   - Connect output bus monitor lines through $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage dividers.
   - Connect common ground to breadboard ground rail.

### Step 5: Firmware Upload & Operation
1. Flash **Arduino UNO R4 WiFi**:
   - Open Arduino IDE $\to$ Select board **Arduino UNO R4 WiFi**.
   - Open `arduino_uno_r4_firmware/arduino_uno_r4_firmware.ino` and click **Upload**.
   - The onboard 12×8 LED matrix boots with a self-test animation.
2. Flash **ESP32-S3 N16R8**:
   - Select board **ESP32S3 Dev Module** (Flash Size: 16MB, PSRAM: OPI PSRAM).
   - Open `esp32_firmware/esp32_firmware.ino` and click **Upload**.
3. Connect to the Wi-Fi AP:
   - **SSID**: `ALU-Testbench-AP`
   - **Password**: `decaproject`
4. Navigate to `http://192.168.4.1` in any browser to open the live telemetry HUD!
5. Toggle DIP switches or click buttons on the web interface: observe synchronized updates across physical LEDs, the 74HCT85 Silicon Match LED, the Arduino 12×8 matrix HUD, and the web browser!

---

## 🛠️ Technology Stack Summary

```
 ┌─────────────────────┐   ┌─────────────────────┐   ┌─────────────────────┐
 │    DISCRETE LOGIC   │   │     EMBEDDED MCU    │   │     WEB TELEMETRY   │
 ├─────────────────────┤   ├─────────────────────┤   ├─────────────────────┤
 │ • 74LS181N ALU      │   │ • Arduino UNO R4 WFi│   │ • HTML5 / ES6 WebUI │
 │ • 74LS83N 4-Bit Add │   │   (RA4M1 48MHz M4)  │   │ • WebSockets Async  │
 │ • 74HCT85 Comparator │   │ • Arduino R4 Minima │   │ • Live Radix HUD    │
 │ • 74HCT151 8:1 MUX   │   │   (Power Station)   │   │ • Real-time SVG Bus │
 │ • 74HC86N/08/32/04   │   │ • ESP32-S3 N16R8    │   │ • Vector Test Sweep │
 └─────────────────────┘   └─────────────────────┘   └─────────────────────┘
```

---

## 📜 Academic Integrity & License

This system was engineered as a comprehensive Minor Project for the **Digital Electronics & Computer Architecture (DECA)** curriculum. Released under the open-source [MIT License](LICENSE). Educational institutions, researchers, and students are welcome to adapt, replicate, and extend this work with proper academic attribution.
