# Step-by-Step Breadboard Assembly Guide
## DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU (74LS181N × Arduino R4 WiFi)

---

## 1. Safety & Preparation Guidelines

> [!IMPORTANT]
> **Key Differences in This Alternative Assembly**:
> 1. **Only 1 DIP IC to Wire**: The tedious 10-IC gate-level lower ALU is replaced entirely by the single **74LS181N (DIP-24)**, dramatically reducing wire congestion and assembly time from ~10 hours down to ~1.5 hours.
> 2. **No Discrete LEDs**: You do NOT need to plug in or wire any loose indicator LEDs or series 330Ω resistors. All outputs, bit states, and carry flags are rendered on the Arduino UNO R4 WiFi's built-in **12×8 LED Matrix**!
> 3. **Three-Board Power Isolation**: Never connect the 5V pins of the WiFi R4 or ESP32-S3 to the breadboard logic rails. The Arduino UNO R4 Minima is the sole power provider for the breadboard.

### Required Tools & Materials
- 1× Standard 830-tie-point solderless breadboard (MB-102)
- 1× 74LS181N 4-Bit Bipolar Silicon ALU (24-Pin DIP, 0.6" wide)
- 1× Arduino UNO R4 WiFi (with onboard 12×8 LED Matrix)
- 1× Arduino UNO R4 Minima (dedicated +5V / 1.2A power station)
- 1× ESP32-S3 N16R8 development board
- 2× 8-position DIP switches (SW1: Operand A, SW2: Operand B)
- 1× 4-position DIP switch (SW3: Opcode S[2:0] & Mode M)
- 20× $10\text{ k}\Omega$ $1/4\text{W}$ resistors (pull-downs)
- 19× $4.7\text{ k}\Omega$ $1/4\text{W}$ resistors (ESP32 injection override)
- 9× $1.0\text{ k}\Omega$ and 9× $2.2\text{ k}\Omega$ resistors (5V-to-3.3V voltage dividers)
- 2× $0.1\ \mu\text{F}$ monolithic ceramic capacitors (code 104)
- 1× $100\ \mu\text{F}$ electrolytic bulk capacitor (16V or 25V)
- Jumper wires (assorted colors)
- Multimeter with continuity buzzer

---

## 2. Assembly Stages

### STAGE 1: Power Rails & Decoupling Capacitor Bank
1. Place the breadboard horizontally with the top power rails designated as $+5\text{V}$ (Red) and $\text{GND}$ (Blue/Black).
2. Bridge the top and bottom power rails using two short jumper wires (Red-to-Red, Blue-to-Blue) so both rails share power.
3. Insert the $100\ \mu\text{F}$ electrolytic capacitor across the power rails near the left edge:
   * **Long leg (Anode, +)** $\to$ Red $+5\text{V}$ rail.
   * **Short leg (Cathode with stripe, -)** $\to$ Blue $\text{GND}$ rail.
4. Insert a $0.1\ \mu\text{F}$ ceramic capacitor (104) across the rails alongside the bulk capacitor.
5. **Connect Arduino UNO R4 Minima**:
   * Wire the Minima's **5V pin** to the breadboard $+5\text{V}$ rail.
   * Wire the Minima's **GND pin** to the breadboard $\text{GND}$ rail.
   * **Verification**: Plug a USB-C cable into the Minima. Measure voltage across the breadboard rails with your multimeter. It should read between $+4.95\text{V}$ and $+5.05\text{V}$. Unplug before proceeding!

---

### STAGE 2: Installing IC1 (74LS181N 24-Pin DIP)
1. Locate the wide 0.6" center trough of the breadboard.
2. Seat the **74LS181N** firmly across the trough with the indexing notch oriented to the LEFT:
   * Pins 1 to 12 sit along the bottom row.
   * Pins 13 to 24 sit along the top row.
3. **Power & Ground Connections**:
   * Connect **Pin 24 ($V_{CC}$)** to the top $+5\text{V}$ rail with a short red wire.
   * Connect **Pin 12 (GND)** to the bottom $\text{GND}$ rail with a short black wire.
4. **Local Bypass Capacitor**:
   * Insert a $0.1\ \mu\text{F}$ ceramic capacitor directly between Pin 24 ($V_{CC}$) and Pin 12 (GND). Keep leads short to suppress high-speed bipolar switching transients!
5. **Tie Unused Inputs**:
   * Connect **Pin 7 ($\overline{C}_n$)** to $+5\text{V}$ (Active-LOW: $+5\text{V}$ means no initial carry-in for addition).
   * Leave Pins 14 ($X$) and 15 ($Y$) unconnected (NC).
   * Connect a $10\text{ k}\Omega$ resistor from **Pin 17 ($A=B$)** to $+5\text{V}$ (open-collector pull-up).

---

### STAGE 3: DIP Switches & Pull-Down Resistors
1. **Place SW1 (8-Pos DIP - Operand A)** on the left side of the breadboard:
   * Connect all 8 pins on the input side to the $+5\text{V}$ rail.
   * Connect all 8 pins on the output side to 8 independent tie-point columns (buses $A_0$ to $A_7$).
   * Insert eight $10\text{ k}\Omega$ resistors from each column ($A_0..A_7$) to the $\text{GND}$ rail.
2. **Place SW2 (8-Pos DIP - Operand B)** adjacent to SW1:
   * Connect all 8 pins on the input side to $+5\text{V}$.
   * Connect all 8 pins on the output side to 8 columns (buses $B_0$ to $B_7$).
   * Insert eight $10\text{ k}\Omega$ resistors from each column ($B_0..B_7$) to the $\text{GND}$ rail.
3. **Place SW3 (4-Pos DIP - Opcode & Mode)**:
   * Connect the 4 input pins to $+5\text{V}$.
   * Output pins form buses $S_0, S_1, S_2$, and $M$.
   * Insert four $10\text{ k}\Omega$ resistors to $\text{GND}$.

---

### STAGE 4: Wiring the 74LS181N Inputs & Outputs
1. **Operand A Lower Nibble ($A_0..A_3$)**:
   * Wire Bus $A_0$ (SW1 Pin 1) $\to$ 74LS181 **Pin 2 ($\overline{A}_0$)**.
   * Wire Bus $A_1$ (SW1 Pin 2) $\to$ 74LS181 **Pin 23 ($\overline{A}_1$)**.
   * Wire Bus $A_2$ (SW1 Pin 3) $\to$ 74LS181 **Pin 21 ($\overline{A}_2$)**.
   * Wire Bus $A_3$ (SW1 Pin 4) $\to$ 74LS181 **Pin 19 ($\overline{A}_3$)**.
2. **Operand B Lower Nibble ($B_0..B_3$)**:
   * Wire Bus $B_0$ (SW2 Pin 1) $\to$ 74LS181 **Pin 1 ($\overline{B}_0$)**.
   * Wire Bus $B_1$ (SW2 Pin 2) $\to$ 74LS181 **Pin 22 ($\overline{B}_1$)**.
   * Wire Bus $B_2$ (SW2 Pin 3) $\to$ 74LS181 **Pin 20 ($\overline{B}_2$)**.
   * Wire Bus $B_3$ (SW2 Pin 4) $\to$ 74LS181 **Pin 18 ($\overline{B}_3$)**.
3. **Opcode Controls**:
   * Wire Bus $S_0$ $\to$ 74LS181 **Pin 6 ($S_0$)**.
   * Wire Bus $S_1$ $\to$ 74LS181 **Pin 5 ($S_1$)**.
   * Wire Bus $S_2$ $\to$ 74LS181 **Pin 4 ($S_2$)**.
   * Wire 74LS181 **Pin 3 ($S_3$)** to Bus $S_2$ (or map to match arithmetic functions).
   * Wire Bus $M$ $\to$ 74LS181 **Pin 8 ($M$)**.

---

### STAGE 5: Arduino UNO R4 WiFi Interfacing
1. **Common Ground Connection**:
   * Connect an Arduino UNO R4 WiFi **GND pin** to the breadboard Common GND rail.
   * **LEAVE THE 5V PIN DISCONNECTED**.
2. **Carry Cascade Wire Link (The Core Interconnect)**:
   * Connect a jumper wire directly from **74LS181N Pin 16 ($\overline{C}_{n+4}$)** to Arduino UNO R4 WiFi Pin **D6**.
3. **Lower Nibble Results Sensing**:
   * 74LS181 Pin 9 ($\overline{F}_0$) $\to$ Arduino Pin **D2**.
   * 74LS181 Pin 10 ($\overline{F}_1$) $\to$ Arduino Pin **D3**.
   * 74LS181 Pin 11 ($\overline{F}_2$) $\to$ Arduino Pin **D4**.
   * 74LS181 Pin 13 ($\overline{F}_3$) $\to$ Arduino Pin **D5**.
4. **Upper Nibble Inputs Sensing**:
   * Bus $A_4..A_7$ $\to$ Arduino Pins **D7, D8, D9, D10**.
   * Bus $B_4..B_7$ $\to$ Arduino Pins **D11, D12, D13, A0**.
5. **Opcode & Mode Sensing**:
   * Bus $S_0, S_1, S_2$ $\to$ Arduino Pins **A1, A2, A3**.
   * Bus $M$ $\to$ Arduino Pin **A4**.
   * Bus $C_0$ $\to$ Arduino Pin **A5**.

---

### STAGE 6: ESP32-S3 Dashboard & Override Resistors
1. **Common Ground Connection**:
   * Connect an ESP32-S3 **GND pin** to the breadboard Common GND rail.
   * **LEAVE THE VIN / 5V PIN DISCONNECTED**.
2. **Install 19× $4.7\text{ k}\Omega$ Override Resistors**:
   * From ESP32 GPIO 1, 2, 4, 5 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus lines $A_0..A_3$.
   * From ESP32 GPIO 6, 7, 8, 9 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus lines $A_4..A_7$.
   * From ESP32 GPIO 10..13 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus lines $B_0..B_3$.
   * From ESP32 GPIO 14..17 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus lines $B_4..B_7$.
   * From ESP32 GPIO 18, 35, 36 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus lines $S_0, S_1, S_2$.
   * From ESP32 GPIO 37 $\to$ $4.7\text{ k}\Omega$ $\to$ Bus line $M$.
3. **Install 9× Voltage Divider Pairs ($1.0\text{ k}\Omega / 2.2\text{ k}\Omega$)**:
   * On breadboard tie points, form 9 voltage divider nodes:
     $$\text{5V Signal} \longrightarrow [1.0\text{ k}\Omega] \longrightarrow \text{Node (Tap to ESP32)} \longrightarrow [2.2\text{ k}\Omega] \longrightarrow \text{GND}$$
   * Connect the taps to ESP32 GPIO 38..41 ($Y_0..Y_3$), GPIO 42, 47, 48, 21 ($Y_4..Y_7$), and GPIO 3 ($C_8$).

---

## 3. Power-On Checklist & Smoke Test

1. **Pre-Power Continuity Check (Multimeter in Ohms/Buzzer)**:
   * Measure resistance between the breadboard $+5\text{V}$ rail and $\text{GND}$. It should read $> 1\text{ k}\Omega$ (charging capacitors). If it reads $< 10\ \Omega$, STOP: there is a short!
   * Verify continuity between Minima GND, WiFi R4 GND, and ESP32 GND.
2. **Power-On Sequence**:
   * **Step 1**: Plug USB-C into Arduino UNO R4 Minima (Powers breadboard logic).
   * **Step 2**: Plug USB-C into Arduino UNO R4 WiFi (The 12×8 matrix should instantly light up!).
   * **Step 3**: Plug USB-C into ESP32-S3 (Wi-Fi AP `DECA-ALU-HYBRID` becomes active).
3. **Smoke Test Verification**:
   * Touch the 74LS181N chip with your finger. It should feel lukewarm ($30^\circ\text{C}-40^\circ\text{C}$). If it is burning hot, disconnect immediately!
   * Toggle SW1 Pin 1 ($A_0 = 1$) and SW2 Pin 1 ($B_0 = 1$) with Opcode `000` (ADD).
   * Look at the Arduino 12×8 LED Matrix: Column 10 ($Y_1$) should light up ($1 + 1 = 2$), while Column 11 ($Y_0$) goes dark!
