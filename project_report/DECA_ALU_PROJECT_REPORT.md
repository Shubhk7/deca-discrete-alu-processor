# Minor Project Report: 4-Bit ALU with Register Bank and Reconfigurable Architecture

**Course**: Digital Electronics & Computer Architecture (DECA)  
**Project Title**: Design, Simulation, and Hardware Prototyping of a 4-Bit Arithmetic Logic Unit (ALU) with Register Bank  
**Author**: Shubh Kashyap  

---

## 1. Abstract

This project presents the comprehensive design, software simulation, and physical hardware realization of a 4-bit Arithmetic Logic Unit (ALU) integrated with a clocked register bank and control unit. The physical prototype is constructed using standard Transistor-Transistor Logic (74LS series) Integrated Circuits, incorporating an arithmetic full adder/subtractor, discrete logic gates, and 8-to-1 multiplexers for operational selection. The circuit supports 8 operations: Addition, Subtraction (using two's complement arithmetic), bitwise AND, bitwise OR, bitwise XOR, bitwise NOT, Increment, and Clear. A dual-stage prototyping methodology was employed: the system was initially verified on a solderless breadboard before migration to a permanent, solder-assembled dot-matrix PCB (perfboard) with socketed ICs. Furthermore, the complete processor datapath—including operand registers, accumulator, clock generation, and status flags (Carry, Zero, Sign)—is modeled and simulated in Logisim Evolution. The design demonstrates core digital principles including modular datapath design, propagation delay considerations, and hardware-software co-verification.

---

## 2. Introduction & Architectural Overview

The Arithmetic Logic Unit (ALU) is the fundamental computational core of every central processing unit (CPU). It executes elementary mathematical and logical instructions dictated by the control unit. In modern computer architecture, ALUs are embedded within complex pipelined datapaths; however, understanding discrete gate-level operation, bit-slicing, and control decoding is essential for digital systems design.

The system is structured into four primary functional units:
1. **Input Interface**: 4-bit Operands $A$ and $B$, conditioned with pull-down resistor arrays, and a 3-bit Opcode selector ($S_2, S_1, S_0$).
2. **Arithmetic Core**: 4-bit binary full adder (74LS283) paired with conditional two's complement inversion gates (74LS86) for addition and subtraction.
3. **Logic Array**: Parallel combinational logic array providing bitwise AND (74LS08), OR (74LS32), XOR (74LS86), and NOT (74LS04) operations.
4. **Operation Multiplexer**: A bank of four 8-to-1 data selectors (74LS151) routing the designated result to the 4-bit output bus ($Y_3 Y_2 Y_1 Y_0$).
5. **Sequential Storage (Simulation)**: Clock-driven edge-triggered D-flip-flop registers for operand retention and result accumulation.

```
                  +-----------------------------------+
                  |   4-Bit Operand A & Operand B    |
                  +-----------------+-----------------+
                                    |
          +-------------------------+-------------------------+
          |                         |                         |
          v                         v                         v
+--------------------+    +--------------------+    +--------------------+
|  Arithmetic Core   |    |    Logic Array     |    |   Special Logic    |
| (74LS283 + 74LS86) |    | (AND, OR, XOR, NOT)|    |   (INC / CLEAR)    |
+---------+----------+    +---------+----------+    +---------+----------+
          |                         |                         |
          |       8 Candidate Result Signals (Bits 0..3)      |
          +-------------------------+-------------------------+
                                    |
                                    v
                  +-----------------------------------+
                  |  4x 74LS151 8:1 Multiplexer Bank  | <-- Opcode (S2 S1 S0)
                  +-----------------+-----------------+
                                    |
                                    v
                  +-----------------------------------+
                  | 4-Bit Output Bus + Status Flags   |
                  +-----------------------------------+
```

---

## 3. Operations & Truth Table

The 3-bit control code ($S_2, S_1, S_0$) specifies 8 distinct operations:

| Opcode ($S_2 S_1 S_0$) | Operation | Category | Mathematical Function | Carry Out ($C_{out}$) Behavior |
| :---: | :--- | :--- | :--- | :--- |
| `000` | **ADD** | Arithmetic | $Y = A + B$ | $1$ if $A + B > 15$ (Overflow) |
| `001` | **SUB** | Arithmetic | $Y = A - B = A + \overline{B} + 1$ | $1$ if $A \ge B$ (No borrow); $0$ if borrow |
| `010` | **AND** | Logical | $Y_i = A_i \cdot B_i$ | Not asserted ($0$) |
| `011` | **OR** | Logical | $Y_i = A_i + B_i$ | Not asserted ($0$) |
| `100` | **XOR** | Logical | $Y_i = A_i \oplus B_i$ | Not asserted ($0$) |
| `101` | **NOT A** | Logical | $Y_i = \overline{A_i}$ | Not asserted ($0$) |
| `110` | **INC A** | Arithmetic | $Y = A + 1$ | $1$ if $A = 15$ ($1111_2 + 1$) |
| `111` | **CLEAR** | Control | $Y_i = 0$ | $0$ |

---

## 4. Mathematical Formulations & Circuit Derivations

### 4.1 Two's Complement Subtraction
Binary subtraction of $B$ from $A$ is converted into two's complement addition:
$$A - B = A + (-B) = A + \overline{B} + 1$$

To implement both addition and subtraction on a single 74LS283 adder without hardware duplication, a conditional inversion control signal $SUB$ is used:
$$SUB = \overline{S_2} \cdot \overline{S_1} \cdot S_0$$
For each bit $i \in \{0, 1, 2, 3\}$, the modified operand $B'_i$ is:
$$B'_i = B_i \oplus SUB$$
When $SUB = 0$ (Addition): $B'_i = B_i \oplus 0 = B_i$, and Carry-In $C_{in} = SUB = 0$.  
When $SUB = 1$ (Subtraction): $B'_i = B_i \oplus 1 = \overline{B_i}$, and Carry-In $C_{in} = SUB = 1$.

### 4.2 Carry vs. Borrow Distinction
In standard two's complement binary adders:
* For addition, $C_{out} = 1$ indicates a true arithmetic carry out of the 4th bit.
* For subtraction, the adder computes $A + (15 - B) + 1 = A - B + 16$.
  * If $A \ge B$, the result is $\ge 16$, generating an end-around carry ($C_{out} = 1$, representing **no borrow**).
  * If $A < B$, the result is $< 16$, producing no carry ($C_{out} = 0$, representing **borrow required**).
  * True Borrow Flag $= \overline{C_{out}}$.

### 4.3 Incrementer Formulation
For the increment operation ($A + 1$):
* $Y_0 = \overline{A_0}$
* $Y_1 = A_1 \oplus A_0$
* $Y_2 = A_2 \oplus (A_1 \cdot A_0)$
* $Y_3 = A_3 \oplus (A_2 \cdot A_1 \cdot A_0)$

---

## 5. Hardware Implementation & Bill of Materials

### 5.1 Component List
* **74LS283**: 4-bit binary full adder with lookahead carry.
* **74LS86** (2 ICs): Quad 2-input XOR gates (IC #1: conditional subtraction inverter; IC #2: logic XOR).
* **74LS08**: Quad 2-input AND gates.
* **74LS32**: Quad 2-input OR gates.
* **74LS04**: Hex inverters.
* **74LS151 / 74HCT151** (4 ICs): 8-to-1 data selectors/multiplexers.
* **Passive Components**: 10 kΩ pull-down DIP resistor network, 330 Ω LED current-limiting resistors, 0.1 µF ceramic bypass capacitors.

### 5.2 Breadboard to Perfboard Migration
To ensure high physical reliability:
1. **IC Sockets**: Solderless 14-pin and 16-pin dual-wipe DIP sockets were soldered onto the dot-matrix PCB. Chips are never soldered directly to prevent thermal stress.
2. **Noise Immunity**: 0.1 µF bypass capacitors are positioned directly across pins $V_{CC}$ and $\text{GND}$ of each socket to decouple high-frequency switching transients.
3. **Floating Inputs**: All mechanical DIP switch outputs incorporate 10 kΩ pull-down resistors to define valid logic LOW states when open.

---

## 6. Simulation & Verification Results

### 6.1 Demonstration Test Vectors
The circuit was verified across all functional modes:

| Test # | Vector ($A, B$) | Opcode | Expected $Y$ | Observed $Y$ | Carry | Status |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 1 | $A=0101, B=0011$ | `000` (ADD) | `1000` (8) | `1000` | 0 | PASS |
| 2 | $A=1111, B=0001$ | `000` (ADD) | `0000` (0) | `0000` | 1 | PASS |
| 3 | $A=0111, B=0010$ | `001` (SUB) | `0101` (5) | `0101` | 1 (No borrow) | PASS |
| 4 | $A=0010, B=0101$ | `001` (SUB) | `1101` (-3 in 2's comp) | `1101` | 0 (Borrow) | PASS |
| 5 | $A=1100, B=1010$ | `010` (AND) | `1000` | `1000` | 0 | PASS |
| 6 | $A=1100, B=1010$ | `011` (OR) | `1110` | `1110` | 0 | PASS |
| 7 | $A=1100, B=1010$ | `100` (XOR) | `0110` | `0110` | 0 | PASS |
| 8 | $A=1010, B=XXXX$ | `101` (NOT) | `0101` | `0101` | 0 | PASS |
| 9 | $A=0110, B=XXXX$ | `110` (INC) | `0111` | `0111` | 0 | PASS |
| 10| $A=1111, B=XXXX$ | `111` (CLEAR)| `0000` | `0000` | 0 | PASS |

---

## 7. Viva-Voce Preparation Questions & Model Answers

### Q1: Why did you use 8-to-1 multiplexers instead of logic gates for output selection?
**Answer**: Multiplexers provide a clean, standardized, and modular method to select among multiple candidate functions without creating bus contention or requiring complex tri-state bus logic. With four 74LS151 ICs, each bit of the output is independently steered by the 3-bit opcode, reducing gate count and propagation delay uncertainty.

### Q2: How does the same adder circuit perform both addition and subtraction?
**Answer**: By exploiting two's complement arithmetic ($A - B = A + \overline{B} + 1$). The subtrahend $B$ is selectively inverted using XOR gates controlled by the $SUB$ line ($B \oplus 1 = \overline{B}$), and the same $SUB$ line is connected to the adder's carry-in ($C_0$). Thus, setting $SUB=1$ simultaneously inverts $B$ and adds $1$.

### Q3: Why is Carry-Out ($C_{out}$) active HIGH when subtracting larger numbers from smaller numbers?
**Answer**: In two's complement subtraction, $C_{out} = 1$ signifies that the result did not underflow (i.e., $A \ge B$), meaning **no borrow was needed**. If $A < B$, an underflow occurs and $C_{out} = 0$, indicating a borrow. Therefore, the actual borrow signal is $\overline{C_{out}}$.

### Q4: Why are pull-down resistors necessary on DIP switches?
**Answer**: When a mechanical switch is open, the input terminal is disconnected and floating. TTL logic treats floating inputs unpredictably (often drifting to logic HIGH due to internal emitter pull-up), while CMOS inputs oscillate rapidly and consume high power. A 10 kΩ pull-down resistor forces the node reliably to Ground (0V) when the switch is open.

### Q5: What is the purpose of decoupling capacitors across the IC power pins?
**Answer**: When TTL gates switch states, both output transistors momentarily conduct, creating short transient current spikes on the power rail. A 0.1 µF ceramic capacitor placed adjacent to each IC socket acts as a local reservoir to supply these transients, preventing voltage drops and inductive ringing from glitching neighboring gates.

---

## 8. Conclusion

The 4-bit Arithmetic Logic Unit successfully executes 8 fundamental arithmetic and logical operations, verifying the foundational principles of computer datapath engineering. The transition from software simulation in Logisim Evolution to modular breadboard testing, and finally to a permanent, socketed perfboard prototype, provided valuable experience in noise decoupling, logic family interfacing, and practical hardware debugging.
