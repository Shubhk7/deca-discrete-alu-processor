# Master Pin-to-Pin Interconnection Guide
## Complete Benchtop Wiring Instruction Set: Reconfigurable 8-Bit / 4-Bit Gate-Level Discrete ALU

This document serves as the **exhaustive physical benchtop wiring manual** for the discrete 74HCT gate-level ALU. Every physical pin across all 27 Integrated Circuits (ICs), DIP switches, microcontrollers, and resistor networks is cataloged in exact numerical sequence with source, destination, and wire color recommendations.

---

## Master Bill of Integrated Circuits (ICs)

| IC Designator | Part Number | Logic Function | Package |
| :--- | :--- | :--- | :---: |
| **XOR1** | 74HCT86 | Quad 2-Input XOR (Adder Sum Stages: Bit 0, Bit 1) | 14-Pin DIP |
| **XOR2** | 74HCT86 | Quad 2-Input XOR (Adder Sum Stages: Bit 2, Bit 3) | 14-Pin DIP |
| **XOR3** | 74HCT86 | Quad 2-Input XOR (Adder Sum Stages: Bit 4, Bit 5) | 14-Pin DIP |
| **XOR4** | 74HCT86 | Quad 2-Input XOR (Adder Sum Stages: Bit 6, Bit 7) | 14-Pin DIP |
| **XOR5** | 74HCT86 | Quad 2-Input XOR (Subtraction Conditional Inversion: $B[3:0] \oplus SUB$) | 14-Pin DIP |
| **XOR6** | 74HCT86 | Quad 2-Input XOR (Subtraction Conditional Inversion: $B[7:4] \oplus SUB$) | 14-Pin DIP |
| **AND1** | 74HCT08 | Quad 2-Input AND (Adder Carry Generate/Propagate: Bit 0, Bit 1) | 14-Pin DIP |
| **AND2** | 74HCT08 | Quad 2-Input AND (Adder Carry Generate/Propagate: Bit 2, Bit 3) | 14-Pin DIP |
| **AND3** | 74HCT08 | Quad 2-Input AND (Adder Carry Generate/Propagate: Bit 4, Bit 5) | 14-Pin DIP |
| **AND4** | 74HCT08 | Quad 2-Input AND (Adder Carry Generate/Propagate: Bit 6, Bit 7) | 14-Pin DIP |
| **AND5** | 74HCT08 | Quad 2-Input AND (Bitwise Logic Operation: $A[3:0] \cdot B[3:0]$) | 14-Pin DIP |
| **AND6** | 74HCT08 | Quad 2-Input AND (Bitwise Logic: $A[6:4] \cdot B[6:4]$ + Mode Carry Gate) | 14-Pin DIP |
| **OR1** | 74HCT32 | Quad 2-Input OR (Adder Carry Chain: Lower Nibble $C_1, C_2, C_3, C_4$) | 14-Pin DIP |
| **OR2** | 74HCT32 | Quad 2-Input OR (Adder Carry Chain: Upper Nibble $C_5, C_6, C_7, C_8$) | 14-Pin DIP |
| **OR3** | 74HCT32 | Quad 2-Input OR (Bitwise Logic Operation: $A[3:0] + B[3:0]$) | 14-Pin DIP |
| **OR4** | 74HCT32 | Quad 2-Input OR (Bitwise Logic Operation: $A[7:4] + B[7:4]$) | 14-Pin DIP |
| **NOT1** | 74HCT04 | Hex Inverter (Bitwise Logic Operation: $\overline{A[5:0]}$) | 14-Pin DIP |
| **NOT2** | 74HCT04 | Hex Inverter (Bitwise Logic Operation: $\overline{A[7:6]}$ + Control Inverters) | 14-Pin DIP |
| **MUX1** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 0 Result $Y_0$) | 16-Pin DIP |
| **MUX2** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 1 Result $Y_1$) | 16-Pin DIP |
| **MUX3** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 2 Result $Y_2$) | 16-Pin DIP |
| **MUX4** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 3 Result $Y_3$) | 16-Pin DIP |
| **MUX5** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 4 Result $Y_4$) | 16-Pin DIP |
| **MUX6** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 5 Result $Y_5$) | 16-Pin DIP |
| **MUX7** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 6 Result $Y_6$) | 16-Pin DIP |
| **MUX8** | 74HCT151 | 8-to-1 Multiplexer (Selects Bit 7 Result $Y_7$) | 16-Pin DIP |
| **BUF1** | 74HCT245 | Octal Bus Transceiver (Drives Clean Buffered 8-Bit Output Bus $Y_{OUT}[7:0]$) | 20-Pin DIP |

---

# SECTION 1: Adder Gate ICs (XOR1–4, AND1–4, OR1–2)

Every 1-bit full adder slice evaluates:
$$X_k = A_k \oplus B'_k, \quad S_k = X_k \oplus C_k, \quad G_k = A_k \cdot B'_k, \quad P_k = C_k \cdot X_k, \quad C_{k+1} = G_k + P_k$$

### XOR1: 74HCT86 (Adder Sum XOR Stages: Bit 0, Bit 1)
* **Function**: Computes partial sum $X_0, X_1$ and final sum $S_0, S_1$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** (Input Bus $A_0$) | Bit 0 Operand A input |
| **2** | 1B | **XOR5 Pin 3 ($B'_0$)** | Bit 0 conditioned Operand B |
| **3** | 1Y | **XOR1 Pin 4 (2A), AND1 Pin 5 (2B), MUX1 Pin 15 ($D_4$)** | $X_0 = A_0 \oplus B'_0$ (Partial sum & XOR op) |
| **4** | 2A | **XOR1 Pin 3 (1Y / $X_0$)** | Partial sum input to Stage 2 |
| **5** | 2B | **$SUB$ Control Line ($C_{in0}$)** | Carry-in to Bit 0 ($SUB = 1$ on subtraction) |
| **6** | 2Y | **MUX1 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_0$ (ADD / SUB result bit 0) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR1 Pin 12 (4A), AND1 Pin 13 (4B), MUX2 Pin 15 ($D_4$)** | $X_1 = A_1 \oplus B'_1$ (Partial sum & XOR op) |
| **9** | 3A | **DIP Switch $A_1$** (Input Bus $A_1$) | Bit 1 Operand A input |
| **10** | 3B | **XOR5 Pin 6 ($B'_1$)** | Bit 1 conditioned Operand B |
| **11** | 4Y | **MUX2 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_1$ (ADD / SUB result bit 1) |
| **12** | 4A | **XOR1 Pin 8 (3Y / $X_1$)** | Partial sum input to Stage 2 |
| **13** | 4B | **OR1 Pin 3 ($C_1$)** | Carry-in from Bit 0 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### XOR2: 74HCT86 (Adder Sum XOR Stages: Bit 2, Bit 3)
* **Function**: Computes partial sum $X_2, X_3$ and final sum $S_2, S_3$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_2$** (Input Bus $A_2$) | Bit 2 Operand A input |
| **2** | 1B | **XOR5 Pin 8 ($B'_2$)** | Bit 2 conditioned Operand B |
| **3** | 1Y | **XOR2 Pin 4 (2A), AND2 Pin 5 (2B), MUX3 Pin 15 ($D_4$)** | $X_2 = A_2 \oplus B'_2$ (Partial sum & XOR op) |
| **4** | 2A | **XOR2 Pin 3 (1Y / $X_2$)** | Partial sum input to Stage 2 |
| **5** | 2B | **OR1 Pin 6 ($C_2$)** | Carry-in from Bit 1 |
| **6** | 2Y | **MUX3 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_2$ (ADD / SUB result bit 2) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR2 Pin 12 (4A), AND2 Pin 13 (4B), MUX4 Pin 15 ($D_4$)** | $X_3 = A_3 \oplus B'_3$ (Partial sum & XOR op) |
| **9** | 3A | **DIP Switch $A_3$** (Input Bus $A_3$) | Bit 3 Operand A input |
| **10** | 3B | **XOR5 Pin 11 ($B'_3$)** | Bit 3 conditioned Operand B |
| **11** | 4Y | **MUX4 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_3$ (ADD / SUB result bit 3) |
| **12** | 4A | **XOR2 Pin 8 (3Y / $X_3$)** | Partial sum input to Stage 2 |
| **13** | 4B | **OR1 Pin 8 ($C_3$)** | Carry-in from Bit 2 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### XOR3: 74HCT86 (Adder Sum XOR Stages: Bit 4, Bit 5)
* **Function**: Computes partial sum $X_4, X_5$ and final sum $S_4, S_5$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_4$** (Input Bus $A_4$) | Bit 4 Operand A input |
| **2** | 1B | **XOR6 Pin 3 ($B'_4$)** | Bit 4 conditioned Operand B |
| **3** | 1Y | **XOR3 Pin 4 (2A), AND3 Pin 5 (2B), MUX5 Pin 15 ($D_4$)** | $X_4 = A_4 \oplus B'_4$ (Partial sum & XOR op) |
| **4** | 2A | **XOR3 Pin 3 (1Y / $X_4$)** | Partial sum input to Stage 2 |
| **5** | 2B | **AND6 Pin 11 ($C_{in4}$)** | Gated Carry-in from Mode Gate |
| **6** | 2Y | **MUX5 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_4$ (ADD / SUB result bit 4) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR3 Pin 12 (4A), AND3 Pin 13 (4B), MUX6 Pin 15 ($D_4$)** | $X_5 = A_5 \oplus B'_5$ (Partial sum & XOR op) |
| **9** | 3A | **DIP Switch $A_5$** (Input Bus $A_5$) | Bit 5 Operand A input |
| **10** | 3B | **XOR6 Pin 6 ($B'_5$)** | Bit 5 conditioned Operand B |
| **11** | 4Y | **MUX6 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_5$ (ADD / SUB result bit 5) |
| **12** | 4A | **XOR3 Pin 8 (3Y / $X_5$)** | Partial sum input to Stage 2 |
| **13** | 4B | **OR2 Pin 3 ($C_5$)** | Carry-in from Bit 4 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### XOR4: 74HCT86 (Adder Sum XOR Stages: Bit 6, Bit 7)
* **Function**: Computes partial sum $X_6, X_7$ and final sum $S_6, S_7$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_6$** (Input Bus $A_6$) | Bit 6 Operand A input |
| **2** | 1B | **XOR6 Pin 8 ($B'_6$)** | Bit 6 conditioned Operand B |
| **3** | 1Y | **XOR4 Pin 4 (2A), AND4 Pin 5 (2B), MUX7 Pin 15 ($D_4$)** | $X_6 = A_6 \oplus B'_6$ (Partial sum & XOR op) |
| **4** | 2A | **XOR4 Pin 3 (1Y / $X_6$)** | Partial sum input to Stage 2 |
| **5** | 2B | **OR2 Pin 6 ($C_6$)** | Carry-in from Bit 5 |
| **6** | 2Y | **MUX7 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_6$ (ADD / SUB result bit 6) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR4 Pin 12 (4A), AND4 Pin 13 (4B), MUX8 Pin 15 ($D_4$)** | $X_7 = A_7 \oplus B'_7$ (Partial sum & XOR op) |
| **9** | 3A | **DIP Switch $A_7$** (Input Bus $A_7$) | Bit 7 Operand A input (MSB) |
| **10** | 3B | **XOR6 Pin 11 ($B'_7$)** | Bit 7 conditioned Operand B |
| **11** | 4Y | **MUX8 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Final Sum $S_7$ (ADD / SUB result bit 7) |
| **12** | 4A | **XOR4 Pin 8 (3Y / $X_7$)** | Partial sum input to Stage 2 |
| **13** | 4B | **OR2 Pin 8 ($C_7$)** | Carry-in from Bit 6 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### AND1: 74HCT08 (Adder Carry Generate & Propagate: Bit 0, Bit 1)
* **Function**: Generates carry components $G_0, P_0, G_1, P_1$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 Operand A |
| **2** | 1B | **XOR5 Pin 3 ($B'_0$)** | Bit 0 conditioned Operand B |
| **3** | 1Y | **OR1 Pin 1 (1A)** | $G_0 = A_0 \cdot B'_0$ (Carry generate 0) |
| **4** | 2A | **$SUB$ Control Line ($C_{in0}$)** | Initial Carry-in to Bit 0 |
| **5** | 2B | **XOR1 Pin 3 ($X_0$)** | Bit 0 propagate signal |
| **6** | 2Y | **OR1 Pin 2 (1B)** | $P_0 = C_{in0} \cdot X_0$ (Carry propagate 0) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **OR1 Pin 4 (2A)** | $G_1 = A_1 \cdot B'_1$ (Carry generate 1) |
| **9** | 3A | **DIP Switch $A_1$** | Bit 1 Operand A |
| **10** | 3B | **XOR5 Pin 6 ($B'_1$)** | Bit 1 conditioned Operand B |
| **11** | 4Y | **OR1 Pin 5 (2B)** | $P_1 = C_1 \cdot X_1$ (Carry propagate 1) |
| **12** | 4A | **OR1 Pin 3 ($C_1$)** | Carry ripple from Bit 0 |
| **13** | 4B | **XOR1 Pin 8 ($X_1$)** | Bit 1 propagate signal |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### AND2: 74HCT08 (Adder Carry Generate & Propagate: Bit 2, Bit 3)
* **Function**: Generates carry components $G_2, P_2, G_3, P_3$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_2$** | Bit 2 Operand A |
| **2** | 1B | **XOR5 Pin 8 ($B'_2$)** | Bit 2 conditioned Operand B |
| **3** | 1Y | **OR1 Pin 9 (3A)** | $G_2 = A_2 \cdot B'_2$ (Carry generate 2) |
| **4** | 2A | **OR1 Pin 6 ($C_2$)** | Carry ripple from Bit 1 |
| **5** | 2B | **XOR2 Pin 3 ($X_2$)** | Bit 2 propagate signal |
| **6** | 2Y | **OR1 Pin 10 (3B)** | $P_2 = C_2 \cdot X_2$ (Carry propagate 2) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **OR1 Pin 12 (4A)** | $G_3 = A_3 \cdot B'_3$ (Carry generate 3) |
| **9** | 3A | **DIP Switch $A_3$** | Bit 3 Operand A |
| **10** | 3B | **XOR5 Pin 11 ($B'_3$)** | Bit 3 conditioned Operand B |
| **11** | 4Y | **OR1 Pin 13 (4B)** | $P_3 = C_3 \cdot X_3$ (Carry propagate 3) |
| **12** | 4A | **OR1 Pin 8 ($C_3$)** | Carry ripple from Bit 2 |
| **13** | 4B | **XOR2 Pin 8 ($X_3$)** | Bit 3 propagate signal |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### AND3: 74HCT08 (Adder Carry Generate & Propagate: Bit 4, Bit 5)
* **Function**: Generates carry components $G_4, P_4, G_5, P_5$.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_4$** | Bit 4 Operand A |
| **2** | 1B | **XOR6 Pin 3 ($B'_4$)** | Bit 4 conditioned Operand B |
| **3** | 1Y | **OR2 Pin 1 (1A)** | $G_4 = A_4 \cdot B'_4$ (Carry generate 4) |
| **4** | 2A | **AND6 Pin 11 ($C_{in4}$)** | Gated Carry-in from Mode Gate |
| **5** | 2B | **XOR3 Pin 3 ($X_4$)** | Bit 4 propagate signal |
| **6** | 2Y | **OR2 Pin 2 (1B)** | $P_4 = C_{in4} \cdot X_4$ (Carry propagate 4) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **OR2 Pin 4 (2A)** | $G_5 = A_5 \cdot B'_5$ (Carry generate 5) |
| **9** | 3A | **DIP Switch $A_5$** | Bit 5 Operand A |
| **10** | 3B | **XOR6 Pin 6 ($B'_5$)** | Bit 5 conditioned Operand B |
| **11** | 4Y | **OR2 Pin 5 (2B)** | $P_5 = C_5 \cdot X_5$ (Carry propagate 5) |
| **12** | 4A | **OR2 Pin 3 ($C_5$)** | Carry ripple from Bit 4 |
| **13** | 4B | **XOR3 Pin 8 ($X_5$)** | Bit 5 propagate signal |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### AND4: 74HCT08 (Adder Carry Generate & Propagate: Bit 6, Bit 7)
* **Function**: Generates carry components $G_6, P_6, G_7, P_7$ (and Logic AND Bit 7).
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_6$** | Bit 6 Operand A |
| **2** | 1B | **XOR6 Pin 8 ($B'_6$)** | Bit 6 conditioned Operand B |
| **3** | 1Y | **OR2 Pin 9 (3A)** | $G_6 = A_6 \cdot B'_6$ (Carry generate 6) |
| **4** | 2A | **OR2 Pin 6 ($C_6$)** | Carry ripple from Bit 5 |
| **5** | 2B | **XOR4 Pin 3 ($X_6$)** | Bit 6 propagate signal |
| **6** | 2Y | **OR2 Pin 10 (3B)** | $P_6 = C_6 \cdot X_6$ (Carry propagate 6) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **OR2 Pin 12 (4A) AND MUX8 Pin 2 ($D_2$)** | $G_7 = A_7 \cdot B'_7$ (Carry gen 7 & AND Op Bit 7) |
| **9** | 3A | **DIP Switch $A_7$** | Bit 7 Operand A |
| **10** | 3B | **XOR6 Pin 11 ($B'_7$)** | Bit 7 conditioned Operand B |
| **11** | 4Y | **OR2 Pin 13 (4B)** | $P_7 = C_7 \cdot X_7$ (Carry propagate 7) |
| **12** | 4A | **OR2 Pin 8 ($C_7$)** | Carry ripple from Bit 6 |
| **13** | 4B | **XOR4 Pin 8 ($X_7$)** | Bit 7 propagate signal |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### OR1: 74HCT32 (Adder Carry Chain: Lower Nibble $C_1, C_2, C_3, C_4$)
* **Function**: Combines Generate and Propagate terms to ripple carry across bits 0..3.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **AND1 Pin 3 ($G_0$)** | Carry generate bit 0 |
| **2** | 1B | **AND1 Pin 6 ($P_0$)** | Carry propagate bit 0 |
| **3** | 1Y | **XOR1 Pin 13 (4B) and AND1 Pin 12 (4A)** | $C_1 = G_0 + P_0$ (Carry out of Bit 0) |
| **4** | 2A | **AND1 Pin 8 ($G_1$)** | Carry generate bit 1 |
| **5** | 2B | **AND1 Pin 11 ($P_1$)** | Carry propagate bit 1 |
| **6** | 2Y | **XOR2 Pin 5 (2B) and AND2 Pin 4 (2A)** | $C_2 = G_1 + P_1$ (Carry out of Bit 1) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR2 Pin 13 (4B) and AND2 Pin 12 (4A)** | $C_3 = G_2 + P_2$ (Carry out of Bit 2) |
| **9** | 3A | **AND2 Pin 3 ($G_2$)** | Carry generate bit 2 |
| **10** | 3B | **AND2 Pin 6 ($P_2$)** | Carry propagate bit 2 |
| **11** | 4Y | **AND6 Pin 12 (4A)** | $C_4 = G_3 + P_3$ (Lower Nibble Carry Out) |
| **12** | 4A | **AND2 Pin 8 ($G_3$)** | Carry generate bit 3 |
| **13** | 4B | **AND2 Pin 11 ($P_3$)** | Carry propagate bit 3 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### OR2: 74HCT32 (Adder Carry Chain: Upper Nibble $C_5, C_6, C_7, C_8$)
* **Function**: Combines Generate and Propagate terms to ripple carry across bits 4..7.
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **AND3 Pin 3 ($G_4$)** | Carry generate bit 4 |
| **2** | 1B | **AND3 Pin 6 ($P_4$)** | Carry propagate bit 4 |
| **3** | 1Y | **XOR3 Pin 13 (4B) and AND3 Pin 12 (4A)** | $C_5 = G_4 + P_4$ (Carry out of Bit 4) |
| **4** | 2A | **AND3 Pin 8 ($G_5$)** | Carry generate bit 5 |
| **5** | 2B | **AND3 Pin 11 ($P_5$)** | Carry propagate bit 5 |
| **6** | 2Y | **XOR4 Pin 5 (2B) and AND4 Pin 4 (2A)** | $C_6 = G_5 + P_5$ (Carry out of Bit 5) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR4 Pin 13 (4B) and AND4 Pin 12 (4A)** | $C_7 = G_6 + P_6$ (Carry out of Bit 6) |
| **9** | 3A | **AND4 Pin 3 ($G_6$)** | Carry generate bit 6 |
| **10** | 3B | **AND4 Pin 6 ($P_6$)** | Carry propagate bit 6 |
| **11** | 4Y | **Arduino R4 Pin A0 & ESP32 GPIO 26 (via divider)** | $C_8 = G_7 + P_7$ (Final 8-Bit Carry Out) |
| **12** | 4A | **AND4 Pin 8 ($G_7$)** | Carry generate bit 7 |
| **13** | 4B | **AND4 Pin 11 ($P_7$)** | Carry propagate bit 7 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

# SECTION 2: Subtraction Inversion ICs (XOR5, XOR6)

During subtraction ($SUB = 1$), operand $B$ is bitwise inverted ($B'_k = B_k \oplus 1 = \overline{B_k}$) and $C_{in0} = 1$, creating 2's complement: $A - B = A + \overline{B} + 1$.

### XOR5: 74HCT86 (Subtraction Inverter for Lower Nibble $B[3:0]$)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **$SUB$ Control Signal (Opcode switch $S_0$)** | Active HIGH on Opcode 001 |
| **2** | 1B | **DIP Switch $B_0$** (Input Bus $B_0$) | Raw Operand B Bit 0 |
| **3** | 1Y | **XOR1 Pin 2 and AND1 Pin 2** | $B'_0 = B_0 \oplus SUB$ |
| **4** | 2A | **$SUB$ Control Signal** | Tied to Pin 1 |
| **5** | 2B | **DIP Switch $B_1$** (Input Bus $B_1$) | Raw Operand B Bit 1 |
| **6** | 2Y | **XOR1 Pin 10 and AND1 Pin 10** | $B'_1 = B_1 \oplus SUB$ |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR2 Pin 2 and AND2 Pin 2** | $B'_2 = B_2 \oplus SUB$ |
| **9** | 3A | **DIP Switch $B_2$** (Input Bus $B_2$) | Raw Operand B Bit 2 |
| **10** | 3B | **$SUB$ Control Signal** | Tied to Pin 1 |
| **11** | 4Y | **XOR2 Pin 10 and AND2 Pin 10** | $B'_3 = B_3 \oplus SUB$ |
| **12** | 4A | **DIP Switch $B_3$** (Input Bus $B_3$) | Raw Operand B Bit 3 |
| **13** | 4B | **$SUB$ Control Signal** | Tied to Pin 1 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### XOR6: 74HCT86 (Subtraction Inverter for Upper Nibble $B[7:4]$)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **$SUB$ Control Signal (Opcode switch $S_0$)** | Active HIGH on Opcode 001 |
| **2** | 1B | **DIP Switch $B_4$** (Input Bus $B_4$) | Raw Operand B Bit 4 |
| **3** | 1Y | **XOR3 Pin 2 and AND3 Pin 2** | $B'_4 = B_4 \oplus SUB$ |
| **4** | 2A | **$SUB$ Control Signal** | Tied to Pin 1 |
| **5** | 2B | **DIP Switch $B_5$** (Input Bus $B_5$) | Raw Operand B Bit 5 |
| **6** | 2Y | **XOR3 Pin 10 and AND3 Pin 10** | $B'_5 = B_5 \oplus SUB$ |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **XOR4 Pin 2 and AND4 Pin 2** | $B'_6 = B_6 \oplus SUB$ |
| **9** | 3A | **DIP Switch $B_6$** (Input Bus $B_6$) | Raw Operand B Bit 6 |
| **10** | 3B | **$SUB$ Control Signal** | Tied to Pin 1 |
| **11** | 4Y | **XOR4 Pin 10 and AND4 Pin 10** | $B'_7 = B_7 \oplus SUB$ |
| **12** | 4A | **DIP Switch $B_7$** (Input Bus $B_7$) | Raw Operand B Bit 7 |
| **13** | 4B | **$SUB$ Control Signal** | Tied to Pin 1 |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

# SECTION 3: Logic Operation Gate ICs (AND5–6, OR3–4, NOT1–2)

### AND5: 74HCT08 (Bitwise Logic AND for Bits 0..3)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input A |
| **2** | 1B | **DIP Switch $B_0$** | Bit 0 input B |
| **3** | 1Y | **MUX1 Pin 2 ($D_2$)** | $A_0 \cdot B_0$ (Logic AND Bit 0) |
| **4** | 2A | **DIP Switch $A_1$** | Bit 1 input A |
| **5** | 2B | **DIP Switch $B_1$** | Bit 1 input B |
| **6** | 2Y | **MUX2 Pin 2 ($D_2$)** | $A_1 \cdot B_1$ (Logic AND Bit 1) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **MUX3 Pin 2 ($D_2$)** | $A_2 \cdot B_2$ (Logic AND Bit 2) |
| **9** | 3A | **DIP Switch $A_2$** | Bit 2 input A |
| **10** | 3B | **DIP Switch $B_2$** | Bit 2 input B |
| **11** | 4Y | **MUX4 Pin 2 ($D_2$)** | $A_3 \cdot B_3$ (Logic AND Bit 3) |
| **12** | 4A | **DIP Switch $A_3$** | Bit 3 input A |
| **13** | 4B | **DIP Switch $B_3$** | Bit 3 input B |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### AND6: 74HCT08 (Bitwise Logic AND for Bits 4..6 + Mode Carry Gate)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_4$** | Bit 4 input A |
| **2** | 1B | **DIP Switch $B_4$** | Bit 4 input B |
| **3** | 1Y | **MUX5 Pin 2 ($D_2$)** | $A_4 \cdot B_4$ (Logic AND Bit 4) |
| **4** | 2A | **DIP Switch $A_5$** | Bit 5 input A |
| **5** | 2B | **DIP Switch $B_5$** | Bit 5 input B |
| **6** | 2Y | **MUX6 Pin 2 ($D_2$)** | $A_5 \cdot B_5$ (Logic AND Bit 5) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **MUX7 Pin 2 ($D_2$)** | $A_6 \cdot B_6$ (Logic AND Bit 6) |
| **9** | 3A | **DIP Switch $A_6$** | Bit 6 input A |
| **10** | 3B | **DIP Switch $B_6$** | Bit 6 input B |
| **11** | 4Y | **XOR3 Pin 5 (2B) and AND3 Pin 4 (2A)** | **Gated Carry ($C_{in4} = C_4 \cdot MODE$)** |
| **12** | 4A | **OR1 Pin 11 ($C_4$)** | Lower Nibble Carry Out |
| **13** | 4B | **Mode Toggle Switch (MODE line)** | 1 = 8-Bit Cascade, 0 = Dual 4-Bit |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

*(Note on Bit 7 AND: $A_7 \cdot B_7$ is generated directly by `AND4 Pin 8` and routed to MUX8 Pin 2).*

---

### OR3: 74HCT32 (Bitwise Logic OR for Bits 0..3)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input A |
| **2** | 1B | **DIP Switch $B_0$** | Bit 0 input B |
| **3** | 1Y | **MUX1 Pin 1 ($D_3$)** | $A_0 + B_0$ (Logic OR Bit 0) |
| **4** | 2A | **DIP Switch $A_1$** | Bit 1 input A |
| **5** | 2B | **DIP Switch $B_1$** | Bit 1 input B |
| **6** | 2Y | **MUX2 Pin 1 ($D_3$)** | $A_1 + B_1$ (Logic OR Bit 1) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **MUX3 Pin 1 ($D_3$)** | $A_2 + B_2$ (Logic OR Bit 2) |
| **9** | 3A | **DIP Switch $A_2$** | Bit 2 input A |
| **10** | 3B | **DIP Switch $B_2$** | Bit 2 input B |
| **11** | 4Y | **MUX4 Pin 1 ($D_3$)** | $A_3 + B_3$ (Logic OR Bit 3) |
| **12** | 4A | **DIP Switch $A_3$** | Bit 3 input A |
| **13** | 4B | **DIP Switch $B_3$** | Bit 3 input B |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### OR4: 74HCT32 (Bitwise Logic OR for Bits 4..7)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_4$** | Bit 4 input A |
| **2** | 1B | **DIP Switch $B_4$** | Bit 4 input B |
| **3** | 1Y | **MUX5 Pin 1 ($D_3$)** | $A_4 + B_4$ (Logic OR Bit 4) |
| **4** | 2A | **DIP Switch $A_5$** | Bit 5 input A |
| **5** | 2B | **DIP Switch $B_5$** | Bit 5 input B |
| **6** | 2Y | **MUX6 Pin 1 ($D_3$)** | $A_5 + B_5$ (Logic OR Bit 5) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 3Y | **MUX7 Pin 1 ($D_3$)** | $A_6 + B_6$ (Logic OR Bit 6) |
| **9** | 3A | **DIP Switch $A_6$** | Bit 6 input A |
| **10** | 3B | **DIP Switch $B_6$** | Bit 6 input B |
| **11** | 4Y | **MUX8 Pin 1 ($D_3$)** | $A_7 + B_7$ (Logic OR Bit 7) |
| **12** | 4A | **DIP Switch $A_7$** | Bit 7 input A |
| **13** | 4B | **DIP Switch $B_7$** | Bit 7 input B |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### NOT1: 74HCT04 (Hex Inverter: Bitwise NOT for Bits 0..5)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_0$** | Bit 0 input |
| **2** | 1Y | **MUX1 Pin 14 ($D_5$)** | $\overline{A_0}$ (Logic NOT Bit 0) |
| **3** | 2A | **DIP Switch $A_1$** | Bit 1 input |
| **4** | 2Y | **MUX2 Pin 14 ($D_5$)** | $\overline{A_1}$ (Logic NOT Bit 1) |
| **5** | 3A | **DIP Switch $A_2$** | Bit 2 input |
| **6** | 3Y | **MUX3 Pin 14 ($D_5$)** | $\overline{A_2}$ (Logic NOT Bit 2) |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 4Y | **MUX4 Pin 14 ($D_5$)** | $\overline{A_3}$ (Logic NOT Bit 3) |
| **9** | 4A | **DIP Switch $A_3$** | Bit 3 input |
| **10** | 5Y | **MUX5 Pin 14 ($D_5$)** | $\overline{A_4}$ (Logic NOT Bit 4) |
| **11** | 5A | **DIP Switch $A_4$** | Bit 4 input |
| **12** | 6Y | **MUX6 Pin 14 ($D_5$)** | $\overline{A_5}$ (Logic NOT Bit 5) |
| **13** | 6A | **DIP Switch $A_5$** | Bit 5 input |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

### NOT2: 74HCT04 (Hex Inverter: Bitwise NOT for Bits 6..7 + Spares)
* **Pin 14 ($V_{CC}$)**: +5V Power Rail | **Pin 7 (GND)**: 0V Ground Rail

| Pin | Name | Connected To (Source / Destination) | Function / Notes |
| :---: | :--- | :--- | :--- |
| **1** | 1A | **DIP Switch $A_6$** | Bit 6 input |
| **2** | 1Y | **MUX7 Pin 14 ($D_5$)** | $\overline{A_6}$ (Logic NOT Bit 6) |
| **3** | 2A | **DIP Switch $A_7$** | Bit 7 input |
| **4** | 2Y | **MUX8 Pin 14 ($D_5$)** | $\overline{A_7}$ (Logic NOT Bit 7) |
| **5** | 3A | **0V Ground Rail** | Unused input (Tied to GND to prevent oscillation) |
| **6** | 3Y | *Leave OPEN* | Unused output |
| **7** | GND | **0V Ground Rail** | Power supply ground |
| **8** | 4Y | *Leave OPEN* | Unused output |
| **9** | 4A | **0V Ground Rail** | Unused input (Tied to GND) |
| **10** | 5Y | *Leave OPEN* | Unused output |
| **11** | 5A | **0V Ground Rail** | Unused input (Tied to GND) |
| **12** | 6Y | *Leave OPEN* | Unused output |
| **13** | 6A | **0V Ground Rail** | Unused input (Tied to GND) |
| **14** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

# SECTION 4: Mode Carry Gate (AND6 Gate 4)

The 4-bit / 8-bit configurability is controlled by **Gate 4 of AND6**:
* **Input 1 (Pin 12)**: Lower Nibble Carry Out ($C_4$ from OR1 Pin 11).
* **Input 2 (Pin 13)**: Physical MODE Toggle Switch.
  * When $MODE = 1$ (+5V): $C_{in4} = C_4 \cdot 1 = C_4$ $\implies$ Full 8-bit carry cascade.
  * When $MODE = 0$ (0V): $C_{in4} = C_4 \cdot 0 = 0$ $\implies$ Dual independent 4-bit ALUs.
* **Output (Pin 11)**: Connects to $C_{in4}$ on Upper Nibble Adder (`XOR3 Pin 5` and `AND3 Pin 4`).

---

# SECTION 5: MUX Array (8× 74HCT151: MUX1 through MUX8)

All eight 74HCT151 ICs share common control lines:
* **Pin 16 ($V_{CC}$)**: +5V Power Rail
* **Pin 8 (GND)**: 0V Ground Rail
* **Pin 7 ($\overline{\text{STROBE}}$)**: **Tie directly to 0V Ground Rail** (Active-LOW enable)
* **Pin 11 ($A$ / Select $S_0$ LSB)**: Opcode Switch $S_0$
* **Pin 10 ($B$ / Select $S_1$ Mid)**: Opcode Switch $S_1$
* **Pin 9 ($C$ / Select $S_2$ MSB)**: Opcode Switch $S_2$
* **Pin 6 ($\overline{Y}$)**: *Leave OPEN*

### Complete Multiplexer Bit-Routing Map:

| Pin | MUX1 (Bit 0) | MUX2 (Bit 1) | MUX3 (Bit 2) | MUX4 (Bit 3) | MUX5 (Bit 4) | MUX6 (Bit 5) | MUX7 (Bit 6) | MUX8 (Bit 7) | Opcode Mapped |
| :---: | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **4 ($D_0$)** | XOR1 Pin 6 ($S_0$) | XOR1 Pin 11 ($S_1$) | XOR2 Pin 6 ($S_2$) | XOR2 Pin 11 ($S_3$) | XOR3 Pin 6 ($S_4$) | XOR3 Pin 11 ($S_5$) | XOR4 Pin 6 ($S_6$) | XOR4 Pin 11 ($S_7$) | **ADD** ($A + B$) |
| **3 ($D_1$)** | XOR1 Pin 6 ($S_0$) | XOR1 Pin 11 ($S_1$) | XOR2 Pin 6 ($S_2$) | XOR2 Pin 11 ($S_3$) | XOR3 Pin 6 ($S_4$) | XOR3 Pin 11 ($S_5$) | XOR4 Pin 6 ($S_6$) | XOR4 Pin 11 ($S_7$) | **SUB** ($A - B$) |
| **2 ($D_2$)** | AND5 Pin 3 | AND5 Pin 6 | AND5 Pin 8 | AND5 Pin 11 | AND6 Pin 3 | AND6 Pin 6 | AND6 Pin 8 | AND4 Pin 8 | **AND** ($A \cdot B$) |
| **1 ($D_3$)** | OR3 Pin 3 | OR3 Pin 6 | OR3 Pin 8 | OR3 Pin 11 | OR4 Pin 3 | OR4 Pin 6 | OR4 Pin 8 | OR4 Pin 11 | **OR** ($A + B$) |
| **15 ($D_4$)**| XOR1 Pin 3 ($X_0$) | XOR1 Pin 8 ($X_1$) | XOR2 Pin 3 ($X_2$) | XOR2 Pin 8 ($X_3$) | XOR3 Pin 3 ($X_4$) | XOR3 Pin 8 ($X_5$) | XOR4 Pin 3 ($X_6$) | XOR4 Pin 8 ($X_7$) | **XOR** ($A \oplus B$) |
| **14 ($D_5$)**| NOT1 Pin 2 ($\overline{A_0}$) | NOT1 Pin 4 ($\overline{A_1}$) | NOT1 Pin 6 ($\overline{A_2}$) | NOT1 Pin 8 ($\overline{A_3}$) | NOT1 Pin 10 ($\overline{A_4}$) | NOT1 Pin 12 ($\overline{A_5}$) | NOT2 Pin 2 ($\overline{A_6}$) | NOT2 Pin 4 ($\overline{A_7}$) | **NOT** ($\overline{A}$) |
| **13 ($D_6$)**| **GND Rail (0)** | DIP Switch $A_0$ | DIP Switch $A_1$ | DIP Switch $A_2$ | DIP Switch $A_3$ | DIP Switch $A_4$ | DIP Switch $A_5$ | DIP Switch $A_6$ | **SHL** ($A \ll 1$) |
| **12 ($D_7$)**| DIP Switch $A_1$ | DIP Switch $A_2$ | DIP Switch $A_3$ | DIP Switch $A_4$ | DIP Switch $A_5$ | DIP Switch $A_6$ | DIP Switch $A_7$ | **GND Rail (0)** | **SHR** ($A \gg 1$) |
| **5 ($Y$)** | **BUF1 Pin 2 ($A_1$)** | **BUF1 Pin 3 ($A_2$)** | **BUF1 Pin 4 ($A_3$)** | **BUF1 Pin 5 ($A_4$)** | **BUF1 Pin 6 ($A_5$)** | **BUF1 Pin 7 ($A_6$)** | **BUF1 Pin 8 ($A_7$)** | **BUF1 Pin 9 ($A_8$)** | **Output Result** |

---

# SECTION 6: Output Bus Buffer (BUF1: 74HCT245)

The 74HCT245 isolates the internal multiplexer outputs and provides strong capacitive line drive for downstream MCU connections:
* **Pin 20 ($V_{CC}$)**: +5V Power Rail
* **Pin 10 (GND)**: 0V Ground Rail
* **Pin 1 (DIR)**: **Tie directly to +5V Rail** (Sets drive direction $A \to B$)
* **Pin 19 ($\overline{\text{OE}}$)**: **Tie directly to 0V Ground Rail** (Active-LOW enable)

| Pin | Name | Connected To (Source / Destination) | Functional Description |
| :---: | :--- | :--- | :--- |
| **2** | A1 | **MUX1 Pin 5 ($Y_0$)** | Raw ALU Result Bit 0 |
| **3** | A2 | **MUX2 Pin 5 ($Y_1$)** | Raw ALU Result Bit 1 |
| **4** | A3 | **MUX3 Pin 5 ($Y_2$)** | Raw ALU Result Bit 2 |
| **5** | A4 | **MUX4 Pin 5 ($Y_3$)** | Raw ALU Result Bit 3 |
| **6** | A5 | **MUX5 Pin 5 ($Y_4$)** | Raw ALU Result Bit 4 |
| **7** | A6 | **MUX6 Pin 5 ($Y_5$)** | Raw ALU Result Bit 5 |
| **8** | A7 | **MUX7 Pin 5 ($Y_6$)** | Raw ALU Result Bit 6 |
| **9** | A8 | **MUX8 Pin 5 ($Y_7$)** | Raw ALU Result Bit 7 |
| **10** | GND | **0V Ground Rail** | Power supply ground |
| **11** | B8 | **Arduino R4 Pin D9 & ESP32 GPIO 3 (via divider)** | Buffered Output $Y_{OUT}7$ (MSB) |
| **12** | B7 | **Arduino R4 Pin D8 & ESP32 GPIO 48 (via divider)** | Buffered Output $Y_{OUT}6$ |
| **13** | B6 | **Arduino R4 Pin D7 & ESP32 GPIO 47 (via divider)** | Buffered Output $Y_{OUT}5$ |
| **14** | B5 | **Arduino R4 Pin D6 & ESP32 GPIO 44 (via divider)** | Buffered Output $Y_{OUT}4$ |
| **15** | B4 | **Arduino R4 Pin D5 & ESP32 GPIO 43 (via divider)** | Buffered Output $Y_{OUT}3$ |
| **16** | B3 | **Arduino R4 Pin D4 & ESP32 GPIO 42 (via divider)** | Buffered Output $Y_{OUT}2$ |
| **17** | B2 | **Arduino R4 Pin D3 & ESP32 GPIO 41 (via divider)** | Buffered Output $Y_{OUT}1$ |
| **18** | B1 | **Arduino R4 Pin D2 & ESP32 GPIO 40 (via divider)** | Buffered Output $Y_{OUT}0$ (LSB) |
| **19** | $\overline{\text{OE}}$ | **0V Ground Rail** | Output Enable (Active LOW) |
| **20** | $V_{CC}$ | **+5V Power Rail** | Positive supply voltage |

---

# SECTION 7: Arduino UNO R4 WiFi Verification Interface

The Arduino UNO R4 WiFi computes a C++ software golden model and displays real-time PASS/FAIL status on its onboard 12×8 LED Matrix and external status LEDs.

| Arduino R4 Pin | Connected Source / Destination | Logic Level | Functional Description |
| :---: | :--- | :---: | :--- |
| **5V Pin** | **Breadboard +5V Power Rails** | +5.0V | Supplies main VCC power to all 27 ICs |
| **GND Pin** | **Breadboard Ground Rails & ESP32 GND** | 0V | Common system ground reference |
| **Pin D2** | **BUF1 Pin 18 ($Y_{OUT}0$)** | 5V TTL | Reads hardware result bit 0 |
| **Pin D3** | **BUF1 Pin 17 ($Y_{OUT}1$)** | 5V TTL | Reads hardware result bit 1 |
| **Pin D4** | **BUF1 Pin 16 ($Y_{OUT}2$)** | 5V TTL | Reads hardware result bit 2 |
| **Pin D5** | **BUF1 Pin 15 ($Y_{OUT}3$)** | 5V TTL | Reads hardware result bit 3 |
| **Pin D6** | **BUF1 Pin 14 ($Y_{OUT}4$)** | 5V TTL | Reads hardware result bit 4 |
| **Pin D7** | **BUF1 Pin 13 ($Y_{OUT}5$)** | 5V TTL | Reads hardware result bit 5 |
| **Pin D8** | **BUF1 Pin 12 ($Y_{OUT}6$)** | 5V TTL | Reads hardware result bit 6 |
| **Pin D9** | **BUF1 Pin 11 ($Y_{OUT}7$)** | 5V TTL | Reads hardware result bit 7 |
| **Pin D10** | **Opcode DIP Switch $S_0$** | 5V TTL | Reads Opcode LSB |
| **Pin D11** | **Opcode DIP Switch $S_1$** | 5V TTL | Reads Opcode Mid bit |
| **Pin D12** | **Opcode DIP Switch $S_2$** | 5V TTL | Reads Opcode MSB |
| **Pin A0** | **OR2 Pin 11 ($C_8$)** | 5V TTL | Reads hardware carry-out flag |
| **Pin A1** | **220 Ω Resistor $\to$ 🟢 Green PASS LED $\to$ GND** | 5V Out | Lit when Hardware == Software Golden Model |
| **Pin A2** | **220 Ω Resistor $\to$ 🔴 Red FAIL LED $\to$ GND** | 5V Out | Lit when Hardware $\neq$ Software Golden Model |

---

# SECTION 8: ESP32-S3 Voltage Divider Connections (5V $\to$ 3.3V)

The ESP32-S3 N16R8 operates strictly at 3.3V CMOS logic. Every monitored 5V breadboard line passes through an independent **$1.0\text{ k}\Omega$ series / $2.2\text{ k}\Omega$ shunt** voltage divider:

```
 Breadboard 5V Signal ───────[ 1.0 kΩ ]───────┬───────► ESP32-S3 GPIO Pin (3.3V Safe)
                                              │
                                         [ 2.2 kΩ ]
                                              │
                                             GND
```

| Monitored Signal | Breadboard Source Node | Divider Resistors | ESP32-S3 GPIO | Telemetry Channel |
| :---: | :--- | :---: | :---: | :--- |
| **$Y_{OUT}0$** | BUF1 Pin 18 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 40** | Result Bus Bit 0 |
| **$Y_{OUT}1$** | BUF1 Pin 17 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 41** | Result Bus Bit 1 |
| **$Y_{OUT}2$** | BUF1 Pin 16 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 42** | Result Bus Bit 2 |
| **$Y_{OUT}3$** | BUF1 Pin 15 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 43** | Result Bus Bit 3 |
| **$Y_{OUT}4$** | BUF1 Pin 14 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 44** | Result Bus Bit 4 |
| **$Y_{OUT}5$** | BUF1 Pin 13 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 47** | Result Bus Bit 5 |
| **$Y_{OUT}6$** | BUF1 Pin 12 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 48** | Result Bus Bit 6 |
| **$Y_{OUT}7$** | BUF1 Pin 11 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 3** | Result Bus Bit 7 |
| **Carry $C_8$** | OR2 Pin 11 | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 26** | Carry Out Flag |
| **Mode Switch**| Mode Switch Contact | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 39** | Operational Mode Status |
| **Opcode $S_0$**| DIP Opcode $S_0$ | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 10** | Function Select LSB |
| **Opcode $S_1$**| DIP Opcode $S_1$ | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 11** | Function Select Mid |
| **Opcode $S_2$**| DIP Opcode $S_2$ | $1.0\text{ k}\Omega$ / $2.2\text{ k}\Omega$ | **GPIO 12** | Function Select MSB |

---

# SECTION 9: Hybrid Input Override Resistors (Remote Injection)

To allow the ESP32-S3 to inject arbitrary test vectors over WebSockets without mechanical switch interference:
* Every switch contact connects between $+5\text{V}$ and a $10\text{ k}\Omega$ pull-down to $\text{GND}$.
* An override line connects from the ESP32 GPIO through a **$4.7\text{ k}\Omega$ current-limiting resistor** directly to the gate input node.
* When DIP switches are left in the **OFF (Open)** position:
  $$\text{ESP32 HIGH (3.3V)} \implies V_{in} = 3.3\text{ V} \times \frac{10\text{ k}\Omega}{4.7\text{ k}\Omega + 10\text{ k}\Omega} = 2.24\text{ V} \ge 2.0\text{ V} = V_{IH(\min)} \text{ for 74HCT}$$
  $$\text{ESP32 LOW (0V)} \implies V_{in} = 0.0\text{ V} \le 0.8\text{ V} = V_{IL(\max)} \text{ for 74HCT}$$

| Injected Signal | ESP32-S3 GPIO | Override Resistor | Breadboard Destination Gate Input |
| :---: | :---: | :---: | :--- |
| **$A_0$ Override** | **GPIO 1** | $4.7\text{ k}\Omega$ | XOR1 Pin 1, AND1 Pin 1, AND5 Pin 1, OR3 Pin 1, NOT1 Pin 1 |
| **$A_1$ Override** | **GPIO 2** | $4.7\text{ k}\Omega$ | XOR1 Pin 9, AND1 Pin 9, AND5 Pin 4, OR3 Pin 4, NOT1 Pin 3 |
| **$A_2$ Override** | **GPIO 4** | $4.7\text{ k}\Omega$ | XOR2 Pin 1, AND2 Pin 1, AND5 Pin 9, OR3 Pin 9, NOT1 Pin 5 |
| **$A_3$ Override** | **GPIO 5** | $4.7\text{ k}\Omega$ | XOR2 Pin 9, AND2 Pin 9, AND5 Pin 12, OR3 Pin 12, NOT1 Pin 9 |
| **$B_0$ Override** | **GPIO 6** | $4.7\text{ k}\Omega$ | XOR5 Pin 2, AND5 Pin 2, OR3 Pin 2 |
| **$B_1$ Override** | **GPIO 7** | $4.7\text{ k}\Omega$ | XOR5 Pin 5, AND5 Pin 5, OR3 Pin 5 |
| **$B_2$ Override** | **GPIO 8** | $4.7\text{ k}\Omega$ | XOR5 Pin 9, AND5 Pin 10, OR3 Pin 10 |
| **$B_3$ Override** | **GPIO 9** | $4.7\text{ k}\Omega$ | XOR5 Pin 12, AND5 Pin 13, OR3 Pin 13 |
| **$A_4$ Override** | **GPIO 13** | $4.7\text{ k}\Omega$ | XOR3 Pin 1, AND3 Pin 1, AND6 Pin 1, OR4 Pin 1, NOT1 Pin 11 |
| **$A_5$ Override** | **GPIO 14** | $4.7\text{ k}\Omega$ | XOR3 Pin 9, AND3 Pin 9, AND6 Pin 4, OR4 Pin 4, NOT1 Pin 13 |
| **$A_6$ Override** | **GPIO 15** | $4.7\text{ k}\Omega$ | XOR4 Pin 1, AND4 Pin 1, AND6 Pin 9, OR4 Pin 9, NOT2 Pin 1 |
| **$A_7$ Override** | **GPIO 16** | $4.7\text{ k}\Omega$ | XOR4 Pin 9, AND4 Pin 9, OR4 Pin 12, NOT2 Pin 3 |
| **$B_4$ Override** | **GPIO 17** | $4.7\text{ k}\Omega$ | XOR6 Pin 2, AND6 Pin 2, OR4 Pin 2 |
| **$B_5$ Override** | **GPIO 18** | $4.7\text{ k}\Omega$ | XOR6 Pin 5, AND6 Pin 5, OR4 Pin 5 |
| **$B_6$ Override** | **GPIO 21** | $4.7\text{ k}\Omega$ | XOR6 Pin 9, AND6 Pin 10, OR4 Pin 10 |
| **$B_7$ Override** | **GPIO 38** | $4.7\text{ k}\Omega$ | XOR6 Pin 12, AND4 Pin 10, OR4 Pin 13 |
