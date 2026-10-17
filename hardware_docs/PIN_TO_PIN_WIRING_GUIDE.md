# Master Pin-to-Pin Interconnection Guide
## Complete Chip-Level Physical Wiring Directory

This document is your **benchtop reference manual**. It lists every physical pin on every Integrated Circuit (IC) in numerical order, exactly where that pin connects, and what wire type/color to use.

---

## Table of ICs

| IC Label | Part Number | Function | Package |
| :--- | :--- | :--- | :---: |
| **IC1** | 74LS283 | 4-Bit Binary Full Adder with Fast Carry | 16-Pin DIP |
| **IC2** | 74LS86 #1 | Quad 2-Input XOR (Subtraction Inverter) | 14-Pin DIP |
| **IC3** | 74LS86 #2 | Quad 2-Input XOR (Bitwise Logic XOR) | 14-Pin DIP |
| **IC4** | 74LS08 | Quad 2-Input AND Gate | 14-Pin DIP |
| **IC5** | 74LS32 | Quad 2-Input OR Gate | 14-Pin DIP |
| **IC6** | 74LS04 | Hex Inverter / NOT Gate | 14-Pin DIP |
| **IC7** | 74LS151 #0 | 8-to-1 Multiplexer (Selects Bit 0 $Y_0$) | 16-Pin DIP |
| **IC8** | 74LS151 #1 | 8-to-1 Multiplexer (Selects Bit 1 $Y_1$) | 16-Pin DIP |
| **IC9** | 74LS151 #2 | 8-to-1 Multiplexer (Selects Bit 2 $Y_2$) | 16-Pin DIP |
| **IC10** | 74LS151 #3 | 8-to-1 Multiplexer (Selects Bit 3 $Y_3$) | 16-Pin DIP |
| **IC11** | 74LS181 | 4-Bit Commercial ALU (Golden Reference & Upper Nibble) | 24-Pin Skinny DIP |
| **IC12** | 74LS85 | 4-Bit Magnitude Comparator (Equivalence Verifier) | 16-Pin DIP |

---

## IC1: 74LS283 (4-Bit Binary Adder - 16 Pins)

```
                 +---V---+
           S1 -- | 1   16| -- VCC (+5V)
           B1 -- | 2   15| -- A1
           A2 -- | 3   14| -- B2
           S2 -- | 4   13| -- S0
          VCC -- | 5   12| -- C0 (Carry In)
           B3 -- | 6   11| -- B0
           A3 -- | 7   10| -- A0
          GND -- | 8    9| -- C4 (Carry Out)
                 +-------+
```

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | $S_1$ | **IC8 (MUX 1) Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Sum bit 1 to MUX ADD/SUB inputs |
| **2** | $B'_1$ | **IC2 (74LS86 #1) Pin 6** | Conditioned operand bit $B_1$ from XOR gate |
| **3** | $A_2$ | **DIP Switch $A_2$** (and IC4 Pin 10, IC5 Pin 10, IC3 Pin 10, IC6 Pin 5) | Operand $A$ bit 2 |
| **4** | $S_2$ | **IC9 (MUX 2) Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Sum bit 2 to MUX ADD/SUB inputs |
| **5** | $V_{CC}$ | **+5V Power Rail** | Internal $V_{CC}$ tie (must connect to +5V) |
| **6** | $B'_3$ | **IC2 (74LS86 #1) Pin 11** | Conditioned operand bit $B_3$ from XOR gate |
| **7** | $A_3$ | **DIP Switch $A_3$** (and IC4 Pin 13, IC5 Pin 13, IC3 Pin 13, IC6 Pin 11) | Operand $A$ bit 3 (MSB of lower nibble) |
| **8** | GND | **0V Ground Rail** | Power supply ground |
| **9** | $C_4$ | **IC11 (74LS181) Pin 15 ($\overline{C_n}$)** | Carry ripple to 74LS181 in 8-bit Compute Mode |
| **10** | $A_0$ | **DIP Switch $A_0$** (and IC4 Pin 1, IC5 Pin 1, IC3 Pin 1, IC6 Pin 1) | Operand $A$ bit 0 (LSB) |
| **11** | $B'_0$ | **IC2 (74LS86 #1) Pin 3** | Conditioned operand bit $B_0$ from XOR gate |
| **12** | $C_0$ | **$SUB$ Line (Opcode switch $S_0$ in arithmetic mode)** | Carry-in (+1 for 2's complement subtraction) |
| **13** | $S_0$ | **IC7 (MUX 0) Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Sum bit 0 to MUX ADD/SUB inputs |
| **14** | $B'_2$ | **IC2 (74LS86 #1) Pin 8** | Conditioned operand bit $B_2$ from XOR gate |
| **15** | $A_1$ | **DIP Switch $A_1$** (and IC4 Pin 4, IC5 Pin 4, IC3 Pin 4, IC6 Pin 3) | Operand $A$ bit 1 |
| **16** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC2: 74LS86 #1 (Quad XOR - Subtraction Inverter - 14 Pins)

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

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **$SUB$ Control Signal (Opcode switch $S_0$)** | XOR control for bit 0 |
| **2** | 1B | **DIP Switch $B_0$** (and IC4 Pin 2, IC5 Pin 2, IC3 Pin 2) | Raw Operand $B$ bit 0 |
| **3** | 1Y | **IC1 (74LS283) Pin 11 ($B'_0$)** | $B'_0 = B_0 \oplus SUB$ |
| **4** | 2A | **$SUB$ Control Signal** | XOR control for bit 1 |
| **5** | 2B | **DIP Switch $B_1$** (and IC4 Pin 5, IC5 Pin 5, IC3 Pin 5) | Raw Operand $B$ bit 1 |
| **6** | 2Y | **IC1 (74LS283) Pin 2 ($B'_1$)** | $B'_1 = B_1 \oplus SUB$ |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **IC1 (74LS283) Pin 14 ($B'_2$)** | $B'_2 = B_2 \oplus SUB$ |
| **9** | 3A | **DIP Switch $B_2$** (and IC4 Pin 9, IC5 Pin 9, IC3 Pin 9) | Raw Operand $B$ bit 2 |
| **10** | 3B | **$SUB$ Control Signal** | XOR control for bit 2 |
| **11** | 4Y | **IC1 (74LS283) Pin 6 ($B'_3$)** | $B'_3 = B_3 \oplus SUB$ |
| **12** | 4A | **DIP Switch $B_3$** (and IC4 Pin 12, IC5 Pin 12, IC3 Pin 12) | Raw Operand $B$ bit 3 |
| **13** | 4B | **$SUB$ Control Signal** | XOR control for bit 3 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC3: 74LS86 #2 (Quad XOR - Bitwise Logic XOR - 14 Pins)

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input A |
| **2** | 1B | **DIP Switch $B_0$** | Bit 0 input B |
| **3** | 1Y | **IC7 (MUX 0) Pin 15 ($D_4$)** | $A_0 \oplus B_0$ (Logic XOR bit 0) |
| **4** | 2A | **DIP Switch $A_1$** | Bit 1 input A |
| **5** | 2B | **DIP Switch $B_1$** | Bit 1 input B |
| **6** | 2Y | **IC8 (MUX 1) Pin 15 ($D_4$)** | $A_1 \oplus B_1$ (Logic XOR bit 1) |
| **7** | GND | **0V Ground Rail** | Ground |
| **8** | 3Y | **IC9 (MUX 2) Pin 15 ($D_4$)** | $A_2 \oplus B_2$ (Logic XOR bit 2) |
| **9** | 3A | **DIP Switch $B_2$** | Bit 2 input B |
| **10** | 3B | **DIP Switch $A_2$** | Bit 2 input A |
| **11** | 4Y | **IC10 (MUX 3) Pin 15 ($D_4$)** | $A_3 \oplus B_3$ (Logic XOR bit 3) |
| **12** | 4A | **DIP Switch $B_3$** | Bit 3 input B |
| **13** | 4B | **DIP Switch $A_3$** | Bit 3 input A |
| **14** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC4: 74LS08 (Quad AND Gate - 14 Pins)

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input A |
| **2** | 1B | **DIP Switch $B_0$** | Bit 0 input B |
| **3** | 1Y | **IC7 (MUX 0) Pin 2 ($D_2$)** | $A_0 \cdot B_0$ (Logic AND bit 0) |
| **4** | 2A | **DIP Switch $A_1$** | Bit 1 input A |
| **5** | 2B | **DIP Switch $B_1$** | Bit 1 input B |
| **6** | 2Y | **IC8 (MUX 1) Pin 2 ($D_2$)** | $A_1 \cdot B_1$ (Logic AND bit 1) |
| **7** | GND | **0V Ground Rail** | Ground |
| **8** | 3Y | **IC9 (MUX 2) Pin 2 ($D_2$)** | $A_2 \cdot B_2$ (Logic AND bit 2) |
| **9** | 3A | **DIP Switch $B_2$** | Bit 2 input B |
| **10** | 3B | **DIP Switch $A_2$** | Bit 2 input A |
| **11** | 4Y | **IC10 (MUX 3) Pin 2 ($D_2$)** | $A_3 \cdot B_3$ (Logic AND bit 3) |
| **12** | 4A | **DIP Switch $B_3$** | Bit 3 input B |
| **13** | 4B | **DIP Switch $A_3$** | Bit 3 input A |
| **14** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC5: 74LS32 (Quad OR Gate - 14 Pins)

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input A |
| **2** | 1B | **DIP Switch $B_0$** | Bit 0 input B |
| **3** | 1Y | **IC7 (MUX 0) Pin 1 ($D_3$)** | $A_0 + B_0$ (Logic OR bit 0) |
| **4** | 2A | **DIP Switch $A_1$** | Bit 1 input A |
| **5** | 2B | **DIP Switch $B_1$** | Bit 1 input B |
| **6** | 2Y | **IC8 (MUX 1) Pin 1 ($D_3$)** | $A_1 + B_1$ (Logic OR bit 1) |
| **7** | GND | **0V Ground Rail** | Ground |
| **8** | 3Y | **IC9 (MUX 2) Pin 1 ($D_3$)** | $A_2 + B_2$ (Logic OR bit 2) |
| **9** | 3A | **DIP Switch $B_2$** | Bit 2 input B |
| **10** | 3B | **DIP Switch $A_2$** | Bit 2 input A |
| **11** | 4Y | **IC10 (MUX 3) Pin 1 ($D_3$)** | $A_3 + B_3$ (Logic OR bit 3) |
| **12** | 4A | **DIP Switch $B_3$** | Bit 3 input B |
| **13** | 4B | **DIP Switch $A_3$** | Bit 3 input A |
| **14** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC6: 74LS04 (Hex Inverter / NOT Gate - 14 Pins)

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input |
| **2** | 1Y | **IC7 (MUX 0) Pin 14 ($D_5$)** | $\overline{A_0}$ (Logic NOT bit 0) |
| **3** | 2A | **DIP Switch $A_1$** | Bit 1 input |
| **4** | 2Y | **IC8 (MUX 1) Pin 14 ($D_5$)** | $\overline{A_1}$ (Logic NOT bit 1) |
| **5** | 3A | **DIP Switch $A_2$** | Bit 2 input |
| **6** | 3Y | **IC9 (MUX 2) Pin 14 ($D_5$)** | $\overline{A_2}$ (Logic NOT bit 2) |
| **7** | GND | **0V Ground Rail** | Ground |
| **8** | 4Y | **330 Ω Resistor $\to$ Red "FAIL" LED** | Inverted Match signal (turns ON when Match = 0) |
| **9** | 4A | **IC12 (74LS85) Pin 6 ($O_{A=B}$)** | Input from Comparator Match Pin |
| **10** | 5Y | **IC10 (MUX 3) Pin 14 ($D_5$)** | $\overline{A_3}$ (Logic NOT bit 3) |
| **11** | 5A | **DIP Switch $A_3$** | Bit 3 input |
| **12** | 6Y | *Unconnected (Spare)* | Leave open |
| **13** | 6A | **Connect to GND** | Tie unused TTL input to GND to prevent noise |
| **14** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC7, IC8, IC9, IC10: 4× 74LS151 (8:1 Multiplexers - 16 Pins each)

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

### Common Pins across ALL FOUR 74LS151 ICs:
* **Pin 7 ($\overline{\text{STROBE}}$)**: **Tie directly to 0V Ground Rail** (Active-low enable).
* **Pin 8 (GND)**: **0V Ground Rail**.
* **Pin 9 ($C$ / Select $S_2$ MSB)**: Opcode switch $S_2$.
* **Pin 10 ($B$ / Select $S_1$ Mid)**: Opcode switch $S_1$.
* **Pin 11 ($A$ / Select $S_0$ LSB)**: Opcode switch $S_0$.
* **Pin 12 ($D_7$)**: **Tie directly to 0V Ground Rail** (Constant `0` for CLEAR).
* **Pin 16 ($V_{CC}$)**: **+5V Power Rail**.

### Bit-Specific Connections for Each Multiplexer:

| Pin | MUX 0 (IC7) - Bit 0 | MUX 1 (IC8) - Bit 1 | MUX 2 (IC9) - Bit 2 | MUX 3 (IC10) - Bit 3 | Operation Mapped |
| :---: | :--- | :--- | :--- | :--- | :--- |
| **1 ($D_3$)** | IC5 (OR) Pin 3 | IC5 (OR) Pin 6 | IC5 (OR) Pin 8 | IC5 (OR) Pin 11 | **OR** ($A + B$) |
| **2 ($D_2$)** | IC4 (AND) Pin 3 | IC4 (AND) Pin 6 | IC4 (AND) Pin 8 | IC4 (AND) Pin 11 | **AND** ($A \cdot B$) |
| **3 ($D_1$)** | IC1 (Adder) Pin 13 | IC1 (Adder) Pin 1 | IC1 (Adder) Pin 4 | IC1 (Adder) Pin 1 | **SUB** ($A - B$) |
| **4 ($D_0$)** | IC1 (Adder) Pin 13 | IC1 (Adder) Pin 1 | IC1 (Adder) Pin 4 | IC1 (Adder) Pin 1 | **ADD** ($A + B$) |
| **5 ($Y$)** | **To IC12 Pin 10 & ESP32 GPIO 40** | **To IC12 Pin 12 & ESP32 GPIO 41** | **To IC12 Pin 13 & ESP32 GPIO 42** | **To IC12 Pin 15 & ESP32 GPIO 43** | **Output Result $Y_k$** |
| **6 ($\overline{Y}$)** | *Unconnected* | *Unconnected* | *Unconnected* | *Unconnected* | Inverted Out (Open) |
| **13 ($D_6$)**| IC6 (NOT) Pin 2 | IC3 (XOR) Pin 3 | IC3 (XOR) Pin 6 | IC3 (XOR) Pin 11 | **INC A** ($A + 1$) |
| **14 ($D_5$)**| IC6 (NOT) Pin 2 | IC6 (NOT) Pin 4 | IC6 (NOT) Pin 6 | IC6 (NOT) Pin 10 | **NOT A** ($\overline{A}$) |
| **15 ($D_4$)**| IC3 (XOR) Pin 3 | IC3 (XOR) Pin 6 | IC3 (XOR) Pin 8 | IC3 (XOR) Pin 11 | **XOR** ($A \oplus B$) |

---

## IC11: 74LS181 (Commercial 4-Bit ALU - 24 Pins Skinny DIP)

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
           GND - | 12         13 | - A=B
                 +---------------+
```

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | $B_0$ | **From Steering MUX (Mode 0: $B_4$, Mode 1: $B_0$)** | Operand B bit 0 |
| **2** | $A_0$ | **From Steering MUX (Mode 0: $A_4$, Mode 1: $A_0$)** | Operand A bit 0 |
| **3** | $S_3$ | **Opcode Decoder / Control Bus Line $S_3$** | Function select bit 3 |
| **4** | $S_2$ | **Opcode Switch $S_2$** | Function select bit 2 |
| **5** | $S_1$ | **Opcode Switch $S_1$** | Function select bit 1 |
| **6** | $S_0$ | **Opcode Switch $S_0$** | Function select bit 0 |
| **7** | $\overline{C_{n+4}}$ | **To ESP32 GPIO 26 (via 1k/2.2k divider)** | Final Carry Out ($C_8$ in Compute Mode) |
| **8** | $M$ | **Mode Select line (0 = Arithmetic, 1 = Logic)** | Arithmetic / Logic mode switch |
| **9** | $F_0$ | **To IC12 (74LS85) Pin 9 ($B_0$) AND ESP32 GPIO 44 (via divider)** | Result Bit 4 / Golden Ref Bit 0 |
| **10** | $F_1$ | **To IC12 (74LS85) Pin 11 ($B_1$) AND ESP32 GPIO 47 (via divider)**| Result Bit 5 / Golden Ref Bit 1 |
| **11** | $F_2$ | **To IC12 (74LS85) Pin 14 ($B_2$) AND ESP32 GPIO 48 (via divider)**| Result Bit 6 / Golden Ref Bit 2 |
| **12** | GND | **0V Ground Rail** | Power supply ground |
| **13** | $A=B$ | *Unconnected (Open-collector output)* | Leave open (using 74LS85 instead) |
| **14** | $F_3$ | **To IC12 (74LS85) Pin 1 ($B_3$) AND ESP32 GPIO 3 (via divider)** | Result Bit 7 / Golden Ref Bit 3 |
| **15** | $\overline{C_n}$ | **IC1 (74LS283) Pin 9 ($C_4$)** | Carry ripple from lower nibble in Mode 0 |
| **16** | $\overline{P}$ | *Unconnected* | Carry propagate (leave open) |
| **17** | $G$ | *Unconnected* | Carry generate (leave open) |
| **18** | $B_3$ | **From Steering MUX (Mode 0: $B_7$, Mode 1: $B_3$)** | Operand B bit 3 |
| **19** | $A_3$ | **From Steering MUX (Mode 0: $A_7$, Mode 1: $A_3$)** | Operand A bit 3 |
| **20** | $B_2$ | **From Steering MUX (Mode 0: $B_6$, Mode 1: $B_2$)** | Operand B bit 2 |
| **21** | $A_2$ | **From Steering MUX (Mode 0: $A_6$, Mode 1: $A_2$)** | Operand A bit 2 |
| **22** | $B_1$ | **From Steering MUX (Mode 0: $B_5$, Mode 1: $B_1$)** | Operand B bit 1 |
| **23** | $A_1$ | **From Steering MUX (Mode 0: $A_5$, Mode 1: $A_1$)** | Operand A bit 1 |
| **24** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## IC12: 74LS85 (4-Bit Magnitude Comparator - 16 Pins)

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

| Pin | Name | Connects To (Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | $B_3$ | **IC11 (74LS181) Pin 14 ($F_3$)** | 74LS181 Golden bit 3 |
| **2** | $I_{A<B}$ | **0V Ground Rail** | Cascading input (Tie to GND) |
| **3** | $I_{A=B}$ | **+5V Power Rail** | Cascading input (**MUST TIE TO +5V**) |
| **4** | $I_{A>B}$ | **0V Ground Rail** | Cascading input (Tie to GND) |
| **5** | $O_{A>B}$ | *Unconnected* | Leave open |
| **6** | $O_{A=B}$ | **1. 330 Ω Resistor $\to$ Green PASS LED $\to$ GND<br/>2. IC6 (74LS04) Pin 9 (Inverter to Red LED)<br/>3. Arduino UNO R4 Pin 2 (5V Direct)<br/>4. ESP32 GPIO 38 (via 1k/2.2k divider)** | **PRIMARY HARDWARE VERIFIED OUTPUT**<br/>(HIGH = Both ALUs match 100%) |
| **7** | $O_{A<B}$ | *Unconnected* | Leave open |
| **8** | GND | **0V Ground Rail** | Power supply ground |
| **9** | $B_0$ | **IC11 (74LS181) Pin 9 ($F_0$)** | 74LS181 Golden bit 0 |
| **10** | $A_0$ | **IC7 (MUX 0) Pin 5 ($Y_0$)** | Custom ALU result bit 0 |
| **11** | $B_1$ | **IC11 (74LS181) Pin 10 ($F_1$)** | 74LS181 Golden bit 1 |
| **12** | $A_1$ | **IC8 (MUX 1) Pin 5 ($Y_1$)** | Custom ALU result bit 1 |
| **13** | $A_2$ | **IC9 (MUX 2) Pin 5 ($Y_2$)** | Custom ALU result bit 2 |
| **14** | $B_2$ | **IC11 (74LS181) Pin 11 ($F_2$)** | 74LS181 Golden bit 2 |
| **15** | $A_3$ | **IC10 (MUX 3) Pin 5 ($Y_3$)** | Custom ALU result bit 3 |
| **16** | $V_{CC}$ | **+5V Power Rail** | Main IC power |

---

## Microcontroller Hookup Table

### 1. Arduino UNO R4 WiFi (5V Native - No Resistors Needed!)
| Arduino R4 Pin | Connected To | Purpose |
| :---: | :--- | :--- |
| **Pin 2** | **IC12 (74LS85) Pin 6 ($O_{A=B}$)** | Hardware Match/Pass signal (triggers `✓` on 12x8 matrix) |
| **Pin 0 (RX)** | **ESP32-S3 GPIO 18 (TX)** | Receives serial strings (`"ADD"`, `"SUB"`, test logs) |
| **GND** | **System Common Ground Rail** | Common ground reference |

---

### 2. ESP32-S3 N16R8 (3.3V Logic - Use 1.0 kΩ / 2.2 kΩ Resistor Dividers!)
| ESP32-S3 GPIO | Signal Read | Breadboard Source IC | Divider Required? |
| :---: | :--- | :--- | :---: |
| **GPIO 40** | $Y_0$ (Custom Bit 0) | IC7 (MUX 0) Pin 5 | **Yes (1k/2.2k)** |
| **GPIO 41** | $Y_1$ (Custom Bit 1) | IC8 (MUX 1) Pin 5 | **Yes (1k/2.2k)** |
| **GPIO 42** | $Y_2$ (Custom Bit 2) | IC9 (MUX 2) Pin 5 | **Yes (1k/2.2k)** |
| **GPIO 43** | $Y_3$ (Custom Bit 3) | IC10 (MUX 3) Pin 5 | **Yes (1k/2.2k)** |
| **GPIO 44** | $Y_4$ (74181 Bit 4) | IC11 (74LS181) Pin 9 | **Yes (1k/2.2k)** |
| **GPIO 47** | $Y_5$ (74181 Bit 5) | IC11 (74LS181) Pin 10 | **Yes (1k/2.2k)** |
| **GPIO 48** | $Y_6$ (74181 Bit 6) | IC11 (74LS181) Pin 11 | **Yes (1k/2.2k)** |
| **GPIO 3** | $Y_7$ (74181 Bit 7) | IC11 (74LS181) Pin 14 | **Yes (1k/2.2k)** |
| **GPIO 26** | Carry Out ($C_8$) | IC11 (74LS181) Pin 7 | **Yes (1k/2.2k)** |
| **GPIO 38** | Match / Pass Flag | IC12 (74LS85) Pin 6 | **Yes (1k/2.2k)** |
| **GPIO 39** | Mode Switch Monitor | Mode Switch Pin | Optional / Direct |
| **GPIO 18 (TX)** | UART Telemetry TX | To Arduino R4 Pin 0 (RX) | Direct 3.3V $\to$ 5V (Works directly) |
| **GND** | System Ground | Breadboard Ground Rail | Direct wire |
