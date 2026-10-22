# Breadboard Assembly & Testing Guide (Step-by-Step)
## 12-IC Dual-Mode Discrete & Silicon ALU Processor with ESP32-S3 IoT Dashboard & Arduino R4 Verifier

---

## 📖 Executive Summary & Architectural Concept

This guide provides a rigorous, step-by-step physical assembly protocol for constructing the **Dual-Mode Hybrid Arithmetic Logic Unit (ALU)** on solderless breadboards. The system bridges fundamental gate-level digital architecture with historical integrated silicon, augmented by a heterogeneous 3-board embedded instrumentation infrastructure.

### The System at a Glance:
1. **The 12-IC Core Logic Engine**:
   - **IC1**: `74LS83N` (4-Bit Binary Full Adder with Fast Carry)
   - **IC2**: `74HC86N` (Quad 2-Input XOR — Subtraction Inversion Conditioning)
   - **IC3**: `74HC86N` (Quad 2-Input XOR — Bitwise XOR Logic Array)
   - **IC4**: `74HCT08` (Quad 2-Input AND — Bitwise AND Logic Array)
   - **IC5**: `74HCT32` (Quad 2-Input OR — Bitwise OR Logic Array)
   - **IC6**: `74HCT04` (Hex Inverter — Bitwise NOT Logic Array)
   - **IC7 - IC10**: 4× `74HCT151` (8:1 Multiplexers for Bits 0, 1, 2, 3)
   - **IC11**: `74LS181N` (4-Bit Monolithic High-Speed Arithmetic Logic Unit)
   - **IC12**: `74HCT85` (4-Bit Magnitude Comparator — Real-Time Silicon Match Detector)

2. **The Heterogeneous 3-Board Instrumentation Infrastructure**:
   - **Arduino UNO R4 Minima ("The Power Station")**: Dedicated clean +5V/1A USB-C power rail supplying all breadboard logic and eliminating TTL switching transients.
   - **Arduino UNO R4 WiFi ("The Verifier")**: Native 5V bus sniffer, executing an algorithmic C++ Software Golden Model HIL Oracle and driving the onboard 12×8 LED Matrix HUD (`✓` PASS / `✗` FAIL) and status LEDs.
   - **ESP32-S3 N16R8 ("The Brain")**: Broadcasts autonomous SoftAP Wi-Fi (`DECA-ALU`), serves a real-time WebSocket Web Dashboard, and drives non-destructive remote test injection via $4.7\text{ k}\Omega$ override resistors.

3. **Dual Reconfigurable Operational Modes**:
   - **Mode 0 (8-Bit Cascaded ALU Processor)**: Custom 4-Bit ALU computes the lower nibble ($Y[3:0]$) and cascades carry-out ($C_4$) into the `74LS181N` computing the upper nibble ($Y[7:4]$), forming a true 8-bit parallel arithmetic processor slice ($Y[7:0]$).
   - **Mode 1 (Real-Time Silicon Verification)**: Both Custom ALU and `74LS181N` receive identical 4-bit inputs ($A[3:0], B[3:0]$) and identical opcodes. The `74HCT85` comparator tests outputs in nanosecond hardware ($A=B$), lighting an instant Green PASS LED.

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
     | - B-Invert (SUB): 74HC86N (IC2)           |         | - Internal Lookahead Carry Generation    |
     | - Bitwise XOR:    74HC86N (IC3)           |         | - 16 Arithmetic + 16 Logic Functions     |
     | - Bitwise AND:    74HCT08 (IC4)           |         | - Function Mapped via S[3:0], M, Cn      |
     | - Bitwise OR:     74HCT32 (IC5)           |         +------------------------------------------+
     | - Bitwise NOT:    74HCT04 (IC6)           |                              |
     | - Operation MUX: 4x 74HCT151 (IC7-IC10)   |                              |
     +------------------------------------------+                              |
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
                     |                                   [ 🟢 SILICON MATCH LED (Pin 6) ]
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
     +------------------------------------------+      +------------------------------------------+
```

---

## 🧰 Complete Hardware Bill of Materials (Exact Physical Inventory)

| Item | Component | Package | Qty | Description & Circuit Role |
| :---: | :--- | :---: | :---: | :--- |
| **IC1** | **74LS83N** | DIP-16 | 1 | 4-Bit Binary Full Adder with Fast Carry (VCC=P5, GND=P12) |
| **IC2** | **74HC86N** | DIP-14 | 1 | Quad 2-Input XOR (B-inversion for two's complement subtraction) |
| **IC3** | **74HC86N** | DIP-14 | 1 | Quad 2-Input XOR (Bitwise XOR logic array) |
| **IC4** | **74HCT08** | DIP-14 | 1 | Quad 2-Input AND (Bitwise AND logic array) |
| **IC5** | **74HCT32** | DIP-14 | 1 | Quad 2-Input OR (Bitwise OR logic array) |
| **IC6** | **74HCT04** | DIP-14 | 1 | Hex Inverter (Bitwise NOT array + control inverter) |
| **IC7** | **74HCT151** | DIP-16 | 1 | 8:1 Multiplexer (Selects Bit 0 Result $Y_0$) |
| **IC8** | **74HCT151** | DIP-16 | 1 | 8:1 Multiplexer (Selects Bit 1 Result $Y_1$) |
| **IC9** | **74HCT151** | DIP-16 | 1 | 8:1 Multiplexer (Selects Bit 2 Result $Y_2$) |
| **IC10**| **74HCT151** | DIP-16 | 1 | 8:1 Multiplexer (Selects Bit 3 Result $Y_3$) |
| **IC11**| **74LS181N**| DIP-24 | 1 | 4-Bit Monolithic Bipolar Arithmetic Logic Unit (Upper nibble / Golden silicon) |
| **IC12**| **74HCT85**  | DIP-16 | 1 | 4-Bit Magnitude Comparator (Real-time $A=B$ hardware equivalence) |
| **MCU1**| **Arduino UNO R4 Minima** | Board | 1 | Dedicated Clean 5V/1A USB-C Power Station |
| **MCU2**| **Arduino UNO R4 WiFi**   | Board | 1 | 5V Software Golden Model Verifier + 12×8 LED Matrix HUD |
| **MCU3**| **ESP32-S3 N16R8**        | Board | 1 | Web Dashboard AP + Hybrid Input Injector |
| **SW1** | 8-Position DIP Switch     | DIP-16 | 1 | Manual Input for Operand A ($A[7:0]$) |
| **SW2** | 8-Position DIP Switch     | DIP-16 | 1 | Manual Input for Operand B ($B[7:0]$) |
| **SW3** | 4-Position DIP Switch     | DIP-8  | 1 | Manual Opcode ($S_2, S_1, S_0$) + Mode Switch |
| **R_PD**| $10\text{ k}\Omega$ 1/4W Resistors | Axial | 20 | Pull-down resistors for all DIP switch inputs |
| **R_OV**| $4.7\text{ k}\Omega$ 1/4W Resistors | Axial | 19 | Series override resistors for ESP32 input injection |
| **R_DIV**| $1.0\text{ k}\Omega$ & $2.2\text{ k}\Omega$ | Axial | 13+13 | Voltage dividers for ESP32 3.3V safe inputs |
| **R_LED**| $220\ \Omega$ / $330\ \Omega$ | Axial | 12 | Current-limiting resistors for LEDs |
| **C_DEC**| $0.1\ \mu\text{F}$ Monolithic Ceramic | Radial | 14 | Decoupling capacitors (one per IC socket + rails) |
| **C_BLK**| $100\ \mu\text{F}$ 25V Electrolytic | Radial | 2 | Bulk smoothing capacitors across power rails |
| **LEDs**| 3mm / 5mm LEDs            | Radial | 12 | 8× Blue ($Y[7:0]$), 1× Yellow ($C_{out}$), 1× Green (PASS), 1× Red (FAIL), 1× Green (Silicon Match) |
| **BB**  | 830-Tie Point Breadboards  | Module | 3–4 | Interlocking solderless breadboards |

---

## 🗺️ Breadboard Floorplan & Workstation Geometry

Assemble 4 standard 830-tie-point solderless breadboards side-by-side:

```
+---------------------------------------------------------------------------------------------------------+
|                                  BREADBOARD 1: POWER, INPUTS & CONTROL                                  |
|  - Arduino R4 Minima 5V Power Station Entry & 100uF Bulk Capacitors                                     |
|  - DIP Switch SW1 (A[7:0]) + 10k Pull-downs                                                             |
|  - DIP Switch SW2 (B[7:0]) + 10k Pull-downs                                                             |
|  - DIP Switch SW3 (Opcode S[2:0] & Mode Switch) + 10k Pull-downs                                        |
|  - ESP32-S3 4.7k Override Resistor Header                                                               |
+---------------------------------------------------------------------------------------------------------+
|                                  BREADBOARD 2: CUSTOM 4-BIT ALU CORE                                    |
|  - IC1: 74LS83N (Adder)              - IC2: 74HC86N (Subtraction XOR)                                    |
|  - IC3: 74HC86N (Bitwise XOR)         - IC4: 74HCT08 (Bitwise AND)                                        |
|  - IC5: 74HCT32 (Bitwise OR)          - IC6: 74HCT04 (Hex Inverter)                                       |
+---------------------------------------------------------------------------------------------------------+
|                                  BREADBOARD 3: MULTIPLEXERS & COMPARATOR                                |
|  - IC7:  74HCT151 (MUX Bit 0)         - IC8:  74HCT151 (MUX Bit 1)                                        |
|  - IC9:  74HCT151 (MUX Bit 2)         - IC10: 74HCT151 (MUX Bit 3)                                        |
|  - IC12: 74HCT85 (4-Bit Magnitude Comparator)                                                            |
+---------------------------------------------------------------------------------------------------------+
|                                  BREADBOARD 4: 74LS181N SILICON & INSTRUMENTATION                       |
|  - IC11: 74LS181N (24-pin DIP 4-Bit ALU)                                                                |
|  - Mode Steering Switch & Carry Cascade Line                                                            |
|  - Status & Bus Visualizer LEDs (Y[7:0], Cout, PASS, FAIL, Match)                                      |
|  - Arduino UNO R4 WiFi & ESP32-S3 Interface Headers                                                     |
+---------------------------------------------------------------------------------------------------------+
```

---

## Stage 1: Power Distribution & Multi-Board Common Ground Setup

```
   Arduino UNO R4 Minima
  (Power Station 5V/1A USB-C)
  ┌───────────────────┐
  │          5V Pin ──┼───────────────────────► Top Red (+5V) Rail (All Breadboards)
  │                   │
  │         GND Pin ──┼──────────────────┬────► Top Blue (GND) Rail (All Breadboards)
  └───────────────────┘                  │
                                         │
   Arduino UNO R4 WiFi                   │
  (Software Verifier)                    │
  ┌───────────────────┐                  │
  │  (USB Powered)    │                  │
  │         GND Pin ──┼──────────────────┤
  └───────────────────┘                  │
                                         │
   ESP32-S3 N16R8                        │
  (Web Dashboard & Brain)                │
  ┌───────────────────┐                  │
  │  (USB Powered)    │                  │
  │         GND Pin ──┼──────────────────┴────► Shared Star-Ground Reference Bus
  └───────────────────┘
```

### Assembly Procedure:
1. **Link All Rails**:
   - Run solid-core 22 AWG red jumper wires connecting the top and bottom $+5\text{V}$ rails across all four breadboards.
   - Run solid-core 22 AWG black/blue jumper wires connecting the top and bottom $\text{GND}$ rails across all four breadboards.
2. **Power Station Feed**:
   - Connect the **5V pin** of the **Arduino UNO R4 Minima** directly to the main $+5\text{V}$ breadboard rail.
   - Connect the **GND pin** of the Arduino UNO R4 Minima directly to the main $\text{GND}$ breadboard rail.
3. **Common Ground Tie**:
   - Connect a GND pin from the **Arduino UNO R4 WiFi** to the breadboard GND rail.
   - Connect a GND pin from the **ESP32-S3** to the breadboard GND rail.
   - > [!IMPORTANT]
     > All 3 microcontrollers and all breadboard logic must share a single, unbroken ground plane. Floating grounds will induce severe voltage drift and false comparison results.
4. **Power Smoothing & Decoupling**:
   - Insert a $100\ \mu\text{F}$ electrolytic capacitor across the $+5\text{V}$ and $\text{GND}$ rails on Breadboard 1 (observe polarity: negative stripe to GND).
   - Insert a second $100\ \mu\text{F}$ capacitor on Breadboard 4.
   - Reserve fourteen $0.1\ \mu\text{F}$ ceramic capacitors for placement directly between pin 14/16/24 and pin 7/8/12 of every IC.

### ✅ Testing Checklist 1:
- [ ] Connect the Arduino UNO R4 Minima via USB-C to a dedicated 5V/1A+ power source.
- [ ] Set your digital multimeter to DC Volts. Measure between $+5\text{V}$ and $\text{GND}$ at the far end of Breadboard 4. Verify reading is between **$+4.90\text{V}$ and $+5.15\text{V}$**.
- [ ] Switch multimeter to Continuity mode (beep). Verify $0.0\ \Omega$ resistance between:
  - Arduino Minima GND and Breadboard GND rail.
  - Arduino R4 WiFi GND and Breadboard GND rail.
  - ESP32-S3 GND and Breadboard GND rail.
- [ ] Verify that no red power rail is accidentally shorted to a blue ground rail ($R > 100\text{ k}\Omega$ with no ICs inserted).

---

## Stage 2: Custom 4-Bit ALU Core Assembly (IC1 - IC10)

In this stage, you build the **Custom 4-Bit Discrete ALU Slice** consisting of 10 ICs.

```
          [ Operand A: A[3:0] ]             [ Operand B: B[3:0] ]
                    │                                 │
                    ├──────────────┬──────────────────┤
                    │              │                  │
                    │              ▼                  │
                    │      +---------------+          │
                    │      | IC2: 74HC86N   | ◄── SUB (Opcode 001)
                    │      | B-Invert (SUB)|          │
                    │      +-------┬-------+          │
                    │              │ B'[3:0]          │
                    ▼              ▼                  │
            +------------------------------+          │
            |  IC1: 74LS83N 4-Bit Adder    |          │
            |  A[3:0] + B'[3:0] + Cin(SUB) |          │
            +--------------┬---------------+          │
                           │ Sum[3:0]                 │
                           ├──────────────────────────┼───────────────┐
                           │                          │               │
                           ▼                          ▼               ▼
                   +---------------+          +---------------+  +---------------+
                   | IC4: 74HCT08   |          | IC5: 74HCT32   |  | IC3: 74HC86N   |
                   | Bitwise AND   |          | Bitwise OR    |  | Bitwise XOR   |
                   +-------┬-------+          +-------┬-------+  +-------┬-------+
                           │                          │                  │
                           └───────────────┬──────────┴──────────────────┘
                                           │
                                           ▼
            +--------------------------------------------------------------+
            | IC7 - IC10: 4× 74HCT151 8:1 Multiplexers                      |
            | D0=ADD, D1=SUB, D2=AND, D3=OR, D4=XOR, D5=NOT, D6=SHL, D7=SHR |
            +------------------------------┬-------------------------------+
                                           │
                                           ▼
                               Custom ALU Output Y_custom[3:0]
```

### 2.1 IC Placement & Power Hookups (Breadboards 2 & 3):
Place all ICs straddling the center divider groove with notches facing **LEFT**:
> [!CAUTION]
> **CRITICAL POWER WARNING FOR 74LS83N**:
> - **Pin 5 is +5V (VCC)**
> - **Pin 12 is GND (0V)**
> DO NOT wire Pin 16 to +5V or Pin 8 to GND! Place a $0.1\ \mu\text{F}$ cap directly between Pin 5 and Pin 12.
* **IC1**: `74LS83N` (16-Pin DIP) — Pin 5 to $+5\text{V}$, Pin 12 to $\text{GND}$.
* **IC2**: `74HC86N` (14-Pin DIP) — Pin 14 to $+5\text{V}$, Pin 7 to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 14 & 7.
* **IC3**: `74HC86N` (14-Pin DIP) — Pin 14 to $+5\text{V}$, Pin 7 to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 14 & 7.
* **IC4**: `74HCT08` (14-Pin DIP) — Pin 14 to $+5\text{V}$, Pin 7 to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 14 & 7.
* **IC5**: `74HCT32` (14-Pin DIP) — Pin 14 to $+5\text{V}$, Pin 7 to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 14 & 7.
* **IC6**: `74HCT04` (14-Pin DIP) — Pin 14 to $+5\text{V}$, Pin 7 to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 14 & 7.
* **IC7 - IC10**: 4× `74HCT151` (16-Pin DIP) — Pin 16 to $+5\text{V}$, Pin 8 to $\text{GND}$, Pin 7 ($\overline{E}$ / Strobe) tied to $\text{GND}$, $0.1\ \mu\text{F}$ cap across Pins 16 & 8.

### 2.2 Subtraction Conditioning Wiring (IC2: 74HC86N):
IC2 conditionally inverts $B[3:0]$ whenever Opcode is `001` (SUB active):
* Connect the **SUB control signal** to IC2 Pin 2 (1B), Pin 5 (2B), Pin 10 (3B), and Pin 13 (4B).
* Connect Operand $B[0]$ to IC2 Pin 1 (1A) $\to$ Output Pin 3 is $B'_0$.
* Connect Operand $B[1]$ to IC2 Pin 4 (2A) $\to$ Output Pin 6 is $B'_1$.
* Connect Operand $B[2]$ to IC2 Pin 9 (3A) $\to$ Output Pin 8 is $B'_2$.
* Connect Operand $B[3]$ to IC2 Pin 12 (4A) $\to$ Output Pin 11 is $B'_3$.
* Connect the **SUB control signal** also to IC1 Pin 13 ($C_0$ / Carry-In of adder) to provide the $+1$ for two's complement.

### 2.3 4-Bit Adder Wiring (IC1: 74LS83N):
Wire operands $A[3:0]$ and conditioned $B'[3:0]$ to IC1 (observing non-standard 74LS83N pinout):
* Bit 0: Pin 10 ($A_1$) $\leftarrow A_0$, Pin 11 ($B_1$) $\leftarrow B'_0$ (IC2 Pin 3). Sum output: Pin 9 ($\Sigma_1$).
* Bit 1: Pin 8 ($A_2$) $\leftarrow A_1$, Pin 7 ($B_2$) $\leftarrow B'_1$ (IC2 Pin 6). Sum output: Pin 6 ($\Sigma_2$).
* Bit 2: Pin 3 ($A_3$) $\leftarrow A_2$, Pin 4 ($B_3$) $\leftarrow B'_2$ (IC2 Pin 8). Sum output: Pin 2 ($\Sigma_3$).
* Bit 3: Pin 15 ($A_4$) $\leftarrow A_3$, Pin 16 ($B_4$) $\leftarrow B'_3$ (IC2 Pin 11). Sum output: Pin 1 ($\Sigma_4$).
* Carry-in: Pin 13 ($C_0$) $\leftarrow$ SUB control line ($SUB = 1$ injects $+1$ for 2's complement).
* Carry-out: Pin 14 ($C_4$) $\to$ Raw Carry-out (routed to inverter IC6 Pin 11 for active-LOW $\overline{C_n}$ input on 74LS181 in Stage 5).

### 2.4 Logic Gate Arrays (IC3, IC4, IC5, IC6):
* **IC3 (Bitwise XOR - 74HC86N)**:
  - Gate 1: Pin 1 ($A_0$), Pin 2 ($B_0$) $\to$ Pin 3 ($A_0 \oplus B_0$).
  - Gate 2: Pin 4 ($A_1$), Pin 5 ($B_1$) $\to$ Pin 6 ($A_1 \oplus B_1$).
  - Gate 3: Pin 9 ($A_2$), Pin 10 ($B_2$) $\to$ Pin 8 ($A_2 \oplus B_2$).
  - Gate 4: Pin 12 ($A_3$), Pin 13 ($B_3$) $\to$ Pin 11 ($A_3 \oplus B_3$).
* **IC4 (Bitwise AND - 74HCT08)**:
  - Gate 1: Pin 1 ($A_0$), Pin 2 ($B_0$) $\to$ Pin 3 ($A_0 \cdot B_0$).
  - Gate 2: Pin 4 ($A_1$), Pin 5 ($B_1$) $\to$ Pin 6 ($A_1 \cdot B_1$).
  - Gate 3: Pin 9 ($A_2$), Pin 10 ($B_2$) $\to$ Pin 8 ($A_2 \cdot B_2$).
  - Gate 4: Pin 12 ($A_3$), Pin 13 ($B_3$) $\to$ Pin 11 ($A_3 \cdot B_3$).
* **IC5 (Bitwise OR - 74HCT32)**:
  - Gate 1: Pin 1 ($A_0$), Pin 2 ($B_0$) $\to$ Pin 3 ($A_0 + B_0$).
  - Gate 2: Pin 4 ($A_1$), Pin 5 ($B_1$) $\to$ Pin 6 ($A_1 + B_1$).
  - Gate 3: Pin 9 ($A_2$), Pin 10 ($B_2$) $\to$ Pin 8 ($A_2 + B_2$).
  - Gate 4: Pin 12 ($A_3$), Pin 13 ($B_3$) $\to$ Pin 11 ($A_3 + B_3$).
* **IC6 (Bitwise NOT - 74HCT04)**:
  - Gate 1: Pin 1 ($A_0$) $\to$ Pin 2 ($\overline{A_0}$).
  - Gate 2: Pin 3 ($A_1$) $\to$ Pin 4 ($\overline{A_1}$).
  - Gate 3: Pin 5 ($A_2$) $\to$ Pin 6 ($\overline{A_2}$).
  - Gate 4: Pin 9 ($A_3$) $\to$ Pin 8 ($\overline{A_3}$).

### 2.5 Multiplexer Bank Routing (IC7 - IC10: 74HCT151):
Connect common select lines across all 4 MUXes:
* Opcode $S_0 \to$ Pin 11 (A) of IC7, IC8, IC9, IC10.
* Opcode $S_1 \to$ Pin 10 (B) of IC7, IC8, IC9, IC10.
* Opcode $S_2 \to$ Pin 9 (C) of IC7, IC8, IC9, IC10.
* Tie Pin 7 ($\overline{E}$) to GND on all 4 MUXes.

Now connect the 8 data channels for each bit slice:

| MUX Input | Opcode | Function | IC7 (Bit 0) | IC8 (Bit 1) | IC9 (Bit 2) | IC10 (Bit 3) |
| :--- | :---: | :--- | :--- | :--- | :--- | :--- |
| **$D_0$ (Pin 4)** | `000` | ADD | IC1 Pin 9 ($\Sigma_1$) | IC1 Pin 6 ($\Sigma_2$) | IC1 Pin 2 ($\Sigma_3$) | IC1 Pin 1 ($\Sigma_4$) |
| **$D_1$ (Pin 3)** | `001` | SUB | IC1 Pin 9 ($\Sigma_1$) | IC1 Pin 6 ($\Sigma_2$) | IC1 Pin 2 ($\Sigma_3$) | IC1 Pin 1 ($\Sigma_4$) |
| **$D_2$ (Pin 2)** | `010` | AND | IC4 Pin 3 (AND 0) | IC4 Pin 6 (AND 1) | IC4 Pin 8 (AND 2) | IC4 Pin 11 (AND 3) |
| **$D_3$ (Pin 1)** | `011` | OR  | IC5 Pin 3 (OR 0) | IC5 Pin 6 (OR 1) | IC5 Pin 8 (OR 2) | IC5 Pin 11 (OR 3) |
| **$D_4$ (Pin 15)**| `100` | XOR | IC3 Pin 3 (XOR 0) | IC3 Pin 6 (XOR 1) | IC3 Pin 8 (XOR 2) | IC3 Pin 11 (XOR 3) |
| **$D_5$ (Pin 14)**| `101` | NOT | IC6 Pin 2 (NOT 0) | IC6 Pin 4 (NOT 1) | IC6 Pin 6 (NOT 2) | IC6 Pin 8 (NOT 3) |
| **$D_6$ (Pin 13)**| `110` | SHL | **GND (0)** | $A_0$ | $A_1$ | $A_2$ |
| **$D_7$ (Pin 12)**| `111` | SHR | $A_1$ | $A_2$ | $A_3$ | **GND (0)** |

Outputs appear at **Pin 5 (Y)** of each MUX:
- IC7 Pin 5 = $Y_{custom}[0]$
- IC8 Pin 5 = $Y_{custom}[1]$
- IC9 Pin 5 = $Y_{custom}[2]$
- IC10 Pin 5 = $Y_{custom}[3]$

### ✅ Testing Checklist 2:
- [ ] Connect temporary LEDs (with 330 $\Omega$ series resistors) to IC7..IC10 Pin 5 ($Y_{custom}[3:0]$).
- [ ] Connect manual test jumpers for $A[3:0]$, $B[3:0]$, and $S[2:0]$.
- [ ] **Test ADD (`000`)**: Set $A = 0101_2$ (5), $B = 0011_2$ (3). Verify output is $1000_2$ (8).
- [ ] **Test SUB (`001`)**: Set $A = 1001_2$ (9), $B = 0100_2$ (4). Verify output is $0101_2$ (5).
- [ ] **Test AND (`010`)**: Set $A = 1100_2$, $B = 1010_2$. Verify output is $1000_2$.
- [ ] **Test OR (`011`)**: Set $A = 1100_2$, $B = 1010_2$. Verify output is $1110_2$.
- [ ] **Test XOR (`100`)**: Set $A = 1100_2$, $B = 1010_2$. Verify output is $0110_2$.
- [ ] **Test NOT (`101`)**: Set $A = 1010_2$. Verify output is $0101_2$ (independent of B).
- [ ] **Test SHL (`110`)**: Set $A = 0011_2$. Verify output is $0110_2$.
- [ ] **Test SHR (`111`)**: Set $A = 1100_2$. Verify output is $0110_2$.

---

## Stage 3: 74LS181N ALU Integration (IC11) & Opcode Mapping

The `74LS181N` is a 24-pin DIP monolithic 4-bit ALU.

```
                   74LS181N (24-Pin DIP)
                     ┌───────∪───────┐
             /B0  1 ─┤               ├─ 24  VCC (+5V)
             /A0  2 ─┤               ├─ 23  /A1
              S3  3 ─┤               ├─ 22  /B1
              S2  4 ─┤               ├─ 21  /A2
              S1  5 ─┤   74LS181N    ├─ 20  /B2
              S0  6 ─┤               ├─ 19  /A3
             /Cn  7 ─┤               ├─ 18  /B3
               M  8 ─┤               ├─ 17  /G
             /F0  9 ─┤               ├─ 16  Cn+4 (Cout)
             /F1 10 ─┤               ├─ 15  /P
             /F2 11 ─┤               ├─ 14  A=B (Open Coll.)
             GND 12 ─┤               ├─ 13  /F3
                     └───────────────┘
```

### 3.1 IC11 Placement & Power:
* Place IC11 across the center groove on Breadboard 4.
* Connect **Pin 24 ($V_{CC}$)** to the $+5\text{V}$ power rail.
* Connect **Pin 12 (GND)** to the $\text{GND}$ power rail.
* Install a $0.1\ \mu\text{F}$ ceramic capacitor directly between Pin 24 and Pin 12.

### 3.2 74LS181 Function Mapping & Active-High Convention:
In positive/active-high logic:
- Pin 2 = $A_0$, Pin 1 = $B_0$, Pin 9 = $F_0$
- Pin 23 = $A_1$, Pin 22 = $B_1$, Pin 10 = $F_1$
- Pin 21 = $A_2$, Pin 20 = $B_2$, Pin 11 = $F_2$
- Pin 19 = $A_3$, Pin 18 = $B_3$, Pin 13 = $F_3$

To align the `74LS181N` with the 3-bit Opcode $S[2:0]$, configure control pins:

| System Opcode ($S_2 S_1 S_0$) | Desired Function | 74LS181 Control ($M, S_3 S_2 S_1 S_0$) | Carry-In ($\overline{C_n}$) | 74LS181 Result $F[3:0]$ |
| :---: | :--- | :---: | :---: | :--- |
| `000` | **ADD** ($A + B$) | $M=0$, $S_3..S_0 = 1001$ (`0x9`) | $\overline{C_n} = 1$ | $F = A + B$ |
| `001` | **SUB** ($A - B$) | $M=0$, $S_3..S_0 = 0110$ (`0x6`) | $\overline{C_n} = 0$ | $F = A - B$ |
| `010` | **AND** ($A \cdot B$) | $M=1$, $S_3..S_0 = 1011$ (`0xB`) | $\overline{C_n} = \text{X}$ | $F = A \cdot B$ |
| `011` | **OR** ($A + B$) | $M=1$, $S_3..S_0 = 1110$ (`0xE`) | $\overline{C_n} = \text{X}$ | $F = A + B$ |
| `100` | **XOR** ($A \oplus B$) | $M=1$, $S_3..S_0 = 0110$ (`0x6`) | $\overline{C_n} = \text{X}$ | $F = A \oplus B$ |
| `101` | **NOT** ($\overline{A}$) | $M=1$, $S_3..S_0 = 0000$ (`0x0`) | $\overline{C_n} = \text{X}$ | $F = \overline{A}$ |

> [!NOTE]
> For logical shifts (SHL, SHR) in Mode 1, the hardware equivalence comparator evaluates the arithmetic/logic operations directly.

### ✅ Testing Checklist 3:
- [ ] Connect Pin 24 to +5V and Pin 12 to GND. Check current draw with multimeter in series ($\approx 70\text{ mA} - 100\text{ mA}$ typical for 74LS bipolar).
- [ ] Test ADD: Apply $A = 0100_2$ (4), $B = 0010_2$ (2), $M=0, S=\text{1001}, C_n=1$. Verify outputs $F_3..F_0 = 0110_2$ (6).
- [ ] Test SUB: Apply $A = 0111_2$ (7), $B = 0011_2$ (3), $M=0, S=\text{0110}, C_n=0$. Verify outputs $F_3..F_0 = 0100_2$ (4).
- [ ] Test XOR: Apply $A = 1100_2, B = 1010_2, M=1, S=\text{0110}$. Verify outputs $F_3..F_0 = 0110_2$.

---

## Stage 4: 74HCT85 Hardware Comparator Integration (IC12)

The `74HCT85` 4-bit magnitude comparator performs **nanosecond-level physical hardware equivalence checking** between the Custom 4-Bit Discrete ALU and the historical `74LS181N` benchmark.

```
                 74HCT85 (16-Pin DIP)
                   ┌───────∪───────┐
               B3  1 ─┤               ├─ 16  VCC (+5V)
            I(A<B) 2 ─┤               ├─ 15  A3
            I(A=B) 3 ─┤   74HCT85      ├─ 14  B2
            I(A>B) 4 ─┤   4-Bit       ├─ 13  A2
            O(A>B) 5 ─┤ Comparator    ├─ 12  A1
            O(A=B) 6 ─┤               ├─ 11  B1
            O(A<B) 7 ─┤               ├─ 10  A0
              GND  8 ─┤               ├─  9  B0
                   └───────────────┘
```

### 4.1 Wiring the Comparator:
1. **Power**: Pin 16 to $+5\text{V}$, Pin 8 to $\text{GND}$, $0.1\ \mu\text{F}$ capacitor across Pins 16 & 8.
2. **Expansion Inputs** (Crucial for proper standalone operation):
   - **Pin 3 ($I_{A=B}$)**: Tie to **$+5\text{V}$**.
   - **Pin 2 ($I_{A<B}$)**: Tie to **$\text{GND}$**.
   - **Pin 4 ($I_{A>B}$)**: Tie to **$\text{GND}$**.
3. **Connect Inputs A (From Custom Discrete ALU)**:
   - Pin 10 ($A_0$) $\leftarrow$ IC7 Pin 5 ($Y_{custom}[0]$)
   - Pin 12 ($A_1$) $\leftarrow$ IC8 Pin 5 ($Y_{custom}[1]$)
   - Pin 13 ($A_2$) $\leftarrow$ IC9 Pin 5 ($Y_{custom}[2]$)
   - Pin 15 ($A_3$) $\leftarrow$ IC10 Pin 5 ($Y_{custom}[3]$)
4. **Connect Inputs B (From 74LS181N Silicon ALU)**:
   - Pin 9 ($B_0$) $\leftarrow$ IC11 Pin 9 ($F_0$)
   - Pin 11 ($B_1$) $\leftarrow$ IC11 Pin 10 ($F_1$)
   - Pin 14 ($B_2$) $\leftarrow$ IC11 Pin 11 ($F_2$)
   - Pin 1 ($B_3$) $\leftarrow$ IC11 Pin 13 ($F_3$)
5. **Silicon Match Indicator LED**:
   - Connect **Pin 6 ($O_{A=B}$)** to the anode of a high-efficiency **Green LED**.
   - Connect the cathode through a $330\ \Omega$ resistor to $\text{GND}$.
   - Connect Pin 6 also to Arduino UNO R4 WiFi pin `D12` for software-layer acquisition.

```
   IC12 Pin 6 (A=B Out) ────┬────[ 330Ω ]────►(Anode) [ 🟢 SILICON MATCH LED ] (Cathode) ──► GND
                            │
                            └────────────────► Arduino UNO R4 WiFi Pin D12
```

### ✅ Testing Checklist 4:
- [ ] Set inputs on both ALUs to matching values: $A = 0101_2, B = 0011_2$, Opcode = `000` (ADD).
- [ ] Verify both ALUs output $1000_2$ (8).
- [ ] Verify that IC12 Pin 6 asserts **$+4.8\text{V}$ to $+5.0\text{V}$** (HIGH) and the **Green Silicon Match LED lights up brightly**!
- [ ] Temporarily pull one input line of Custom ALU to GND (inject a deliberate mismatch).
- [ ] Verify that IC12 Pin 6 immediately drops to **$0.0\text{V}$** (LOW) and the Green Silicon Match LED extinguishes within nanoseconds.
- [ ] Restore normal wiring.

---

## Stage 5: Dual-Mode Switch Implementation (Mode 0 vs Mode 1)

The system reconfigures dynamically between **Mode 0** (Unified 8-Bit Cascaded ALU) and **Mode 1** (Silicon Verification Bench) via a single Mode Switch:

```
  MODE SWITCH = 0 : 8-Bit Cascaded ALU Processor Slice
  MODE SWITCH = 1 : Real-Time Silicon Hardware Verification
```

### 5.1 Mode Steering Architecture:

```
  When MODE = 0 (8-Bit Cascaded ALU):
  ------------------------------------
  * Inputs:
    - Custom 4-Bit ALU receives Lower Nibble: A[3:0], B[3:0]
    - 74LS181N receives Upper Nibble: A[7:4], B[7:4]
  * Carry Chain:
    - IC1 Pin 14 (C4 of 74LS83N) passes through inverter IC6 (Pin 11 5A -> Pin 10 5Y) to provide active-LOW /Cn to IC11 Pin 7 (74LS181)
  * Outputs:
    - Y[3:0] = Custom ALU Pin Y_custom[3:0]
    - Y[7:4] = 74LS181 Pin F[3:0]
    - Combined 8-bit output: Y[7:0] = { Y[7:4], Y[3:0] }
    - Carry-Out: IC11 Pin 16 (Cn+4) = Final 8-Bit Cout

  When MODE = 1 (Silicon Verification):
  ------------------------------------
  * Inputs:
    - Custom 4-Bit ALU receives: A[3:0], B[3:0]
    - 74LS181N receives IDENTICAL: A[3:0], B[3:0]
  * Comparator:
    - IC12 compares Y_custom[3:0] vs Y_181[3:0]
  * Outputs:
    - Green LED on IC12 Pin 6 verifies hardware silicon equivalence
```

### 5.2 Mode Switch Hookup:
* Use Switch 4 on SW3 (Control DIP Switch).
* Connect the top pin to $+5\text{V}$.
* Connect the bottom pin to a $10\text{ k}\Omega$ pull-down resistor to $\text{GND}$.
* This node is the **MODE signal**.
* Route **MODE** to:
  - Arduino UNO R4 WiFi pin `A3` (Mode sense).
  - ESP32-S3 GPIO `37` (via $4.7\text{ k}\Omega$ series override resistor).
  - Mode steering multiplexing lines.

### ✅ Testing Checklist 5:
- [ ] Switch to **Mode 0** ($0\text{V}$):
  - Apply $A = \text{0x25}$ ($0010\ 0101$), $B = \text{0x13}$ ($0001\ 0011$), Opcode = `000` (ADD).
  - Verify Lower Nibble produces $0101 + 0011 = 1000$ (`0x8`).
  - Verify Upper Nibble produces $0010 + 0001 = 0011$ (`0x3`).
  - Verify combined 8-bit result is **`0x38`**.
- [ ] Test Carry Cascade in **Mode 0**:
  - Apply $A = \text{0x0F}$ ($0000\ 1111$), $B = \text{0x01}$ ($0000\ 0001$), Opcode = `000` (ADD).
  - Lower nibble $1111 + 0001$ produces $0000$ with Carry-out $C_4 = 1$.
  - Carry-out ripples into Upper nibble $0000 + 0000 + 1 = 0001$.
  - Verify combined 8-bit result is **`0x10`** (16 decimal) — proves authentic 8-bit ripple carry!
- [ ] Switch to **Mode 1** ($+5\text{V}$):
  - Apply $A[3:0] = 7, B[3:0] = 5$, Opcode = `000` (ADD).
  - Verify Green Silicon Match LED illuminates.

---

## Stage 6: Arduino UNO R4 WiFi 12x8 LED Matrix Scoreboard & Status LEDs Hookup

The **Arduino UNO R4 WiFi** acts as the native 5V Software Golden Model Verifier and visual HUD.

```
                           Arduino UNO R4 WiFi
                     ┌──────────────∪──────────────┐
                     │ [12x8 LED MATRIX HUD]       │
                     │ Displays:                   │
                     │   '✓' (PASS Animated)       │
                     │   '✗' (FAIL Blinking)       │
                     │   Opcode & Test Counter     │
                     └─────────────────────────────┘
                                  │
      ALU Bus Pins (D2..D9)       │ Control & Indicators
      ┌────────────────────────┐  │  ┌────────────────────────┐
      │ D2  <── Y[0]           │  │  │ D10 ──► Green PASS LED │
      │ D3  <── Y[1]           │  │  │ D11 ──► Red FAIL LED   │
      │ D4  <── Y[2]           │  │  │ D12 <── IC12 Pin 6     │
      │ D5  <── Y[3]           │  │  │         (Silicon Match)│
      │ D6  <── Y[4]           │  │  │ A0  <── Opcode S0      │
      │ D7  <── Y[5]           │  │  │ A1  <── Opcode S1      │
      │ D8  <── Y[6]           │  │  │ A2  <── Opcode S2      │
      │ D9  <── Y[7]           │  │  │ A3  <── Mode Switch    │
      └────────────────────────┘  │  └────────────────────────┘
  (Note: D0/D1 RX/TX are reserved for USB serial monitor to prevent bus contention)
```

### 6.1 Wiring Connections to Arduino UNO R4 WiFi:
1. **Output Bus $Y[7:0]$ Sampling (Pins D2..D9)**:
   - Connect $Y_0$ (IC7 Pin 5) $\to$ Arduino `D2`
   - Connect $Y_1$ (IC8 Pin 5) $\to$ Arduino `D3`
   - Connect $Y_2$ (IC9 Pin 5) $\to$ Arduino `D4`
   - Connect $Y_3$ (IC10 Pin 5) $\to$ Arduino `D5`
   - Connect $Y_4$ (IC11 Pin 9 in Mode 0) $\to$ Arduino `D6`
   - Connect $Y_5$ (IC11 Pin 10 in Mode 0) $\to$ Arduino `D7`
   - Connect $Y_6$ (IC11 Pin 11 in Mode 0) $\to$ Arduino `D8`
   - Connect $Y_7$ (IC11 Pin 13 in Mode 0) $\to$ Arduino `D9`
2. **Control Inputs Sampling (Analog/Digital Pins A0..A3)**:
   - Connect Opcode $S_0 \to$ Arduino `A0`
   - Connect Opcode $S_1 \to$ Arduino `A1`
   - Connect Opcode $S_2 \to$ Arduino `A2`
   - Connect Mode Switch $\to$ Arduino `A3`
3. **Hardware Equivalence Match Input**:
   - Connect IC12 Pin 6 ($Q_{A=B}$) $\to$ Arduino `D12`
4. **External Status Indicators (Digital Outputs)**:
   - Arduino `D10` $\to [220\ \Omega] \to$ Green PASS LED Anode $\to$ Cathode to GND.
   - Arduino `D11` $\to [220\ \Omega] \to$ Red FAIL LED Anode $\to$ Cathode to GND.

### ✅ Testing Checklist 6:
- [ ] Open Arduino IDE, compile and flash `arduino_uno_r4_firmware.ino` to the Arduino UNO R4 WiFi.
- [ ] Open Serial Monitor at `115200 baud`. Verify live telemetry packets outputting sampled bus values.
- [ ] Run valid additions on the breadboard. Verify the onboard **12×8 LED Matrix displays an animated Checkmark (`✓`)** and the external **Green PASS LED illuminates**.
- [ ] Disconnect one bit wire to create a deliberate fault. Verify the LED matrix immediately displays a **Blinking Cross (`✗`)** and the external **Red FAIL LED illuminates**.
- [ ] Reconnect wire. Verify checkmark returns.

---

## Stage 7: ESP32-S3 Hybrid Input & Web Dashboard Integration

The **ESP32-S3 N16R8** runs the autonomous Wi-Fi Access Point, WebSocket telemetry server, and remote test-vector injection engine.

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

### 7.1 Hybrid Input Injection Hookup (4.7kΩ Override Resistors):
Connect each ESP32 output GPIO in series with a $4.7\text{ k}\Omega$ resistor to the input node between the DIP switch and the $10\text{ k}\Omega$ pull-down.

> [!IMPORTANT]
> **GROUND TRUTH GPIO MAPPING**: The contiguous GPIO assignment below aligns strictly with the Master Wiring Guide (`PIN_TO_PIN_WIRING_GUIDE.md`), the Master Netlist (`CIRCUIT_DIAGRAM.md`), and the ESP32 firmware (`pin_definitions.h`). It guarantees that software vector injection aligns with physical hardware buses without scrambling bits.

| Signal Name | DIP Switch Pin | ESP32-S3 GPIO | Series Override Resistor | Destination IC Pin(s) | Signal Function |
| :--- | :---: | :---: | :---: | :--- | :--- |
| **$A_0$** | SW1 Pin 1 | **GPIO 1**  | $4.7\text{ k}\Omega$ | IC1 Pin 10 ($A_1$), IC3 Pin 1, IC4 Pin 1, IC5 Pin 1, IC6 Pin 1, IC8 Pin 13 | Operand A Bit 0 (LSB) |
| **$A_1$** | SW1 Pin 2 | **GPIO 2**  | $4.7\text{ k}\Omega$ | IC1 Pin 8 ($A_2$), IC3 Pin 4, IC4 Pin 4, IC5 Pin 4, IC6 Pin 3, IC7 Pin 12, IC9 Pin 13 | Operand A Bit 1 |
| **$A_2$** | SW1 Pin 3 | **GPIO 4**  | $4.7\text{ k}\Omega$ | IC1 Pin 3 ($A_3$), IC3 Pin 9, IC4 Pin 9, IC5 Pin 9, IC6 Pin 5, IC8 Pin 12, IC10 Pin 13 | Operand A Bit 2 |
| **$A_3$** | SW1 Pin 4 | **GPIO 5**  | $4.7\text{ k}\Omega$ | IC1 Pin 15 ($A_4$), IC3 Pin 12, IC4 Pin 12, IC5 Pin 12, IC6 Pin 9, IC9 Pin 12 | Operand A Bit 3 (Lower MSB) |
| **$A_4$** | SW1 Pin 5 | **GPIO 6**  | $4.7\text{ k}\Omega$ | IC11 Pin 2 ($\overline{A_0}$, Mode 0), IC10 Pin 12 ($D_7$, Mode 0) | Operand A Bit 4 (Upper LSB) |
| **$A_5$** | SW1 Pin 6 | **GPIO 7**  | $4.7\text{ k}\Omega$ | IC11 Pin 19 ($\overline{A_1}$, Mode 0) | Operand A Bit 5 |
| **$A_6$** | SW1 Pin 7 | **GPIO 8**  | $4.7\text{ k}\Omega$ | IC11 Pin 21 ($\overline{A_2}$, Mode 0) | Operand A Bit 6 |
| **$A_7$** | SW1 Pin 8 | **GPIO 9**  | $4.7\text{ k}\Omega$ | IC11 Pin 23 ($\overline{A_3}$, Mode 0) | Operand A Bit 7 (Upper MSB) |
| **$B_0$** | SW2 Pin 1 | **GPIO 10** | $4.7\text{ k}\Omega$ | IC2 Pin 1 (1A), IC3 Pin 2, IC4 Pin 2, IC5 Pin 2, IC11 Pin 1 (Mode 1) | Operand B Bit 0 (LSB) |
| **$B_1$** | SW2 Pin 2 | **GPIO 11** | $4.7\text{ k}\Omega$ | IC2 Pin 4 (2A), IC3 Pin 5, IC4 Pin 5, IC5 Pin 5, IC11 Pin 20 (Mode 1) | Operand B Bit 1 |
| **$B_2$** | SW2 Pin 3 | **GPIO 12** | $4.7\text{ k}\Omega$ | IC2 Pin 9 (3A), IC3 Pin 10, IC4 Pin 10, IC5 Pin 10, IC11 Pin 22 (Mode 1) | Operand B Bit 2 |
| **$B_3$** | SW2 Pin 4 | **GPIO 13** | $4.7\text{ k}\Omega$ | IC2 Pin 12 (4A), IC3 Pin 13, IC4 Pin 13, IC5 Pin 13, IC11 Pin 24 (Mode 1) | Operand B Bit 3 (Lower MSB) |
| **$B_4$** | SW2 Pin 5 | **GPIO 14** | $4.7\text{ k}\Omega$ | IC11 Pin 1 ($\overline{B_0}$, Mode 0) | Operand B Bit 4 (Upper LSB) |
| **$B_5$** | SW2 Pin 6 | **GPIO 15** | $4.7\text{ k}\Omega$ | IC11 Pin 20 ($\overline{B_1}$, Mode 0) | Operand B Bit 5 |
| **$B_6$** | SW2 Pin 7 | **GPIO 16** | $4.7\text{ k}\Omega$ | IC11 Pin 22 ($\overline{B_2}$, Mode 0) | Operand B Bit 6 |
| **$B_7$** | SW2 Pin 8 | **GPIO 17** | $4.7\text{ k}\Omega$ | IC11 Pin 24 ($\overline{B_3}$, Mode 0) | Operand B Bit 7 (Upper MSB) |
| **$S_0$** | SW3 Pin 1 | **GPIO 18** | $4.7\text{ k}\Omega$ | IC7-IC10 Pin 11 ($S_0$), IC2 Pins 2,5,10,13, IC1 Pin 13 ($C_0$) | Opcode Select Bit 0 ($SUB$) |
| **$S_1$** | SW3 Pin 2 | **GPIO 35** | $4.7\text{ k}\Omega$ | IC7-IC10 Pin 10 ($S_1$) | Opcode Select Bit 1 |
| **$S_2$** | SW3 Pin 3 | **GPIO 36** | $4.7\text{ k}\Omega$ | IC7-IC10 Pin 9 ($S_2$) | Opcode Select Bit 2 (MSB) |
| **MODE**  | SW3 Pin 4 | **GPIO 37** | $4.7\text{ k}\Omega$ | Datapath Mode Steering Line (0 = 8-Bit, 1 = Verify) | Mode Select |

### 7.2 ESP32-S3 Output Bus & Telemetry Monitoring (1.0kΩ / 2.2kΩ Voltage Dividers):
To allow the 3.3V ESP32 to safely read the 5V signals, connect precision resistive dividers (1.0 kΩ series / 2.2 kΩ shunt to GND):

```
  5V Signal Line (Y_k / PASS) ────[ 1.0 kΩ ]────┬────► ESP32-S3 GPIO (Sense Pin)
                                                │
                                           [ 2.2 kΩ ]
                                                │
                                               GND
  V_sense = 5.0V * (2.2k / (1.0k + 2.2k)) = 5.0V * (2.2 / 3.2) = 3.43V (Safe for ESP32 3.3V I/O)
```

| Signal Name | Source Pin | ESP32-S3 GPIO | Divider Resistors | Architectural Function |
| :--- | :--- | :---: | :---: | :--- |
| **$Y_0$** | IC7 Pin 5 (Y) | **GPIO 38** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Custom ALU Output Bit 0 |
| **$Y_1$** | IC8 Pin 5 (Y) | **GPIO 39** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Custom ALU Output Bit 1 |
| **$Y_2$** | IC9 Pin 5 (Y) | **GPIO 40** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Custom ALU Output Bit 2 |
| **$Y_3$** | IC10 Pin 5 (Y) | **GPIO 41** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Custom ALU Output Bit 3 |
| **$Y_4$** | IC11 Pin 9 ($\overline{F_0}$) | **GPIO 42** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Upper Nibble / Silicon ALU Bit 0 Output |
| **$Y_5$** | IC11 Pin 10 ($\overline{F_1}$) | **GPIO 47** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Upper Nibble / Silicon ALU Bit 1 Output |
| **$Y_6$** | IC11 Pin 11 ($\overline{F_2}$) | **GPIO 48** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Upper Nibble / Silicon ALU Bit 2 Output |
| **$Y_7$** | IC11 Pin 13 ($\overline{F_3}$) | **GPIO 21** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | Upper Nibble / Silicon ALU Bit 3 Output |
| **Hardware PASS** | IC12 Pin 6 ($Q_{A=B}$) | **GPIO 3** | $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ | 74HCT85 Silicon Equivalence Match Flag |

### ✅ Testing Checklist 7:
- [ ] Flash `esp32_firmware.ino` to the ESP32-S3 via USB-C.
- [ ] Connect your laptop or smartphone to the Wi-Fi Access Point: **`DECA-ALU`** (or configured SSID).
- [ ] Open a browser and navigate to `http://192.168.4.1`.
- [ ] Verify that the **Web Dashboard** loads with live bus radices (Hex, Dec, Bin), interactive DIP controls, and verification score.
- [ ] Toggle a physical DIP switch on the breadboard; verify that the browser UI updates instantly over WebSockets.
- [ ] Trigger an **Automated Test Sweep (2,048 vectors)** from the Web Dashboard. Verify that the ESP32 injects vectors through the $4.7\text{ k}\Omega$ resistors, the breadboard executes the arithmetic, and the Arduino R4 matrix HUD validates 100% PASS with 0 mismatches!

---

## 🏁 Final System Commissioning Sign-Off

Upon completing Stages 1 through 7:
1. **Power**: Stable $+5.0\text{V}$ delivered by Arduino R4 Minima. Total idle current draw $\approx 180\text{ mA} - 250\text{ mA}$.
2. **Mode 0 (8-Bit Cascaded ALU)**: Full 8-bit parallel arithmetic with verified inter-nibble ripple carry ($C_4 \to \overline{C_n}$).
3. **Mode 1 (Silicon Equivalence)**: Real-time hardware comparison ($t_{pd} < 25\text{ ns}$) asserted by `74HCT85` with Green Silicon Match LED illuminated.
4. **Software Oracle**: Arduino UNO R4 WiFi golden model confirming 100% equivalence on onboard 12×8 LED Matrix HUD.
5. **IoT Telemetry**: ESP32-S3 responsive web dashboard streaming live packets and providing remote vector injection.
