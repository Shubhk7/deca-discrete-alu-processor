# Perfboard Transfer & Soldering Guide (Dot-Matrix Zero-PCB)
## Permanent Hardware Prototyping for 8-Bit / 4-Bit Gate-Level Discrete ALU with ESP32-S3 & Arduino R4

Transferring your verified breadboard circuit to a permanent perfboard (dot-matrix zero-PCB) transforms a prototype into a rugged, noise-immune, laboratory-grade demonstration unit suitable for university evaluation.

This guide provides complete floorplans, socket footprints, power distribution grids, signal routing guidelines, and pre-power validation protocols tailored specifically to the **8-Bit / 4-Bit Configurable Discrete Gate-Level ALU**.

---

## 1. Golden Rules of Perfboard Assembly

> [!CAUTION]
> **RULE 1: NEVER SOLDER ICs DIRECTLY TO THE PERFBOARD!**
> Always solder high-quality Dual In-line Package (DIP) IC sockets first. Solder heat can easily degrade or destroy CMOS gates. Sockets also allow effortless chip replacement in the event of an accidental short circuit or pin damage.

> [!IMPORTANT]
> **RULE 2: SEPARATE SIGNAL LAYERS BY COLOR & GUAGE**
> Use heavy 20–22 AWG bare tinned copper wire for the $+5\text{ V}$ and $\text{GND}$ power grids. Use insulated 30 AWG Kynar (PVDF) wire-wrap wire for point-to-point logic signals to prevent accidental shorts across dense solder pads.

> [!TIP]
> **RULE 3: UNIFORM NOTCH ORIENTATION**
> Orient the indexing notch on every single DIP socket in the exact same direction (facing **LEFT** or facing **UP**). Inconsistent socket orientations are the leading cause of backwards chip insertion during final assembly.

---

## 2. Hardware Materials & Socket Requirements

### 2.1 DIP IC Sockets:
* 20× **14-Pin DIP Sockets**:
  * 6× for 74HCT86 (Quad XOR)
  * 7× for 74HCT08 (Quad AND)
  * 5× for 74HCT32 (Quad OR)
  * 2× for 74HCT04 (Hex Inverter)
* 8× **16-Pin DIP Sockets**:
  * 8× for 74HCT151 (8:1 Multiplexers $Y_0$ through $Y_7$)
* 1× **20-Pin DIP Socket**:
  * 1× for 74HCT245 (Octal Bus Transceiver / Buffer)

### 2.2 Board Size & Mechanical Substrate:
* **Recommended Substrate**: High-grade FR-4 double-sided fiberglass perfboard with through-hole plated pads (0.1" / 2.54 mm standard grid spacing).
* **Dimensions**: One large $18\text{ cm} \times 30\text{ cm}$ board, or two interconnected medium $12\text{ cm} \times 18\text{ cm}$ boards linked via robust multi-pin header strips.

### 2.3 Passives, Connectors & Indicators:
* 29× $0.1\ \mu\text{F}$ monolithic ceramic capacitors (decoupling for each socket).
* 20× $10\text{ k}\Omega$ 1/4W resistors (DIP switch pull-downs).
* 19× $4.7\text{ k}\Omega$ 1/4W resistors (ESP32 remote input injection and protection).
* 13× $1.0\text{ k}\Omega$ + 13× $2.2\text{ k}\Omega$ 1/4W resistors (5V-to-3.3V ESP32 voltage dividers).
* 12× $220\ \Omega$ or $330\ \Omega$ 1/4W resistors (LED current limiters).
* 2× 8-position DIP switches (Operand $A$ and Operand $B$).
* 1× 4-position DIP switch (Opcode $S_0, S_1, S_2$ + Mode switch).
* 1× 40-pin breakable female header strip (for plugging in Arduino UNO R4 and ESP32-S3 DuPont connections).
* 8× 3mm Blue LEDs (Output bus), 1× Yellow LED (Carry-out), 1× Green LED (PASS), 1× Red LED (FAIL).

---

## 3. Perfboard Floorplan & Component Placement Layout

A disciplined architectural floorplan ensures that data flows cleanly from **Left (Inputs) $\to$ Center (Gate Arithmetic & Logic) $\to$ Right (Multiplexing, Driving & Output Scoreboard)**.

```
+----------------------------------------------------------------------------------------------------------------------------------------+
| [POWER JACK / 5V TERMINAL]  ============== +5V MASTER POWER BUS RAIL (20 AWG TINNED WIRE) ==============                             |
|                                                                                                                                        |
|  +--------------+  +--------------------------------+  +--------------------------------+  +--------------------------------+  +-----+ |
|  | DIP SWITCH A |  | ROW 1: LOWER NIBBLE ADDER      |  | ROW 2: UPPER NIBBLE ADDER      |  | ROW 3: OPERATION MULTIPLEXERS  |  | LED | |
|  | A[7..0]      |  | - 74HCT86 #1 (XOR Conditioning)|  | - 74HCT86 #2 (XOR Conditioning)|  | - 74HCT151 MUX 0 (Bit 0)       |  | BUS | |
|  | + 10k Pull-  |  | - 74HCT86 #3 (Half-Sums 0..3)  |  | - 74HCT86 #4 (Half-Sums 4..7)  |  | - 74HCT151 MUX 1 (Bit 1)       |  | Y0..| |
|  |   downs      |  | - 74HCT08 #1 (Carries 0..3)    |  | - 74HCT08 #2 (Carries 4..7)    |  | - 74HCT151 MUX 2 (Bit 2)       |  | Y7  | |
|  +--------------+  | - 74HCT32 #1 (Carry Merge 0..3)|  | - 74HCT32 #2 (Carry Merge 4..7)|  | - 74HCT151 MUX 3 (Bit 3)       |  +-----+ |
|                    +--------------------------------+  +--------------------------------+  +--------------------------------+  | 🟢  | |
|  +--------------+                                                                                                              | PASS| |
|  | DIP SWITCH B |  +--------------------------------+  +--------------------------------+  +--------------------------------+  | 🔴  | |
|  | B[7..0]      |  | ROW 4: BITWISE LOGIC GATES     |  | ROW 5: MODE & LOGIC GATES      |  | ROW 6: HIGH NIBBLE MULTIPLEXERS|  | FAIL| |
|  | + 10k Pull-  |  | - 74HCT08 #3 (AND bits 0..3)   |  | - 74HCT08 #5 (Mode AND Gates)  |  | - 74HCT151 MUX 4 (Bit 4)       |  +-----+ |
|  |   downs      |  | - 74HCT08 #4 (AND bits 4..7)   |  | - 74HCT32 #5 (Mode OR Gate)    |  | - 74HCT151 MUX 5 (Bit 5)       |  +-----+ |
|  +--------------+  | - 74HCT32 #3 (OR bits 0..3)    |  | - 74HCT04 #1 (Inverter / NOT)  |  | - 74HCT151 MUX 6 (Bit 6)       |          |
|                    | - 74HCT32 #4 (OR bits 4..7)    |  | - 74HCT04 #2 (Inverter / NOT)  |  | - 74HCT151 MUX 7 (Bit 7)       |  +-----+ |
|  +--------------+  | - 74HCT86 #5 (XOR bits 0..3)   |                                   +--------------------------------+  |     | |
|  | OPCODE & MODE|  | - 74HCT86 #6 (XOR bits 4..7)   |                                   | 74HCT245 BUS TRANSCEIVER       |  | BUF | |
|  | SWITCHES     |  +--------------------------------+                                   +--------------------------------+  +-----+ |
|  +--------------+                                                                                                                      |
|  [ESP32 OVERRIDE]  [ESP32 VOLTAGE DIVIDER ARRAY: 1kΩ / 2.2kΩ]                          [ARDUINO R4 INTERFACE FEMALE HEADERS]         |
|  [4.7k RESISTORS]                                                                                                                      |
|                                                                                                                                        |
| ============================== COMMON GROUND BUS RAIL (20 AWG TINNED WIRE) =========================================================== |
+----------------------------------------------------------------------------------------------------------------------------------------+
```

---

## 4. Power Bus & Ground Grid Routing

Discrete TTL/CMOS circuits containing 29 ICs draw significant current when multiple gates transition simultaneously. Poor power routing creates ground loops, voltage sag, and switching spikes that corrupt ripple-carry propagation.

```
       +5V Supply ───────┬────────────────────────────────────────────────────────┐
                         │                                                        │
                    +5V Bus Wire                                             +5V Bus Wire
                         │                                                        │
                         ├── Pin 14/16/20 (VCC)                   Pin 14/16/20 (VCC) ─┤
                         │                                                        │
                      [ 0.1µF ] Decoupling                     [ 0.1µF ] Decoupling
                         │                                                        │
                         ├── Pin 7/8/10 (GND)                      Pin 7/8/10 (GND) ──┤
                         │                                                        │
                    GND Bus Wire                                             GND Bus Wire
                         │                                                        │
       Common Ground ────┴────────────────────────────────────────────────────────┘
```

1. **Heavy Bus Rails**: Run uninsulated 20 AWG solid tinned wire along the perimeter and between each row of sockets. Solder the wire to the perfboard pads every 5 holes for structural rigidity.
2. **Dedicated Bypass Capacitors**: Solder one $0.1\ \mu\text{F}$ ceramic capacitor directly across the $V_{CC}$ and $\text{GND}$ pins on every single socket. The leads must be as short as physically possible ($< 5\text{ mm}$).
3. **Star Grounding**: Connect the ground return of the LED display array and the Arduino/ESP32 headers back to the primary power input terminal rather than daisy-chaining through sensitive logic ground traces.

---

## 5. Signal Routing & Carry-Chain Optimization

### 5.1 The Critical Carry Chain:
The ripple-carry path through the 8-bit adder is the **critical timing and noise path** of your entire system. A glitch on $C_{in4}$ or $C_{out3}$ can produce false results during additions and subtractions.

* **Keep Carry Traces Ultra-Short**: Place the XOR, AND, and OR sockets for bit 0 directly adjacent to bit 1, bit 1 adjacent to bit 2, etc. The carry-out pin of stage $i$ should solder directly across 2–4 board holes to the carry-in pin of stage $i+1$.
* **Avoid Wire Crossings over the Carry Bus**: Never route high-speed clock lines, PWM signals, or Wi-Fi antenna traces directly over the ripple-carry chain.

### 5.2 Multiplexer Operation Bus Distribution:
The eight 74HCT151 MUXes share three common select lines ($S_0, S_1, S_2$) and the enable line ($\overline{EN}$).
* **Daisy-Chain the Control Bus**: Solder four parallel color-coded Kynar wires connecting Pin 11 ($S_0$), Pin 10 ($S_1$), Pin 9 ($S_2$), and Pin 7 ($\overline{EN}$ tied to GND) from MUX 0 across to MUX 7.
* **Shift Operation Routing (Zero-Silicon Logic)**:
  * Route bit $i-1$ of Operand $A$ directly to Pin 13 ($D_6$) of MUX $i$ for **Shift Left (SHL)**. Solder Pin 13 of MUX 0 to GND.
  * Route bit $i+1$ of Operand $A$ directly to Pin 12 ($D_7$) of MUX $i$ for **Shift Right (SHR)**. Solder Pin 12 of MUX 7 to GND.

### 5.3 Wire Color-Coding Scheme:
To make troubleshooting painless, maintain strict adherence to this wiring palette:

| Wire Color | Recommended Gauge | Intended Signal Net |
| :--- | :---: | :--- |
| **Red** | 20 AWG Tinned / 24 AWG Insulated | Master +5V Power Rails |
| **Black** | 20 AWG Tinned / 24 AWG Insulated | Master Ground (0V) Rails |
| **Yellow** | 30 AWG Kynar | Carry Chain ($C_{in0} \dots C_{out8}$) & Mode Carry Gate |
| **Green** | 30 AWG Kynar | Operand $A[7:0]$ Input Lines |
| **White** | 30 AWG Kynar | Operand $B[7:0]$ and $B'[7:0]$ Conditioned Lines |
| **Orange** | 30 AWG Kynar | Opcode Select Bus ($S_0, S_1, S_2$) and Mode Switch |
| **Blue** | 30 AWG Kynar | ALU Multiplexed Output Bus ($Y_0 \dots Y_7$) |
| **Purple** | 30 AWG Kynar | ESP32 Remote Override Lines & Divider Networks |

---

## 6. Step-by-Step Soldering Execution Plan

Follow this chronological soldering sequence to prevent tight-space soldering collisions:

### Phase 1: Power Bus Rails & Socket Mechanical Mounts
1. Insert all 29 DIP sockets into the perfboard matching the floorplan.
2. Ensure every socket indexing notch points towards the **LEFT**.
3. On the bottom (solder) side, bend two diagonally opposite corner pins slightly outward ($30^\circ$) to secure each socket mechanically.
4. Solder all socket corner pins first, inspect alignment, then solder the remaining pins.
5. Solder the bare 20 AWG $+5\text{ V}$ and $\text{GND}$ power rails.
6. Solder the $0.1\ \mu\text{F}$ decoupling capacitors across $V_{CC}$ and $\text{GND}$ of each socket.

### Phase 2: Input Controls, Pull-Downs & Hybrid Override Resistors
1. Solder the DIP switches on the left edge.
2. Solder the twenty $10\text{ k}\Omega$ pull-down resistors from the switch contacts to the $\text{GND}$ rail.
3. Solder the nineteen $4.7\text{ k}\Omega$ hybrid injection resistors between the DIP switch nodes and the ESP32 input female header pins.

### Phase 3: Gate-Level Full Adder & Subtractor
1. Wire the subtraction XOR array (74HCT86 #1 and #2) to condition $B_i \to B'_i$.
2. Solder the lower nibble adder gates (Bits 0..3).
3. Solder the Mode Toggle carry gate circuit ($C_{in4} = (C_{out3} \cdot \text{MODE}) + (SUB \cdot \overline{\text{MODE}})$).
4. Solder the upper nibble adder gates (Bits 4..7).
5. Route the final carry-out $C_{out8}$ to the Carry Yellow LED and telemetry header.

### Phase 4: Logic Gates & Multiplexer Array
1. Wire the bitwise AND (74HCT08), OR (74HCT32), XOR (74HCT86), and NOT (74HCT04) arrays from operands $A$ and $B$.
2. Solder the Opcode select bus daisy-chain across all eight 74HCT151 MUX sockets.
3. Solder the 8 operation candidate inputs ($D_0..D_7$) to each corresponding MUX socket pin.
4. Solder the MUX outputs ($Y_0..Y_7$) to the inputs of the 74HCT245 bus transceiver socket.

### Phase 5: ESP32 Voltage Dividers & Arduino Interface Headers
1. Build the thirteen $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ resistor divider pairs for ESP32 inputs.
2. Solder female header strips for connecting the ESP32-S3 and Arduino UNO R4 WiFi boards.
3. Wire the direct 5V connections between the 74HCT245 outputs and the Arduino R4 header pins (D2–D9).

### Phase 6: Output LEDs & Display Indicators
1. Solder the 8-bit output bus LEDs with $330\ \Omega$ current-limiting resistors to ground.
2. Solder the Green "PASS" LED (Arduino D12 via $220\ \Omega$) and Red "FAIL" LED (Arduino D13 via $220\ \Omega$).

---

## 7. The Pre-Power "Smoke Test" & Commissioning Protocol

> [!CAUTION]
> **DO NOT INSERT ANY ICs INTO THE SOCKETS YET!**
> Perform all electrical checks with the sockets completely empty.

```
       [ STEP 1: RESISTANCE CHECK ] ──► Must be Open (> 100 kΩ)
                    │
       [ STEP 2: APPLY +5V POWER  ] ──► Check Pin VCC / GND Voltages
                    │
       [ STEP 3: LEVEL SHIFTER    ] ──► Check Divider Nodes (< 3.45V)
                    │
       [ STEP 4: POWER OFF        ] ──► Carefully Seat All ICs
                    │
       [ STEP 5: FINAL POWER-ON   ] ──► Execute Verification Suite
```

### Step 1: Cold Resistance / Short-Circuit Test
* Set your multimeter to **Resistance Mode ($\Omega$)**.
* Probe between the $+5\text{ V}$ power bus and the $\text{GND}$ bus.
* **Pass Criteria**: Resistance must measure greater than $50\text{ k}\Omega$ (or Open Loop `OL`). If it reads $< 100\ \Omega$, there is a direct solder bridge between power and ground that must be cleared before applying power.

### Step 2: Empty-Socket Voltage Verification
* Plug in the +5V power supply to the master terminal.
* With all sockets empty, measure DC voltage with your multimeter:
  * Probe Pin 14 of all 14-pin sockets: **Must measure $+4.9\text{ V}$ to $+5.1\text{ V}$**.
  * Probe Pin 16 of all 16-pin sockets: **Must measure $+4.9\text{ V}$ to $+5.1\text{ V}$**.
  * Probe Pin 20 of the 74HCT245 socket: **Must measure $+4.9\text{ V}$ to $+5.1\text{ V}$**.
  * Probe Pin 7, Pin 8, and Pin 10 ground pins: **Must measure $0.00\text{ V}$**.

### Step 3: Voltage Divider Clamping Verification
* Temporarily tie the 5V bus to the 74HCT245 output socket pins.
* Measure the voltage at the intermediate divider points destined for the ESP32 GPIOs.
* **Pass Criteria**: Every point must measure between **$+3.2\text{ V}$ and $+3.45\text{ V}$**. No pin may exceed $+3.5\text{ V}$.

### Step 4: Seat All Integrated Circuits
* **Disconnect power completely**.
* Place the board on an anti-static mat.
* Carefully align and press each 74HCT chip into its corresponding socket, confirming that the orientation notch on every IC points towards the **LEFT** matching the socket.
* Double-check that no IC pin folded or bent underneath the package during insertion.

### Step 5: Live Commissioning & Verification
* Reapply power.
* Connect Arduino UNO R4 WiFi and ESP32-S3 N16R8.
* Toggle through all 8 opcodes using the manual DIP switches:
  * Confirm that the 8-bit bus LEDs illuminate correctly.
  * Confirm that the Arduino UNO R4 verifies each operation in software and lights the **Green PASS LED**.
  * Open the ESP32-S3 Web Dashboard on a phone or laptop and confirm live telemetry matches physical hardware down to the exact bit!
