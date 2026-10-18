# ⚡ 8-Bit / 4-Bit Configurable Discrete Logic ALU with Dual-MCU HIL Verification & IoT Dashboard

[![DECA Minor Project](https://img.shields.io/badge/Course-DECA%20Minor%20Project-blue.svg?style=for-the-badge)](https://github.com/)
[![Architecture](https://img.shields.io/badge/Architecture-74HCT%20Discrete%20Logic-orange.svg?style=for-the-badge)](https://github.com/)
[![Verifier](https://img.shields.io/badge/Verifier-Arduino%20UNO%20R4%20WiFi-teal.svg?style=for-the-badge)](https://github.com/)
[![Dashboard](https://img.shields.io/badge/Dashboard-ESP32--S3%20N16R8-red.svg?style=for-the-badge)](https://github.com/)
[![Status](https://img.shields.io/badge/Verification-Golden%20Model%20HIL-brightgreen.svg?style=for-the-badge)](https://github.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

**Course**: Digital Electronics & Computer Architecture (DECA) Minor Project  
**Author**: Shubh Kashyap  
**Institution**: Department of Electronics & Computer Engineering  

---

## 📖 Project Overview

This project presents the design, breadboard implementation, and automated Hardware-in-the-Loop (HIL) verification of an **8-Bit / Dual 4-Bit Configurable Arithmetic Logic Unit (ALU)** engineered **entirely from discrete 74HCT-series logic gates** (XOR, AND, OR, NOT) and 8:1 multiplexers. Built without relying on proprietary adder or commercial ALU ICs, the arithmetic datapath implements an authentic gate-level ripple-carry adder/subtractor with two's complement subtraction and a configurable carry-isolation gate for switching between single 8-bit and dual independent 4-bit operational modes. The system introduces an advanced **hybrid input architecture** pairing physical 8-position DIP switches with WebSocket-driven remote digital injection via an **ESP32-S3 N16R8** microcontroller over a standalone Wi-Fi Access Point. Automated real-time verification is performed by an **Arduino UNO R4 WiFi**, which continuously samples the physical 8-bit ALU output bus, executes an internal software golden model in real-time, displays PASS/FAIL diagnostics on its onboard 12×8 LED matrix, and drives external visual status indicators.

---

## 🏛️ System Architecture

The following ASCII block diagram illustrates the complete end-to-end signal flow—from hybrid input acquisition through discrete gate-level computation to dual-MCU verification and IoT telemetry:

```
 ======================== HYBRID INPUT STAGE ========================
 [ Physical DIP Switches ]             [ Remote Web Dashboard ]
   - Operand A[7:0]                      - WebSocket Injection
   - Operand B[7:0]                      - Testbench Automation
   - Opcode S[2:0]                                 |
   - Mode Switch (0=4-Bit, 1=8-Bit)                v
         |                              [ ESP32-S3 N16R8 GPIOs ]
         | (10k Pull-down)                         |
         +----------------<[ 4.7k Override Resistors ]<+
         |
         v
 ================= 74HCT GATE-LEVEL DISCRETE ALU ====================
         |
         +-----------------------+-----------------------+
         |                       |                       |
         v                       v                       v
 +-------------------+   +-------------------+   +-------------------+
 | 8-Bit Ripple-Carry|   |   Bitwise Logic   |   |   Hardwired Bit   |
 | Adder/Subtractor  |   |   Gating Array    |   |      Shifters     |
 |                   |   |                   |   |                   |
 | - 74HCT86 (XOR)   |   | - 74HCT08 (AND)   |   | - SHL (A << 1)    |
 | - 74HCT08 (AND)   |   | - 74HCT32 (OR)    |   | - SHR (A >> 1)    |
 | - 74HCT32 (OR)    |   | - 74HCT86 (XOR)   |   |                   |
 | - B Invert (SUB)  |   | - 74HCT04 (NOT)   |   |                   |
 +-------------------+   +-------------------+   +-------------------+
         | Carry C4              |                       |
         v                       |                       |
   [ Mode Gate ]                 |                       |
   (C4 AND Mode)                 |                       |
         |                       |                       |
         +-----------+-----------+-----------+-----------+
                     |                       |
                     v                       v
         +-----------------------------------------------+
         |       8x 74HCT151 8:1 Multiplexer Bank        |
         |   (Selects operation per bit via S[2:0])      |
         +-----------------------------------------------+
                                 |
                                 v  (8-Bit Parallel Output Bus + Flags)
 ======================== VERIFICATION & TELEMETRY ==================
                                 |
         +-----------------------+-----------------------+
         |                                               |
         | (Direct 5V Logic)                             | (1k / 2.2k Voltage Dividers)
         v                                               v
 +----------------------------------+    +----------------------------------+
 |      Arduino UNO R4 WiFi         |    |         ESP32-S3 N16R8           |
 |     (HIL Golden Model Oracle)    |    |     (IoT Telemetry & AP)         |
 |----------------------------------|    |----------------------------------|
 | - Reads A, B, Opcode, Mode, Y    |    | - Wi-Fi AP ("ALU-Testbench-AP")  |
 | - Computes algorithmic Y_exp     |    | - Asynchronous Web Server & WS   |
 | - Compares: (Y_hw == Y_exp)      |    | - Real-time Bus Telemetry HUD    |
 | - Onboard 12x8 LED Matrix HUD    |    | - Remote Signal Injection (4.7k) |
 | - Drives Green (PASS) / Red(FAIL)|    | - 2,048-Vector Sweep Engine      |
 +----------------------------------+    +----------------------------------+
```

---

## ✨ Features & Capabilities

- 🧩 **100% Discrete Gate-Level Datapath**: Complete 8-bit ripple-carry adder and subtractor constructed strictly from basic gates (74HCT86, 74HCT08, 74HCT32, 74HCT04)—no dedicated adder or commercial ALU ICs.
- ⚙️ **8 Full-Featured Operations**: Standard 3-bit instruction set executing ADD, SUB (two's complement), AND, OR, XOR, NOT, Logical Shift Left (SHL), and Logical Shift Right (SHR).
- 🔀 **Hardware Reconfigurable Topology (4-Bit / 8-Bit)**: A physical toggle switch gates the inter-nibble carry bit ($C_4$) via an AND gate:
  - **Mode = 1 (8-Bit Mode)**: Carry cascades seamlessly from lower to upper nibble for full 8-bit mathematical operations.
  - **Mode = 0 (Dual 4-Bit Mode)**: Carry propagation is severed, configuring the breadboard as two isolated 4-bit ALUs operating concurrently.
- 🎛️ **Hybrid Input with Safe GPIO Override**: Dual 8-position DIP switches provide hands-on manual control. Concurrently, ESP32-S3 digital outputs interface through 4.7 kΩ series resistors, enabling software testbenches to override physical switches without short-circuit risk.
- 🛡️ **Real-Time Embedded Golden Model Verifier**: Arduino UNO R4 WiFi reads the hardware output bus in real time, runs an exact software emulation of the expected operation, and asserts PASS (Green) or FAIL (Red) instantly.
- 📟 **Dynamic 12×8 LED Matrix Visualizer**: The Arduino UNO R4 onboard matrix renders visual feedback: animated checkmarks (`✓`) for verified calculations, fault indicators (`✗`) on failure, and scrolling operation names or output hex codes.
- 🌐 **Web Telemetry & IoT Dashboard**: The ESP32-S3 broadcasts a local Wi-Fi Access Point and serves an interactive web dashboard with virtual LEDs, real-time radix conversion (Hex, Dec, Bin), and automated test vector generators.
- ⚡ **Single-Supply 5V Architecture**: Regulated 5V from the Arduino UNO R4 USB supply powers the entire 74HCT breadboard array, with shared common ground and resistive voltage dividers (1 kΩ / 2.2 kΩ) protecting the ESP32-S3 3.3V inputs.

---

## 🗂️ Repository Structure & File Index

```
.
├── README.md                           # Master Project Documentation (This File)
├── hardware_docs/                      # Physical Breadboard Schematics & Wiring Guides
│   ├── PIN_TO_PIN_WIRING_GUIDE.md      # Benchtop pin-by-pin interconnection lookup
│   ├── CIRCUIT_DIAGRAM.svg             # Full high-resolution vector schematic
│   ├── CIRCUIT_DIAGRAM.md              # Detailed subsystem schematics & breadboard netlists
│   ├── PINOUTS_AND_SCHEMATICS.md       # Pinout reference, Boolean equations & MUX mappings
│   ├── BREADBOARD_ASSEMBLY_GUIDE.md    # Stage-by-stage assembly procedure with sanity checks
│   └── PERFBOARD_SOLDERING_GUIDE.md    # Dot-matrix perfboard migration & soldering manual
├── esp32_firmware/                     # ESP32-S3 N16R8 Firmware & Web Dashboard
│   ├── esp32_firmware.ino              # Wi-Fi AP, WebSocket server, GPIO telemetry & injection
│   ├── web_page.h                      # HTML5/CSS3/JavaScript responsive web interface
│   └── pin_definitions.h               # ESP32-S3 pin mapping & level-shifting assignments
├── arduino_uno_r4_firmware/            # Arduino UNO R4 WiFi Firmware
│   └── arduino_uno_r4_firmware.ino     # Golden Model HIL verifier & 12x8 LED matrix display
├── simulation/                         # Digital Logic Simulation
│   └── dual_mode_8bit_alu_processor.circ # Logisim Evolution model with registers & flag logic
└── project_report/                     # University Academic Documentation
    └── DECA_ALU_PROJECT_REPORT.md      # Formal minor project report & Viva-Voce defense guide
```

---

## 📦 Bill of Materials (BOM) — Gate-Level Implementation

| Category | Component / IC | Package | Qty | Functional Purpose |
| :--- | :--- | :---: | :---: | :--- |
| **Arithmetic / Logic** | **74HCT86** (Quad 2-Input XOR) | DIP-14 | **8** | Subtraction B-inversion (2 ICs) + Full Adder Sum generation (4 ICs) + Bitwise XOR logic (2 ICs) |
| **Arithmetic / Logic** | **74HCT08** (Quad 2-Input AND) | DIP-14 | **7** | Full Adder carry generation (4 ICs) + Bitwise AND logic (2 ICs) + Mode carry-gating (1 gate) |
| **Arithmetic / Logic** | **74HCT32** (Quad 2-Input OR) | DIP-14 | **4** | Full Adder carry propagation (2 ICs) + Bitwise OR logic (2 ICs) |
| **Logic Inverter** | **74HCT04** (Hex Inverter) | DIP-14 | **2** | Bitwise NOT operations ($A_7..A_0$) and signal buffering |
| **Operation Select** | **74HCT151** (8:1 Multiplexer) | DIP-16 | **8** | Bit-slice multiplexing: 1 MUX per bit selects among 8 operations into $Y[7:0]$ |
| **Bus Buffer (Opt)** | **74HCT245** (Octal Transceiver) | DIP-20 | **1** | Output bus line buffering and breadboard isolation |
| **Verification MCU** | **Arduino UNO R4 WiFi** | Board | **1** | 5V Golden Model verifier, 12×8 LED Matrix HUD, PASS/FAIL LED driver |
| **IoT Telemetry MCU** | **ESP32-S3 N16R8** | DevKit | **1** | Wi-Fi AP, WebSocket dashboard server, remote input injection |
| **Input Switches** | 8-Position DIP Switch | DIP-16 | **2** | Manual input for Operand $A[7:0]$ and Operand $B[7:0]$ |
| **Control Switches** | 4-Position DIP Switch | DIP-8 | **1** | Opcode select ($S_2, S_1, S_0$) and Mode switch ($0=4\text{-bit}, 1=8\text{-bit}$) |
| **Visual Indicators** | 5mm LEDs (Blue / Yellow) | Radial | **8** | Hardware ALU Output Data Bus ($Y_7..Y_0$) |
| **Status Indicators** | 5mm LEDs (Green / Red) | Radial | **2** | Visual Pass/Fail verification indicators (Green = PASS, Red = FAIL) |
| **Pull-down Resistors**| 10 kΩ, 1/4W Metal Film | Axial | **20** | Bus pull-downs for DIP switches and control lines |
| **Override Resistors** | 4.7 kΩ, 1/4W Metal Film | Axial | **16** | Series current-limiting override resistors for ESP32 input injection |
| **Voltage Dividers** | 1.0 kΩ & 2.2 kΩ, 1/4W | Axial | **16+16**| 5V $\to$ 3.3V voltage divider network for ESP32 GPIO protection |
| **Current Limiting** | 220 Ω / 330 Ω, 1/4W | Axial | **10** | Current-limiting resistors for output bus and status LEDs |
| **Decoupling Caps** | 0.1 µF (104 Ceramic) | Radial | **25** | High-frequency power rail decoupling across each IC $V_{CC}$/GND |
| **Prototyping** | Solderless Breadboards | 830-pt | **4** | Interlocked breadboard matrix for IC datapath |
| **Wiring** | 22 AWG Solid Core Wire | Spool | Assorted| Color-coded interconnects (Red=+5V, Black=GND, Blue=Data, Yellow=Control) |

---

## 📊 Operation Truth Table

The 74HCT151 multiplexer bank routes one of 8 internal operational results to the output bus $Y[7:0]$ based on the 3-bit Opcode $S[2:0]$.

| Opcode ($S_2 S_1 S_0$) | MUX Channel | Operation | Functional Definition | Arithmetic / Logic Details |
| :---: | :---: | :--- | :--- | :--- |
| `000` | $D_0$ | **ADD** | $Y = A + B$ | 8-bit ripple-carry addition ($C_{in0} = 0$, $B' = B$) |
| `001` | $D_1$ | **SUB** | $Y = A - B$ | Two's complement subtraction: $A + \overline{B} + 1$ ($C_{in0} = 1$, $B' = B \oplus 1$) |
| `010` | $D_2$ | **AND** | $Y = A \cdot B$ | Bitwise logical AND via 74HCT08 array |
| `011` | $D_3$ | **OR** | $Y = A + B$ | Bitwise logical OR via 74HCT32 array |
| `100` | $D_4$ | **XOR** | $Y = A \oplus B$ | Bitwise logical exclusive-OR via 74HCT86 array |
| `101` | $D_5$ | **NOT** | $Y = \overline{A}$ | Bitwise bit inversion of operand $A$ via 74HCT04 array |
| `110` | $D_6$ | **SHL** | $Y = A \ll 1$ | Logical shift left: $Y_i = A_{i-1}$, $Y_0 = 0$; $C_{out} = A_7$ |
| `111` | $D_7$ | **SHR** | $Y = A \gg 1$ | Logical shift right: $Y_i = A_{i+1}$, $Y_7 = 0$; $C_{out} = A_0$ |

### Mode Switch Configuration (Carry Gating)

The Mode Switch directly commands an AND gate located between the lower nibble carry-out ($C_4$) and the upper nibble carry-in ($C_{in4}$):

$$\text{Effective } C_{in4} = C_4 \cdot \text{MODE}$$

- **Mode = 1 (8-Bit Mode)**: The AND gate is transparent ($C_{in4} = C_4$). Carry ripples across all 8 bits from $A_0..B_0$ up through $A_7..B_7$, yielding an authentic 8-bit addition or subtraction result.
- **Mode = 0 (Dual 4-Bit Mode)**: The AND gate forces $C_{in4} = 0$. The lower nibble ($A[3:0], B[3:0]$) and upper nibble ($A[7:4], B[7:4]$) function as two independent 4-bit ALUs running in parallel.

---

## 🔬 How Real-Time Verification Works (Arduino Golden Model)

Rather than relying on obsolete hardware comparator chips, verification is handled by an **Arduino UNO R4 WiFi** acting as a high-speed **Hardware-in-the-Loop (HIL) Oracle**:

```
 [ Hardware Inputs: A, B, Opcode, Mode ] ------+
                       |                        |
                       v                        v
             [ Discrete 74HCT ALU ]   [ Arduino UNO R4 WiFi ]
                       |                        |
                       v                        v
             Hardware Output (Y_hw)      Software Golden Model
                       |                 (Y_exp = computeExpected(A, B, Op))
                       |                        |
                       +-------> [ == ? ] <-----+
                                    |
                    +---------------+---------------+
                    |                               |
              [ Match: PASS ]              [ Mismatch: FAIL ]
              - 🟢 Green LED ON             - 🔴 Red LED ON
              - 12x8 Matrix: '✓'            - 12x8 Matrix: '✗'
              - Serial: Y_hw == Y_exp       - Serial: Print Bit Fault
```

1. **Direct 5V Bus Sampling**: Because the Arduino UNO R4 operates with 5V-tolerant digital I/O, it directly connects to the 74HCT ALU output lines $Y[7:0]$, carry flags, opcode pins $S[2:0]$, and mode toggle without level shifting.
2. **Software Reference Execution**: In each loop cycle (~microsecond latency), the 32-bit Renesas RA4M1 ARM Cortex-M4 microprocessor samples the current operand and opcode lines and evaluates an idealized C++ algorithm:
   ```cpp
   uint8_t computeExpected(uint8_t a, uint8_t b, uint8_t op, bool mode, bool &c_out) {
       switch(op) {
           case 0: { // ADD
               uint16_t sum = (mode) ? (a + b) : ((a & 0x0F) + (b & 0x0F));
               c_out = (mode) ? (sum > 0xFF) : (sum > 0x0F);
               return (uint8_t)sum;
           }
           case 1: { // SUB
               uint16_t diff = (mode) ? (a - b) : ((a & 0x0F) - (b & 0x0F));
               c_out = (a >= b);
               return (uint8_t)diff;
           }
           case 2: return a & b;         // AND
           case 3: return a | b;         // OR
           case 4: return a ^ b;         // XOR
           case 5: return ~a;            // NOT
           case 6: return a << 1;        // SHL
           case 7: return a >> 1;        // SHR
           default: return 0;
       }
   }
   ```
3. **Equivalence Checking**: The hardware output $Y_{hw}$ is checked against $Y_{exp}$.
   - If identical: Asserts Digital Pin driving **Green PASS LED** and renders a checkmark (`✓`) on the 12×8 matrix.
   - If divergent: Asserts Digital Pin driving **Red FAIL LED**, displays a fault mark (`✗`), and prints exact bit mismatch diagnostics to the serial terminal.

---

## 🌐 How Hybrid Input Works (DIP + WebSocket Injection)

The architecture accommodates simultaneous tactile manual input and automated cloud/remote testing using a **non-destructive resistive priority circuit**:

```
 +5V Rail -------------------------+
                                   |
                             [ DIP Switch ]
                                   |
 IC Input Pin <--------------------+--------------------<[ 4.7k Resistor ]<--- ESP32 GPIO
                                   |
                            [ 10k Resistor ]
                                   |
 GND Rail -------------------------+
```

### Modes of Input Operation:
1. **Manual Tactile Mode (ESP32 in High-Impedance `INPUT` Mode)**:
   - When remote injection is inactive, all ESP32 override GPIOs are set to `pinMode(pin, INPUT)` ($Z \approx 10^{12}\,\Omega$).
   - The DIP switch controls the logic level completely:
     - DIP ON: Direct connection to $+5\text{V} \implies \text{Logic HIGH}$.
     - DIP OFF: Pulled to $\text{GND}$ through the $10\text{ k}\Omega$ resistor $\implies \text{Logic LOW}$.
2. **Remote WebSocket Injection Mode (ESP32 in `OUTPUT` Mode)**:
   - To inject test vectors remotely, the ESP32 drives its GPIO to `OUTPUT`:
     - **Injecting HIGH (3.3V)**: With DIP OFF, the $4.7\text{ k}\Omega$ resistor forms a divider with the $10\text{ k}\Omega$ pull-down:
       $$V_{in} = 3.3\text{ V} \times \frac{10\text{ k}\Omega}{4.7\text{ k}\Omega + 10\text{ k}\Omega} \approx 2.24\text{ V}$$
       Since $2.24\text{ V} > V_{IH,\min} (2.0\text{ V})$ for 74HCT logic, this reliably registers as a valid **Logic HIGH**.
     - **Injecting LOW (0V)**: If the DIP switch is accidentally left ON (+5V), current is limited to $I = \frac{5\text{ V}}{4.7\text{ k}\Omega} \approx 1.06\text{ mA}$, preventing any pin damage or power brownout.
   - This allows users to operate the ALU from a browser over Wi-Fi without having to touch physical switches.

---

## ⚡ Quick Start & Deployment Guide

### Step 1: Breadboard Hardware Assembly
1. Interlink four standard 830-tie-point breadboards and bridge the $+5\text{V}$ and $\text{GND}$ power rails across all boards.
2. Insert decoupling capacitors ($0.1\ \mu\text{F}$) across pins $V_{CC}$ and $\text{GND}$ of each 74HCT IC.
3. Wire the 1-bit full adders using 74HCT86 (XOR), 74HCT08 (AND), and 74HCT32 (OR) according to the [Breadboard Assembly Guide](hardware_docs/BREADBOARD_ASSEMBLY_GUIDE.md).
4. Connect the Mode Switch AND gate between bit 3 carry-out ($C_4$) and bit 4 carry-in ($C_{in4}$).
5. Mount the eight 74HCT151 8:1 MUXes, tie the address select lines ($S_2, S_1, S_0$) in parallel, and route the operation outputs to inputs $D_0..D_7$.
6. Wire the output bus to 8 Blue LEDs via $330\ \Omega$ current-limiting resistors.

### Step 2: Microcontroller Interfacing & Power
1. Connect the Arduino UNO R4 regulated $+5\text{V}$ pin to the breadboard $+5\text{V}$ rail, and connect common GND to all boards.
2. Route the 8-bit output bus directly to Arduino R4 digital pins `D2–D9`.
3. Connect the Arduino R4 status pins to the Green LED (Pin `D10`) and Red LED (Pin `D11`) via $220\ \Omega$ resistors.
4. Route the output bus to ESP32-S3 inputs through $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage dividers to step down 5V logic to 3.3V.
5. Connect ESP32-S3 remote override pins to the breadboard inputs through $4.7\text{ k}\Omega$ series resistors.

### Step 3: Flash Firmware
1. **Arduino UNO R4 WiFi**:
   - Open Arduino IDE $\to$ Select board **Arduino UNO R4 WiFi**.
   - Open `arduino_uno_r4_firmware/arduino_uno_r4_firmware.ino`.
   - Click **Upload**. The onboard 12×8 LED matrix will boot with a startup checkmark animation.
2. **ESP32-S3 N16R8**:
   - Select board **ESP32S3 Dev Module** (Flash Size: 16MB, PSRAM: OPI PSRAM).
   - Open `esp32_firmware/esp32_firmware.ino`.
   - Click **Upload**.

### Step 4: Verification & Dashboard Testing
1. Connect your smartphone or laptop to the Wi-Fi AP:
   - **SSID**: `ALU-Testbench-AP`
   - **Password**: `decaproject`
2. Open any web browser and navigate to `http://192.168.4.1`.
3. Toggle inputs on the web dashboard or flip physical DIP switches.
4. Observe the synchronized responses across breadboard LEDs, Arduino R4 matrix, and web telemetry HUD!

---

## 🛠️ Technology Stack

| Domain | Technologies & Components |
| :--- | :--- |
| **Logic Family** | High-Speed CMOS / TTL-Compatible (74HCT Series: 74HCT86, 74HCT08, 74HCT32, 74HCT04, 74HCT151, 74HCT245) |
| **Verification Controller** | Arduino UNO R4 WiFi (Renesas RA4M1 32-bit ARM Cortex-M4 @ 48 MHz + ESP32-S3 companion) |
| **Telemetry & IoT Node** | Espressif ESP32-S3 N16R8 (Xtensa Dual-Core 32-bit LX7 @ 240 MHz, 16MB Flash, 8MB PSRAM) |
| **Firmware Framework** | Embedded C++, Arduino Core, ESP-AsyncWebServer, WebSockets protocol |
| **Web Frontend** | Modern HTML5, Responsive CSS3 Grid/Flexbox, ES6 JavaScript, SVG Dynamic Indicators |
| **Simulation & Modeling**| Logisim Evolution v3.8+ (Behavioral and structural logic simulation with register transfer level) |
| **Hardware Prototyping** | High-density solderless breadboards, 22 AWG solid-core interconnects, precision metal-film resistors |

---

## 📜 Academic Integrity & License

This project was developed as a Minor Project for the **Digital Electronics & Computer Architecture (DECA)** curriculum. It is open-source under the [MIT License](LICENSE). Educational institutions and students are welcome to reference, modify, and build upon this architecture with appropriate attribution.
