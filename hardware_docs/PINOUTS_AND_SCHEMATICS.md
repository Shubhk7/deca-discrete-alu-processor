# Pinouts, Truth Tables, and Complete Interconnection Schematics
## 8-Bit / 4-Bit Configurable Discrete Gate-Level ALU with ESP32-S3 Telemetry & Arduino R4 Golden Verifier

This document provides exhaustive chip pinouts, logic truth tables, gate-level schematics, interface circuitry, and connection blueprints for the **8-Bit / 4-Bit Configurable Discrete ALU**. 

The design is constructed entirely from standard **74HCT-series discrete logic ICs** (quad logic gates and 8:1 multiplexers) with no dedicated monolithic arithmetic chips (no 74LS181, no 74LS283, and no 74LS85). Arithmetic verification is performed in real-time by an **Arduino UNO R4 WiFi** executing a software golden model, while an **ESP32-S3 N16R8** provides a Wi-Fi web dashboard and remote test-vector injection.

---

## 1. System Architectural Overview

```
                                [ MODE SWITCH ]
                         (1 = 8-Bit Mode, 0 = Dual 4-Bit)
                                        │
                                        ▼
             ┌─────────────────────────────────────────────────────┐
             │       CARRY-CHAIN GATING CIRCUIT (74HCT08/32)       │
             │   Cin4 = (Cout3 · MODE) + (SUB · NOT MODE)          │
             └──────────────┬───────────────────────────┬──────────┘
                            │                           │
  [ Inputs A7..A4, B7..B4 ] │                           │ [ Inputs A3..A0, B3..B0 ]
            │               ▼                           ▼           │
            │   ┌────────────────────────┐  ┌────────────────────────┐ │
            └──►│  UPPER NIBBLE ADDER    │  │  LOWER NIBBLE ADDER    │◄┘
                │  (4x 1-Bit Gate FAs)   │  │  (4x 1-Bit Gate FAs)   │
                │  74HCT86, 74HCT08/32   │  │  74HCT86, 74HCT08/32   │
                └───────────┬────────────┘  └───────────┬────────────┘
                            │ Carry Ripple Cout3        │
                            │ (gated by Mode Switch)    │
                            └───────────────────────────┘
                                        │
                                        ▼
                 ┌─────────────────────────────────────────────┐
                 │       8-BIT MULTIPLEXER ARRAY               │
                 │       (8x 74HCT151 8:1 Data Selectors)      │
                 │   Select lines: S2, S1, S0 (Opcode)         │
                 └──────────────────────┬──────────────────────┘
                                        │
                                        ▼
                 ┌─────────────────────────────────────────────┐
                 │    74HCT245 OCTAL BUS TRANSCEIVER / BUFFER  │
                 └──────────────────────┬──────────────────────┘
                                        │
                         8-Bit Output Bus Y[7:0]
                                        ├──────────────────────────────┐
                                        ▼                              ▼
                 ┌─────────────────────────────┐        ┌──────────────────────────────┐
                 │   ARDUINO UNO R4 WIFI       │        │   ESP32-S3 N16R8             │
                 │   - Direct 5V Bus Read      │        │   - 1kΩ / 2.2kΩ Level Shifts │
                 │   - Golden Model Verifier   │        │   - Web Dashboard & Socket   │
                 │   - 12x8 LED Matrix Display │        │   - 4.7kΩ DIP Override       │
                 │   - 🟢 PASS / 🔴 FAIL LEDs  │        └──────────────────────────────┘
                 └─────────────────────────────┘
```

---

## 2. Integrated Circuit (IC) Pinout Reference

All integrated circuits are Dual In-line Packages (DIP) viewed from above with the indexing notch oriented at the top. The **74HCT** family is chosen specifically for 5V operation with TTL-compatible input switching thresholds ($V_{IH(\min)} = 2.0\text{ V}$, $V_{IL(\max)} = 0.8\text{ V}$), enabling direct drive from 3.3V microcontrollers (ESP32-S3) while running on a single +5V supply.

### 2.1 74HCT86 (Quad 2-Input Exclusive-OR Gate) — 14 Pins
Used for: 1-bit Full Adder sum generation, subtraction operand inversion ($B \oplus SUB$), and bitwise XOR operation.

```
                  +-------V-------+
             1A - | 1          14 | - VCC (+5V)
             1B - | 2          13 | - 4B
             1Y - | 3          12 | - 4A
             2A - | 4          11 | - 4Y
             2B - | 5          10 | - 3B
             2Y - | 6           9 | - 3A
            GND - | 7           8 | - 3Y
                  +---------------+
```

* **VCC**: Pin 14 (+5.0 V)
* **GND**: Pin 7 (0 V)
* **Logic Function**: $Y = A \oplus B = A\overline{B} + \overline{A}B$
* **Pin Mapping Table**:
  | Gate | Input A | Input B | Output Y |
  | :---: | :---: | :---: | :---: |
  | Gate 1 | Pin 1 (1A) | Pin 2 (1B) | Pin 3 (1Y) |
  | Gate 2 | Pin 4 (2A) | Pin 5 (2B) | Pin 6 (2Y) |
  | Gate 3 | Pin 9 (3A) | Pin 10 (3B) | Pin 8 (3Y) |
  | Gate 4 | Pin 12 (4A) | Pin 13 (4B) | Pin 11 (4Y) |

---

### 2.2 74HCT08 (Quad 2-Input AND Gate) — 14 Pins
Used for: Full Adder carry generation ($A \cdot B$ and $Cin \cdot (A \oplus B)$), bitwise AND operation, and Mode gating.

```
                  +-------V-------+
             1A - | 1          14 | - VCC (+5V)
             1B - | 2          13 | - 4B
             1Y - | 3          12 | - 4A
             2A - | 4          11 | - 4Y
             2B - | 5          10 | - 3B
             2Y - | 6           9 | - 3A
            GND - | 7           8 | - 3Y
                  +---------------+
```

* **VCC**: Pin 14 (+5.0 V)
* **GND**: Pin 7 (0 V)
* **Logic Function**: $Y = A \cdot B$
* **Pin Mapping Table**:
  | Gate | Input A | Input B | Output Y |
  | :---: | :---: | :---: | :---: |
  | Gate 1 | Pin 1 (1A) | Pin 2 (1B) | Pin 3 (1Y) |
  | Gate 2 | Pin 4 (2A) | Pin 5 (2B) | Pin 6 (2Y) |
  | Gate 3 | Pin 9 (3A) | Pin 10 (3B) | Pin 8 (3Y) |
  | Gate 4 | Pin 12 (4A) | Pin 13 (4B) | Pin 11 (4Y) |

---

### 2.3 74HCT32 (Quad 2-Input OR Gate) — 14 Pins
Used for: Full Adder carry combining ($C_{out} = G + P\cdot C_{in}$), bitwise OR operation, and Mode multiplexing.

```
                  +-------V-------+
             1A - | 1          14 | - VCC (+5V)
             1B - | 2          13 | - 4B
             1Y - | 3          12 | - 4A
             2A - | 4          11 | - 4Y
             2B - | 5          10 | - 3B
             2Y - | 6           9 | - 3A
            GND - | 7           8 | - 3Y
                  +---------------+
```

* **VCC**: Pin 14 (+5.0 V)
* **GND**: Pin 7 (0 V)
* **Logic Function**: $Y = A + B$
* **Pin Mapping Table**:
  | Gate | Input A | Input B | Output Y |
  | :---: | :---: | :---: | :---: |
  | Gate 1 | Pin 1 (1A) | Pin 2 (1B) | Pin 3 (1Y) |
  | Gate 2 | Pin 4 (2A) | Pin 5 (2B) | Pin 6 (2Y) |
  | Gate 3 | Pin 9 (3A) | Pin 10 (3B) | Pin 8 (3Y) |
  | Gate 4 | Pin 12 (4A) | Pin 13 (4B) | Pin 11 (4Y) |

---

### 2.4 74HCT04 (Hex Inverter / NOT Gate) — 14 Pins
Used for: Bitwise NOT operation ($\overline{A}$), complement generation, and active-low signal inversion.

```
                  +-------V-------+
             1A - | 1          14 | - VCC (+5V)
             1Y - | 2          13 | - 6A
             2A - | 3          12 | - 6Y
             2Y - | 4          11 | - 5A
             3A - | 5          10 | - 5Y
             3Y - | 6           9 | - 4A
            GND - | 7           8 | - 4Y
                  +---------------+
```

* **VCC**: Pin 14 (+5.0 V)
* **GND**: Pin 7 (0 V)
* **Logic Function**: $Y = \overline{A}$
* **Pin Mapping Table**:
  | Inverter | Input A | Output Y |
  | :---: | :---: | :---: |
  | Inv 1 | Pin 1 (1A) | Pin 2 (1Y) |
  | Inv 2 | Pin 3 (2A) | Pin 4 (2Y) |
  | Inv 3 | Pin 5 (3A) | Pin 6 (3Y) |
  | Inv 4 | Pin 9 (4A) | Pin 8 (4Y) |
  | Inv 5 | Pin 11 (5A) | Pin 10 (5Y) |
  | Inv 6 | Pin 13 (6A) | Pin 12 (6Y) |

---

### 2.5 74HCT151 (8-to-1 Data Selector / Multiplexer) — 16 Pins
Used for: Operational selection. One 74HCT151 is allocated to each output bit ($Y_0$ through $Y_7$), allowing the 3-bit Opcode ($S_2, S_1, S_0$) to route one of eight operations to that output bit.

```
                  +-------V-------+
             D3 - | 1          16 | - VCC (+5V)
             D2 - | 2          15 | - D4
             D1 - | 3          14 | - D5
             D0 - | 4          13 | - D6
              Y - | 5          12 | - D7
             /Y - | 6          11 | - S0 (Select Bit 0 - LSB)
            /EN - | 7          10 | - S1 (Select Bit 1)
            GND - | 8           9 | - S2 (Select Bit 2 - MSB)
                  +---------------+
```

* **VCC**: Pin 16 (+5.0 V)
* **GND**: Pin 8 (0 V)
* **Enable ($\overline{EN}$ / Strobe, Pin 7)**: Tie permanently to **GND** (0 V) to enable the multiplexer outputs.
* **Select Lines ($S_2, S_1, S_0$)**: Pins 9, 10, 11 (Connected across all 8 MUXes in parallel).
* **Data Inputs ($D_0..D_7$)**: Pins 4, 3, 2, 1, 15, 14, 13, 12.
* **Outputs**:
  * **Pin 5 ($Y$)**: True active-high multiplexed output bit.
  * **Pin 6 ($\overline{Y}$)**: Inverted complementary output (leave unconnected or use for telemetry).

#### 74HCT151 Internal Multiplexing Truth Table:
| Enable ($\overline{EN}$) | $S_2$ | $S_1$ | $S_0$ | Selected Data Input | Output $Y$ (Pin 5) | ALU Operation Routed |
| :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **H** | X | X | X | None | **L** (Disabled) | MUX Inactive |
| **L** | 0 | 0 | 0 | $D_0$ (Pin 4) | $D_0$ | **ADD** ($A + B$) |
| **L** | 0 | 0 | 1 | $D_1$ (Pin 3) | $D_1$ | **SUB** ($A - B$) |
| **L** | 0 | 1 | 0 | $D_2$ (Pin 2) | $D_2$ | **AND** ($A \cdot B$) |
| **L** | 0 | 1 | 1 | $D_3$ (Pin 1) | $D_3$ | **OR** ($A + B$) |
| **L** | 1 | 0 | 0 | $D_4$ (Pin 15) | $D_4$ | **XOR** ($A \oplus B$) |
| **L** | 1 | 0 | 1 | $D_5$ (Pin 14) | $D_5$ | **NOT** ($\overline{A}$) |
| **L** | 1 | 1 | 0 | $D_6$ (Pin 13) | $D_6$ | **SHL** ($A \ll 1$) |
| **L** | 1 | 1 | 1 | $D_7$ (Pin 12) | $D_7$ | **SHR** ($A \gg 1$) |

---

### 2.6 74HCT245 (Octal Bus Transceiver with 3-State Outputs) — 20 Pins
Used for: Output bus isolation, high-current LED driving, and clean bus interfacing to external microcontrollers.

```
                  +-------V-------+
            DIR - | 1          20 | - VCC (+5V)
             A0 - | 2          19 | - /OE (Output Enable)
             A1 - | 3          18 | - B0
             A2 - | 4          17 | - B1
             A3 - | 5          16 | - B2
             A4 - | 6          15 | - B3
             A5 - | 7          14 | - B4
             A6 - | 8          13 | - B5
             A7 - | 9          12 | - B6
            GND - | 10         11 | - B7
                  +---------------+
```

* **VCC**: Pin 20 (+5.0 V)
* **GND**: Pin 10 (0 V)
* **Direction Control (DIR, Pin 1)**: Tie to **+5V** ($A \to B$ drive direction).
* **Output Enable ($\overline{OE}$, Pin 19)**: Tie to **GND** (0 V) to permanently enable bus drivers.
* **Channel Connections**:
  * Inputs $A_0..A_7$ (Pins 2 to 9) $\leftarrow$ Connect to MUX outputs $Y_0..Y_7$ (Pin 5 of each 74HCT151).
  * Outputs $B_0..B_7$ (Pins 18 to 11) $\rightarrow$ Connect to 8-Bit LED display bus and Arduino/ESP32 read lines.

---

## 3. Gate-Level Full Adder & Subtractor Schematics

Rather than relying on monolithic adder ICs, this architecture implements an explicit **gate-level ripple-carry adder** constructed from first principles using elementary XOR, AND, and OR gates.

### 3.1 1-Bit Full Adder / Subtractor Cell Theory

To support both addition and two's complement subtraction:
1. Each bit of operand $B$ is conditionally complemented: $B'_i = B_i \oplus SUB$.
   * When $SUB = 0$ (Addition): $B'_i = B_i \oplus 0 = B_i$.
   * When $SUB = 1$ (Subtraction): $B'_i = B_i \oplus 1 = \overline{B_i}$ (1's complement).
2. The initial carry-in $C_{in0}$ is tied to $SUB$:
   * For Addition ($SUB = 0$): $C_{in0} = 0 \implies \text{Result} = A + B$.
   * For Subtraction ($SUB = 1$): $C_{in0} = 1 \implies \text{Result} = A + \overline{B} + 1 = A - B$ (2's complement).

#### Boolean Equations for Bit $i$:
$$\begin{aligned}
B'_i &= B_i \oplus SUB \\
P_i &= A_i \oplus B'_i \quad \text{(Propagate / Half-Sum)} \\
S_i &= P_i \oplus C_{ini} = A_i \oplus B'_i \oplus C_{ini} \quad \text{(Full Sum)} \\
G_i &= A_i \cdot B'_i \quad \text{(Generate)} \\
K_i &= C_{ini} \cdot P_i \quad \text{(Carry Propagate Term)} \\
C_{outi} &= G_i + K_i = (A_i \cdot B'_i) + (C_{ini} \cdot (A_i \oplus B'_i))
\end{aligned}$$

#### ASCII Gate Schematic (Single 1-Bit Cell):

```
       B[i] ─────────────┐
                         ├───────[ 74HCT86 XOR ]──────── B'[i]
        SUB ─────────────┘
                           │
       A[i] ───────────────┼─────────────────────────┐
                           │                         │
                           ├─────[ 74HCT86 XOR ]─────┴─► P[i] ───┐
       B'[i] ──────────────┘                                     ├─────[ 74HCT86 XOR ]──────► Sum S[i]
                                                                 │                             (to MUX D0, D1)
     Cin[i] ─────────────────────────────────────────────────────┴────────────────────────┐
                                                                                          │
       A[i] ───────────────┐                                                              │
                           ├─────[ 74HCT08 AND ]─────► G[i] ────────┐                     │
      B'[i] ───────────────┘                                        │                     │
                                                                    ├─────[ 74HCT32 OR ]──┴──► Cout[i]
       P[i] ───────────────┐                                        │                           (to next Cin)
                           ├─────[ 74HCT08 AND ]─────► K[i] ────────┘
     Cin[i] ───────────────┘
```

* **Gate Requirements per 1-Bit Adder Cell**:
  * Subtraction Conditioning: 1 XOR gate
  * Sum Generation: 2 XOR gates
  * Carry Generation: 2 AND gates + 1 OR gate
  * **Total per bit**: 3 XOR + 2 AND + 1 OR.

---

### 3.2 4-Bit Ripple Carry Nibble Adder Cascade

Four 1-bit cells cascade directly through their carry pins to form a 4-bit nibble adder:

```
                  ┌──────────────┐
  Cin0 (=SUB) ───►│ 1-Bit Adder0 │──► Sum S0
  A0, B0 ────────►│              │──┐ Cout0
                  └──────────────┘  │
                                    ▼
                  ┌──────────────┐
                  │ 1-Bit Adder1 │──► Sum S1
  A1, B1 ────────►│              │──┐ Cout1
                  └──────────────┘  │
                                    ▼
                  ┌──────────────┐
                  │ 1-Bit Adder2 │──► Sum S2
  A2, B2 ────────►│              │──┐ Cout2
                  └──────────────┘  │
                                    ▼
                  ┌──────────────┐
                  │ 1-Bit Adder3 │──► Sum S3
  A3, B3 ────────►│              │──► Cout3 (Lower Nibble Carry-Out)
                  └──────────────┘
```

---

### 3.3 8-Bit Architecture with 4-Bit / 8-Bit Mode Toggle Gate

In traditional fixed adders, $C_{out3}$ wires directly to $C_{in4}$. In this configurable architecture, an **electronic Mode Switch** controls the carry propagation between the lower nibble (bits 0–3) and upper nibble (bits 4–7).

* **Mode 1 (8-Bit Unified Mode)**: $C_{in4} = C_{out3}$. Carries propagate across the entire 8-bit word.
* **Mode 0 (Dual Independent 4-Bit Mode)**: $C_{in4} = SUB$. The upper nibble behaves as an independent 4-bit adder/subtractor operating on $A[7:4]$ and $B[7:4]$ with initial carry $SUB$ ($0$ for ADD, $1$ for SUB).

#### Carry Gating Logic Equation:
$$C_{in4} = (C_{out3} \cdot \text{MODE}) + (SUB \cdot \overline{\text{MODE}})$$

#### Carry Gating Schematic:

```
                  Cout3 (from Bit 3) ────┐
                                         ├─────[ 74HCT08 AND #1 ]────┐
                   MODE (1=8-bit) ───────┤                           │
                                         │                           ├─────[ 74HCT32 OR ]──► Cin4 (to Bit 4)
             MODE ──[ 74HCT04 NOT ]──────┼─────┐                     │
                                         │     ├─[ 74HCT08 AND #2 ]──┘
              SUB ───────────────────────┴─────┘
```

* **When $\text{MODE} = 1$**: AND #1 output $= C_{out3}$, AND #2 output $= 0 \implies C_{in4} = C_{out3}$ (**Full 8-Bit ripple**).
* **When $\text{MODE} = 0$**: AND #1 output $= 0$, AND #2 output $= SUB \implies C_{in4} = SUB$ (**Dual 4-Bit independent add/sub**).

---

## 4. Operation Mapping & 74HCT151 MUX Selection

The ALU implements eight core arithmetic and logical operations, selected by a 3-bit Opcode ($S_2, S_1, S_0$):

| Opcode ($S_2 S_1 S_0$) | MUX Input | Operation | Expression / Datapath | Description |
| :---: | :---: | :--- | :--- | :--- |
| `000` | $D_0$ | **ADD** | $Y = A + B$ | Gate-level ripple adder sum ($SUB=0, Cin_0=0$) |
| `001` | $D_1$ | **SUB** | $Y = A - B$ | 2's complement difference ($SUB=1, Cin_0=1$) |
| `010` | $D_2$ | **AND** | $Y = A \cdot B$ | Discrete 74HCT08 bitwise AND |
| `011` | $D_3$ | **OR** | $Y = A + B$ | Discrete 74HCT32 bitwise OR |
| `100` | $D_4$ | **XOR** | $Y = A \oplus B$ | Discrete 74HCT86 bitwise Exclusive-OR |
| `101` | $D_5$ | **NOT** | $Y = \overline{A}$ | Discrete 74HCT04 bitwise Inversion |
| `110` | $D_6$ | **SHL** | $Y = A \ll 1$ | Hardwired logical shift left ($Y_i = A_{i-1}, Y_0 = 0$) |
| `111` | $D_7$ | **SHR** | $Y = A \gg 1$ | Hardwired logical shift right ($Y_i = A_{i+1}, Y_7 = 0$) |

### 4.1 Multiplexer Wiring per Bit $i \in [0..7]$

Each of the eight 74HCT151 ICs selects the active operation for its corresponding bit $Y_i$:

```
                       74HCT151 Multiplexer for Bit i
                      +-----------------------------+
   Sum S[i] (ADD) --->| Pin 4  (D0)                 |
   Sum S[i] (SUB) --->| Pin 3  (D1)                 |
   (A[i] · B[i])  --->| Pin 2  (D2)                 |
   (A[i] + B[i])  --->| Pin 1  (D3)                 |
   (A[i] ⊕ B[i])  --->| Pin 15 (D4)     Output Y    |---> Output Y[i]
   NOT(A[i])      --->| Pin 14 (D5)     (Pin 5)     |     (to 74HCT245 Pin i+2)
   A[i-1] (SHL)*  --->| Pin 13 (D6)                 |
   A[i+1] (SHR)** --->| Pin 12 (D7)                 |
                      |                             |
     Opcode S0    --->| Pin 11 (S0)                 |
     Opcode S1    --->| Pin 10 (S1)                 |
     Opcode S2    --->| Pin 9  (S2)                 |
     Ground (0V)  --->| Pin 7  (/EN)                |
                      +-----------------------------+

   * For Bit 0: D6 connects to GND (0 shifted into LSB).
   ** For Bit 7: D7 connects to GND (0 shifted into MSB).
```

---

## 5. Hybrid Input Override Circuit (Physical DIP + Remote ESP32)

To enable both physical manual control (via 8-position DIP switches) and remote automated control (via ESP32-S3 WebSockets), a **4.7 kΩ series override resistor** network is implemented on every input line:

```
     +5V Rail ──────────────┐
                            │
                      [ DIP SWITCH ]
                            │
     ALU IC Input Node ─────┴─────────────┬─────────────[ 10kΩ Pull-Down ]─────► GND
                                          │
                                    [ 4.7kΩ Resistor ]
                                          │
     ESP32-S3 GPIO ───────────────────────┘
```

### Theory of Operation:
1. **Manual DIP Mode (ESP32 Passive)**:
   * The ESP32-S3 configures its GPIO as high-impedance `INPUT`.
   * When the DIP switch is **CLOSED** (ON), the node is tied to $+5.0\text{ V}$ ($\text{HIGH}$).
   * When the DIP switch is **OPEN** (OFF), the $10\text{ k}\Omega$ pull-down resistor discharges the node to $0.0\text{ V}$ ($\text{LOW}$).
   * Leakage through the $4.7\text{ k}\Omega$ resistor into the high-Z ESP32 input is negligible ($< 1\ \mu\text{A}$).
2. **Remote Injection Mode (ESP32 Active Override)**:
   * The user leaves the DIP switch **OPEN** (OFF).
   * The ESP32 reconfigures its GPIO as `OUTPUT`.
   * When driving **HIGH (3.3V)**: The 3.3V passes through the $4.7\text{ k}\Omega$ resistor into the 74HCT input. Because 74HCT ICs have an input threshold of $V_{IH(\min)} = 2.0\text{ V}$, the 3.3V level is recognized as a solid, noise-immune logic HIGH. The $10\text{ k}\Omega$ pull-down forms a harmless divider: $V_{node} = 3.3\text{V} \times \frac{10}{10 + 4.7} = 2.24\text{ V} > 2.0\text{ V}$.
   * When driving **LOW (0V)**: Both the ESP32 and the $10\text{ k}\Omega$ resistor pull the line securely to $0\text{ V}$.
3. **Safety Protection**:
   * If a DIP switch is accidentally left **CLOSED (+5V)** while the ESP32 GPIO drives **LOW (0V)**, the $4.7\text{ k}\Omega$ resistor limits the short-circuit current to $I = \frac{5.0\text{ V}}{4.7\text{ k}\Omega} \approx 1.06\text{ mA}$, far below the ESP32 GPIO damage threshold ($\pm 40\text{ mA}$).

---

## 6. ESP32-S3 Voltage Divider Level-Shifter Circuit

The ESP32-S3 GPIO pins are strictly **3.3V tolerant**. Connecting 5V outputs directly from the 74HCT ICs would cause permanent silicon damage. Every 5V output signal ($Y_0..Y_7$, $C_{out8}$, Mode flag) passes through a precision two-resistor voltage divider before reaching the ESP32:

```
  5V ALU Signal (from 74HCT245/MUX) ─────[ R1 = 1.0 kΩ ]─────┬─────► ESP32-S3 3.3V GPIO Pin
                                                             │
                                                       [ R2 = 2.2 kΩ ]
                                                             │
  Common Ground Rail ────────────────────────────────────────┴─────► GND
```

### Transfer Ratio Calculation:
$$V_{\text{ESP32}} = V_{\text{5V}} \times \left( \frac{R_2}{R_1 + R_2} \right) = 5.0\text{ V} \times \left( \frac{2.2\text{ k}\Omega}{1.0\text{ k}\Omega + 2.2\text{ k}\Omega} \right) = 5.0\text{ V} \times \frac{2.2}{3.2} = 3.4375\text{ V}$$

* Under normal loaded conditions (accounting for pin input impedance), the voltage is comfortably clamped around **$3.3\text{ V}$**, guaranteeing compliance with the ESP32-S3 maximum ratings while maintaining crisp digital switching edges.

#### Complete ESP32-S3 Telemetry Pinout:
| Signal Name | 5V Source Pin | Divider Network | ESP32-S3 GPIO Pin | Function |
| :--- | :--- | :---: | :---: | :--- |
| **$Y_0$ (LSB)** | 74HCT245 Pin 18 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 40** | Result bit 0 |
| **$Y_1$** | 74HCT245 Pin 17 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 41** | Result bit 1 |
| **$Y_2$** | 74HCT245 Pin 16 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 42** | Result bit 2 |
| **$Y_3$** | 74HCT245 Pin 15 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 43** | Result bit 3 |
| **$Y_4$** | 74HCT245 Pin 14 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 44** | Result bit 4 |
| **$Y_5$** | 74HCT245 Pin 13 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 47** | Result bit 5 |
| **$Y_6$** | 74HCT245 Pin 12 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 48** | Result bit 6 |
| **$Y_7$ (MSB)** | 74HCT245 Pin 11 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 3** | Result bit 7 |
| **$C_{out8}$** | Bit 7 Adder Cout | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 26** | Carry flag |
| **MODE** | Mode Switch Node | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 39** | Mode status |
| **$S_0$** | Opcode Switch 0 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 10** | Opcode bit 0 |
| **$S_1$** | Opcode Switch 1 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 11** | Opcode bit 1 |
| **$S_2$** | Opcode Switch 2 | $1\text{k}\Omega / 2.2\text{k}\Omega$ | **GPIO 12** | Opcode bit 2 |

---

## 7. Arduino UNO R4 WiFi Software Golden Verifier Interface

The Arduino UNO R4 WiFi features native **5V logic tolerance**, meaning all 5V breadboard lines connect directly to the Arduino headers without voltage dividers. The Arduino acts as an **Independent Hardware Verification Unit**:
1. Reads raw physical ALU inputs: $A[7:0]$, $B[7:0]$, $S[2:0]$, $\text{MODE}$.
2. Reads the hardware ALU outputs: $Y[7:0]$ and $C_{out8}$.
3. Computes the mathematically exact expected result in software (golden model ALU).
4. Compares bit-for-bit:
   * **MATCH**: Lights external **Green PASS LED** (Pin 12 via $220\ \Omega$) and renders a crisp `✓ PASS` graphic on the built-in $12\times 8$ LED matrix.
   * **MISMATCH**: Lights external **Red FAIL LED** (Pin 13 via $220\ \Omega$) and renders an animated `✗ FAIL` alert.

```
       74HCT ALU Outputs (5V Direct) ──────────► Arduino R4 Pins D2–D9 (Inputs)
       Physical Opcode S[2:0] (5V Direct) ─────► Arduino R4 Pins D10, D11, A4 (Inputs)
       Carry Out Cout8 (5V Direct) ────────────► Arduino R4 Pin A5 (Input)

       Arduino R4 Pin D12 ─────[ 220Ω ]─────► 🟢 Green LED (PASS) ─────► GND
       Arduino R4 Pin D13 ─────[ 220Ω ]─────► 🔴 Red LED (FAIL) ──────► GND
       Arduino R4 5V Pin  ──────────────────► Breadboard +5V Power Rail
       Arduino R4 GND Pin ──────────────────► Common GND Bus
```

#### Arduino Software Golden Model Function:
```cpp
uint16_t compute_golden_model(uint8_t a, uint8_t b, uint8_t opcode, bool mode_8bit) {
    uint16_t expected = 0;
    switch(opcode) {
        case 0b000: // ADD
            if (mode_8bit) expected = (uint16_t)a + b;
            else expected = ((uint16_t)((a & 0x0F) + (b & 0x0F)) & 0x1F) |
                            (((uint16_t)(((a >> 4) & 0x0F) + ((b >> 4) & 0x0F)) & 0x1F) << 4);
            break;
        case 0b001: // SUB
            if (mode_8bit) expected = (uint16_t)((a - b) & 0xFF);
            else expected = (((a & 0x0F) - (b & 0x0F)) & 0x0F) |
                            ((((a >> 4) - (b >> 4)) & 0x0F) << 4);
            break;
        case 0b010: expected = a & b; break;         // AND
        case 0b011: expected = a | b; break;         // OR
        case 0b100: expected = a ^ b; break;         // XOR
        case 0b101: expected = (~a) & 0xFF; break;  // NOT
        case 0b110: expected = (a << 1) & 0xFF; break; // SHL
        case 0b111: expected = (a >> 1) & 0xFF; break; // SHR
    }
    return expected;
}
```

---

## 8. Master Bill of Materials (BOM) & Gate Count

| Part Number | Description | Quantity | Primary Circuit Role |
| :--- | :--- | :---: | :--- |
| **74HCT86** | Quad 2-Input XOR Gate | 6 | Adder half-sums (4 ICs) + Subtraction inversion (2 ICs) |
| **74HCT08** | Quad 2-Input AND Gate | 7 | Adder carry generate/propagate (4 ICs) + Bitwise AND (2 ICs) + Mode gate (1 IC) |
| **74HCT32** | Quad 2-Input OR Gate | 5 | Adder carry merge (2 ICs) + Bitwise OR (2 ICs) + Mode merge (1 IC) |
| **74HCT04** | Hex Inverter / NOT Gate | 2 | Bitwise NOT operation (2 ICs) + Mode inverter |
| **74HCT151** | 8:1 Data Selector / MUX | 8 | Operation selector (1 IC per output bit $Y_0..Y_7$) |
| **74HCT245** | Octal Bus Transceiver | 1 | Output bus buffer / line driver |
| **Arduino R4 WiFi** | Microcontroller Board | 1 | Software Golden Model verifier, 5V breadboard power, LED matrix |
| **ESP32-S3 N16R8**| Dual-Core Wi-Fi SoC | 1 | Web dashboard host, WebSocket telemetry, remote input injector |
| **Resistors** | 10 kΩ 1/4W (DIP pull-downs)| 20 | Input conditioning on A, B, Opcode, Mode |
| **Resistors** | 4.7 kΩ 1/4W (ESP32 overrides)| 19 | Dual-master override and pin protection |
| **Resistors** | 1.0 kΩ 1/4W (Divider R1) | 13 | 5V to 3.3V level conversion |
| **Resistors** | 2.2 kΩ 1/4W (Divider R2) | 13 | 5V to 3.3V level conversion |
| **Resistors** | 220 Ω / 330 Ω (LED limiters)| 12 | Current limiting for status and bus LEDs |
| **Capacitors** | 0.1 µF Ceramic Disc | 29 | High-frequency power decoupling (one across VCC/GND of each IC) |
| **DIP Switches** | 8-Position Slide Switch | 2 | Operands A[7:0] and B[7:0] |
| **DIP Switches** | 4-Position Slide Switch | 1 | Opcode S[2:0] + Mode Switch |
| **LEDs** | 3mm/5mm Blue/Green/Red | 12 | 8-bit bus LEDs, Carry LED, Pass/Fail status LEDs |
