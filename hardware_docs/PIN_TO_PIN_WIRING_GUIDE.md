# Master Pin-to-Pin Interconnection Guide
## Benchtop Physical Wiring Manual: Dual-Mode Discrete & Silicon ALU Processor
### Exact 12-IC Inventory + 3-Board Infrastructure (ESP32-S3 + Arduino UNO R4 WiFi + Arduino UNO R4 Minima)

This document is the **definitive physical benchtop assembly manual** for the Dual-Mode Discrete & Silicon ALU Processor. Every physical pin across all 12 Integrated Circuits (ICs), DIP switch arrays, resistor networks, status LEDs, and microcontroller development boards is cataloged in exact numerical sequence with source, destination, and recommended wire color codes.

---

## 1. Master Bill of Integrated Circuits (Exact Physical Inventory)

| IC Designator | Chip Model | Logic Function & Architectural Role | Package | Breadboard Location |
| :--- | :--- | :--- | :---: | :---: |
| **IC1** | **74LS83N** | 4-Bit Binary Full Adder with Fast Carry (Lower Nibble ADD/SUB Core) | 16-Pin DIP | Breadboard 2 |
| **IC2** | **74HC86N** | Quad 2-Input Exclusive-OR Gate (Conditional Subtraction Inversion: $B \oplus SUB$) | 14-Pin DIP | Breadboard 2 |
| **IC3** | **74HC86N** | Quad 2-Input Exclusive-OR Gate (Bitwise XOR Logic Array: $A \oplus B$) | 14-Pin DIP | Breadboard 2 |
| **IC4** | **74HCT08** | Quad 2-Input AND Gate (Bitwise AND Logic Array: $A \cdot B$) | 14-Pin DIP | Breadboard 2 |
| **IC5** | **74HCT32** | Quad 2-Input OR Gate (Bitwise OR Logic Array: $A + B$) | 14-Pin DIP | Breadboard 2 |
| **IC6** | **74HCT04** | Hex Inverter (Bitwise NOT Array: $\bar{A}$ + Carry Inversion $C_4 \to \bar{C}_n$) | 14-Pin DIP | Breadboard 2 |
| **IC7** | **74HCT151** | 8:1 Multiplexer (Operation Selector for Custom ALU Bit 0: $Y_0$) | 16-Pin DIP | Breadboard 3 |
| **IC8** | **74HCT151** | 8:1 Multiplexer (Operation Selector for Custom ALU Bit 1: $Y_1$) | 16-Pin DIP | Breadboard 3 |
| **IC9** | **74HCT151** | 8:1 Multiplexer (Operation Selector for Custom ALU Bit 2: $Y_2$) | 16-Pin DIP | Breadboard 3 |
| **IC10** | **74HCT151** | 8:1 Multiplexer (Operation Selector for Custom ALU Bit 3: $Y_3$) | 16-Pin DIP | Breadboard 3 |
| **IC11** | **74LS181N** | 4-Bit Bipolar ALU (Upper Nibble in Mode 0 / Silicon Target in Mode 1) | 24-Pin DIP (0.6") | Breadboard 4 |
| **IC12** | **74HCT85** | 4-Bit Magnitude Comparator (Hardware Equivalence Verifier: $Y_{custom}$ vs $F_{181}$) | 16-Pin DIP | Breadboard 4 |

---

## 2. Power Distribution & Star Grounding Architecture

### 2.1 Power Source Wiring (Arduino UNO R4 Minima Dedicated Station)
* **Arduino UNO R4 Minima** is the sole power provider for the breadboard logic rails (+5V @ up to 1.2A via USB-C).
* **Arduino UNO R4 WiFi** and **ESP32-S3** are powered via their own dedicated USB-C cables. **Their 5V/VIN pins MUST NOT be connected to the breadboard 5V rail!**

| Board / Rail | Pin / Terminal | Connected To | Function / Description | Wire Color |
| :--- | :--- | :--- | :--- | :--- |
| **Arduino UNO R4 Minima** | `5V` Pin | Breadboard +5V Master Power Rail (Board 1..4) | Master clean +5V supply rail | **RED (Heavy 20 AWG)** |
| **Arduino UNO R4 Minima** | `GND` Pin | Breadboard Common Ground Bus (Board 1..4) | Master 0V reference | **BLACK (Heavy 20 AWG)** |
| **Arduino UNO R4 WiFi** | `GND` Pin | Breadboard Common Ground Bus | Bond to star ground plane | **BLACK (22 AWG)** |
| **ESP32-S3 N16R8** | `GND` Pin | Breadboard Common Ground Bus | Bond to star ground plane | **BLACK (22 AWG)** |
| **Main Rail Entry** | Top/Bottom Rails | $100\ \mu\text{F}$ 25V Electrolytic Capacitor across +5V and GND | Low-frequency ripple suppression | Radial Can |
| **IC Sockets (All 12)**| Across VCC & GND | $0.1\ \mu\text{F}$ Monolithic Ceramic Capacitor (x12) | High-frequency transient bypass | Yellow Ceramic (104) |

---

## 3. IC-by-IC Master Pin-to-Pin Interconnection Tables

---

> [!CAUTION]
> **CRITICAL HARDWARE PINOUT WARNING — 74LS83N POWER PINS**:
> The **74LS83N** does NOT use standard corner power pins!
> - **Pin 5 is $V_{CC}$ (+5V)**
> - **Pin 12 is GND (0V)**
> DO NOT connect Pin 16 to +5V or Pin 8 to GND! Pin 16 is Data Input $B_4$ and Pin 8 is Data Input $A_2$. Connecting Pin 16 to +5V will damage the input!

### IC1: 74LS83N (4-Bit Binary Full Adder with Fast Carry) — 16-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 5 $\to$ +5V Rail, Pin 12 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 5 and Pin 12.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $\Sigma_4$ | **IC10 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Bit 3 ADD / SUB Result ($S_3$) | Blue |
| **2** | $\Sigma_3$ | **IC9 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Bit 2 ADD / SUB Result ($S_2$) | Blue |
| **3** | $A_3$ | **Input Bus $A_2$ (DIP SW A Pin 3 / ESP32 GPIO 4 via 4.7kΩ)** | Bit 2 Operand A input | Yellow |
| **4** | $B_3$ | **IC2 Pin 8 (3Y)** | Bit 2 Conditioned Operand $B'_2 = B_2 \oplus SUB$ | Green |
| **5** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply (⚠️ NON-STANDARD PIN) | Red |
| **6** | $\Sigma_2$ | **IC8 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Bit 1 ADD / SUB Result ($S_1$) | Blue |
| **7** | $B_2$ | **IC2 Pin 6 (2Y)** | Bit 1 Conditioned Operand $B'_1 = B_1 \oplus SUB$ | Green |
| **8** | $A_2$ | **Input Bus $A_1$ (DIP SW A Pin 2 / ESP32 GPIO 2 via 4.7kΩ)** | Bit 1 Operand A input | Yellow |
| **9** | $\Sigma_1$ | **IC7 Pin 4 ($D_0$) and Pin 3 ($D_1$)** | Bit 0 ADD / SUB Result ($S_0$) | Blue |
| **10** | $A_1$ | **Input Bus $A_0$ (DIP SW A Pin 1 / ESP32 GPIO 1 via 4.7kΩ)** | Bit 0 Operand A input | Yellow |
| **11** | $B_1$ | **IC2 Pin 3 (1Y)** | Bit 0 Conditioned Operand $B'_0 = B_0 \oplus SUB$ | Green |
| **12** | GND | **Breadboard Common Ground Rail** | 0V Power Ground (⚠️ NON-STANDARD PIN) | Black |
| **13** | $C_0$ | **$SUB$ Line (Opcode DIP SW Pin 1 / ESP32 GPIO 18 via 4.7kΩ)** | Carry-in for lower nibble ($C_0 = 1$ on subtraction) | Violet |
| **14** | $C_4$ | **IC6 Pin 11 (5A)** | Raw Carry Out from Bit 3 (fed to inverter for 74LS181) | Orange |
| **15** | $A_4$ | **Input Bus $A_3$ (DIP SW A Pin 4 / ESP32 GPIO 5 via 4.7kΩ)** | Bit 3 Operand A input | Yellow |
| **16** | $B_4$ | **IC2 Pin 11 (4Y)** | Bit 3 Conditioned Operand $B'_3 = B_3 \oplus SUB$ | Green |

---

### IC2: 74HC86N (Quad 2-Input XOR Gate #1) — Subtraction Inverter — 14-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 14 $\to$ +5V Rail, Pin 7 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 14 and Pin 7.
* **Function**: Inverts Operand B when $SUB = 1$: $B'_i = B_i \oplus SUB$.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | 1A | **Input Bus $B_0$ (DIP SW B Pin 1 / ESP32 GPIO 10 via 4.7kΩ)** | Bit 0 Operand B raw input | Green |
| **2** | 1B | **$SUB$ Line (Opcode S0 line)** | Subtraction mode control signal | Violet |
| **3** | 1Y | **IC1 Pin 11 ($B_1$)** | Conditioned Bit 0: $B'_0 = B_0 \oplus SUB$ | White |
| **4** | 2A | **Input Bus $B_1$ (DIP SW B Pin 2 / ESP32 GPIO 11 via 4.7kΩ)** | Bit 1 Operand B raw input | Green |
| **5** | 2B | **$SUB$ Line (Opcode S0 line)** | Subtraction mode control signal | Violet |
| **6** | 2Y | **IC1 Pin 7 ($B_2$)** | Conditioned Bit 1: $B'_1 = B_1 \oplus SUB$ | White |
| **7** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **8** | 3Y | **IC1 Pin 4 ($B_3$)** | Conditioned Bit 2: $B'_2 = B_2 \oplus SUB$ | White |
| **9** | 3A | **Input Bus $B_2$ (DIP SW B Pin 3 / ESP32 GPIO 12 via 4.7kΩ)** | Bit 2 Operand B raw input | Green |
| **10** | 3B | **$SUB$ Line (Opcode S0 line)** | Subtraction mode control signal | Violet |
| **11** | 4Y | **IC1 Pin 16 ($B_4$)** | Conditioned Bit 3: $B'_3 = B_3 \oplus SUB$ | White |
| **12** | 4A | **Input Bus $B_3$ (DIP SW B Pin 4 / ESP32 GPIO 13 via 4.7kΩ)** | Bit 3 Operand B raw input | Green |
| **13** | 4B | **$SUB$ Line (Opcode S0 line)** | Subtraction mode control signal | Violet |
| **14** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC3: 74HC86N (Quad 2-Input XOR Gate #2) — Bitwise XOR Logic Array — 14-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 14 $\to$ +5V Rail, Pin 7 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 14 and Pin 7.
* **Function**: Computes $A_i \oplus B_i$ for lower nibble.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | 1A | **Input Bus $A_0$** | Bit 0 Operand A | Yellow |
| **2** | 1B | **Input Bus $B_0$** | Bit 0 Operand B | Green |
| **3** | 1Y | **IC7 Pin 15 ($D_4$)** | Bit 0 XOR Result ($A_0 \oplus B_0$) to MUX0 | Gray |
| **4** | 2A | **Input Bus $A_1$** | Bit 1 Operand A | Yellow |
| **5** | 2B | **Input Bus $B_1$** | Bit 1 Operand B | Green |
| **6** | 2Y | **IC8 Pin 15 ($D_4$)** | Bit 1 XOR Result ($A_1 \oplus B_1$) to MUX1 | Gray |
| **7** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **8** | 3Y | **IC9 Pin 15 ($D_4$)** | Bit 2 XOR Result ($A_2 \oplus B_2$) to MUX2 | Gray |
| **9** | 3A | **Input Bus $A_2$** | Bit 2 Operand A | Yellow |
| **10** | 3B | **Input Bus $B_2$** | Bit 2 Operand B | Green |
| **11** | 4Y | **IC10 Pin 15 ($D_4$)** | Bit 3 XOR Result ($A_3 \oplus B_3$) to MUX3 | Gray |
| **12** | 4A | **Input Bus $A_3$** | Bit 3 Operand A | Yellow |
| **13** | 4B | **Input Bus $B_3$** | Bit 3 Operand B | Green |
| **14** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC4: 74HCT08 (Quad 2-Input AND Gate) — Bitwise AND Logic Array — 14-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 14 $\to$ +5V Rail, Pin 7 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 14 and Pin 7.
* **Function**: Computes $A_i \cdot B_i$ for lower nibble.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | 1A | **Input Bus $A_0$** | Bit 0 Operand A | Yellow |
| **2** | 1B | **Input Bus $B_0$** | Bit 0 Operand B | Green |
| **3** | 1Y | **IC7 Pin 2 ($D_2$)** | Bit 0 AND Result ($A_0 \cdot B_0$) to MUX0 | Brown |
| **4** | 2A | **Input Bus $A_1$** | Bit 1 Operand A | Yellow |
| **5** | 2B | **Input Bus $B_1$** | Bit 1 Operand B | Green |
| **6** | 2Y | **IC8 Pin 2 ($D_2$)** | Bit 1 AND Result ($A_1 \cdot B_1$) to MUX1 | Brown |
| **7** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **8** | 3Y | **IC9 Pin 2 ($D_2$)** | Bit 2 AND Result ($A_2 \cdot B_2$) to MUX2 | Brown |
| **9** | 3A | **Input Bus $A_2$** | Bit 2 Operand A | Yellow |
| **10** | 3B | **Input Bus $B_2$** | Bit 2 Operand B | Green |
| **11** | 4Y | **IC10 Pin 2 ($D_2$)** | Bit 3 AND Result ($A_3 \cdot B_3$) to MUX3 | Brown |
| **12** | 4A | **Input Bus $A_3$** | Bit 3 Operand A | Yellow |
| **13** | 4B | **Input Bus $B_3$** | Bit 3 Operand B | Green |
| **14** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC5: 74HCT32 (Quad 2-Input OR Gate) — Bitwise OR Logic Array — 14-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 14 $\to$ +5V Rail, Pin 7 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 14 and Pin 7.
* **Function**: Computes $A_i + B_i$ for lower nibble.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | 1A | **Input Bus $A_0$** | Bit 0 Operand A | Yellow |
| **2** | 1B | **Input Bus $B_0$** | Bit 0 Operand B | Green |
| **3** | 1Y | **IC7 Pin 1 ($D_3$)** | Bit 0 OR Result ($A_0 + B_0$) to MUX0 | Purple |
| **4** | 2A | **Input Bus $A_1$** | Bit 1 Operand A | Yellow |
| **5** | 2B | **Input Bus $B_1$** | Bit 1 Operand B | Green |
| **6** | 2Y | **IC8 Pin 1 ($D_3$)** | Bit 1 OR Result ($A_1 + B_1$) to MUX1 | Purple |
| **7** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **8** | 3Y | **IC9 Pin 1 ($D_3$)** | Bit 2 OR Result ($A_2 + B_2$) to MUX2 | Purple |
| **9** | 3A | **Input Bus $A_2$** | Bit 2 Operand A | Yellow |
| **10** | 3B | **Input Bus $B_2$** | Bit 2 Operand B | Green |
| **11** | 4Y | **IC10 Pin 1 ($D_3$)** | Bit 3 OR Result ($A_3 + B_3$) to MUX3 | Purple |
| **12** | 4A | **Input Bus $A_3$** | Bit 3 Operand A | Yellow |
| **13** | 4B | **Input Bus $B_3$** | Bit 3 Operand B | Green |
| **14** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC6: 74HCT04 (Hex Inverter) — Bitwise NOT & Carry Inversion — 14-Pin DIP
* **Location**: Breadboard 2 | **Power**: Pin 14 $\to$ +5V Rail, Pin 7 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 14 and Pin 7.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | 1A | **Input Bus $A_0$** | Bit 0 Operand A | Yellow |
| **2** | 1Y | **IC7 Pin 14 ($D_5$)** | Bit 0 Inverted ($\overline{A_0}$) to MUX0 | White |
| **3** | 2A | **Input Bus $A_1$** | Bit 1 Operand A | Yellow |
| **4** | 2Y | **IC8 Pin 14 ($D_5$)** | Bit 1 Inverted ($\overline{A_1}$) to MUX1 | White |
| **5** | 3A | **Input Bus $A_2$** | Bit 2 Operand A | Yellow |
| **6** | 3Y | **IC9 Pin 14 ($D_5$)** | Bit 2 Inverted ($\overline{A_2}$) to MUX2 | White |
| **7** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **8** | 4Y | **IC10 Pin 14 ($D_5$)** | Bit 3 Inverted ($\overline{A_3}$) to MUX3 | White |
| **9** | 4A | **Input Bus $A_3$** | Bit 3 Operand A | Yellow |
| **10** | 5Y | **IC11 Pin 7 ($\bar{C}_n$, via Mode Switch)** | Inverted Carry: $\overline{C_4}$ (Active-LOW for 74LS181) | Orange |
| **11** | 5A | **IC1 Pin 14 ($C_4$)** | Raw Carry-out from 74xx283 Adder | Orange |
| **12** | 6Y | **NC (Unconnected)** | Spare Inverter Output | - |
| **13** | 6A | **Breadboard Common Ground Rail** | Spare Input (Tied LOW to prevent floating oscillation) | Black |
| **14** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC7: 74HCT151 (8-to-1 Multiplexer) — Bit 0 Result Selector ($Y_0$) — 16-Pin DIP
* **Location**: Breadboard 3 | **Power**: Pin 16 $\to$ +5V Rail, Pin 8 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 8.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $D_3$ | **IC5 Pin 3 (1Y)** | Bitwise OR Result ($A_0 + B_0$) | Purple |
| **2** | $D_2$ | **IC4 Pin 3 (1Y)** | Bitwise AND Result ($A_0 \cdot B_0$) | Brown |
| **3** | $D_1$ | **IC1 Pin 9 ($\Sigma_1$)** | Subtraction Result ($A_0 - B_0$) | Blue |
| **4** | $D_0$ | **IC1 Pin 9 ($\Sigma_1$)** | Addition Result ($A_0 + B_0$) | Blue |
| **5** | Y | **IC12 Pin 10 ($A_0$), Arduino R4 D2, ESP32 GPIO 38 (via 1k/2.2k)** | **Custom ALU Bit 0 Output ($Y_0$)** | Blue |
| **6** | W | **NC (Unconnected)** | Complementary Output (Unused) | - |
| **7** | $\bar{E}$ / $\bar{G}$ | **Breadboard Common Ground Rail** | Strobe / Enable (Active-LOW, permanently enabled) | Black |
| **8** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **9** | $S_2$ | **Opcode Bus $S_2$ (DIP SW Op Pin 3 / ESP32 GPIO 36 via 4.7kΩ)** | Opcode Select Bit 2 (MSB) | Gray |
| **10** | $S_1$ | **Opcode Bus $S_1$ (DIP SW Op Pin 2 / ESP32 GPIO 35 via 4.7kΩ)** | Opcode Select Bit 1 | White |
| **11** | $S_0$ | **Opcode Bus $S_0$ (DIP SW Op Pin 1 / ESP32 GPIO 18 via 4.7kΩ)** | Opcode Select Bit 0 (LSB) | Violet |
| **12** | $D_7$ | **Input Bus $A_1$ (DIP SW A Pin 2)** | Hardwired SHR Bit 0 ($A_{i+1} = A_1$) | Yellow |
| **13** | $D_6$ | **Breadboard Common Ground Rail** | Hardwired SHL Bit 0 ($A_{-1} = 0$) | Black |
| **14** | $D_5$ | **IC6 Pin 2 (1Y)** | Bitwise NOT Result ($\overline{A_0}$) | White |
| **15** | $D_4$ | **IC3 Pin 3 (1Y)** | Bitwise XOR Result ($A_0 \oplus B_0$) | Gray |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC8: 74HCT151 (8-to-1 Multiplexer) — Bit 1 Result Selector ($Y_1$) — 16-Pin DIP
* **Location**: Breadboard 3 | **Power**: Pin 16 $\to$ +5V Rail, Pin 8 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 8.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $D_3$ | **IC5 Pin 6 (2Y)** | Bitwise OR Result ($A_1 + B_1$) | Purple |
| **2** | $D_2$ | **IC4 Pin 6 (2Y)** | Bitwise AND Result ($A_1 \cdot B_1$) | Brown |
| **3** | $D_1$ | **IC1 Pin 6 ($\Sigma_2$)** | Subtraction Result ($A_1 - B_1$) | Blue |
| **4** | $D_0$ | **IC1 Pin 6 ($\Sigma_2$)** | Addition Result ($A_1 + B_1$) | Blue |
| **5** | Y | **IC12 Pin 12 ($A_1$), Arduino R4 D3, ESP32 GPIO 39 (via 1k/2.2k)** | **Custom ALU Bit 1 Output ($Y_1$)** | Blue |
| **6** | W | **NC (Unconnected)** | Complementary Output (Unused) | - |
| **7** | $\bar{E}$ / $\bar{G}$ | **Breadboard Common Ground Rail** | Strobe / Enable (Active-LOW, permanently enabled) | Black |
| **8** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **9** | $S_2$ | **Opcode Bus $S_2$** | Opcode Select Bit 2 (MSB) | Gray |
| **10** | $S_1$ | **Opcode Bus $S_1$** | Opcode Select Bit 1 | White |
| **11** | $S_0$ | **Opcode Bus $S_0$** | Opcode Select Bit 0 (LSB) | Violet |
| **12** | $D_7$ | **Input Bus $A_2$ (DIP SW A Pin 3)** | Hardwired SHR Bit 1 ($A_{i+1} = A_2$) | Yellow |
| **13** | $D_6$ | **Input Bus $A_0$ (DIP SW A Pin 1)** | Hardwired SHL Bit 1 ($A_{i-1} = A_0$) | Yellow |
| **14** | $D_5$ | **IC6 Pin 4 (2Y)** | Bitwise NOT Result ($\overline{A_1}$) | White |
| **15** | $D_4$ | **IC3 Pin 6 (2Y)** | Bitwise XOR Result ($A_1 \oplus B_1$) | Gray |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC9: 74HCT151 (8-to-1 Multiplexer) — Bit 2 Result Selector ($Y_2$) — 16-Pin DIP
* **Location**: Breadboard 3 | **Power**: Pin 16 $\to$ +5V Rail, Pin 8 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 8.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $D_3$ | **IC5 Pin 8 (3Y)** | Bitwise OR Result ($A_2 + B_2$) | Purple |
| **2** | $D_2$ | **IC4 Pin 8 (3Y)** | Bitwise AND Result ($A_2 \cdot B_2$) | Brown |
| **3** | $D_1$ | **IC1 Pin 2 ($\Sigma_3$)** | Subtraction Result ($A_2 - B_2$) | Blue |
| **4** | $D_0$ | **IC1 Pin 2 ($\Sigma_3$)** | Addition Result ($A_2 + B_2$) | Blue |
| **5** | Y | **IC12 Pin 13 ($A_2$), Arduino R4 D4, ESP32 GPIO 40 (via 1k/2.2k)** | **Custom ALU Bit 2 Output ($Y_2$)** | Blue |
| **6** | W | **NC (Unconnected)** | Complementary Output (Unused) | - |
| **7** | $\bar{E}$ / $\bar{G}$ | **Breadboard Common Ground Rail** | Strobe / Enable (Active-LOW, permanently enabled) | Black |
| **8** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **9** | $S_2$ | **Opcode Bus $S_2$** | Opcode Select Bit 2 (MSB) | Gray |
| **10** | $S_1$ | **Opcode Bus $S_1$** | Opcode Select Bit 1 | White |
| **11** | $S_0$ | **Opcode Bus $S_0$** | Opcode Select Bit 0 (LSB) | Violet |
| **12** | $D_7$ | **Input Bus $A_3$ (DIP SW A Pin 4)** | Hardwired SHR Bit 2 ($A_{i+1} = A_3$) | Yellow |
| **13** | $D_6$ | **Input Bus $A_1$ (DIP SW A Pin 2)** | Hardwired SHL Bit 2 ($A_{i-1} = A_1$) | Yellow |
| **14** | $D_5$ | **IC6 Pin 6 (3Y)** | Bitwise NOT Result ($\overline{A_2}$) | White |
| **15** | $D_4$ | **IC3 Pin 8 (3Y)** | Bitwise XOR Result ($A_2 \oplus B_2$) | Gray |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC10: 74HCT151 (8-to-1 Multiplexer) — Bit 3 Result Selector ($Y_3$) — 16-Pin DIP
* **Location**: Breadboard 3 | **Power**: Pin 16 $\to$ +5V Rail, Pin 8 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 8.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $D_3$ | **IC5 Pin 11 (4Y)** | Bitwise OR Result ($A_3 + B_3$) | Purple |
| **2** | $D_2$ | **IC4 Pin 11 (4Y)** | Bitwise AND Result ($A_3 \cdot B_3$) | Brown |
| **3** | $D_1$ | **IC1 Pin 1 ($\Sigma_4$)** | Subtraction Result ($A_3 - B_3$) | Blue |
| **4** | $D_0$ | **IC1 Pin 1 ($\Sigma_4$)** | Addition Result ($A_3 + B_3$) | Blue |
| **5** | Y | **IC12 Pin 15 ($A_3$), Arduino R4 D5, ESP32 GPIO 41 (via 1k/2.2k)** | **Custom ALU Bit 3 Output ($Y_3$)** | Blue |
| **6** | W | **NC (Unconnected)** | Complementary Output (Unused) | - |
| **7** | $\bar{E}$ / $\bar{G}$ | **Breadboard Common Ground Rail** | Strobe / Enable (Active-LOW, permanently enabled) | Black |
| **8** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **9** | $S_2$ | **Opcode Bus $S_2$** | Opcode Select Bit 2 (MSB) | Gray |
| **10** | $S_1$ | **Opcode Bus $S_1$** | Opcode Select Bit 1 | White |
| **11** | $S_0$ | **Opcode Bus $S_0$** | Opcode Select Bit 0 (LSB) | Violet |
| **12** | $D_7$ | **Breadboard GND (Mode 1) / Input Bus $A_4$ (Mode 0)** | Hardwired SHR Bit 3 ($A_{i+1}$) | Black / Yellow |
| **13** | $D_6$ | **Input Bus $A_2$ (DIP SW A Pin 3)** | Hardwired SHL Bit 3 ($A_{i-1} = A_2$) | Yellow |
| **14** | $D_5$ | **IC6 Pin 8 (4Y)** | Bitwise NOT Result ($\overline{A_3}$) | White |
| **15** | $D_4$ | **IC3 Pin 11 (4Y)** | Bitwise XOR Result ($A_3 \oplus B_3$) | Gray |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

### IC11: 74LS181N (4-Bit Bipolar Arithmetic Logic Unit) — 24-Pin DIP (0.6" Wide)
* **Location**: Breadboard 4 | **Power**: Pin 24 / Pin 16 $\to$ +5V Rail, Pin 12 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 12.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $\bar{B}_0$ | **Mode Steering: Bus $B_4$ (Mode 0) / Bus $B_0$ (Mode 1)** | Operand B Bit 0 input to 74LS181 | Green |
| **2** | $\bar{A}_0$ | **Mode Steering: Bus $A_4$ (Mode 0) / Bus $A_0$ (Mode 1)** | Operand A Bit 0 input to 74LS181 | Yellow |
| **3** | $S_3$ | **Opcode Translation Bus $S_{181,3}$** | Select Line 3 (MSB) | White |
| **4** | $S_2$ | **Opcode Translation Bus $S_{181,2}$** | Select Line 2 | White |
| **5** | $S_1$ | **Opcode Translation Bus $S_{181,1}$** | Select Line 1 | White |
| **6** | $S_0$ | **Opcode Translation Bus $S_{181,0}$** | Select Line 0 (LSB) | White |
| **7** | $\bar{C}_n$ | **Mode Steering: IC6 Pin 10 ($\overline{C_4}$) in Mode 0 / +5V in Mode 1** | Carry-in (Active LOW; 0 = Carry present) | Orange |
| **8** | $M$ | **Opcode Translation Bus $M_{181}$** | Mode Control (HIGH = Logic, LOW = Arithmetic) | Violet |
| **9** | $\bar{F}_0$ | **Output Bus $Y_4$ (Mode 0) / IC12 Pin 9 ($B_0$, Mode 1)** | Function Output Bit 0 ($F_0$) | Blue |
| **10** | $\bar{F}_1$ | **Output Bus $Y_5$ (Mode 0) / IC12 Pin 11 ($B_1$, Mode 1)** | Function Output Bit 1 ($F_1$) | Blue |
| **11** | $\bar{F}_2$ | **Output Bus $Y_6$ (Mode 0) / IC12 Pin 14 ($B_2$, Mode 1)** | Function Output Bit 2 ($F_2$) | Blue |
| **12** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **13** | $\bar{F}_3$ | **Output Bus $Y_7$ (Mode 0) / IC12 Pin 1 ($B_3$, Mode 1)** | Function Output Bit 3 ($F_3$) | Blue |
| **14** | $A=B$ | **NC or 10kΩ Pull-up to +5V** | Open-collector zero compare (Unused) | - |
| **15** | $\bar{P}$ | **NC (Unconnected)** | Carry Propagate (Lookahead generator link) | - |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |
| **17** | $\bar{G}$ | **NC (Unconnected)** | Carry Generate (Lookahead generator link) | - |
| **18** | $\bar{C}_{n+4}$ | **Amber LED via 330Ω to GND** | Final 8-Bit Carry-out Flag (Active LOW) | Amber |
| **19** | $\bar{A}_1$ | **Mode Steering: Bus $A_5$ (Mode 0) / Bus $A_1$ (Mode 1)** | Operand A Bit 1 input to 74LS181 | Yellow |
| **20** | $\bar{B}_1$ | **Mode Steering: Bus $B_5$ (Mode 0) / Bus $B_1$ (Mode 1)** | Operand B Bit 1 input to 74LS181 | Green |
| **21** | $\bar{A}_2$ | **Mode Steering: Bus $A_6$ (Mode 0) / Bus $A_2$ (Mode 1)** | Operand A Bit 2 input to 74LS181 | Yellow |
| **22** | $\bar{B}_2$ | **Mode Steering: Bus $B_6$ (Mode 0) / Bus $B_2$ (Mode 1)** | Operand B Bit 2 input to 74LS181 | Green |
| **23** | $\bar{A}_3$ | **Mode Steering: Bus $A_7$ (Mode 0) / Bus $A_3$ (Mode 1)** | Operand A Bit 3 input to 74LS181 | Yellow |
| **24** | $\bar{B}_3$ | **Mode Steering: Bus $B_7$ (Mode 0) / Bus $B_3$ (Mode 1)** | Operand B Bit 3 input to 74LS181 | Green |

---

### IC12: 74HCT85 (4-Bit Magnitude Comparator) — 16-Pin DIP
* **Location**: Breadboard 4 | **Power**: Pin 16 $\to$ +5V Rail, Pin 8 $\to$ GND Rail
* **Decoupling**: $0.1\ \mu\text{F}$ capacitor directly between Pin 16 and Pin 8.

| Pin | Pin Name | Connected To (Exact Source / Destination) | Signal Function / Notes | Wire Color |
| :---: | :---: | :--- | :--- | :---: |
| **1** | $B_3$ | **IC11 Pin 13 ($\bar{F}_3$)** | Benchmark Input Word B Bit 3 (from 74LS181) | Blue |
| **2** | $I_{A<B}$ | **Breadboard Common Ground Rail** | Cascading Expansion Input (Tied LOW) | Black |
| **3** | $I_{A=B}$ | **Breadboard +5V Master Power Rail** | Cascading Expansion Input (Tied HIGH for 4-bit) | Red |
| **4** | $I_{A>B}$ | **Breadboard Common Ground Rail** | Cascading Expansion Input (Tied LOW) | Black |
| **5** | $Q_{A>B}$ | **NC (Unconnected)** | Output A > B (Unused) | - |
| **6** | $Q_{A=B}$ | **Green PASS LED (via 330Ω), Arduino R4 D12, ESP32 GPIO 3 (via 1k/2.2k)** | **HARDWARE EQUIVALENCE PASS FLAG** (1 = Match) | **Bright Green** |
| **7** | $Q_{A<B}$ | **NC (Unconnected)** | Output A < B (Unused) | - |
| **8** | GND | **Breadboard Common Ground Rail** | 0V Power Ground | Black |
| **9** | $B_0$ | **IC11 Pin 9 ($\bar{F}_0$)** | Benchmark Input Word B Bit 0 (from 74LS181) | Blue |
| **10** | $A_0$ | **IC7 Pin 5 ($Y_0$)** | Custom ALU Test Input Word A Bit 0 | Blue |
| **11** | $B_1$ | **IC11 Pin 10 ($\bar{F}_1$)** | Benchmark Input Word B Bit 1 (from 74LS181) | Blue |
| **12** | $A_1$ | **IC8 Pin 5 ($Y_1$)** | Custom ALU Test Input Word A Bit 1 | Blue |
| **13** | $A_2$ | **IC9 Pin 5 ($Y_2$)** | Custom ALU Test Input Word A Bit 2 | Blue |
| **14** | $B_2$ | **IC11 Pin 11 ($\bar{F}_2$)** | Benchmark Input Word B Bit 2 (from 74LS181) | Blue |
| **15** | $A_3$ | **IC10 Pin 5 ($Y_3$)** | Custom ALU Test Input Word A Bit 3 | Blue |
| **16** | $V_{CC}$ | **Breadboard +5V Master Power Rail** | +5V Logic Supply | Red |

---

## 4. Microcontroller Pinout & Benchtop Hookup Tables

### 4.1 Arduino UNO R4 WiFi (Software Golden Model Oracle & 12×8 HUD)
* **Power**: Independent USB-C from PC. **Do NOT connect 5V pin!** Only connect GND.
* **Logic Level**: 5V Native TTL (Direct connection to breadboard signals).

| Arduino Pin | Hardware Signal | Connected To (Breadboard Node) | Signal Function / Notes |
| :---: | :---: | :--- | :--- |
| **GND** | Star Ground | Breadboard Common Ground Bus | Equipotential 0V reference |
| **D2** | $Y_0$ | IC7 Pin 5 | Custom ALU Output Bit 0 |
| **D3** | $Y_1$ | IC8 Pin 5 | Custom ALU Output Bit 1 |
| **D4** | $Y_2$ | IC9 Pin 5 | Custom ALU Output Bit 2 |
| **D5** | $Y_3$ | IC10 Pin 5 | Custom ALU Output Bit 3 |
| **D6** | $Y_4$ | IC11 Pin 9 | Upper Nibble / Silicon ALU Bit 0 |
| **D7** | $Y_5$ | IC11 Pin 10 | Upper Nibble / Silicon ALU Bit 1 |
| **D8** | $Y_6$ | IC11 Pin 11 | Upper Nibble / Silicon ALU Bit 2 |
| **D9** | $Y_7$ | IC11 Pin 13 | Upper Nibble / Silicon ALU Bit 3 |
| **D10** | PASS Output | $220\ \Omega \to$ Anode of 🟢 Green PASS LED | Software Golden Model Pass Indicator |
| **D11** | FAIL Output | $220\ \Omega \to$ Anode of 🔴 Red FAIL LED | Software Golden Model Fail Indicator |
| **D12** | HW Equiv In | IC12 Pin 6 ($Q_{A=B}$) | Direct read of hardware 74xx85 match flag |
| **A0** | Opcode $S_0$ | Opcode Bus $S_0$ Node | Monitors active Opcode Bit 0 |
| **A1** | Opcode $S_1$ | Opcode Bus $S_1$ Node | Monitors active Opcode Bit 1 |
| **A2** | Opcode $S_2$ | Opcode Bus $S_2$ Node | Monitors active Opcode Bit 2 |
| **A3** | Mode Switch | Mode Switch Node | Monitors Datapath Mode (0 = 8-Bit, 1 = Verify) |

---

### 4.2 ESP32-S3 N16R8 (IoT Dashboard, WebSockets & Remote Injection)
* **Power**: Independent USB-C. **Do NOT connect 5V pin!** Only connect GND.
* **Injection Signals**: Pass through **$4.7\text{ k}\Omega$ series override resistors**.
* **Monitor Signals**: Pass through **$1.0\text{ k}\Omega$ (series) / $2.2\text{ k}\Omega$ (shunt)** voltage dividers.

| ESP32 GPIO | Mode | Network Resistor | Connected To (Breadboard Node) | Architectural Function |
| :---: | :---: | :--- | :--- | :--- |
| **GND** | - | Direct | Breadboard Common Ground Bus | 0V Reference |
| **GPIO 1** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_0$ | Inject / monitor Operand A Bit 0 |
| **GPIO 2** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_1$ | Inject / monitor Operand A Bit 1 |
| **GPIO 4** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_2$ | Inject / monitor Operand A Bit 2 |
| **GPIO 5** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_3$ | Inject / monitor Operand A Bit 3 |
| **GPIO 6** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_4$ | Inject / monitor Operand A Bit 4 |
| **GPIO 7** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_5$ | Inject / monitor Operand A Bit 5 |
| **GPIO 8** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_6$ | Inject / monitor Operand A Bit 6 |
| **GPIO 9** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $A_7$ | Inject / monitor Operand A Bit 7 |
| **GPIO 10** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_0$ | Inject / monitor Operand B Bit 0 |
| **GPIO 11** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_1$ | Inject / monitor Operand B Bit 1 |
| **GPIO 12** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_2$ | Inject / monitor Operand B Bit 2 |
| **GPIO 13** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_3$ | Inject / monitor Operand B Bit 3 |
| **GPIO 14** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_4$ | Inject / monitor Operand B Bit 4 |
| **GPIO 15** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_5$ | Inject / monitor Operand B Bit 5 |
| **GPIO 16** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_6$ | Inject / monitor Operand B Bit 6 |
| **GPIO 17** | Bidirectional | $4.7\text{ k}\Omega$ series | Input Bus $B_7$ | Inject / monitor Operand B Bit 7 |
| **GPIO 18** | Bidirectional | $4.7\text{ k}\Omega$ series | Opcode Bus $S_0$ ($SUB$) | Inject / monitor Opcode Bit 0 |
| **GPIO 35** | Bidirectional | $4.7\text{ k}\Omega$ series | Opcode Bus $S_1$ | Inject / monitor Opcode Bit 1 |
| **GPIO 36** | Bidirectional | $4.7\text{ k}\Omega$ series | Opcode Bus $S_2$ | Inject / monitor Opcode Bit 2 |
| **GPIO 37** | Bidirectional | $4.7\text{ k}\Omega$ series | Mode Select Line | Inject / monitor Mode Switch |
| **GPIO 38** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_0$ (IC7 Pin 5) | Telemetry monitor Bit 0 |
| **GPIO 39** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_1$ (IC8 Pin 5) | Telemetry monitor Bit 1 |
| **GPIO 40** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_2$ (IC9 Pin 5) | Telemetry monitor Bit 2 |
| **GPIO 41** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_3$ (IC10 Pin 5) | Telemetry monitor Bit 3 |
| **GPIO 42** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_4$ (IC11 Pin 9) | Telemetry monitor Bit 4 |
| **GPIO 47** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_5$ (IC11 Pin 10) | Telemetry monitor Bit 5 |
| **GPIO 48** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_6$ (IC11 Pin 11) | Telemetry monitor Bit 6 |
| **GPIO 21** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | Output Bus $Y_7$ (IC11 Pin 13) | Telemetry monitor Bit 7 |
| **GPIO 3** | Input | $1\text{k}\Omega / 2.2\text{k}\Omega$ | IC12 Pin 6 ($Q_{A=B}$) | Telemetry monitor Hardware Match |

---

## 5. Visual Status Indicators & LED Wiring

| Indicator | Color | Anode Resistor | Cathode | Triggering Event |
| :--- | :---: | :---: | :---: | :--- |
| **Output Bus $Y[7:0]$** | 8x Blue | $330\ \Omega$ series to $Y_i$ | Common GND | Real-time visual display of active data bits |
| **Silicon Match LED** | Green | $330\ \Omega$ series to IC12 Pin 6 | Common GND | **Lights instantly when Custom ALU == 74LS181N** |
| **Software PASS LED** | Green | $220\ \Omega$ series to Arduino D10 | Common GND | Lights when Arduino C++ Golden Model matches bus |
| **Software FAIL LED** | Red | $220\ \Omega$ series to Arduino D11 | Common GND | Lights when Arduino C++ detects hardware mismatch |
| **Carry-Out Flag LED** | Amber | $330\ \Omega$ series to IC11 Pin 18 | Common GND | Lights when arithmetic carry/borrow is generated |

---

## 6. Pre-Power Electrical Verification Checklist

Before connecting USB-C power to any board, perform these four electrical tests with a digital multimeter:

- [ ] **Test 1: Zero Power Rail Shorts**: Measure resistance between breadboard +5V Master Rail and Common GND. Verify reading is $> 10\text{ k}\Omega$ (charging capacitor behavior). If resistance is $< 100\ \Omega$, locate and resolve short immediately!
- [ ] **Test 2: Equipotential Star Ground**: Measure resistance between Arduino Minima GND, Arduino WiFi GND, ESP32-S3 GND, and all breadboard GND rails. Verify $0.0\ \Omega$ across all nodes.
- [ ] **Test 3: Power Isolation**: Verify that the 5V pin of Arduino UNO R4 WiFi and the 5V pin of ESP32-S3 are completely disconnected from the breadboard 5V rail.
- [ ] **Test 4: Supply Voltage Verification**: Connect USB-C to Arduino Minima only. Measure voltage between +5V rail and GND rail. Verify reading is between $+4.95\text{ V}$ and $+5.10\text{ V}$.
