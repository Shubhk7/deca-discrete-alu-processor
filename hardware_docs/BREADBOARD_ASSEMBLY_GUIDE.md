# Breadboard Assembly & Testing Guide (Step-by-Step)
## 8-Bit / 4-Bit Configurable Discrete Gate-Level ALU with ESP32-S3 HIL & Arduino R4 Verifier

This guide provides a modular, benchtop-tested assembly protocol for constructing the **8-Bit / 4-Bit Configurable Gate-Level ALU** on solderless breadboards. 

By assembling the circuit in discrete, verifiable stages and executing the **Testing Checklist** at the end of each section, you eliminate wiring bugs incrementally and ensure rapid, flawless project completion.

---

## Required Hardware Components

* **Breadboards**: 4× standard 830 tie-point solderless breadboards mounted side-by-side on an aluminum or acrylic backplane.
* **Master Power Source**: Arduino UNO R4 WiFi via USB-C cable (provides regulated +5V DC @ up to 1.5A to the breadboard power rails).
* **Microcontroller Hosts**:
  * 1× **Arduino UNO R4 WiFi** (Real-time software golden model verifier + onboard $12\times 8$ LED matrix scorekeeper).
  * 1× **ESP32-S3 N16R8** (Local Wi-Fi Access Point, WebSocket telemetry server, and remote input injector).
* **Integrated Circuits (74HCT Series DIP)**:
  * 6× **74HCT86** (Quad 2-Input XOR Gate) — Full Adder sums + Subtraction conditioning + Logic XOR
  * 7× **74HCT08** (Quad 2-Input AND Gate) — Full Adder carry terms + Logic AND + Mode toggle gating
  * 5× **74HCT32** (Quad 2-Input OR Gate) — Full Adder carry combine + Logic OR + Mode combine
  * 2× **74HCT04** (Hex Inverter) — Logic NOT + Complement generation
  * 8× **74HCT151** (8:1 Multiplexer) — Operation selection per bit ($Y_0$ through $Y_7$)
  * 1× **74HCT245** (Octal Bus Transceiver) — Output bus buffer / LED driver
* **Switches & Passive Components**:
  * 2× 8-position DIP switches (Operand $A[7:0]$ and Operand $B[7:0]$)
  * 1× 4-position DIP switch (Opcode $S_0, S_1, S_2$ and Mode switch)
  * 20× $10\text{ k}\Omega$ 1/4W resistors (DIP switch pull-downs)
  * 19× $4.7\text{ k}\Omega$ 1/4W resistors (ESP32 remote override and pin protection)
  * 13× $1.0\text{ k}\Omega$ 1/4W resistors (ESP32 voltage divider upper arm $R_1$)
  * 13× $2.2\text{ k}\Omega$ 1/4W resistors (ESP32 voltage divider lower arm $R_2$)
  * 12× $220\ \Omega$ or $330\ \Omega$ resistors (LED current limiters)
  * 29× $0.1\ \mu\text{F}$ monolithic ceramic capacitors (IC power decoupling)
  * 8× Blue/Green LEDs (8-bit output bus $Y[7:0]$)
  * 1× Amber/Yellow LED (Carry-out $C_{out8}$)
  * 1× Bright Green LED (Verifier PASS)
  * 1× Bright Red LED (Verifier FAIL)
* **Wiring Material**: Pre-formed solid-core 22 AWG jumper wire kit + male-to-male and male-to-female flexible DuPont jumpers.

---

## Power Distribution & Rail Setup

```
   Arduino R4 WiFi
  ┌───────────────┐
  │      5V Pin ──┼───────────────────────► Top Red (+5V) Rail (All Breadboards)
  │               │
  │     GND Pin ──┼───────────────────────► Top Blue (GND) Rail (All Breadboards)
  └───────────────┘                          │
                                             ▼
  ┌──────────────────────────────────────────────────────────────────────────────┐
  │ Common Ground Bus (Interconnects Arduino GND, ESP32 GND, and All Rail GNDs)  │
  └──────────────────────────────────────────────────────────────────────────────┘
```

1. **Interconnect Horizontal Rails**: Use short 22 AWG jumpers to link the top and bottom $+5\text{ V}$ (red) and $\text{GND}$ (blue) power rails across all 4 breadboards.
2. **Common Ground Rule**: Connect the Arduino UNO R4 `GND` pin, the ESP32-S3 `GND` pin, and all breadboard ground rails together. **A shared, unbroken ground reference is strictly required** for reliable logic switching and level shifting.
3. **Decoupling Capacitors**: Place a $0.1\ \mu\text{F}$ ceramic capacitor directly across the $V_{CC}$ and $\text{GND}$ pins of every IC socket location to absorb high-frequency transient switching current.

---

## Section 1: Place Input DIP Switches & Pull-Down Resistors

Mount the physical input controls on Breadboard #1 (Leftmost edge):

```
        +5V Rail ───────────────┬───────────────────────────────┐
                                │                               │
                          [ DIP SW A0 ]                   [ DIP SW B0 ]
                                │                               │
    IC Input Line <─────────────┼───────────────────────────────┤
                                │                               │
                         [ 10k Resistor ]                [ 10k Resistor ]
                                │                               │
        GND Rail ───────────────┴───────────────────────────────┘
```

1. **Mount DIP Switches**:
   * **SW 1 (Operand A)**: 8-position switch ($A_0..A_7$). Connect the common top pins to the $+5\text{ V}$ rail.
   * **SW 2 (Operand B)**: 8-position switch ($B_0..B_7$). Connect the common top pins to the $+5\text{ V}$ rail.
   * **SW 3 (Control)**: 4-position switch.
     * Switch 1 = Opcode $S_0$ (LSB)
     * Switch 2 = Opcode $S_1$
     * Switch 3 = Opcode $S_2$ (MSB)
     * Switch 4 = **MODE** ($1 = \text{8-Bit Unified}, 0 = \text{Dual 4-Bit}$)
2. **Install $10\text{ k}\Omega$ Pull-Down Resistors**:
   * For each switch output pin (20 lines total), install a $10\text{ k}\Omega$ resistor connected directly to the breadboard $\text{GND}$ rail.
   * This guarantees that when a switch is open (OFF), the line sits cleanly at $0.0\text{ V}$ rather than floating.

### ✅ Testing Checklist 1:
- [ ] Connect Arduino USB-C to power the $+5\text{ V}$ rail.
- [ ] Multimeter test: Measure voltage between the $+5\text{ V}$ rail and $\text{GND}$ rail. Verify $+4.9\text{ V}$ to $+5.1\text{ V}$.
- [ ] Switch test: Measure the output node of switch $A_0$ with a multimeter.
  - When switch is OFF: Voltage must be $< 0.1\text{ V}$.
  - When switch is ON: Voltage must be $> 4.8\text{ V}$.
- [ ] Repeat test for $B_0..B_7$, $S_0..S_2$, and MODE. Ensure no pin is floating.

---

## Section 2: Wire Gate-Level Full Adder & Subtractor

Assemble the 8-bit gate-level ripple-carry adder across Breadboards #1 and #2.

```
       B[i] ─────────────┐
                         ├───────[ 74HCT86 XOR ]──────── B'[i]
        SUB ─────────────┘
                           │
       A[i] ───────────────┼─────────────────────────┐
                           │                         │
                           ├─────[ 74HCT86 XOR ]─────┴─► P[i] ───┐
       B'[i] ──────────────┘                                     ├─────[ 74HCT86 XOR ]──────► Sum S[i]
                                                                 │
     Cin[i] ─────────────────────────────────────────────────────┴────────────────────────┐
                                                                                          │
       A[i] ───────────────┐                                                              │
                           ├─────[ 74HCT08 AND ]─────► G[i] ────────┐                     │
      B'[i] ───────────────┘                                        │                     │
                                                                    ├─────[ 74HCT32 OR ]──┴──► Cout[i]
       P[i] ───────────────┐                                        │
                           ├─────[ 74HCT08 AND ]─────► K[i] ────────┘
     Cin[i] ───────────────┘
```

### 2.1 Subtraction Conditioning Network ($B \oplus SUB$):
* Use 2× **74HCT86** ICs (8 XOR gates total).
* One input of every XOR gate connects to the common $SUB$ signal (Opcode $S_0$).
* The second input receives raw operand $B_i$.
* The output produces conditioned operand $B'_i$:
  $$B'_i = B_i \oplus SUB$$

### 2.2 Lower Nibble Adder (Bits 0–3):
* Construct four 1-bit full adder cells using 74HCT86, 74HCT08, and 74HCT32.
* **Bit 0**:
  * $C_{in0} = SUB$ (Connect directly to the $SUB$ line so that $C_{in0} = 1$ when subtracting).
  * Compute Half-Sum $P_0 = A_0 \oplus B'_0$ and Full Sum $S_0 = P_0 \oplus C_{in0}$.
  * Compute $C_{out0} = (A_0 \cdot B'_0) + (C_{in0} \cdot P_0)$.
* **Bits 1, 2, 3**:
  * Ripple $C_{out0} \to C_{in1}$, $C_{out1} \to C_{in2}$, $C_{out2} \to C_{in3}$.
  * Outputs produce Sum bits $S_0, S_1, S_2, S_3$ and lower nibble carry $C_{out3}$.

### 2.3 Mode Toggle Carry Gate:
* Wire the carry steering between nibble 0 and nibble 1 using 1 AND gate (from 74HCT08), 1 inverter (from 74HCT04), and 1 OR gate (from 74HCT32):
  $$C_{in4} = (C_{out3} \cdot \text{MODE}) + (SUB \cdot \overline{\text{MODE}})$$
* When $\text{MODE} = 1$: $C_{in4} = C_{out3}$ (Standard 8-Bit Addition / Subtraction).
* When $\text{MODE} = 0$: $C_{in4} = SUB$ (Two independent 4-bit ALUs running in parallel).

### 2.4 Upper Nibble Adder (Bits 4–7):
* Construct four 1-bit full adder cells for bits 4 through 7.
* Ripple carries from $C_{in4}$ through to $C_{out7}$ ($C_{out8}$ master carry flag).
* Outputs produce Sum bits $S_4, S_5, S_6, S_7$.

### ✅ Testing Checklist 2:
- [ ] Connect 4 temporary test LEDs with $330\ \Omega$ resistors to $S_0..S_3$.
- [ ] Test 4-bit addition: Set $A = 0101_2$ (5) and $B = 0011_2$ (3), $SUB=0$. LEDs must read $1000_2$ (8).
- [ ] Test 4-bit subtraction: Set $A = 0111_2$ (7) and $B = 0010_2$ (2), $SUB=1$. LEDs must read $0101_2$ (5).
- [ ] Test Mode Toggle: Set $A_3=1, B_3=1$ to generate $C_{out3}=1$. Toggle the MODE switch:
  - When $\text{MODE} = 1$: Verify that $C_{in4} = 1$ (causes bit 4 to increment).
  - When $\text{MODE} = 0$: Verify that $C_{in4} = 0$ (upper nibble remains isolated).
- [ ] Remove temporary LEDs before proceeding.

---

## Section 3: Wire Bitwise Logic Operations (AND, OR, XOR, NOT)

Place the logic operation ICs on Breadboard #2 and #3:

1. **Bitwise AND (74HCT08)**:
   * Wire $A_i$ and $B_i$ to each AND gate input ($i = 0..7$).
   * Outputs produce $AND_0..AND_7$.
2. **Bitwise OR (74HCT32)**:
   * Wire $A_i$ and $B_i$ to each OR gate input ($i = 0..7$).
   * Outputs produce $OR_0..OR_7$.
3. **Bitwise XOR (74HCT86)**:
   * Wire $A_i$ and $B_i$ to each XOR gate input ($i = 0..7$).
   * Outputs produce $XOR_0..XOR_7$.
4. **Bitwise NOT (74HCT04)**:
   * Wire $A_i$ to each inverter input ($i = 0..7$).
   * Outputs produce $NOT_0..NOT_7$ ($\overline{A_0}..\overline{A_7}$).

### ✅ Testing Checklist 3:
- [ ] Probe pin 3 of 74HCT08 (Bit 0 AND): Set $A_0=1, B_0=1 \implies 5\text{V}$; Set $A_0=1, B_0=0 \implies 0\text{V}$.
- [ ] Probe pin 3 of 74HCT32 (Bit 0 OR): Set $A_0=0, B_0=1 \implies 5\text{V}$; Set $A_0=0, B_0=0 \implies 0\text{V}$.
- [ ] Probe pin 3 of 74HCT86 (Bit 0 XOR): Set $A_0=1, B_0=1 \implies 0\text{V}$; Set $A_0=1, B_0=0 \implies 5\text{V}$.
- [ ] Probe pin 2 of 74HCT04 (Bit 0 NOT): Set $A_0=1 \implies 0\text{V}$; Set $A_0=0 \implies 5\text{V}$.

---

## Section 4: Wire 74HCT151 MUX Array (8 MUXes, One per Output Bit)

Mount the eight **74HCT151** multiplexer ICs along Breadboard #3 and #4. Each IC corresponds to one bit of the ALU output word:

```
  MUX IC0 -> Bit 0 (Y0)    MUX IC2 -> Bit 2 (Y2)    MUX IC4 -> Bit 4 (Y4)    MUX IC6 -> Bit 6 (Y6)
  MUX IC1 -> Bit 1 (Y1)    MUX IC3 -> Bit 3 (Y3)    MUX IC5 -> Bit 5 (Y5)    MUX IC7 -> Bit 7 (Y7)
```

1. **Power & Control Bus Distribution**:
   * Pin 16 $\to +5\text{ V}$, Pin 8 $\to \text{GND}$ on all 8 ICs.
   * **Strobe ($\overline{EN}$, Pin 7)**: Tie permanently to **GND** on all 8 ICs.
   * **Select Lines**: Connect Pin 11 ($S_0$), Pin 10 ($S_1$), and Pin 9 ($S_2$) in parallel across all 8 MUXes, and wire back to the Opcode switches.
2. **Data Input Wiring per Bit $i$**:
   * **Pin 4 ($D_0$)**: Adder Sum bit $S_i$ (for ADD).
   * **Pin 3 ($D_1$)**: Adder Sum bit $S_i$ (for SUB — conditioned when $SUB=1$).
   * **Pin 2 ($D_2$)**: Logic $AND_i$ (from 74HCT08).
   * **Pin 1 ($D_3$)**: Logic $OR_i$ (from 74HCT32).
   * **Pin 15 ($D_4$)**: Logic $XOR_i$ (from 74HCT86).
   * **Pin 14 ($D_5$)**: Logic $NOT_i$ (from 74HCT04).
   * **Pin 13 ($D_6$)**: **Shift Left (SHL)**:
     * For bit $i=0$: Tie Pin 13 to **GND** (0 shifted into LSB).
     * For bits $i=1..7$: Connect Pin 13 to Operand bit $A_{i-1}$.
   * **Pin 12 ($D_7$)**: **Shift Right (SHR)**:
     * For bit $i=7$: Tie Pin 12 to **GND** (0 shifted into MSB).
     * For bits $i=0..6$: Connect Pin 12 to Operand bit $A_{i+1}$.
3. **Connect to 74HCT245 Bus Buffer**:
   * Connect Pin 5 ($Y$) of MUX 0..7 to Pins 2..9 ($A_0..A_7$) of the 74HCT245.
   * Tie 74HCT245 Pin 1 (DIR) to $+5\text{ V}$ and Pin 19 ($\overline{OE}$) to $\text{GND}$.

### ✅ Testing Checklist 4:
- [ ] Set Opcode $S = 000$ (ADD): Verify MUX outputs follow adder sum.
- [ ] Set Opcode $S = 010$ (AND): Verify MUX outputs follow $A \cdot B$.
- [ ] Set Opcode $S = 110$ (SHL): Set $A = 00000001_2 \implies Y$ must output $00000010_2$.
- [ ] Set Opcode $S = 111$ (SHR): Set $A = 10000000_2 \implies Y$ must output $01000000_2$.

---

## Section 5: Wire Output to Arduino R4 Pins

The Arduino UNO R4 WiFi is a **5V-tolerant microcontroller**. Connect the buffered ALU outputs directly to the Arduino female headers:

```
  74HCT245 Output Bus               Arduino UNO R4 WiFi
  ┌──────────────────┐              ┌──────────────────┐
  │  Pin 18 (Y0) ────┼─────────────►│  Digital Pin D2  │
  │  Pin 17 (Y1) ────┼─────────────►│  Digital Pin D3  │
  │  Pin 16 (Y2) ────┼─────────────►│  Digital Pin D4  │
  │  Pin 15 (Y3) ────┼─────────────►│  Digital Pin D5  │
  │  Pin 14 (Y4) ────┼─────────────►│  Digital Pin D6  │
  │  Pin 13 (Y5) ────┼─────────────►│  Digital Pin D7  │
  │  Pin 12 (Y6) ────┼─────────────►│  Digital Pin D8  │
  │  Pin 11 (Y7) ────┼─────────────►│  Digital Pin D9  │
  └──────────────────┘              │                  │
  Opcode S0 ───────────────────────►│  Digital Pin D10 │
  Opcode S1 ───────────────────────►│  Digital Pin D11 │
  Opcode S2 ───────────────────────►│  Analog Pin A4   │
  Mode Switch ─────────────────────►│  Analog Pin A3   │
  Carry Out (Cout8) ───────────────►│  Analog Pin A5   │
                                    └──────────────────┘
```

1. Run neat, labeled solid jumper wires from the 74HCT245 outputs ($B_0..B_7$) to Arduino pins D2 through D9.
2. Connect Opcode switches $S_0, S_1, S_2$ to pins D10, D11, and A4.
3. Connect the Mode switch node to pin A3.
4. Connect the adder carry-out ($C_{out8}$) to pin A5.

### ✅ Testing Checklist 5:
- [ ] Open Arduino IDE, upload the verification sketch `arduino_verifier.ino`.
- [ ] Open Serial Monitor @ 115200 baud.
- [ ] Change DIP switch inputs: Verify that the Serial Monitor prints the exact physical input values and reports `PASS` for all valid combinations.

---

## Section 6: Wire Voltage Dividers for ESP32 Monitoring

> [!CAUTION]
> **CRITICAL VOLTAGE SAFETY**: The ESP32-S3 operates at **3.3V logic**. Connecting 5V signals directly to its GPIO pins will permanently destroy the ESP32 chip. You MUST construct a $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ divider for every monitored line.

Assemble the resistor divider array on Breadboard #4:

```
  5V ALU Signal (from 74HCT245) ────[ R1 = 1.0 kΩ ]────┬────► ESP32-S3 GPIO Pin
                                                       │
                                                 [ R2 = 2.2 kΩ ]
                                                       │
  Common Ground Bus ───────────────────────────────────┴────► GND
```

1. For each of the 8 output bits ($Y_0..Y_7$), Carry-Out ($C_{out8}$), Opcode ($S_0..S_2$), and Mode:
   * Solder/insert a $1.0\text{ k}\Omega$ resistor between the 5V signal node and an intermediate tie-point.
   * Solder/insert a $2.2\text{ k}\Omega$ resistor between the intermediate tie-point and the $\text{GND}$ rail.
2. Connect DuPont jumpers from the intermediate tie-points to the ESP32-S3 pins:
   * $Y_0 \to \text{GPIO 40}$
   * $Y_1 \to \text{GPIO 41}$
   * $Y_2 \to \text{GPIO 42}$
   * $Y_3 \to \text{GPIO 43}$
   * $Y_4 \to \text{GPIO 44}$
   * $Y_5 \to \text{GPIO 47}$
   * $Y_6 \to \text{GPIO 48}$
   * $Y_7 \to \text{GPIO 3}$
   * $C_{out8} \to \text{GPIO 26}$
   * $\text{MODE} \to \text{GPIO 39}$
   * $S_0 \to \text{GPIO 10}, S_1 \to \text{GPIO 11}, S_2 \to \text{GPIO 12}$

### ✅ Testing Checklist 6:
- [ ] **MULTIMETER SAFETY CHECK BEFORE CONNECTING ESP32**:
  - Set all ALU outputs to HIGH (+5V).
  - Measure voltage at each divider output with the ESP32 disconnected.
  - Measured voltage MUST be between $+3.2\text{ V}$ and $+3.45\text{ V}$.
  - If any point reads $+5\text{ V}$, stop immediately and inspect the $2.2\text{ k}\Omega$ ground return resistor!
- [ ] Once all 13 points are verified below $3.45\text{ V}$, plug the jumpers into the ESP32-S3.

---

## Section 7: Wire Hybrid Input Override Resistors

To enable the ESP32-S3 to remotely inject test vectors and override manual DIP switches via its web dashboard:

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

1. For all 16 operand inputs ($A_0..A_7$ and $B_0..B_7$) and 3 opcode lines:
   * Place a $4.7\text{ k}\Omega$ resistor between the ESP32 GPIO pin and the corresponding IC input pin (the node between the DIP switch and the $10\text{ k}\Omega$ pull-down).
2. ESP32 Pin Allocation for Input Injection:
   * $A_0..A_3 \to \text{GPIO 1, 2, 4, 5}$
   * $B_0..B_3 \to \text{GPIO 6, 7, 8, 9}$
   * $A_4..A_7 \to \text{GPIO 15, 16, 17, 18}$
   * $B_4..B_7 \to \text{GPIO 21, 38, 45, 46}$

### ✅ Testing Checklist 7:
- [ ] In manual mode (ESP32 pins in `INPUT` mode), toggle DIP switches and verify normal manual ALU operation.
- [ ] Connect to ESP32 Web Dashboard (`http://192.168.4.1`) from a laptop or phone.
- [ ] Inject remote test vector (e.g., $A=0xAA, B=0x55, \text{Opcode}=\text{ADD}$):
  - Ensure DIP switches are turned OFF (open).
  - Verify that the ALU outputs $0xFF$ on physical LEDs and dashboard telemetry.

---

## Section 8: Wire Status LEDs & Output Display

Install visual indicators on Breadboard #4:

```
  Output Y[i] (from 74HCT245) ─────[ 330Ω ]─────► 💡 Blue LED ─────► GND

  Arduino Pin D12 (PASS) ─────────[ 220Ω ]─────► 🟢 Green LED ────► GND
  Arduino Pin D13 (FAIL) ─────────[ 220Ω ]─────► 🔴 Red LED ──────► GND
  Carry Out Cout8 ────────────────[ 330Ω ]─────► 🟡 Amber LED ────► GND
```

1. **8-Bit Bus LEDs**: Connect 8 Blue or Green 3mm LEDs with individual $330\ \Omega$ current-limiting resistors from 74HCT245 outputs ($B_0..B_7$) to $\text{GND}$.
2. **Carry LED**: Connect 1 Yellow LED with a $330\ \Omega$ resistor to the $C_{out8}$ line.
3. **Verification Scoreboard LEDs**:
   * Connect a bright **Green LED** from Arduino Pin D12 through a $220\ \Omega$ resistor to GND (**PASS**).
   * Connect a bright **Red LED** from Arduino Pin D13 through a $220\ \Omega$ resistor to GND (**FAIL**).

---

## Master System Verification Matrix

Run through this complete test suite once assembly is finished to certify 100% hardware operation:

| Test # | Mode | Opcode | Operand A | Operand B | Expected Output $Y[7:0]$ | Expected $C_{out8}$ | Verifier Status |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | 8-Bit (`1`) | `000` (ADD) | `0010 0011` (35) | `0001 0101` (21) | `0011 1000` (`0x38`, 56) | `0` | **🟢 PASS** |
| **2** | 8-Bit (`1`) | `000` (ADD) | `1111 0000` (240)| `0011 0000` (48) | `0010 0000` (`0x20`, 32) | `1` (Carry) | **🟢 PASS** |
| **3** | 8-Bit (`1`) | `001` (SUB) | `0101 0101` (85) | `0001 0001` (17) | `0100 0100` (`0x44`, 68) | `1` (No borrow)| **🟢 PASS** |
| **4** | 8-Bit (`1`) | `010` (AND) | `1111 0000` | `1010 1010` | `1010 0000` (`0xA0`) | `0` | **🟢 PASS** |
| **5** | 8-Bit (`1`) | `011` (OR)  | `1100 0011` | `0011 1100` | `1111 1111` (`0xFF`) | `0` | **🟢 PASS** |
| **6** | 8-Bit (`1`) | `100` (XOR) | `1010 1100` | `0110 1100` | `1100 0000` (`0xC0`) | `0` | **🟢 PASS** |
| **7** | 8-Bit (`1`) | `101` (NOT) | `1010 1010` | (ignored) | `0101 0101` (`0x55`) | `0` | **🟢 PASS** |
| **8** | 8-Bit (`1`) | `110` (SHL) | `0101 0011` | (ignored) | `1010 0110` (`0xA6`) | `0` | **🟢 PASS** |
| **9** | 8-Bit (`1`) | `111` (SHR) | `1010 0110` | (ignored) | `0101 0011` (`0x53`) | `0` | **🟢 PASS** |
| **10**| 4-Bit (`0`) | `000` (ADD) | `1001 1001` | `1001 1001` | `0010 0010` (Dual nibble) | `0` (Isolated)| **🟢 PASS** |
