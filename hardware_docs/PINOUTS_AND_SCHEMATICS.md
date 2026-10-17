# Pinouts and Complete Interconnection Guide
## 4-Bit Reconfigurable ALU (Hardware Implementation)

This document provides the exact pinouts, connection maps, and electrical rules for building the 4-bit ALU on breadboard and transferring to a plain perfboard/zero-PCB.

---

## 1. Integrated Circuit (IC) Pinout Reference

All ICs are standard Dual In-line Package (DIP), viewed from the top with the notch/dot at the top.

### 1.1 74LS283 (4-Bit Binary Full Adder with Fast Carry) - 16 Pins
```
                +---V---+
          S1 -- | 1   16| -- VCC (+5V)
          B1 -- | 2   15| -- A1
          A2 -- | 3   14| -- B2
          S2 -- | 4   13| -- S0
         VCC -- | 5   12| -- C0 (Carry In)
         B3  -- | 6   11| -- B0
         A3  -- | 7   10| -- A0
         GND -- | 8    9| -- C4 (Carry Out)
                +-------+
```
> **Note on Power Pins**: 
> * **Pin 16**: $V_{CC}$ (+5V)
> * **Pin 5**: Internally connected $V_{CC}$ (tie to +5V)
> * **Pin 8**: GND (0V)
> * Inputs: $A = [A_3, A_2, A_1, A_0]$, $B = [B_3, B_2, B_1, B_0]$, $C_0 = \text{Carry In}$
> * Outputs: $S = [S_3, S_2, S_1, S_0]$, $C_4 = \text{Carry Out}$ (Pin 9)

---

### 1.2 74LS86 (Quad 2-Input XOR Gate) - 14 Pins
```
                +---V---+
          1A -- | 1   14| -- VCC (+5V)
          1B -- | 2   13| -- 4B
          1Y -- | 3   12| -- 4A
          2A -- | 4   11| -- 4Y
          2B -- | 5   10| -- 3B
          2Y -- | 6    9| -- 3A
         GND -- | 7    8| -- 3Y
                +-------+
```
* **Pin 14**: $V_{CC}$ (+5V) | **Pin 7**: GND (0V)
* Gate 1: Inputs Pins 1, 2 $\to$ Output Pin 3 ($1Y = 1A \oplus 1B$)
* Gate 2: Inputs Pins 4, 5 $\to$ Output Pin 6 ($2Y = 2A \oplus 2B$)
* Gate 3: Inputs Pins 9, 10 $\to$ Output Pin 8 ($3Y = 3A \oplus 3B$)
* Gate 4: Inputs Pins 12, 13 $\to$ Output Pin 11 ($4Y = 4A \oplus 4B$)

---

### 1.3 74LS08 (Quad 2-Input AND Gate) - 14 Pins
```
                +---V---+
          1A -- | 1   14| -- VCC (+5V)
          1B -- | 2   13| -- 4B
          1Y -- | 3   12| -- 4A
          2A -- | 4   11| -- 4Y
          2B -- | 5   10| -- 3B
          2Y -- | 6    9| -- 3A
         GND -- | 7    8| -- 3Y
                +-------+
```
* **Pin 14**: $V_{CC}$ (+5V) | **Pin 7**: GND (0V)
* Gate 1: Pins 1, 2 $\to$ Out Pin 3 ($1Y = 1A \cdot 1B$)
* Gate 2: Pins 4, 5 $\to$ Out Pin 6 ($2Y = 2A \cdot 2B$)
* Gate 3: Pins 9, 10 $\to$ Out Pin 8 ($3Y = 3A \cdot 3B$)
* Gate 4: Pins 12, 13 $\to$ Out Pin 11 ($4Y = 4A \cdot 4B$)

---

### 1.4 74LS32 (Quad 2-Input OR Gate) - 14 Pins
```
                +---V---+
          1A -- | 1   14| -- VCC (+5V)
          1B -- | 2   13| -- 4B
          1Y -- | 3   12| -- 4A
          2A -- | 4   11| -- 4Y
          2B -- | 5   10| -- 3B
          2Y -- | 6    9| -- 3A
         GND -- | 7    8| -- 3Y
                +-------+
```
* **Pin 14**: $V_{CC}$ (+5V) | **Pin 7**: GND (0V)
* Gate 1: Pins 1, 2 $\to$ Out Pin 3 ($1Y = 1A + 1B$)
* Gate 2: Pins 4, 5 $\to$ Out Pin 6 ($2Y = 2A + 2B$)
* Gate 3: Pins 9, 10 $\to$ Out Pin 8 ($3Y = 3A + 3B$)
* Gate 4: Pins 12, 13 $\to$ Out Pin 11 ($4Y = 4A + 4B$)

---

### 1.5 74LS04 (Hex Inverter / NOT Gate) - 14 Pins
```
                +---V---+
          1A -- | 1   14| -- VCC (+5V)
          1Y -- | 2   13| -- 6A
          2A -- | 3   12| -- 6Y
          2Y -- | 4   11| -- 5A
          3A -- | 5   10| -- 5Y
          3Y -- | 6    9| -- 4A
         GND -- | 7    8| -- 4Y
                +-------+
```
* **Pin 14**: $V_{CC}$ (+5V) | **Pin 7**: GND (0V)
* Inverter 1: Pin 1 $\to$ Pin 2 ($1Y = \overline{1A}$)
* Inverter 2: Pin 3 $\to$ Pin 4 ($2Y = \overline{2A}$)
* Inverter 3: Pin 5 $\to$ Pin 6 ($3Y = \overline{3A}$)
* Inverter 4: Pin 9 $\to$ Pin 8 ($4Y = \overline{4A}$)
* Inverter 5: Pin 11 $\to$ Pin 10 ($5Y = \overline{5A}$)
* Inverter 6: Pin 13 $\to$ Pin 12 ($6Y = \overline{6A}$)

---

### 1.6 74LS151 / 74HC151 (8-to-1 Line Multiplexer) - 16 Pins
```
                +---V---+
          D3 -- | 1   16| -- VCC (+5V)
          D2 -- | 2   15| -- D4
          D1 -- | 3   14| -- D5
          D0 -- | 4   13| -- D6
           Y -- | 5   12| -- D7
          /Y -- | 6   11| -- A (Select S0)
      /STROBE - | 7   10| -- B (Select S1)
         GND -- | 8    9| -- C (Select S2)
                +-------+
```
* **Pin 16**: $V_{CC}$ (+5V) | **Pin 8**: GND (0V)
* **Pin 7 (/STROBE / Enable)**: **MUST BE TIED TO GND (0V)** to enable output.
* **Select Inputs**:
  * Pin 11: $A$ (Least Significant Bit $S_0$)
  * Pin 10: $B$ (Middle Bit $S_1$)
  * Pin 9: $C$ (Most Significant Bit $S_2$)
* **Data Inputs**: Pins 4 ($D_0$), 3 ($D_1$), 2 ($D_2$), 1 ($D_3$), 15 ($D_4$), 14 ($D_5$), 13 ($D_6$), 12 ($D_7$)
* **Output**: Pin 5 ($Y$ - Active High Output)

---

## 2. Operation Mapping & Multiplexer Inputs

For each bit position $k \in \{0, 1, 2, 3\}$, Multiplexer $k$ selects the $k$-th bit of the result:

| Opcode ($S_2 S_1 S_0$) | Selected Input | Operation | Logic / Equation for Bit $k$ | Source Signal |
| :---: | :---: | :--- | :--- | :--- |
| `000` | $D_0$ | **ADD** ($A + B$) | $S_k$ from 74LS283 (when $SUB=0$) | 74LS283 Sum Output $S_k$ |
| `001` | $D_1$ | **SUB** ($A - B$) | $S_k$ from 74LS283 (when $SUB=1$) | 74LS283 Sum Output $S_k$ *(Tied to $D_0$)* |
| `010` | $D_2$ | **AND** ($A \cdot B$) | $A_k \cdot B_k$ | 74LS08 Output $k$ |
| `011` | $D_3$ | **OR** ($A + B$) | $A_k + B_k$ | 74LS32 Output $k$ |
| `100` | $D_4$ | **XOR** ($A \oplus B$) | $A_k \oplus B_k$ | 74LS86 Output $k$ |
| `101` | $D_5$ | **NOT A** ($\overline{A}$) | $\overline{A_k}$ | 74LS04 Output $k$ |
| `110` | $D_6$ | **INC A** ($A + 1$) | Incrementer logic bit $k$ | Discrete adder/logic bit $k$ |
| `111` | $D_7$ | **CLEAR** (`0000`) | Constant `0` | Tied to GND (0V) |

> **Smart Wiring Trick**: 
> Notice that for MUX input $D_0$ (ADD) and $D_1$ (SUB), the Adder already outputs $A+B$ when $S_0=0$ and $A-B$ when $S_0=1$. 
> Therefore, **Pin $D_0$ and Pin $D_1$ on each 74LS151 MUX can be tied directly together and connected to the corresponding Sum pin of the 74LS283 Adder!** This eliminates redundant circuitry.

---

## 3. Subtraction Control Logic ($SUB$)

Two's complement subtraction:
$$A - B = A + \overline{B} + 1$$

To achieve this:
1. When subtraction is selected (`001`), the $SUB$ line becomes `1`.
   $$SUB = \overline{S_2} \cdot \overline{S_1} \cdot S_0$$
   *(Or simply use the $S_0$ line when $S_2 S_1 = 00$)*.
2. Invert operand $B$: Each bit $B_k$ is fed through an XOR gate with $SUB$:
   $$B'_k = B_k \oplus SUB$$
   * If $SUB = 0$: $B'_k = B_k \oplus 0 = B_k$ (Direct addition).
   * If $SUB = 1$: $B'_k = B_k \oplus 1 = \overline{B_k}$ (Inverted for 2's complement).
3. Connect $SUB$ to the Carry-In ($C_0$, Pin 12) of 74LS283:
   * When $SUB = 1$, $+1$ is automatically added!
