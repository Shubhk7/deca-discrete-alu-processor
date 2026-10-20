# Perfboard Transfer & Soldering Guide (Dot-Matrix Zero-PCB)
## Laboratory-Grade Permanent Construction for 12-IC Dual-Mode ALU Processor

---

## 📖 Executive Summary & Engineering Objective

Transferring your verified breadboard circuit to a permanent perfboard (dot-matrix zero-PCB) converts a temporary prototype into a rugged, noise-immune, laboratory-grade demonstration unit suitable for academic exhibition, thesis presentation, and long-term durability.

This guide provides an exhaustive engineering manual for transferring the **12-IC Dual-Mode Hybrid Discrete & Silicon ALU Processor** to a standard 2.54 mm (0.1") pitch FR-4 fiberglass perfboard. It includes the exact DIP socket catalog, modular component placement floorplans, heavy-gauge power routing grids, disciplined signal routing protocols, and a rigorous multi-stage pre-power smoke test checklist.

---

## ⚡ 1. Golden Rules of Perfboard Assembly

> [!CAUTION]
> **RULE 1: NEVER SOLDER ICs DIRECTLY TO THE PERFBOARD!**
> Always solder high-quality Dual In-line Package (DIP) IC sockets first. Solder heat can easily degrade or permanently destroy bipolar and CMOS gate junctions. Sockets also allow immediate replacement of any chip during commissioning or troubleshooting.

> [!IMPORTANT]
> **RULE 2: STRICT UNIFORM NOTCH ORIENTATION**
> Orient the indexing notch on every single DIP socket in the exact same direction (all facing **LEFT**). Inconsistent socket orientations are the single leading cause of inverted chip insertion and instantaneous component destruction during final assembly.

> [!TIP]
> **RULE 3: LOW-IMPEDANCE POWER GRIDS & DECOUPLING**
> TTL logic chips—especially bipolar processors like the `74LS181N`—draw substantial current transients during state transitions. Use heavy 20–22 AWG bare tinned copper wire for the $+5\text{V}$ and $\text{GND}$ rails. Solder a $0.1\ \mu\text{F}$ monolithic ceramic capacitor directly between the $V_{CC}$ and $\text{GND}$ pins of every socket.

> [!NOTE]
> **RULE 4: TWO-SIDED DISCIPLINED SIGNAL ROUTING**
> - **Top Side (Component Side)**: Sockets, DIP switches, resistors, capacitors, headers, and LEDs.
> - **Bottom Side (Solder Side)**: Bare bus wire power grids and point-to-point insulated 30 AWG Kynar (PVDF) signal wiring. Route wiring in neat orthogonal channels to prevent bird's-nest tangling.

---

## 📦 2. Exact DIP Socket List & Hardware Bill of Materials

### 2.1 Complete DIP IC Socket Inventory (12 Sockets Total):

| Socket Type | Width / Pitch | Qty | Target IC Designator | Functional Role in Circuit |
| :---: | :---: | :---: | :--- | :--- |
| **24-Pin DIP Socket** | **600-mil (0.6") Wide** | **1** | **IC11 (`74LS181N`)** | Upper 4-Bit ALU in Mode 0; Silicon Verification Benchmark in Mode 1 |
| **16-Pin DIP Socket** | **300-mil (0.3") Standard**| **1** | **IC1 (`74xx283`)**  | 4-Bit Binary Full Adder with Fast Carry (Custom ALU core) |
| **16-Pin DIP Socket** | **300-mil (0.3") Standard**| **1** | **IC12 (`74xx85`)**  | 4-Bit Magnitude Comparator (Hardware Equivalence $A=B$) |
| **16-Pin DIP Socket** | **300-mil (0.3") Standard**| **4** | **IC7, IC8, IC9, IC10 (`74xx151`)** | 8:1 Multiplexers for Bit Slices $Y_0, Y_1, Y_2, Y_3$ |
| **14-Pin DIP Socket** | **300-mil (0.3") Standard**| **2** | **IC2, IC3 (`74xx86`)** | IC2: Subtraction Inversion; IC3: Bitwise XOR Logic Array |
| **14-Pin DIP Socket** | **300-mil (0.3") Standard**| **1** | **IC4 (`74xx08`)**  | Quad 2-Input AND Gate (Bitwise AND Logic Array) |
| **14-Pin DIP Socket** | **300-mil (0.3") Standard**| **1** | **IC5 (`74xx32`)**  | Quad 2-Input OR Gate (Bitwise OR Logic Array) |
| **14-Pin DIP Socket** | **300-mil (0.3") Standard**| **1** | **IC6 (`74xx04`)**  | Hex Inverter (Bitwise NOT Logic Array + Control Inversion) |
| **TOTAL DIP SOCKETS** | — | **12** | — | **1× 24-Pin DIP, 6× 16-Pin DIP, 5× 14-Pin DIP** |

### 2.2 Mechanical Substrate & Board Size:
* **Recommended Perfboard**: Double-sided FR-4 high-density fiberglass perfboard with plated through-holes (tinned solder pads on 2.54 mm / 0.1" standard grid).
* **Recommended Dimensions**: Minimum $18\text{ cm} \times 24\text{ cm}$ (or $18\text{ cm} \times 30\text{ cm}$) to accommodate all 12 ICs, three development board headers, hybrid input resistor arrays, and output LED bars with comfortable routing alleys.

### 2.3 Passives, Connectors & Microcontroller Headers:
* **Decoupling Capacitors**: 14× $0.1\ \mu\text{F}$ monolithic ceramic capacitors (50V, X7R or Z5U, 2.54mm lead pitch).
* **Bulk Capacitors**: 2× $100\ \mu\text{F}$ 25V radial electrolytic capacitors (power input smoothing).
* **Pull-Down Resistors**: 20× $10\text{ k}\Omega$ 1/4W 1% metal film resistors (for DIP switches SW1, SW2, SW3).
* **Override Resistors**: 19× $4.7\text{ k}\Omega$ 1/4W 1% metal film resistors (ESP32 remote input injection).
* **Voltage Dividers**: 13× $1.0\text{ k}\Omega$ + 13× $2.2\text{ k}\Omega$ 1/4W 1% resistors (5V-to-3.3V ESP32 safety bus sniffers).
* **LED Current Limiters**: 12× $220\ \Omega$ or $330\ \Omega$ 1/4W metal film resistors.
* **Input Switches**:
  * 2× 8-position DIP switches (SW1 for Operand $A[7:0]$, SW2 for Operand $B[7:0]$).
  * 1× 4-position DIP switch (SW3 for Opcode $S_2, S_1, S_0$ and Mode Switch).
* **Interface Female Headers**:
  * 1× 2-pin screw terminal block (Main +5V and GND input from Arduino UNO R4 Minima Power Station).
  * 1× 1×10 female header strip (Arduino UNO R4 WiFi Output Bus $Y[7:0]$, $C_8$, GND).
  * 1× 1×8 female header strip (Arduino UNO R4 WiFi Control Pins: $S[2:0]$, Mode, Match, PASS, FAIL).
  * 2× 1×15 female header strips (ESP32-S3 GPIO injection and bus sniffing interface).
* **Indicators**:
  * 8× 3mm Blue LEDs (8-Bit Data Output Bus $Y[7:0]$).
  * 1× 3mm Amber LED (8-Bit Carry-Out $C_{out8}$).
  * 1× 5mm Bright Green LED (Arduino R4 Verifier PASS).
  * 1× 5mm Bright Red LED (Arduino R4 Verifier FAIL).
  * 1× 5mm High-Efficiency Green LED (IC12 Silicon Hardware Equivalence Match).

---

## 🗺️ 3. Perfboard Floorplan & Architectural Layout

The perfboard layout follows a clean, single-direction architectural dataflow:
$$\text{Inputs \& Overrides (Left)} \longrightarrow \text{Custom ALU Core (Center-Left)} \longrightarrow \text{MUXes \& Comparator (Center-Right)} \longrightarrow \text{74LS181N \& Verifier Interface (Right)}$$

```
+------------------------------------------------------------------------------------------------------------------------------------+
| [5V/GND TERMINAL] ============================== +5V MASTER BUS (20 AWG TINNED WIRE) ============================================= |
|                                                                                                                                    |
|  +--------------+  +-------------------------------+  +-------------------------------+  +-------------------------------+  +----+ |
|  | DIP SWITCH A |  | ROW 1: ARITHMETIC CORE        |  | ROW 2: BITWISE LOGIC          |  | ROW 3: OPERATION MULTIPLEXERS |  |LED | |
|  | A[7..0]      |  | - IC1: 74LS83N (4-Bit Adder)  |  | - IC3: 74HC86N (Bitwise XOR)  |  | - IC7:  74HCT151 (MUX Bit 0)   |  |BUS | |
|  | + 10k Pull-  |  |   [16-Pin DIP Socket]         |  |   [14-Pin DIP Socket]         |  |   [16-Pin DIP Socket]         |  |    | |
|  |   downs      |  |                               |  |                               |  | - IC8:  74HCT151 (MUX Bit 1)   |  |Y0..| |
|  |              |  | - IC2: 74HC86N (Sub Invert)   |  | - IC4: 74HCT08 (Bitwise AND)  |  |   [16-Pin DIP Socket]         |  |Y7  | |
|  |              |  |   [14-Pin DIP Socket]         |  |   [14-Pin DIP Socket]         |  +-------------------------------+  +----+ |
|  +--------------+  +-------------------------------+  +-------------------------------+                                     |🟢M | |
|                                                                                                                             |MATCH|
|  +--------------+  +-------------------------------+  +-------------------------------+  +-------------------------------+  +----+ |
|  | DIP SWITCH B |  | ROW 4: LOGIC COMBINE & NOT    |  | ROW 5: HARDWARE COMPARATOR    |  | ROW 6: SILICON BENCHMARK ALU  |  |🟢P | |
|  | B[7..0]      |  | - IC5: 74HCT32 (Bitwise OR)   |  | - IC12: 74HCT85 (4-Bit Comp)  |  | - IC11: 74LS181N (4-Bit ALU)  |  |PASS| |
|  | + 10k Pull-  |  |   [14-Pin DIP Socket]         |  |   [16-Pin DIP Socket]         |  |   [24-Pin DIP-600 Socket]     |  |🔴F | |
|  |   downs      |  |                               |  |                               |  |                               |  |FAIL| |
|  |              |  | - IC6: 74HCT04 (Hex Inverter) |  |                               |  |                               |  +----+ |
|  +--------------+  |   [14-Pin DIP Socket]         |  +-------------------------------+  +-------------------------------+         |
|                    +-------------------------------+                                                                               |
|  +--------------+                                                                                                                  |
|  | OPCODE & MODE|  [ESP32 VOLTAGE DIVIDER ARRAY]      [ESP32 OVERRIDE RESISTORS]      [ARDUINO R4 INTERFACE HEADERS]               |
|  | (SW3: 4-Pos) |  [13× 1.0kΩ / 2.2kΩ]                [19× 4.7kΩ RESISTORS]           [FEMALE HEADER STRIPS: D0-D13, A0-A3]        |
|  +--------------+                                                                                                                  |
|                                                                                                                                    |
| ============================== COMMON GROUND BUS RAIL (20 AWG TINNED WIRE) ======================================================== |
+------------------------------------------------------------------------------------------------------------------------------------+
```

---

## ⚡ 4. Power Grid & Star-Ground Bus Construction

A reliable discrete digital processor requires an ultra-low impedance power distribution network. High switching currents across 12 ICs can generate inductive ground bounce, causing false trigger states in the ripple adder and comparator.

```
       5V Power Input (Arduino R4 Minima)
                 │
                 ▼
         [ 2-Pin Screw Terminal ]
          ├── (+) +5V ───────────────────────────────────► Top Master +5V Bus (20 AWG Solid Copper)
          │                                                ├── Pin 16 / 14 / 24 of IC Sockets
          │                                                └── [0.1uF Decoupling Caps to GND]
          │
          └── (-) GND ───────────────────────────────────► Bottom Master GND Bus (20 AWG Solid Copper)
                                                           ├── Pin 8 / 7 / 12 of IC Sockets
                                                           ├── 10k Pull-down Star Returns
                                                           └── MCU Common Ground Header Pins
```

### Power Grid Construction Steps:
1. **Perimeter Power Rails**:
   - Run two continuous, uninsulated 20 AWG solid tinned copper wires: one along the top perimeter for $+5\text{V}$, and one along the bottom perimeter for $\text{GND}$.
   - Solder each rail every 5 holes to the perfboard pads for mechanical rigidity.
2. **Bulk Capacitance Placement**:
   - Solder a $100\ \mu\text{F}$ electrolytic capacitor directly adjacent to the 2-pin power terminal block (positive leg to $+5\text{V}$, negative striped leg to $\text{GND}$).
   - Solder the second $100\ \mu\text{F}$ capacitor at the far right edge of the board near the power-hungry `74LS181N`.
3. **Decoupling Capacitors**:
   - Solder a $0.1\ \mu\text{F}$ ceramic capacitor directly across the diagonal power pins of each socket footprint:
     * 14-Pin Sockets (IC2, IC3, IC4, IC5, IC6): Between Pin 14 ($V_{CC}$) and Pin 7 (GND).
     * 16-Pin Sockets (IC1, IC7..IC10, IC12): Between Pin 16 ($V_{CC}$) and Pin 8 (GND).
     * 24-Pin Socket (IC11): Between Pin 24 ($V_{CC}$) and Pin 12 (GND).
   - Keep capacitor leads as short as physically possible ($\le 3\text{ mm}$) to minimize parasitic inductance.

---

## 🔌 5. Point-to-Point Signal Routing & Wire Color Standards

To maintain total visual traceability and simplify debugging, use 30 AWG insulated Kynar / PTFE wire-wrap wire with the following standardized color code:

| Wire Color | Signal Classification | Example Connections |
| :---: | :--- | :--- |
| 🔴 **Red** | **+5V Power Rail** | Terminal block, socket $V_{CC}$ pins, pull-up nodes |
| ⚫ **Black** | **0V Ground Reference** | Terminal block, socket GND pins, pull-down resistors, MUX enable $\overline{E}$ |
| 🔵 **Blue** | **Operand A Data Bus** | DIP switch SW1 ($A[7:0]$), inputs to adder, logic gates, and 74LS181 |
| 🟢 **Green** | **Operand B Data Bus** | DIP switch SW2 ($B[7:0]$), inputs to XOR inversion IC2, logic gates, 74LS181 |
| 🟡 **Yellow** | **Opcode & Control Lines**| Opcode $S_0, S_1, S_2$, Mode Switch, MUX select lines $A, B, C$ |
| ⚪ **White** | **Intermediate Gate Signals**| IC2 $B'$ outputs to IC1 adder, gate outputs to MUX $D_0..D_7$ channels |
| 🟣 **Purple** | **Result Bus $Y[7:0]$** | MUX outputs $Y[3:0]$ and 74LS181 outputs $F[3:0]$ to headers and LEDs |
| 🟠 **Orange** | **Verification & Flags** | Carry cascade $C_4$, $C_8$, IC12 Pin 6 ($A=B$), Arduino PASS/FAIL triggers |

### Solder Joint Quality Standards:
- **Temperature**: Maintain soldering iron tip at **$340^\circ\text{C} - 360^\circ\text{C}$** ($650^\circ\text{F} - 680^\circ\text{F}$).
- **Joint Profile**: Clean, concave shiny fillets covering the entire through-hole pad and component lead.
- **Dwell Time**: Apply heat for **1.5 to 2.5 seconds** per joint. Excess heat will delaminate perfboard copper pads.

---

## 🛠️ 6. Step-by-Step Soldering Sequence

Follow this disciplined 6-phase sequence from lowest-profile components to highest-profile components:

### Phase 1: Heavy Power Grid & Bulk Capacitors
1. Lay down the 20 AWG bare tinned copper rails for $+5\text{V}$ (top) and $\text{GND}$ (bottom).
2. Solder the 2-pin screw terminal block at the top-left board edge.
3. Install both $100\ \mu\text{F}$ bulk electrolytic capacitors (verify polarity: longer leg is positive).
4. Solder all fourteen $0.1\ \mu\text{F}$ ceramic decoupling capacitors at their respective socket positions.

### Phase 2: Passive Resistor Networks
1. Solder the twenty $10\text{ k}\Omega$ pull-down resistors vertically (hairpin bend) between the input switch lines and the GND rail.
2. Solder the nineteen $4.7\text{ k}\Omega$ series override resistors for the ESP32 input stage.
3. Solder the thirteen $1.0\text{ k}\Omega$ and thirteen $2.2\text{ k}\Omega$ resistors forming the 5V-to-3.3V voltage dividers.
4. Solder the twelve $220\ \Omega$ or $330\ \Omega$ LED current-limiting resistors.

### Phase 3: Sockets, Switches & Headers
1. **DIP Sockets Insertion**:
   - Insert all 12 DIP sockets into their designated coordinates, verifying that every single notch faces **LEFT**.
   - Tack solder pin 1 and the diagonally opposite pin of each socket to ensure flat, flush seating against the board.
   - Solder all remaining pins on all 12 sockets.
2. **DIP Switches**:
   - Insert SW1 (8-pos), SW2 (8-pos), and SW3 (4-pos). Solder all pins.
   - Connect the common top row of pins on all switches to the $+5\text{V}$ rail.
3. **Female Header Strips**:
   - Solder the interface headers for the Arduino UNO R4 WiFi and ESP32-S3 boards.
4. **Status LEDs**:
   - Solder the 8 Blue bus LEDs, 1 Amber carry LED, 1 Green match LED, 1 Green PASS LED, and 1 Red FAIL LED.

### Phase 4: Custom 4-Bit ALU Core Wiring (IC1 - IC10)
1. **IC2 (74xx86 Subtraction Conditioning)**:
   - Wire SUB control line to IC2 pins 2, 5, 10, 13.
   - Wire $B[0..3]$ to IC2 pins 1, 4, 9, 12.
   - Wire outputs $B'[0..3]$ (pins 3, 6, 8, 11) to IC1 (74LS83N) inputs (pins 11, 7, 4, 16).
   - Wire SUB control line also to IC1 pin 13 ($C_0$).
2. **IC1 (74xx283 Binary Adder)**:
   - Wire $A[0..3]$ to IC1 (74LS83N) pins 10, 8, 3, 15.
   - Wire sum outputs $\Sigma[0..3]$ (pins 9, 6, 2, 1) to MUX channels $D_0$ and $D_1$ on IC7..IC10.
3. **Logic Gate Arrays (IC3, IC4, IC5, IC6)**:
   - Wire $A[0..3]$ and $B[0..3]$ to IC3 (XOR), IC4 (AND), IC5 (OR), and IC6 (NOT) per the schematic.
   - Wire gate outputs to MUX channels:
     * IC4 AND outputs $\to D_2$ (Pin 2) of IC7..IC10.
     * IC5 OR outputs $\to D_3$ (Pin 1) of IC7..IC10.
     * IC3 XOR outputs $\to D_4$ (Pin 15) of IC7..IC10.
     * IC6 NOT outputs $\to D_5$ (Pin 14) of IC7..IC10.
4. **Multiplexers (IC7 - IC10: 74xx151)**:
   - Daisy-chain select lines: Opcode $S_0 \to$ Pin 11, $S_1 \to$ Pin 10, $S_2 \to$ Pin 9 on all 4 MUXes.
   - Tie Pin 7 ($\overline{E}$) to GND on all 4 MUXes.
   - Wire hardwired shifts:
     * IC7 (Bit 0): $D_6 = \text{GND}$, $D_7 = A_1$
     * IC8 (Bit 1): $D_6 = A_0$, $D_7 = A_2$
     * IC9 (Bit 2): $D_6 = A_1$, $D_7 = A_3$
     * IC10 (Bit 3): $D_6 = A_2$, $D_7 = \text{GND}$

### Phase 5: 74LS181N (IC11) & 74xx85 Comparator (IC12) Wiring
1. **IC11 (74LS181N 24-Pin DIP)**:
   - Connect Pin 24 to $+5\text{V}$, Pin 12 to $\text{GND}$.
   - Connect control lines ($M$, $S_3..S_0$, $\overline{C_n}$) per the opcode translation network.
   - Wire upper inputs $A[7:4]$ and $B[7:4]$ (or identical $A[3:0], B[3:0]$ in Mode 1) to IC11 pins 2, 23, 21, 19 and 1, 22, 20, 18.
   - Wire outputs $F[0..3]$ (pins 9, 10, 11, 13) to output bus lines $Y[4..7]$ and Comparator inputs $B[0..3]$.
2. **IC12 (74xx85 Comparator)**:
   - Connect Socket 1 (74LS83N) Pin 5 to $+5\text{V}$, Pin 12 to $\text{GND}$. Other 16-pin sockets connect Pin 16 to $+5\text{V}$, Pin 8 to $\text{GND}$.
   - Tie Pin 3 ($I_{A=B}$) to $+5\text{V}$; tie Pins 2 ($I_{A<B}$) and 4 ($I_{A>B}$) to $\text{GND}$.
   - Connect Inputs A ($A_0..A_3$, pins 10, 12, 13, 15) to Custom ALU outputs $Y_{custom}[0..3]$ (IC7..IC10 Pin 5).
   - Connect Inputs B ($B_0..B_3$, pins 9, 11, 14, 1) to 74LS181 outputs $F[0..3]$ (IC11 pins 9, 10, 11, 13).
   - Wire Pin 6 ($O_{A=B}$) to the Green Match LED and to Arduino header pin `D12`.

### Phase 6: Mode Switch & External Interconnects
1. Wire Mode Switch (SW3 Pin 4) to steering lines and Arduino pin `A3`.
2. Connect IC1 Pin 14 ($C_4$) to 74LS181 Pin 7 ($\overline{C_n}$) for Mode 0 ripple carry.
3. Wire output bus lines $Y[7:0]$ to LED indicators, Arduino header pins `D0..D7`, and ESP32 voltage divider inputs.

---

## 🔍 7. Pre-Power Continuity, Smoke Test & Commissioning Checklist

Never apply power to a newly soldered board without executing every step of this rigorous verification protocol:

### Step 1: Visual Macro Inspection (Magnifier / Microscope)
- [ ] Inspect every solder joint under bright lighting and $5\times$ to $10\times$ magnification.
- [ ] Verify that no adjacent solder pads are bridged by excess solder, whiskers, or wire strands.
- [ ] Inspect the bottom side of the board for clipped wire leads or loose solder balls trapped between pads.
- [ ] Verify that **all 12 DIP sockets have their indexing notch facing LEFT**.

### Step 2: Unpowered Power Rail Impedance Test (Multimeter)
- [ ] Ensure **NO ICs are inserted into any socket**.
- [ ] Set digital multimeter to Resistance mode ($\Omega$).
- [ ] Place probes between the $+5\text{V}$ and $\text{GND}$ screw terminals.
- [ ] **Expected Reading**: Resistance should rise above **$100\text{ k}\Omega$** (or open circuit $\text{OL}$) as bulk capacitors charge.
- [ ] > [!CAUTION]
      > If resistance reads under $100\ \Omega$, there is a direct short circuit on the power grid. **DO NOT APPLY POWER.** Locate and eliminate the bridge before proceeding.

### Step 3: Continuity & Star-Ground Verification
- [ ] Set multimeter to Continuity mode (audible beep).
- [ ] Check continuity between the board $\text{GND}$ terminal and:
  - Pin 8 of IC1, IC7, IC8, IC9, IC10, IC12.
  - Pin 7 of IC2, IC3, IC4, IC5, IC6.
  - Pin 12 of IC11.
  - Arduino R4 GND header pin and ESP32 GND header pin.
  - Ground legs of all pull-down and divider resistors.
- [ ] Verify all test points beep continuously ($0.0\ \Omega$).

### Step 4: First Power-On Test (Empty Sockets Voltage Check)
- [ ] Connect the **Arduino UNO R4 Minima 5V Power Station** to the 2-pin screw terminal block.
- [ ] Measure DC voltage at the terminal block: must read **$+4.95\text{V} - +5.10\text{V}$**.
- [ ] Probe the $V_{CC}$ pin on every single empty DIP socket:
  - IC1 (74283): Pin 16 = $+5.0\text{V}$
  - IC2, IC3, IC4, IC5, IC6 (14-pin ICs): Pin 14 = $+5.0\text{V}$
  - IC7, IC8, IC9, IC10 (74151 MUXes): Pin 16 = $+5.0\text{V}$
  - IC11 (74LS181N): Pin 24 = $+5.0\text{V}$
  - IC12 (7485 Comparator): Pin 16 = $+5.0\text{V}$
- [ ] Probe the GND pin on every socket: must read **$0.00\text{V}$**.
- [ ] Disconnect power.

### Step 5: Progressive IC Insertion & Thermal Smoke Test
Insert ICs stage-by-stage to isolate any component issues:
1. **Insert IC1 through IC6** (Adder and Logic Gates).
   - Reapply power. Touch the top of each chip with your finger. Chips should remain completely cool ($< 35^\circ\text{C}$).
   - Measure power supply current: typical draw $\approx 5\text{ mA} - 15\text{ mA}$.
2. **Insert IC7 through IC10** (Multiplexers).
   - Reapply power. Verify all 4 MUXes stay cool.
3. **Insert IC12** (74xx85 Comparator).
   - Reapply power. Verify chip stays cool.
4. **Insert IC11** (74LS181N 24-Pin DIP).
   - Reapply power. The `74LS181N` is a bipolar device and will run lukewarm to the touch ($\approx 40^\circ\text{C} - 45^\circ\text{C}$), which is normal.
   - Total current draw should stabilize at **$180\text{ mA} - 250\text{ mA}$**.
   - > [!WARNING]
     > If any IC becomes burning hot within 3 seconds, cut power immediately! An inverted chip or shorted output is present.

### Step 6: Full Hardware Commissioning Sign-Off
- [ ] **Mode 1 Silicon Match**: Set matching inputs on both ALUs. Verify the **Green Silicon Match LED (IC12 Pin 6) illuminates**.
- [ ] **Mode 0 8-Bit Ripple Cascade**: Set $A = \text{0x0F}, B = \text{0x01}$, Opcode = ADD (`000`). Verify output bus displays **`0x10`** on the Blue LED array.
- [ ] **Arduino UNO R4 WiFi Oracle**: Connect Arduino R4 headers. Verify the onboard **12×8 LED Matrix displays the animated Checkmark (`✓`)** and the external **Green PASS LED lights up**.
- [ ] **ESP32-S3 Web Dashboard**: Connect ESP32 headers. Connect to `DECA-ALU` Wi-Fi, open `http://192.168.4.1`, and execute an automated 2,048-vector sweep. Verify 100% test pass with 0 errors!

---

## 🏆 Final Certificate of Assembly

| Inspection Phase | Verification Parameter | Specification | Result | Inspector Sign-off |
| :--- | :--- | :---: | :---: | :---: |
| **Phase 1: Visual** | DIP Socket Notches & Soldering | All 12 facing LEFT, zero bridges | PASS [ ] | ________________ |
| **Phase 2: Electrical** | Power Bus Impedance (No ICs) | $R_{VCC-GND} > 100\text{ k}\Omega$ | PASS [ ] | ________________ |
| **Phase 3: Voltage** | Pin 14/16/24 $V_{CC}$ Supply | $+4.90\text{V} - +5.15\text{V}$ | PASS [ ] | ________________ |
| **Phase 4: Thermal** | IC Operating Temperatures | Cool to lukewarm ($< 48^\circ\text{C}$) | PASS [ ] | ________________ |
| **Phase 5: Equivalence** | 74xx85 Hardware Match Flag | Asserted HIGH (+5V) on match | PASS [ ] | ________________ |
| **Phase 6: HIL Oracle**| Arduino R4 12×8 Matrix HUD | 100% Validated Checkmark (`✓`)| PASS [ ] | ________________ |
| **Phase 7: IoT Telemetry**| ESP32-S3 Web Dashboard Sweep | 2,048 / 2,048 Vectors Passed | PASS [ ] | ________________ |
