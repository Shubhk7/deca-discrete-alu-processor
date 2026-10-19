# Pinouts, Truth Tables, and Complete Interconnection Schematics
## Dual-Mode Discrete & Silicon ALU Processor (74xx Discrete + 74LS181N + 74xx85)
### With ESP32-S3 IoT Dashboard, Arduino UNO R4 WiFi Verifier & Arduino UNO R4 Minima Power Station

This document provides the exhaustive hardware reference, ASCII pinouts, logic truth tables, gate-level schematics, opcode translation matrices, and multi-board power distribution blueprints for the **Dual-Mode Discrete & Silicon ALU Processor**.

---

## 1. System Architectural Overview & Dual-Mode Theory

The processor implements a reconfigurable hybrid architecture utilizing the user's exact physical inventory of 12 integrated circuits. A physical **Mode Switch** selects between two distinct operating paradigms:

```
                                  [ MODE SWITCH ]
                                         │
                 ┌───────────────────────┴───────────────────────┐
                 │                                               │
                 ▼ (MODE = 0)                                    ▼ (MODE = 1)
     ┌───────────────────────────────┐               ┌───────────────────────────────┐
     │      8-BIT CASCADED ALU       │               │ REAL-TIME SILICON BENCHMARK   │
     │  Lower 4-Bit : Custom ALU     │               │  Custom 4-Bit ALU  vs 74LS181 │
     │  Upper 4-Bit : 74LS181N ALU   │               │  Identical Inputs A[3:0], B   │
     │  Carry Link  : C4 -> /Cn      │               │  Hardware 74xx85 Equivalence  │
     │  Result Bus  : Y[7:0]         │               │  Pin 6 (A=B) -> Green PASS LED│
     └───────────────────────────────┘               └───────────────────────────────┘
```

### 1.1 Mode 0: 8-Bit Cascaded Parallel ALU Processor
- **Lower Nibble ($A[3:0], B[3:0]$)** is processed by the **Custom 4-Bit Discrete ALU**:
  - IC1 (`74xx283` 4-bit binary adder) computes addition and subtraction.
  - IC2 (`74xx86` Quad XOR) performs conditional two's complement B-inversion ($B'_i = B_i \oplus SUB$).
  - IC3 (`74xx86`), IC4 (`74xx08`), IC5 (`74xx32`), and IC6 (`74xx04`) execute bitwise XOR, AND, OR, and NOT.
  - IC7, IC8, IC9, IC10 (4× `74xx151` 8:1 Multiplexers) select the output for bits $Y_0, Y_1, Y_2, Y_3$.
  - Carry-Out ($C_4$) from IC1 Pin 9 cascades into the Carry-In of the upper stage.
- **Upper Nibble ($A[7:4], B[7:4]$)** is processed by the **74LS181N 4-Bit Silicon ALU** (IC11):
  - Receives upper operands $A[7:4]$ and $B[7:4]$.
  - Receives the cascaded carry bit from the lower custom stage into its Carry-In ($\bar{C}_n$, Pin 7).
  - Evaluates the identical arithmetic or logic function commanded by Opcode $S[2:0]$.
  - Produces upper result bits $Y_7, Y_6, Y_5, Y_4$ and final 8-bit carry out ($\bar{C}_{n+4}$, Pin 18).
- **Combined Output**: Authentic 8-bit bus $Y[7:0] = \{ Y_{181}[3:0], Y_{custom}[3:0] \}$.

### 1.2 Mode 1: Real-Time Hardware Silicon Verification
- **Dual Computation**:
  - The Custom 4-Bit Discrete ALU and the `74LS181N` (IC11) receive **identical 4-bit inputs**:
    $$A_{custom}[3:0] = A_{181}[3:0] = A[3:0], \quad B_{custom}[3:0] = B_{181}[3:0] = B[3:0]$$
  - Both ALUs execute the exact same operation selected by Opcode $S[2:0]$.
- **Hardware Equivalence Checking**:
  - Custom ALU outputs $Y_{custom}[3:0]$ feed Word A ($A_3..A_0$) of IC12 (`74xx85` 4-Bit Magnitude Comparator).
  - Vintage `74LS181N` outputs $F[3:0]$ feed Word B ($B_3..B_0$) of IC12.
  - Pin 6 ($A=B$ Output) asserts **HIGH (+5V)** within $\approx 15\text{ ns}$ when all 4 bits match identically.
  - Pin 6 drives the **Green PASS LED (Silicon Match)** and signals the Arduino UNO R4 verifier and ESP32-S3 dashboard.

---

## 2. 3-Board System Responsibilities & Power Distribution

The physical implementation partitions tasks across three microcontrollers to achieve high electrical stability and isolated functional domains:

| Board | Microcontroller / Hardware | Primary Architectural Role | Electrical Specs |
| :--- | :--- | :--- | :--- |
| **Board 1** | **Arduino UNO R4 Minima** | **Dedicated Clean 5V/1A Power Station** | USB-C 5V @ 1.2A clean linear rail directly powering breadboard $V_{CC}$ and all 12 ICs |
| **Board 2** | **Arduino UNO R4 WiFi** | **5V Bus Sniffer & Golden Model Oracle** | RA4M1 32-bit ARM Cortex-M4 @ 48 MHz; direct 5V bus reads ($Y[7:0]$), 12×8 LED Matrix HUD |
| **Board 3** | **ESP32-S3 N16R8** | **IoT Dashboard, AP & Remote Injection** | Dual-core 240 MHz, SoftAP 'DECA-ALU', WebSockets, 4.7kΩ DIP override, 1k/2.2k voltage dividers |

### 2.1 Complete 3-Board Power Distribution Schematic

```
   USB-C Host (PC / 5V 3A Wall Adapter)
              │
              ▼
    ┌─────────────────────────────┐
    │   ARDUINO UNO R4 MINIMA     │
    │   "The Power Station"       │
    │                             │
    │   5V Output Pin ────────────┼──────────► BREADBOARD +5V MASTER RAIL
    │                             │            (Powers IC1..IC12, DIPs, LEDs)
    │   GND Pin ──────────────────┼─────┬────► BREADBOARD COMMON GND BUS (0V)
    └─────────────────────────────┘     │
                                        │
    USB-C Host (PC / Monitor)           │
              │                         │
              ▼                         │
    ┌─────────────────────────────┐     │
    │   ARDUINO UNO R4 WIFI       │     │
    │   "The Golden Verifier"     │     │
    │                             │     │
    │   GND Pin ──────────────────┼─────┤
    │   (5V pin LEFT UNCONNECTED) │     │ (Direct 5V TTL Inputs: Y0..Y7, Opcode, Mode)
    └─────────────────────────────┘     │
                                        │
    USB-C Host (PC / Laptop)            │
              │                         │
              ▼                         │
    ┌─────────────────────────────┐     │
    │   ESP32-S3 N16R8            │     │
    │   "The Brain / Dashboard"   │     │
    │                             │     │
    │   GND Pin ──────────────────┴─────┘
    │   GPIO Injections ──────────────[ 4.7kΩ Override ]──► Breadboard Input Nodes
    │   GPIO Telemetry Inputs ◄───────[ 1.0kΩ / 2.2kΩ ]─── Breadboard Bus Lines
    └─────────────────────────────┘
```

> [!IMPORTANT]
> **Power Rail Isolation Rules**:
> 1. **Do NOT bridge the 5V pins of the two Arduino boards**. The Arduino UNO R4 Minima is the sole power provider for the breadboard logic rails. The Arduino UNO R4 WiFi and ESP32-S3 are powered independently via their own USB-C ports.
> 2. **Common Ground Reference**: All grounds (Minima GND, WiFi R4 GND, ESP32 GND, and breadboard ground rails) MUST be solidly bonded with heavy-gauge jumper wire to form an equipotential star ground.
> 3. **Decoupling Capacitors**: Every IC must have a $0.1\ \mu\text{F}$ monolithic ceramic capacitor seated directly across its $V_{CC}$ and $\text{GND}$ pins. A $100\ \mu\text{F}$ electrolytic bulk capacitor is placed across the main power entry.

---

## 3. Exhaustive IC Pinout Reference (All 12 ICs)

All chips are standard Dual In-line Packages (DIP) viewed from above with the indexing notch oriented at the top.

```
                  ┌───---───┐
            Pin 1 │●        │ Pin N
            Pin 2 │         │ Pin N-1
            ...   │  TOP    │ ...
            Pin k │  VIEW   │ Pin k+1
                  └─────────┘
```

---

### IC1: 74xx283 (4-Bit Binary Full Adder with Fast Carry) — 16-Pin DIP
* **Role**: Primary addition and subtraction arithmetic core for the Custom 4-Bit Lower Nibble ALU.
* **Supply**: Pin 16 = $+5\text{V}$, Pin 8 = $\text{GND}$.

```
                          74xx283
                       ┌───---───┐
              Σ1 (S1)  │ 1    16 │  VCC (+5V)
              B2 (B1)  │ 2    15 │  B3 (B2)
              A2 (A1)  │ 3    14 │  A3 (A2)
              Σ0 (S0)  │ 4    13 │  Σ2 (S2)
              A1 (A0)  │ 5    12 │  A4 (A3)
              B1 (B0)  │ 6    11 │  B4 (B3)
             C0 (Cin)  │ 7    10 │  Σ3 (S3)
            GND (0V)   │ 8     9 │  C4 (Cout)
                       └─────────┘
```

| Pin | Symbol | Connected To | Description |
| :---: | :---: | :--- | :--- |
| **1** | $\Sigma_1$ | IC8 Pin 4 ($D_0$) & Pin 3 ($D_1$) | Bit 1 Adder/Subtractor Sum Output ($S_1$) |
| **2** | $B_2$ | IC2 Pin 6 ($B'_1$) | Bit 1 Conditioned Operand B Input |
| **3** | $A_2$ | Input Bus $A_1$ | Bit 1 Operand A Input |
| **4** | $\Sigma_0$ | IC7 Pin 4 ($D_0$) & Pin 3 ($D_1$) | Bit 0 Adder/Subtractor Sum Output ($S_0$) |
| **5** | $A_1$ | Input Bus $A_0$ | Bit 0 Operand A Input |
| **6** | $B_1$ | IC2 Pin 3 ($B'_0$) | Bit 0 Conditioned Operand B Input |
| **7** | $C_0$ | $SUB$ Control Line | Carry-in to Bit 0 ($SUB = 1$ on subtraction for 2's comp) |
| **8** | GND | Common GND Rail | Ground (0V Reference) |
| **9** | $C_4$ | Mode Carry Link / IC11 Pin 7 | Carry-out from bit 3 (cascades to upper nibble in Mode 0) |
| **10** | $\Sigma_3$ | IC10 Pin 4 ($D_0$) & Pin 3 ($D_1$) | Bit 3 Adder/Subtractor Sum Output ($S_3$) |
| **11** | $B_4$ | IC2 Pin 11 ($B'_3$) | Bit 3 Conditioned Operand B Input |
| **12** | $A_4$ | Input Bus $A_3$ | Bit 3 Operand A Input |
| **13** | $\Sigma_2$ | IC9 Pin 4 ($D_0$) & Pin 3 ($D_1$) | Bit 2 Adder/Subtractor Sum Output ($S_2$) |
| **14** | $A_3$ | Input Bus $A_2$ | Bit 2 Operand A Input |
| **15** | $B_3$ | IC2 Pin 8 ($B'_2$) | Bit 2 Conditioned Operand B Input |
| **16** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC2: 74xx86 (Quad 2-Input XOR Gate #1) — Subtraction Inverter — 14-Pin DIP
* **Role**: Conditional one's complement inversion of lower Operand B ($B'_i = B_i \oplus SUB$).
* **Supply**: Pin 14 = $+5\text{V}$, Pin 7 = $\text{GND}$.

```
                          74xx86 (IC2)
                       ┌───---───┐
                   1A  │ 1    14 │  VCC (+5V)
                   1B  │ 2    13 │  4B
                   1Y  │ 3    12 │  4A
                   2A  │ 4    11 │  4Y
                   2B  │ 5    10 │  3B
                   2Y  │ 6     9 │  3A
             GND (0V)  │ 7     8 │  3Y
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | 1A | Input Bus $B_0$ | Operand B Bit 0 |
| **2** | 1B | $SUB$ Control Line | Subtraction command (Opcode = 001) |
| **3** | 1Y | IC1 Pin 6 ($B_1$) | Inverted/true operand $B'_0 = B_0 \oplus SUB$ |
| **4** | 2A | Input Bus $B_1$ | Operand B Bit 1 |
| **5** | 2B | $SUB$ Control Line | Subtraction command |
| **6** | 2Y | IC1 Pin 2 ($B_2$) | Inverted/true operand $B'_1 = B_1 \oplus SUB$ |
| **7** | GND | Common GND Rail | Ground (0V Reference) |
| **8** | 3Y | IC1 Pin 15 ($B_3$) | Inverted/true operand $B'_2 = B_2 \oplus SUB$ |
| **9** | 3A | Input Bus $B_2$ | Operand B Bit 2 |
| **10** | 3B | $SUB$ Control Line | Subtraction command |
| **11** | 4Y | IC1 Pin 11 ($B_4$) | Inverted/true operand $B'_3 = B_3 \oplus SUB$ |
| **12** | 4A | Input Bus $B_3$ | Operand B Bit 3 |
| **13** | 4B | $SUB$ Control Line | Subtraction command |
| **14** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC3: 74xx86 (Quad 2-Input XOR Gate #2) — Bitwise XOR Logic Array — 14-Pin DIP
* **Role**: Computes bitwise logical Exclusive-OR: $Y_{XOR,i} = A_i \oplus B_i$ for lower nibble.
* **Supply**: Pin 14 = $+5\text{V}$, Pin 7 = $\text{GND}$.

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | 1A | Input Bus $A_0$ | Operand A Bit 0 |
| **2** | 1B | Input Bus $B_0$ | Operand B Bit 0 |
| **3** | 1Y | IC7 Pin 15 ($D_4$) | Bit 0 XOR result ($A_0 \oplus B_0$) to MUX 0 |
| **4** | 2A | Input Bus $A_1$ | Operand A Bit 1 |
| **5** | 2B | Input Bus $B_1$ | Operand B Bit 1 |
| **6** | 2Y | IC8 Pin 15 ($D_4$) | Bit 1 XOR result ($A_1 \oplus B_1$) to MUX 1 |
| **7** | GND | Common GND Rail | Ground (0V Reference) |
| **8** | 3Y | IC9 Pin 15 ($D_4$) | Bit 2 XOR result ($A_2 \oplus B_2$) to MUX 2 |
| **9** | 3A | Input Bus $A_2$ | Operand A Bit 2 |
| **10** | 3B | Input Bus $B_2$ | Operand B Bit 2 |
| **11** | 4Y | IC10 Pin 15 ($D_4$) | Bit 3 XOR result ($A_3 \oplus B_3$) to MUX 3 |
| **12** | 4A | Input Bus $A_3$ | Operand A Bit 3 |
| **13** | 4B | Input Bus $B_3$ | Operand B Bit 3 |
| **14** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC4: 74xx08 (Quad 2-Input AND Gate) — Bitwise AND Logic Array — 14-Pin DIP
* **Role**: Computes bitwise logical AND: $Y_{AND,i} = A_i \cdot B_i$ for lower nibble.
* **Supply**: Pin 14 = $+5\text{V}$, Pin 7 = $\text{GND}$.

```
                          74xx08 (IC4)
                       ┌───---───┐
                   1A  │ 1    14 │  VCC (+5V)
                   1B  │ 2    13 │  4B
                   1Y  │ 3    12 │  4A
                   2A  │ 4    11 │  4Y
                   2B  │ 5    10 │  3B
                   2Y  │ 6     9 │  3A
             GND (0V)  │ 7     8 │  3Y
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | 1A | Input Bus $A_0$ | Operand A Bit 0 |
| **2** | 1B | Input Bus $B_0$ | Operand B Bit 0 |
| **3** | 1Y | IC7 Pin 2 ($D_2$) | Bit 0 AND result ($A_0 \cdot B_0$) to MUX 0 |
| **4** | 2A | Input Bus $A_1$ | Operand A Bit 1 |
| **5** | 2B | Input Bus $B_1$ | Operand B Bit 1 |
| **6** | 2Y | IC8 Pin 2 ($D_2$) | Bit 1 AND result ($A_1 \cdot B_1$) to MUX 1 |
| **7** | GND | Common GND Rail | Ground (0V Reference) |
| **8** | 3Y | IC9 Pin 2 ($D_2$) | Bit 2 AND result ($A_2 \cdot B_2$) to MUX 2 |
| **9** | 3A | Input Bus $A_2$ | Operand A Bit 2 |
| **10** | 3B | Input Bus $B_2$ | Operand B Bit 2 |
| **11** | 4Y | IC10 Pin 2 ($D_2$) | Bit 3 AND result ($A_3 \cdot B_3$) to MUX 3 |
| **12** | 4A | Input Bus $A_3$ | Operand A Bit 3 |
| **13** | 4B | Input Bus $B_3$ | Operand B Bit 3 |
| **14** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC5: 74xx32 (Quad 2-Input OR Gate) — Bitwise OR Logic Array — 14-Pin DIP
* **Role**: Computes bitwise logical OR: $Y_{OR,i} = A_i + B_i$ for lower nibble.
* **Supply**: Pin 14 = $+5\text{V}$, Pin 7 = $\text{GND}$.

```
                          74xx32 (IC5)
                       ┌───---───┐
                   1A  │ 1    14 │  VCC (+5V)
                   1B  │ 2    13 │  4B
                   1Y  │ 3    12 │  4A
                   2A  │ 4    11 │  4Y
                   2B  │ 5    10 │  3B
                   2Y  │ 6     9 │  3A
             GND (0V)  │ 7     8 │  3Y
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | 1A | Input Bus $A_0$ | Operand A Bit 0 |
| **2** | 1B | Input Bus $B_0$ | Operand B Bit 0 |
| **3** | 1Y | IC7 Pin 1 ($D_3$) | Bit 0 OR result ($A_0 + B_0$) to MUX 0 |
| **4** | 2A | Input Bus $A_1$ | Operand A Bit 1 |
| **5** | 2B | Input Bus $B_1$ | Operand B Bit 1 |
| **6** | 2Y | IC8 Pin 1 ($D_3$) | Bit 1 OR result ($A_1 + B_1$) to MUX 1 |
| **7** | GND | Common GND Rail | Ground (0V Reference) |
| **8** | 3Y | IC9 Pin 1 ($D_3$) | Bit 2 OR result ($A_2 + B_2$) to MUX 2 |
| **9** | 3A | Input Bus $A_2$ | Operand A Bit 2 |
| **10** | 3B | Input Bus $B_2$ | Operand B Bit 2 |
| **11** | 4Y | IC10 Pin 1 ($D_3$) | Bit 3 OR result ($A_3 + B_3$) to MUX 3 |
| **12** | 4A | Input Bus $A_3$ | Operand A Bit 3 |
| **13** | 4B | Input Bus $B_3$ | Operand B Bit 3 |
| **14** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC6: 74xx04 (Hex Inverter) — Bitwise NOT & Carry Inversion — 14-Pin DIP
* **Role**: Computes bitwise NOT ($\overline{A_i}$) and inverts active-high carry ($C_4$) to active-low 74LS181 carry-in ($\bar{C}_n$).
* **Supply**: Pin 14 = $+5\text{V}$, Pin 7 = $\text{GND}$.

```
                          74xx04 (IC6)
                       ┌───---───┐
                   1A  │ 1    14 │  VCC (+5V)
                   1Y  │ 2    13 │  6A
                   2A  │ 3    12 │  6Y
                   2Y  │ 4    11 │  5A
                   3A  │ 5    10 │  5Y
                   3Y  │ 6     9 │  4A
             GND (0V)  │ 7     8 │  4Y
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | 1A | Input Bus $A_0$ | Operand A Bit 0 |
| **2** | 1Y | IC7 Pin 14 ($D_5$) | Bit 0 Inverted Output ($\overline{A_0}$) to MUX 0 |
| **3** | 2A | Input Bus $A_1$ | Operand A Bit 1 |
| **4** | 2Y | IC8 Pin 14 ($D_5$) | Bit 1 Inverted Output ($\overline{A_1}$) to MUX 1 |
| **5** | 3A | Input Bus $A_2$ | Operand A Bit 2 |
| **6** | 3Y | IC9 Pin 14 ($D_5$) | Bit 2 Inverted Output ($\overline{A_2}$) to MUX 2 |
| **7** | GND | Common GND Rail | Ground (0V Reference) |
| **8** | 4Y | IC10 Pin 14 ($D_5$) | Bit 3 Inverted Output ($\overline{A_3}$) to MUX 3 |
| **9** | 4A | Input Bus $A_3$ | Operand A Bit 3 |
| **10** | 5Y | Mode Switch / IC11 Pin 7 ($\bar{C}_n$) | Inverted Carry: $\overline{C_4}$ (converts 74283 active-high $C_4$ to 74181 active-low $\bar{C}_n$) |
| **11** | 5A | IC1 Pin 9 ($C_4$) | Carry Output from 74xx283 Lower Nibble Adder |
| **12** | 6Y | Spare / Control Inversion | Gate 6 Output |
| **13** | 6A | GND (Tied LOW if unused) | Gate 6 Input (never leave floating!) |
| **14** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

### IC7, IC8, IC9, IC10: 74xx151 (8-to-1 Multiplexers) — 16-Pin DIP
* **Role**: Bit-slice operation multiplexers selecting active operation for Custom ALU bits $Y_0..Y_3$.
* **Supply**: Pin 16 = $+5\text{V}$, Pin 8 = $\text{GND}$, Pin 7 = $\text{GND}$ (Active-LOW Strobe $\bar{E}$ permanently enabled).

```
                          74xx151 (IC7..IC10)
                       ┌───---───┐
                   D3  │ 1    16 │  VCC (+5V)
                   D2  │ 2    15 │  D4
                   D1  │ 3    14 │  D5
                   D0  │ 4    13 │  D6
                    Y  │ 5    12 │  D7
                    W  │ 6    11 │  S0 (A)
             /E (/G)   │ 7    10 │  S1 (B)
             GND (0V)  │ 8     9 │  S2 (C)
                       └─────────┘
```

#### Shared Control Lines across IC7, IC8, IC9, IC10:
- **Pin 11 ($S_0$)**: Opcode Bit 0
- **Pin 10 ($S_1$)**: Opcode Bit 1
- **Pin 9 ($S_2$)**: Opcode Bit 2
- **Pin 7 ($\bar{E}$)**: Connect directly to $\text{GND}$
- **Pin 6 ($W$)**: Complementary output (Unused / No Connection)
- **Pin 5 ($Y$)**: Custom ALU Output Bit ($Y_0$ on IC7, $Y_1$ on IC8, $Y_2$ on IC9, $Y_3$ on IC10)

#### Data Input Channel Allocation ($D_0..D_7$):

| Channel | Input Pin | Opcode ($S_2 S_1 S_0$) | Selected Operation | Source IC & Pin |
| :---: | :---: | :---: | :---: | :--- |
| **$D_0$** | **Pin 4** | `000` | **ADD** | IC1 74xx283 Sum Output ($\Sigma_i$) |
| **$D_1$** | **Pin 3** | `001` | **SUB** | IC1 74xx283 Sum Output ($\Sigma_i$) with $B \oplus 1, C_0=1$ |
| **$D_2$** | **Pin 2** | `010` | **AND** | IC4 74xx08 AND Output ($A_i \cdot B_i$) |
| **$D_3$** | **Pin 1** | `011` | **OR** | IC5 74xx32 OR Output ($A_i + B_i$) |
| **$D_4$** | **Pin 15** | `100` | **XOR** | IC3 74xx86 XOR Output ($A_i \oplus B_i$) |
| **$D_5$** | **Pin 14** | `101` | **NOT** | IC6 74xx04 Inverter Output ($\overline{A_i}$) |
| **$D_6$** | **Pin 13** | `110` | **SHL** | Hardwired Shift Left ($A_{i-1}$; Bit 0 tied to GND) |
| **$D_7$** | **Pin 12** | `111` | **SHR** | Hardwired Shift Right ($A_{i+1}$; Bit 3 tied to GND or $A_4$) |

#### Shift Bus Connections:
- **IC7 (Bit 0)**: Pin 13 ($D_6$, SHL) = $\text{GND}$; Pin 12 ($D_7$, SHR) = Input Bus $A_1$.
- **IC8 (Bit 1)**: Pin 13 ($D_6$, SHL) = Input Bus $A_0$; Pin 12 ($D_7$, SHR) = Input Bus $A_2$.
- **IC9 (Bit 2)**: Pin 13 ($D_6$, SHL) = Input Bus $A_1$; Pin 12 ($D_7$, SHR) = Input Bus $A_3$.
- **IC10 (Bit 3)**: Pin 13 ($D_6$, SHL) = Input Bus $A_2$; Pin 12 ($D_7$, SHR) = $\text{GND}$ (in 4-bit) / Input Bus $A_4$ (in 8-bit).

---

### IC11: 74LS181N (4-Bit High-Speed Arithmetic Logic Unit) — 24-Pin DIP (0.6" Wide)
* **Role**: Upper Nibble ALU ($Y[7:4]$) in Mode 0; Golden Silicon Hardware Target in Mode 1.
* **Supply**: Pin 24 = $V_{CC}$ (+5V) or Pin 16; Pin 12 = $\text{GND}$.

```
                          74LS181N (IC11)
                       ┌───---───┐
                   /B0 │ 1    24 │  /B3
                   /A0 │ 2    23 │  /A3
                    S3 │ 3    22 │  /B2
                    S2 │ 4    21 │  /A2
                    S1 │ 5    20 │  /B1
                    S0 │ 6    19 │  /A1
                   /Cn │ 7    18 │  /Cn+4
                     M │ 8    17 │  /G
                   /F0 │ 9    16 │  VCC (+5V)
                   /F1 │ 10   15 │  /P
                   /F2 │ 11   14 │  A=B (Open-Coll)
              GND (0V) │ 12   13 │  /F3
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | $\bar{B}_0$ | Mode Switch: $B_4$ (Mode 0) / $B_0$ (Mode 1) | Bit 0 Operand B input |
| **2** | $\bar{A}_0$ | Mode Switch: $A_4$ (Mode 0) / $A_0$ (Mode 1) | Bit 0 Operand A input |
| **3** | $S_3$ | Opcode Translation Bus $S_{181,3}$ | Select line 3 (MSB) |
| **4** | $S_2$ | Opcode Translation Bus $S_{181,2}$ | Select line 2 |
| **5** | $S_1$ | Opcode Translation Bus $S_{181,1}$ | Select line 1 |
| **6** | $S_0$ | Opcode Translation Bus $S_{181,0}$ | Select line 0 (LSB) |
| **7** | $\bar{C}_n$ | Mode Switch: IC6 Pin 10 ($\overline{C_4}$) / +5V | Carry Input (Active LOW; $\bar{C}_n = 0$ means carry present) |
| **8** | $M$ | Opcode Translation Bus $M_{181}$ | Mode Control: HIGH = Logic, LOW = Arithmetic |
| **9** | $\bar{F}_0$ | Output Bus $Y_4$ (Mode 0) / IC12 Pin 9 ($B_0$) | Function Output Bit 0 |
| **10** | $\bar{F}_1$ | Output Bus $Y_5$ (Mode 0) / IC12 Pin 11 ($B_1$) | Function Output Bit 1 |
| **11** | $\bar{F}_2$ | Output Bus $Y_6$ (Mode 0) / IC12 Pin 14 ($B_2$) | Function Output Bit 2 |
| **12** | GND | Common GND Rail | Ground (0V Reference) |
| **13** | $\bar{F}_3$ | Output Bus $Y_7$ (Mode 0) / IC12 Pin 1 ($B_3$) | Function Output Bit 3 |
| **14** | $A=B$ | NC or 10k Pull-up | Internal Comparator / Zero Detect (Open-collector) |
| **15** | $\bar{P}$ | NC (Carry Propagate out) | Unused in ripple configuration |
| **16** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |
| **17** | $\bar{G}$ | NC (Carry Generate out) | Unused in ripple configuration |
| **18** | $\bar{C}_{n+4}$ | Amber LED ($C_{out8}$) via 330Ω | Final Carry-Out (Active LOW) |
| **19** | $\bar{A}_1$ | Mode Switch: $A_5$ (Mode 0) / $A_1$ (Mode 1) | Bit 1 Operand A input |
| **20** | $\bar{B}_1$ | Mode Switch: $B_5$ (Mode 0) / $B_1$ (Mode 1) | Bit 1 Operand B input |
| **21** | $\bar{A}_2$ | Mode Switch: $A_6$ (Mode 0) / $A_2$ (Mode 1) | Bit 2 Operand A input |
| **22** | $\bar{B}_2$ | Mode Switch: $B_6$ (Mode 0) / $B_2$ (Mode 1) | Bit 2 Operand B input |
| **23** | $\bar{A}_3$ | Mode Switch: $A_7$ (Mode 0) / $A_3$ (Mode 1) | Bit 3 Operand A input |
| **24** | $\bar{B}_3$ | Mode Switch: $B_7$ (Mode 0) / $B_3$ (Mode 1) | Bit 3 Operand B input |

---

### IC12: 74xx85 (4-Bit Magnitude Comparator) — 16-Pin DIP
* **Role**: Nanosecond-speed hardware equivalence verifier comparing Custom ALU outputs against 74LS181N silicon outputs.
* **Supply**: Pin 16 = $+5\text{V}$, Pin 8 = $\text{GND}$.

```
                          74xx85 (IC12)
                       ┌───---───┐
                    B3 │ 1    16 │  VCC (+5V)
                 I A<B │ 2    15 │  A3
                 I A=B │ 3    14 │  B2
                 I A>B │ 4    13 │  A2
                 Q A>B │ 5    12 │  A1
                 Q A=B │ 6    11 │  B1
                 Q A<B │ 7    10 │  A0
              GND (0V) │ 8     9 │  B0
                       └─────────┘
```

| Pin | Symbol | Connected To | Function / Description |
| :---: | :---: | :--- | :--- |
| **1** | $B_3$ | IC11 Pin 13 ($F_3$) | Benchmark Input Word B Bit 3 (from 74LS181) |
| **2** | $I_{A<B}$ | Common GND Rail | Expansion Cascading Input $A < B$ (Tied LOW) |
| **3** | $I_{A=B}$ | +5V Master Power Rail | Expansion Cascading Input $A = B$ (Tied HIGH for single 4-bit) |
| **4** | $I_{A>B}$ | Common GND Rail | Expansion Cascading Input $A > B$ (Tied LOW) |
| **5** | $Q_{A>B}$ | NC (Unused) | Output: Word A > Word B |
| **6** | $Q_{A=B}$ | **Green PASS LED & Arduino R4 D12 & ESP32 GPIO 3** | **EQUIVALENCE MATCH OUTPUT** (HIGH on identical match) |
| **7** | $Q_{A<B}$ | NC (Unused) | Output: Word A < Word B |
| **8** | GND | Common GND Rail | Ground (0V Reference) |
| **9** | $B_0$ | IC11 Pin 9 ($F_0$) | Benchmark Input Word B Bit 0 (from 74LS181) |
| **10** | $A_0$ | IC7 Pin 5 ($Y_0$) | Test Input Word A Bit 0 (from Custom ALU) |
| **11** | $B_1$ | IC11 Pin 10 ($F_1$) | Benchmark Input Word B Bit 1 (from 74LS181) |
| **12** | $A_1$ | IC8 Pin 5 ($Y_1$) | Test Input Word A Bit 1 (from Custom ALU) |
| **13** | $A_2$ | IC9 Pin 5 ($Y_2$) | Test Input Word A Bit 2 (from Custom ALU) |
| **14** | $B_2$ | IC11 Pin 11 ($F_2$) | Benchmark Input Word B Bit 2 (from 74LS181) |
| **15** | $A_3$ | IC10 Pin 5 ($Y_3$) | Test Input Word A Bit 3 (from Custom ALU) |
| **16** | $V_{CC}$ | +5V Master Power Rail | Positive Supply (+5V DC) |

---

## 4. 74LS181N Function Table & Opcode Translation Mapping

The `74LS181N` is configured to operate with **Active-High Data** inputs and outputs ($A_i, B_i, F_i$). The table below establishes the direct mathematical equivalence between the Custom 4-Bit ALU multiplexer selections and the `74LS181N` control inputs:

### 4.1 Master Opcode Mapping Table

| Opcode ($S_2 S_1 S_0$) | Hex | Operation | Custom ALU 74151 MUX Channel | 74LS181 Mode ($M$) | 74LS181 Select ($S_3 S_2 S_1 S_0$) | 74LS181 Carry ($\bar{C}_n$) | Mathematical Function |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `000` | `0x0` | **ADD** | $D_0$ (IC1 $\Sigma$) | **0** (Arith) | `1001` (`0x9`) | $1$ (Mode 1) / $\overline{C_4}$ (Mode 0) | $F = A + B$ |
| `001` | `0x1` | **SUB** | $D_1$ (IC1 $\Sigma$, $B \oplus 1$) | **0** (Arith) | `0110` (`0x6`) | **0** (Carry-in = 1) | $F = A - B$ ($A + \bar{B} + 1$) |
| `010` | `0x2` | **AND** | $D_2$ (IC4 7408) | **1** (Logic) | `1011` (`0xB`) | X (Don't Care) | $F = A \cdot B$ |
| `011` | `0x3` | **OR**  | $D_3$ (IC5 7432) | **1** (Logic) | `1110` (`0xE`) | X (Don't Care) | $F = A + B$ |
| `100` | `0x4` | **XOR** | $D_4$ (IC3 7486) | **1** (Logic) | `0110` (`0x6`) | X (Don't Care) | $F = A \oplus B$ |
| `101` | `0x5` | **NOT** | $D_5$ (IC6 7404) | **1** (Logic) | `0000` (`0x0`) | X (Don't Care) | $F = \overline{A}$ |
| `110` | `0x6` | **SHL** | $D_6$ (Hardwired) | **0** (Arith) | `1100` (`0xC`) | $1$ (No Carry) | $F = A + A = A \ll 1$ |
| `111` | `0x7` | **SHR** | $D_7$ (Hardwired) | Hardwired Bus | Hardwired Bus | X | $F = A \gg 1$ |

### 4.2 Arithmetic Two's Complement Inversion Truth Table (IC2)
When Opcode = `001` (SUB), the $SUB$ control signal goes HIGH (+5V):
$$B'_i = B_i \oplus 1 = \overline{B}_i, \quad C_0 = 1 \implies Y = A + \overline{B} + 1 = A - B$$

| $B_i$ | $SUB$ | $B'_i = B_i \oplus SUB$ | Inversion State |
| :---: | :---: | :---: | :--- |
| **0** | 0 | **0** | Non-inverting (ADD operation) |
| **1** | 0 | **1** | Non-inverting (ADD operation) |
| **0** | 1 | **1** | Inverted (SUB operation: bit flipped) |
| **1** | 1 | **0** | Inverted (SUB operation: bit flipped) |

---

## 5. Mode 0 vs Mode 1 Steering & Carry Cascading

The mode steering network switches signal routing between the 8-bit cascaded processor and the 4-bit hardware equivalence verifier:

```
  =============================================================================
                    MODE 0: 8-BIT CASCADED ARITHMETIC PROCESSOR
  =============================================================================
  Input Operands:
    A[3:0] ──► Custom 4-Bit Discrete ALU ────► Lower Output Bus Y[3:0]
    B[3:0] ──► (IC1, IC2, IC3, IC4, IC5, IC6, IC7..IC10)
                 │
                 ▼ C4 (IC1 Pin 9)
             [ IC6 Inverter ] ──► /Cn (Pin 7)
                                      │
    A[7:4] ───────────────────────────┼──────► 74LS181N Silicon ALU (IC11) ──► Upper Bus Y[7:4]
    B[7:4] ───────────────────────────┘

  =============================================================================
             MODE 1: REAL-TIME HARDWARE SILICON VERIFICATION BENCHMARK
  =============================================================================
  Input Operands:
    A[3:0] ──┬───────────────────────────────► Custom 4-Bit ALU ──► Y_custom[3:0] ──┐
             │                                                                      ▼ (Word A)
             └───────────────────────────────► 74LS181N Silicon ──► Y_181[3:0] ────► 74xx85 (IC12)
    B[3:0] ──┬───────────────────────────────► Custom 4-Bit ALU                     ▲ (Word B)
             │                                                                      │
             └───────────────────────────────► 74LS181N Silicon ────────────────────┘
                                                                                    │
                                                                                    ▼ Pin 6 (A=B)
                                                                         [ 🟢 SILICON MATCH LED ]
```

---

## 6. IC12 (74xx85) Hardware Equivalence Comparator Subsystem

The `74xx85` executes parallel 4-bit magnitude comparison in silicon. When comparing $Y_{custom}[3:0]$ vs $F_{181}[3:0]$:

```
          Custom ALU Bits Y[3..0]            74LS181 Output Bits F[3..0]
          (From IC7..IC10 Pin 5)               (From IC11 Pins 9,10,11,13)
                     │                                     │
                     ▼                                     ▼
             Pins 10, 12, 13, 15                   Pins 9, 11, 14, 1
             (Inputs A0,A1,A2,A3)                  (Inputs B0,B1,B2,B3)
             ┌────────────────────────────────────────────────────────┐
             │                 IC12: 74xx85 COMPARATOR                │
             │                                                        │
             │   Pin 3 (I A=B) ◄─── +5V (VCC)                         │
             │   Pin 2 (I A<B) ◄─── GND                               │
             │   Pin 4 (I A>B) ◄─── GND                               │
             │                                                        │
             │   Pin 6 (Q A=B) ──────────┬────────────────────────────┤
             └───────────────────────────┼────────────────────────────┘
                                         │
                                         ├───[ 330Ω ]───►| [ 🟢 GREEN MATCH LED ] ──► GND
                                         │
                                         ├───► Arduino UNO R4 WiFi Pin D12 (5V Direct Read)
                                         │
                                         └───[ 1.0kΩ ]───┬───► ESP32-S3 GPIO 3
                                                         │
                                                       [ 2.2kΩ ]
                                                         │
                                                        GND
```

### Truth Table for 74xx85 Equivalence Checking:
| Comparing Inputs | Cascading Inputs | Outputs | Hardware Indicator |
| :---: | :---: | :---: | :--- |
| $A_3..A_0 == B_3..B_0$ | $I_{A=B}=1, I_{A<B}=0, I_{A>B}=0$ | $Q_{A=B} = \mathbf{1},\ Q_{A<B} = 0,\ Q_{A>B} = 0$ | 🟢 **PASS LED ON** (+5V output) |
| $A_3..A_0 \neq B_3..B_0$ | $I_{A=B}=1, I_{A<B}=0, I_{A>B}=0$ | $Q_{A=B} = \mathbf{0},\ (Q_{A<B} \text{ or } Q_{A>B} = 1)$ | 🔴 **PASS LED OFF** (0V output) |

---

## 7. Hybrid Input Stage & Voltage Level Shifting Schematics

### 7.1 Hybrid Input Injection Architecture
Every input line ($A_0..A_7, B_0..B_7, S_0..S_2, \text{MODE}$) features a non-destructive resistive network allowing concurrent tactile DIP switch control and automated ESP32 test sweeps:

```
  +5V Rail ────────────────────────────┐
                                       │
                                [ DIP Switch ]
                                       │
  ALU Input Line <─────────────────────┼──────────────────<[ 4.7kΩ Override ]<── ESP32 GPIO
                                       │
                                [ 10kΩ Pull-down ]
                                       │
  GND Rail ────────────────────────────┴─────────────────────────────────────────
```

- **Manual Operation (`ESP32 GPIO = INPUT / Tri-State`)**:
  - DIP closed: $V_{in} = +5.0\text{V}$ (Logic HIGH).
  - DIP open: $V_{in} = 0.0\text{V}$ through 10kΩ pull-down (Logic LOW).
- **Automated Remote Injection (`ESP32 GPIO = OUTPUT`)**:
  - Driving HIGH (3.3V) with DIP open:
    $$V_{in} = 3.3\text{ V} \times \frac{10\text{ k}\Omega}{4.7\text{ k}\Omega + 10\text{ k}\Omega} \approx 2.245\text{ V} > V_{IH,\min} (2.0\text{ V})$$
    Reliably latches as TTL Logic HIGH!
  - Short-circuit protection: If DIP switch is closed (+5V) while ESP32 drives LOW (0V):
    $$I_{contention} = \frac{5.0\text{ V} - 0.0\text{ V}}{4.7\text{ k}\Omega} \approx 1.06\text{ mA} \ll 40\text{ mA max GPIO rating}$$

### 7.2 5V to 3.3V Output Level Shifter (Dividers for ESP32)
Every 5V output signal ($Y_0..Y_7$, Carry, Verifier Flag) entering the ESP32-S3 is stepped down via a precision $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage divider:

```
  5V ALU Bus Line ─────[ 1.0kΩ Resistor ]─────┬─────► ESP32-S3 3.3V GPIO
                                              │
                                       [ 2.2kΩ Resistor ]
                                              │
  Common Ground ──────────────────────────────┴─────────────────────────
```
$$V_{gpio} = 5.0\text{ V} \times \frac{2.2\text{ k}\Omega}{1.0\text{ k}\Omega + 2.2\text{ k}\Omega} = 5.0\text{ V} \times \frac{2200}{3200} = 3.4375\text{ V}$$
*(Within ESP32-S3 absolute maximum rating of 3.6V; completely eliminates 5V overvoltage stress).*
