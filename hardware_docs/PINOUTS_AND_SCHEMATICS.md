# Pinouts, Truth Tables, and Complete Interconnection Guide
## Dual-Mode 8-Bit Reconfigurable ALU with 74LS181 Golden Reference & Hardware Verifier

This document provides complete pinouts, truth tables, and connection schematics for the **Dual-Mode Reconfigurable ALU**:
* **Mode 0 (8-Bit Compute Mode)**: Cascades the **Custom 4-Bit ALU** (Lower Nibble $D_0–D_3$) with the **74LS181** (Upper Nibble $D_4–D_7$) via carry propagation ($C_4 \to C_n$).
* **Mode 1 (4-Bit Hardware Verification Mode)**: Both ALUs run the same 4-bit inputs in parallel, and a **74LS85 magnitude comparator** performs real-time equivalence checking (Green PASS / Red FAIL).

---

## 1. Integrated Circuit (IC) Pinout Reference

All ICs are standard Dual In-line Package (DIP), viewed from the top with the orientation notch at the top.

### 1.1 74LS181 (4-Bit Arithmetic Logic Unit / Function Generator) - 24 Pins (Skinny DIP)
```
                  +-------V-------+
            /B0 - | 1          24 | - VCC (+5V)
            /A0 - | 2          23 | - /A1
             S3 - | 3          22 | - /B1
             S2 - | 4          21 | - /A2
             S1 - | 5          20 | - /B2
             S0 - | 6          19 | - /A3
          /Cn+4 - | 7          18 | - /B3
              M - | 8          17 | - G (Generate)
            /F0 - | 9          16 | - /P (Propagate)
            /F1 - | 10         15 | - /Cn (Carry In)
            /F2 - | 11         14 | - /F3
            GND - | 12         13 | - A=B (Open-collector output)
                  +---------------+
```
* **Power**: Pin 24 = $+5\text{ V}$ ($V_{CC}$), Pin 12 = $\text{GND}$ (0V).
* **Mode ($M$)**: Pin 8
  * $M = 1$ (HIGH) $\implies$ **Logic Operations** (internal carries disabled).
  * $M = 0$ (LOW) $\implies$ **Arithmetic Operations** (internal carries active).
* **Carry-In ($\overline{C_n}$)**: Pin 15 (Active-LOW in active-high data convention).
* **Carry-Out ($\overline{C_{n+4}}$)**: Pin 7 (Active-LOW).
* **Function Select ($S_3, S_2, S_1, S_0$)**: Pins 3, 4, 5, 6.
* **Operand Inputs**: $A_0..A_3$ (Pins 2, 23, 21, 19), $B_0..B_3$ (Pins 1, 22, 20, 18).
* **Function Outputs**: $F_0..F_3$ (Pins 9, 10, 11, 14).

#### 74LS181 Function Table (Active-High Data Convention)
| $S_3$ | $S_2$ | $S_1$ | $S_0$ | Logic ($M=1$) | Arithmetic ($M=0, \overline{C_n}=0$ with Carry) | Arithmetic ($M=0, \overline{C_n}=1$ No Carry) |
| :---: | :---: | :---: | :---: | :--- | :--- | :--- |
| 1 | 0 | 0 | 1 | $F = A \oplus B$ (XOR) | **$F = A + B + 1$** | **$F = A + B$ (ADD)** |
| 0 | 1 | 1 | 0 | $F = \overline{A \oplus B}$ (XNOR) | **$F = A - B$ (SUB)** | $F = A - B - 1$ |
| 1 | 1 | 1 | 0 | **$F = A \cdot B$ (AND)** | $F = (A\overline{B}) + A + 1$ | $F = (A\overline{B}) + A$ |
| 1 | 0 | 1 | 1 | **$F = A + B$ (OR)** | $F = (A + B) + 1$ | $F = A + B$ |
| 0 | 0 | 0 | 0 | **$F = \overline{A}$ (NOT A)** | $F = A$ | $F = A - 1$ |
| 1 | 1 | 1 | 1 | $F = A$ | **$F = A + 1$ (INC A)** | $F = A$ |
| 1 | 1 | 0 | 0 | **$F = 0$ (CLEAR)** | $F = (A + A) + 1$ | $F = A + A$ |

---

### 1.2 74LS85 (4-Bit Magnitude Comparator) - 16 Pins
```
                  +-------V-------+
             B3 - | 1          16 | - VCC (+5V)
        I(A<B) - | 2          15 | - A3
        I(A=B) - | 3          14 | - B2
        I(A>B) - | 4          13 | - A2
        O(A>B) - | 5          12 | - A1
        O(A=B) - | 6          11 | - B1
        O(A<B) - | 7          10 | - A0
            GND - | 8           9 | - B0
                  +---------------+
```
* **Power**: Pin 16 = $+5\text{ V}$, Pin 8 = $\text{GND}$.
* **Cascading Inputs (Crucial)**:
  * **Pin 3 ($I_{A=B}$)**: **TIE TO +5V (HIGH)**.
  * **Pin 2 ($I_{A<B}$)**: Tie to GND (LOW).
  * **Pin 4 ($I_{A>B}$)**: Tie to GND (LOW).
* **Comparison Inputs**:
  * $A = [A_3, A_2, A_1, A_0]$: Pins 15, 13, 12, 10 $\to$ Connect to **Custom ALU Outputs $Y_3..Y_0$**.
  * $B = [B_3, B_2, B_1, B_0]$: Pins 1, 14, 11, 9 $\to$ Connect to **74LS181 Outputs $F_3..F_0$**.
* **Equivalence Output**:
  * **Pin 6 ($O_{A=B}$)**: Goes **HIGH (+5V)** when Custom ALU Output == 74LS181 Output!
  * Connect Pin 6 $\to 330\ \Omega \to$ **Green "PASS / VERIFIED" LED** $\to$ GND.
  * Connect Pin 6 $\to$ Inverter $\to 330\ \Omega \to$ **Red "MISMATCH / FAULT" LED** $\to$ GND.

---

### 1.3 74LS157 (Quad 2-to-1 Data Selector / Multiplexer) - 16 Pins
Used for electronic switching between **Mode 0 (8-Bit Compute)** and **Mode 1 (4-Bit Verify)**.
```
                  +-------V-------+
         SELECT - | 1          16 | - VCC (+5V)
             1A - | 2          15 | - /STROBE (GND)
             1B - | 3          14 | - 4A
             1Y - | 4          13 | - 4B
             2A - | 5          12 | - 4Y
             2B - | 6          11 | - 3A
             2Y - | 7          10 | - 3B
            GND - | 8           9 | - 3Y
                  +---------------+
```
* **Pin 1 (SELECT)**:
  * $0$ (LOW) $\implies$ Outputs $1Y..4Y$ follow Inputs $1A..4A$ (**Mode 0: 8-Bit Compute**).
  * $1$ (HIGH) $\implies$ Outputs $1Y..4Y$ follow Inputs $1B..4B$ (**Mode 1: 4-Bit Verify**).
* **Pin 15 ($\overline{\text{STROBE}}$)**: **Tie to GND** (Enable).

---

### 1.4 Custom 4-Bit ALU Core ICs

| IC | Function | Pins | Critical Connections |
| :--- | :--- | :---: | :--- |
| **74LS283** | 4-bit Binary Full Adder | 16 | Pin 16 & 5 to +5V, Pin 8 to GND. Carry In = Pin 12 ($C_0$). Carry Out = Pin 9 ($C_4$). |
| **74LS86** | Quad 2-input XOR Gate (2 ICs) | 14 | IC #1: Conditional B inverter ($B \oplus SUB$). IC #2: Logic XOR. |
| **74LS08** | Quad 2-input AND Gate | 14 | Logic AND ($A \cdot B$). |
| **74LS32** | Quad 2-input OR Gate | 14 | Logic OR ($A + B$). |
| **74LS04** | Hex Inverter / NOT Gate | 14 | Logic NOT ($\overline{A}$). |
| **74LS151** | 8:1 MUX (4 ICs) | 16 | 1 MUX per bit. Pin 7 ($\overline{E}$) to GND. Select lines $S_0, S_1, S_2$ on Pins 11, 10, 9. |

---

## 2. Operation Mapping & Opcode Translation

The system uses a unified 3-bit Opcode ($S_2, S_1, S_0$) that controls both ALUs:

| Opcode ($S_2 S_1 S_0$) | Selected Operation | Custom ALU MUX Input | 74LS181 Mode ($M$) | 74LS181 Select ($S_3 S_2 S_1 S_0$) | 74LS181 Carry-In ($\overline{C_n}$) |
| :---: | :--- | :---: | :---: | :---: | :---: |
| `000` | **ADD** ($A + B$) | $D_0$ | `0` (Arith) | `1 0 0 1` | `1` (No carry-in) |
| `001` | **SUB** ($A - B$) | $D_1$ | `0` (Arith) | `0 1 1 0` | `0` (Active carry-in) |
| `010` | **AND** ($A \cdot B$) | $D_2$ | `1` (Logic) | `1 1 1 0` | X |
| `011` | **OR** ($A + B$) | $D_3$ | `1` (Logic) | `1 0 1 1` | X |
| `100` | **XOR** ($A \oplus B$) | $D_4$ | `1` (Logic) | `1 0 0 1` | X |
| `101` | **NOT A** ($\overline{A}$) | $D_5$ | `1` (Logic) | `0 0 0 0` | X |
| `110` | **INC A** ($A + 1$) | $D_6$ | `0` (Arith) | `1 1 1 1` | `0` (With carry-in) |
| `111` | **CLEAR** (`0000`) | $D_7$ | `1` (Logic) | `1 1 0 0` | X |

---

## 3. Dual-Mode Input Routing Schematic

```
                                [ MODE SWITCH ]
                           (0 = COMPUTE, 1 = VERIFY)
                                       |
                 +---------------------+---------------------+
                 |                                           |
                 v                                           v
       [ 74LS157 #1 (A-Bus) ]                      [ 74LS157 #2 (B-Bus) ]
       1A..4A: A[7..4] (Upper Nibble)              1A..4A: B[7..4] (Upper Nibble)
       1B..4B: A[3..0] (Lower Nibble)              1B..4B: B[3..0] (Lower Nibble)
                 |                                           |
                 +---------------------+---------------------+
                                       |
                                       v
                             [ 74LS181 Inputs ]
                                A3..A0, B3..B0

                             [ Custom ALU Inputs ]
                       Always A[3..0] and B[3..0] Direct
```

### Carry Routing Logic:
* In **Mode 0 (8-Bit Compute)**:
  * Carry-In to Custom ALU: $C_0 = SUB$.
  * Carry-In to 74LS181: Connect Custom ALU Carry-Out ($C_4$) to 74LS181 Carry-In ($\overline{C_n}$).
* In **Mode 1 (4-Bit Verify)**:
  * Both ALUs receive the exact same carry-in condition for independent parallel operation.
