# Breadboard Assembly & Testing Guide (Step-by-Step)
## 4-Bit ALU Hardware Prototype

Follow this modular step-by-step guide. **Never wire the entire circuit at once.** Testing each stage before moving to the next guarantees that you will catch errors immediately.

---

## Required Tools & Materials Checklist

* 1–2 Solderless Breadboards (830 tie-points recommended).
* Regulated 5V DC Power Supply (or 5V from an Arduino/USB power breakout).
* ICs:
  * 1× 74LS283 (Adder)
  * 2× 74LS86 (Quad XOR)
  * 1× 74LS08 (Quad AND)
  * 1× 74LS32 (Quad OR)
  * 1× 74LS04 (Hex Inverter)
  * 4× 74LS151 or 74HCT151 (8:1 MUX)
* Passive Components:
  * 1× 8-position DIP switch (for Operands A & B).
  * 1× 3-position or 4-position DIP switch (for Opcode $S_2, S_1, S_0$).
  * 11× 10 kΩ resistors (pull-down resistors for switches).
  * 5× 330 Ω resistors (current-limiting for output LEDs).
  * 5× LEDs (4 for Result $Y_3..Y_0$, 1 for Carry/Status).
  * 8× 0.1 µF (104) ceramic bypass capacitors (one for each IC).
  * Solid core wire jumpers (color-coded recommended: Red=+5V, Black=GND, Blue=Data, Yellow=Control).

---

## Stage 1: Power Rails & Input Switches (Foundation)

```
       +5V Rail ----------------------------------------+
                                                        |
                                                  [ DIP SWITCH ]
                                                        |
   Input Line (to IC inputs) <--------------------------+
                                                        |
                                                 [ 10k Resistor ]
                                                        |
       GND Rail ----------------------------------------+
```

1. **Connect Power Rails**:
   * Link top and bottom breadboard red rails ($+5\text{ V}$) together.
   * Link top and bottom breadboard blue/black rails ($\text{GND}$) together.
2. **Mount DIP Switches**:
   * Place the 8-pin DIP switch on the left side of the breadboard:
     * Switches 1 to 4: Operand $A$ ($A_0, A_1, A_2, A_3$).
     * Switches 5 to 8: Operand $B$ ($B_0, B_1, B_2, B_3$).
   * Place the 3-pin DIP switch nearby:
     * Switches 1 to 3: Opcode ($S_0, S_1, S_2$).
3. **Wire Pull-Down Resistors (CRUCIAL)**:
   * Connect one side of each switch to $+5\text{ V}$.
   * Connect the other side of each switch to a **$10\text{ k}\Omega$ resistor to GND**.
   * Take your logic signal directly from the junction between the switch and the resistor.
   > **Why?** When the switch is open, the input is pulled cleanly to 0V (LOW). When closed, it connects to +5V (HIGH). Without this, open TTL/CMOS pins float and cause erratic switching.

---

## Stage 2: Arithmetic Unit (74LS283 + 74LS86)

In this stage, you build the 4-bit Adder/Subtractor.

### Wiring Connections:
1. **Power**:
   * 74LS283: Pin 16 $\to +5\text{ V}$, Pin 5 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$.
   * 74LS86 (IC #1): Pin 14 $\to +5\text{ V}$, Pin 7 $\to \text{GND}$.
   * Insert a $0.1\ \mu\text{F}$ capacitor across Pin 14/16 and Pin 7/8 on each IC.
2. **Subtraction XOR Control (74LS86 IC #1)**:
   * Connect $SUB$ signal (from $S_0$ switch) to:
     * 74LS86 Pin 1 (Gate 1 input)
     * 74LS86 Pin 4 (Gate 2 input)
     * 74LS86 Pin 10 (Gate 3 input)
     * 74LS86 Pin 13 (Gate 4 input)
     * 74LS283 Pin 12 ($C_0$, Carry In)
3. **Operand B XOR Gating**:
   * $B_0 \to$ 74LS86 Pin 2 $\implies$ Output Pin 3 ($B'_0$) $\to$ 74LS283 Pin 11 ($B_0$)
   * $B_1 \to$ 74LS86 Pin 5 $\implies$ Output Pin 6 ($B'_1$) $\to$ 74LS283 Pin 2 ($B_1$)
   * $B_2 \to$ 74LS86 Pin 9 $\implies$ Output Pin 8 ($B'_2$) $\to$ 74LS283 Pin 14 ($B_2$)
   * $B_3 \to$ 74LS86 Pin 12 $\implies$ Output Pin 11 ($B'_3$) $\to$ 74LS283 Pin 6 ($B_3$)
4. **Operand A Direct Connection**:
   * $A_0 \to$ 74LS283 Pin 10 ($A_0$)
   * $A_1 \to$ 74LS283 Pin 15 ($A_1$)
   * $A_2 \to$ 74LS283 Pin 3 ($A_2$)
   * $A_3 \to$ 74LS283 Pin 7 ($A_3$)
5. **Temporary Test Verification**:
   * Connect four temporary LEDs (with 330 Ω resistors) to the Adder Sum outputs:
     * Pin 13 ($S_0$), Pin 1 ($S_1$), Pin 4 ($S_2$), Pin 1 ($S_3$).
   * Connect one LED to Pin 9 ($C_4$, Carry Out).
   * **Test Addition ($SUB = 0$)**:
     * Set $A = 0011$ (3) and $B = 0010$ (2) $\implies$ LEDs must show `0101` (5).
   * **Test Subtraction ($SUB = 1$)**:
     * Set $A = 0111$ (7) and $B = 0011$ (3) $\implies$ LEDs must show `0100` (4).
   * **Once verified, remove the temporary LEDs!**

---

## Stage 3: Logic Gates (AND, OR, XOR, NOT)

1. **Place ICs**:
   * 74LS08 (AND), 74LS32 (OR), 74LS86 (XOR IC #2), 74LS04 (NOT).
   * Connect Pin 14 $\to +5\text{ V}$, Pin 7 $\to \text{GND}$ on all 4 ICs.
   * Add $0.1\ \mu\text{F}$ decoupling capacitors across power pins.
2. **Wire Gate Inputs**:
   * For bit $k$ ($0$ to $3$):
     * Connect $A_k$ and $B_k$ to the corresponding inputs of the AND gate (74LS08).
     * Connect $A_k$ and $B_k$ to the corresponding inputs of the OR gate (74LS32).
     * Connect $A_k$ and $B_k$ to the corresponding inputs of the XOR gate (74LS86 IC #2).
     * Connect $A_k$ to the input of the NOT inverter (74LS04).

---

## Stage 4: Multiplexer Output Selection (4× 74LS151)

Place the four 74LS151 ICs side by side.
* **MUX 0**: Selects Bit 0 ($Y_0$)
* **MUX 1**: Selects Bit 1 ($Y_1$)
* **MUX 2**: Selects Bit 2 ($Y_2$)
* **MUX 3**: Selects Bit 3 ($Y_3$)

### Common Connections on All 4 MUX ICs:
1. **Power**: Pin 16 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$.
2. **Strobe/Enable (Pin 7)**: **TIE TO GND** (active-low enable).
3. **Select Lines**:
   * Pin 11 ($A$ / $S_0$ LSB) $\to$ Connect to Opcode Switch $S_0$.
   * Pin 10 ($B$ / $S_1$ Mid) $\to$ Connect to Opcode Switch $S_1$.
   * Pin 9 ($C$ / $S_2$ MSB) $\to$ Connect to Opcode Switch $S_2$.
4. **Data Input Connections for MUX $k$ (Bit $k$)**:
   * **Pin 4 ($D_0$) & Pin 3 ($D_1$)**: Tie together $\to$ Connect to 74LS283 Sum Output $S_k$.
   * **Pin 2 ($D_2$)**: Connect to 74LS08 AND Output $k$.
   * **Pin 1 ($D_3$)**: Connect to 74LS32 OR Output $k$.
   * **Pin 15 ($D_4$)**: Connect to 74LS86 XOR Output $k$.
   * **Pin 14 ($D_5$)**: Connect to 74LS04 NOT Output $k$.
   * **Pin 13 ($D_6$)**: Connect to Incrementer Output $k$ (or tie to Adder if using $A+1$).
   * **Pin 12 ($D_7$)**: **Tie to GND** (Constant `0` for CLEAR).

---

## Stage 5: Final Output LEDs

1. Connect the four MUX Pin 5 ($Y$ outputs) to four output LEDs:
   * MUX 0 Pin 5 $\to 330\ \Omega \to$ LED 0 ($Y_0$ - LSB) $\to \text{GND}$
   * MUX 1 Pin 5 $\to 330\ \Omega \to$ LED 1 ($Y_1$) $\to \text{GND}$
   * MUX 2 Pin 5 $\to 330\ \Omega \to$ LED 2 ($Y_2$) $\to \text{GND}$
   * MUX 3 Pin 5 $\to 330\ \Omega \to$ LED 3 ($Y_3$ - MSB) $\to \text{GND}$
2. Connect 74LS283 Pin 9 ($C_4$) to the Carry LED:
   * Pin 9 $\to 330\ \Omega \to$ Carry LED $\to \text{GND}$

---

## Stage 6: Full Verification Checklist

Run through these 8 test vectors with your switches:

| Opcode ($S_2 S_1 S_0$) | Operation | Operand A | Operand B | Expected LEDs ($Y_3 Y_2 Y_1 Y_0$) | Carry LED | Result Status |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: |
| `000` | ADD | `0101` (5) | `0011` (3) | `1000` (8) | 0 | [ ] PASS |
| `000` | ADD (Overflow) | `1111` (15) | `0001` (1) | `0000` (0) | 1 | [ ] PASS |
| `001` | SUB ($A \ge B$) | `0111` (7) | `0010` (2) | `0101` (5) | 1 (No borrow) | [ ] PASS |
| `010` | AND | `1100` | `1010` | `1000` | - | [ ] PASS |
| `011` | OR | `1100` | `1010` | `1110` | - | [ ] PASS |
| `100` | XOR | `1100` | `1010` | `0110` | - | [ ] PASS |
| `101` | NOT A | `1010` | (any) | `0101` | - | [ ] PASS |
| `111` | CLEAR | `1111` | `1111` | `0000` | - | [ ] PASS |
