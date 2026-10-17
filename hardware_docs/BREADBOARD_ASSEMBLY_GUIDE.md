# Breadboard Assembly & Testing Guide (Step-by-Step)
## Dual-Mode 8-Bit ALU with 74LS181 Golden Model Verifier

Follow this modular step-by-step assembly guide. Testing each stage before proceeding guarantees you will never be stuck with difficult-to-trace wiring bugs.

---

## Required Hardware Components

* **Breadboards**: 2 to 3 standard solderless breadboards (830 tie-points).
* **Power Supply**: Regulated 5V DC (minimum 1.5A) or USB-to-5V breadboard power module.
* **Integrated Circuits (DIP)**:
  * **Custom 4-Bit ALU**: 1× 74LS283 (Adder), 2× 74LS86 (XOR), 1× 74LS08 (AND), 1× 74LS32 (OR), 1× 74LS04 (NOT), 4× 74LS151 (8:1 MUX).
  * **Commercial Golden ALU**: 1× 74LS181 (4-bit ALU, 24-pin DIP).
  * **Hardware Equivalence Verifier**: 1× 74LS85 (4-bit Magnitude Comparator).
  * **Mode Steering (Optional)**: 2× 74LS157 (Quad 2:1 MUX) or an 8-pole slide switch.
* **Passives & Indicators**:
  * 2× 8-position DIP switches (for 8-bit Operands $A$ and $B$).
  * 1× 4-position DIP switch (Opcode $S_2, S_1, S_0$ + Mode Select).
  * 20× 10 kΩ pull-down resistors for switches.
  * 10× 330 Ω current-limiting resistors for LEDs.
  * 8× Blue/Yellow LEDs (8-bit output bus $Y_7..Y_0$).
  * 1× Green LED ("PASS / VERIFIED") + 1× Red LED ("MISMATCH / FAULT").
  * 10× 0.1 µF ceramic bypass capacitors (one across $V_{CC}$/GND of each IC).

---

## Stage 1: Power Distribution & Input Switch Rails

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

1. **Link Power Rails**: Interconnect the $+5\text{ V}$ and $\text{GND}$ rails across all breadboards.
2. **Mount DIP Switches**:
   * **DIP SW 1 (Operand A)**: 8 positions ($A_0..A_3$ Lower Nibble, $A_4..A_7$ Upper Nibble).
   * **DIP SW 2 (Operand B)**: 8 positions ($B_0..B_3$ Lower Nibble, $B_4..B_7$ Upper Nibble).
   * **DIP SW 3 (Control)**: Positions 1–3 = Opcode ($S_0, S_1, S_2$), Position 4 = **MODE SWITCH** ($0 = \text{Compute}, 1 = \text{Verify}$).
3. **Pull-Down Resistors (10 kΩ)**: Connect a $10\text{ k}\Omega$ resistor between every switch output line and GND.

---

## Stage 2: Custom 4-Bit ALU Core Assembly

Assemble your discrete gate-level ALU on the first breadboard:
1. **74LS283 (Adder) + 74LS86 (Subtraction XOR)**:
   * Power: Pin 16 & 5 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$. Add $0.1\ \mu\text{F}$ capacitor.
   * Connect $SUB$ line ($S_0$ when in arithmetic mode) to 74LS283 Pin 12 ($C_0$) and to one input of all 4 XOR gates on 74LS86 #1.
   * Feed $B_0..B_3$ through the XOR gates to 74LS283 inputs $B_0..B_3$.
   * Connect $A_0..A_3$ directly to 74LS283 inputs $A_0..A_3$.
2. **Logic Gate Array (74LS08, 74LS32, 74LS86 #2, 74LS04)**:
   * Power: Pin 14 $\to +5\text{ V}$, Pin 7 $\to \text{GND}$. Add decoupling capacitors.
   * Wire $A_k, B_k$ to the corresponding AND, OR, XOR, and NOT inputs for bits $0..3$.
3. **Multiplexer Bank (4× 74LS151)**:
   * Power: Pin 16 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$.
   * **Strobe (Pin 7)**: Tie to $\text{GND}$ on all 4 ICs.
   * Select Lines: Connect Pins 11, 10, 9 to Opcode switches $S_0, S_1, S_2$.
   * Data Inputs for MUX $k$:
     * $D_0, D_1$ (Pins 4, 3) $\to$ 74LS283 Sum Output $S_k$.
     * $D_2$ (Pin 2) $\to$ 74LS08 AND Output $k$.
     * $D_3$ (Pin 1) $\to$ 74LS32 OR Output $k$.
     * $D_4$ (Pin 15) $\to$ 74LS86 XOR Output $k$.
     * $D_5$ (Pin 14) $\to$ 74LS04 NOT Output $k$.
     * $D_7$ (Pin 12) $\to \text{GND}$ (CLEAR).
4. **Intermediate Checkpoint**: Verify the Custom ALU outputs on 4 temporary LEDs. Once verified, leave outputs $Y_0..Y_3$ connected to the lower 4 display LEDs.

---

## Stage 3: Genuine 74LS181 ALU Integration

Mount the 74LS181 across the center trough of Breadboard #2:
1. **Power**: Pin 24 $\to +5\text{ V}$, Pin 12 $\to \text{GND}$. Place a $0.1\ \mu\text{F}$ capacitor across Pins 24 and 12.
2. **Control Inputs**:
   * Mode ($M$, Pin 8): Connect to Mode signal decoded from opcode table.
   * Select Lines ($S_3..S_0$, Pins 3, 4, 5, 6): Connect to control decoder or DIP selector.
3. **Data Inputs**:
   * Operand A ($A_0..A_3$): Pins 2, 23, 21, 19.
   * Operand B ($B_0..B_3$): Pins 1, 22, 20, 18.
4. **Data Outputs**:
   * $F_0..F_3$ (Pins 9, 10, 11, 14): Connect to Upper LEDs ($Y_4..Y_7$) and to the 74LS85 comparator.

---

## Stage 4: 74LS85 Real-Time Equivalence Verifier

Mount the 74LS85 4-bit Magnitude Comparator:
1. **Power**: Pin 16 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$.
2. **Cascading Pins**:
   * Pin 3 ($I_{A=B}$): **TIE TO +5V**.
   * Pin 2 ($I_{A<B}$) & Pin 4 ($I_{A>B}$): Tie to $\text{GND}$.
3. **Comparison Routing**:
   * Input $A$ (Pins 10, 12, 13, 15) $\to$ Connect to **Custom ALU Outputs $Y_0, Y_1, Y_2, Y_3$**.
   * Input $B$ (Pins 9, 11, 14, 1) $\to$ Connect to **74LS181 Outputs $F_0, F_1, F_2, F_3$**.
4. **Telemetry Indicators**:
   * Pin 6 ($O_{A=B}$) $\to 330\ \Omega \to$ **Green "PASS / VERIFIED" LED** $\to \text{GND}$.
   * Pin 6 $\to$ Inverter (spare 74LS04 gate) $\to 330\ \Omega \to$ **Red "MISMATCH" LED** $\to \text{GND}$.

---

## Stage 5: Dual-Mode Steering (Mode 0 vs Mode 1)

* **Mode 0 (8-Bit Compute)**:
  * Upper nibble switches ($A_7..A_4$ and $B_7..B_4$) route to 74LS181.
  * Custom ALU Carry-Out ($C_4$, 74LS283 Pin 9) routes to 74LS181 Carry-In ($\overline{C_n}$, Pin 15).
  * 8-bit output ($Y_7..Y_0$) lights up the full 8-LED bus.
* **Mode 1 (4-Bit Verify)**:
  * Lower nibble switches ($A_3..A_0$ and $B_3..B_0$) route to **both** Custom ALU and 74LS181.
  * Both ALUs execute the exact same operation simultaneously.
  * The 74LS85 compares both results: **Green LED lights up if identical!**

---

## Stage 6: Dual-Mode Demonstration Test Vectors

### Mode 1 Test: Real-Time Hardware Verification
Set Mode Switch = `1` (Verify). Set any 4-bit values for $A$ and $B$:

| Opcode | Operation | Input A | Input B | Custom ALU ($Y_3..Y_0$) | 74LS181 ($F_3..F_0$) | Comparator Status | Green LED |
| :---: | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| `000` | ADD | `0101` (5) | `0011` (3) | `1000` (8) | `1000` (8) | $A = B$ | **ON (PASS)** |
| `001` | SUB | `0111` (7) | `0010` (2) | `0101` (5) | `0101` (5) | $A = B$ | **ON (PASS)** |
| `010` | AND | `1100` | `1010` | `1000` | `1000` | $A = B$ | **ON (PASS)** |
| `011` | OR  | `1100` | `1010` | `1110` | `1110` | $A = B$ | **ON (PASS)** |
| `100` | XOR | `1100` | `1010` | `0110` | `0110` | $A = B$ | **ON (PASS)** |
| `101` | NOT | `1010` | (any)  | `0101` | `0101` | $A = B$ | **ON (PASS)** |

### Mode 0 Test: Full 8-Bit Computation
Set Mode Switch = `0` (Compute). Test 8-bit operands:

| Operation | 8-Bit Operand A | 8-Bit Operand B | Expected 8-Bit Result | Hex Result | Carry Out ($C_8$) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **8-Bit ADD** | `0010 0011` (35) | `0001 0101` (21) | `0011 1000` (56) | `0x38` | 0 |
| **8-Bit ADD** | `1111 0000` (240) | `0010 0000` (32) | `0001 0000` (16) | `0x10` | 1 (Carry) |
| **8-Bit AND** | `1111 0000` | `1010 1010` | `1010 0000` | `0xA0` | 0 |
| **8-Bit OR**  | `1100 0011` | `0011 1100` | `1111 1111` | `0xFF` | 0 |
