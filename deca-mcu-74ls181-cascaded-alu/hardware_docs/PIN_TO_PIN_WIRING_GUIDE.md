# Pin-to-Pin Master Wiring Guide
## DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU (74LS181N × Arduino R4 WiFi)

---

## 1. System Overview & Power Distribution Architecture

This master wiring guide provides the exact pin-by-pin physical connections for the **DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor**. In this design:
- **Nibble 0 (Bits 3..0)**: Computed in hardware silicon by **IC1 (74LS181N 4-Bit Bipolar ALU)**.
- **Nibble 1 (Bits 7..4)**: Computed in real-time firmware by **MCU2 (Arduino UNO R4 WiFi ARM Cortex-M4)**.
- **Hardware Carry Cascade**: The 74LS181's Carry-Out ($\overline{C}_{n+4}$, Pin 16) is wired directly into Arduino Pin `D6`.
- **Display System**: **Zero discrete LEDs**. The full 8-bit result, intermediate carry, final carry, opcode symbol, and verification status are displayed on the Arduino UNO R4 WiFi's onboard **12×8 LED Matrix**.

```
                           POWER DISTRIBUTION ARCHITECTURE
                           
    +---------------------------+
    |  ARDUINO UNO R4 MINIMA    |  (USB-C 5V / 1.2A Host)
    |  "Clean Power Station"    |
    |                           |
    |  5V Output Pin -----------+-------------------------> BREADBOARD +5V MASTER RAIL
    |                           |                           (Powers IC1, DIP Pull-downs)
    |  GND Pin -----------------+-----------+-------------> BREADBOARD COMMON STAR GND
    +---------------------------+           |
                                            |
    +---------------------------+           |
    |  ARDUINO UNO R4 WIFI      |           |
    |  "5V 4-Bit ALU & 12x8 HUD"|           |
    |                           |           |
    |  5V Pin (LEFT UNCONNECTED)|           | (Powered via its own independent USB-C)
    |  GND Pin -----------------+-----------+
    +---------------------------+           |
                                            |
    +---------------------------+           |
    |  ESP32-S3 N16R8           |           |
    |  "SoftAP Web Dashboard"   |           |
    |                           |           |
    |  VIN/5V (LEFT UNCONNECTED)|           | (Powered via its own independent USB-C)
    |  GND Pin -----------------+-----------+
    +---------------------------+
```

> [!CAUTION]
> **THREE-BOARD POWER ISOLATION RULE**:
> - **NEVER** connect the `5V` pin of the Arduino UNO R4 WiFi or the `VIN`/`5V` pin of the ESP32-S3 to the breadboard $+5\text{V}$ rail! Doing so creates reverse-current loops that can destroy the USB host controllers.
> - **ALWAYS** tie all three `GND` pins (Minima, WiFi R4, and ESP32) solidly together to the Breadboard Common Ground rail to maintain an equipotential reference.

---

## 2. IC1: 74LS181N 4-Bit Silicon ALU (24-Pin DIP)

* **Physical Location**: Centered across the breadboard center trough.
* **Orientation**: Indexing notch facing LEFT or UP.
* **Power Supply**: Pin 24 = $+5\text{V}$ Master Rail, Pin 12 = Common GND.
* **Decoupling**: $0.1\ \mu\text{F}$ monolithic ceramic capacitor seated directly across Pins 24 and 12.

| Pin # | Pin Name | Signal Description | Physical Wire Destination |
| :---: | :--- | :--- | :--- |
| **1** | $\overline{B}_0$ | Input B Bit 0 (Active-LOW) | Inverted from Bus $B_0$ (or direct via DIP SW2 Pin 1) |
| **2** | $\overline{A}_0$ | Input A Bit 0 (Active-LOW) | Inverted from Bus $A_0$ (or direct via DIP SW1 Pin 1) |
| **3** | $S_3$ | Function Select Bit 3 | Bus Line $S_3$ (Opcode Mapping / MCU D6) |
| **4** | $S_2$ | Function Select Bit 2 | Breadboard Bus $S_2$ (DIP SW3 Pin 3 / ESP32 G36 / MCU A3) |
| **5** | $S_1$ | Function Select Bit 1 | Breadboard Bus $S_1$ (DIP SW3 Pin 2 / ESP32 G35 / MCU A2) |
| **6** | $S_0$ | Function Select Bit 0 | Breadboard Bus $S_0$ (DIP SW3 Pin 1 / ESP32 G18 / MCU A1) |
| **7** | $\overline{C}_n$ | Carry-In (Active-LOW) | Breadboard Bus $C_0$ (Active-LOW: Tie to +5V for no carry) |
| **8** | $M$ | Mode Select (0=Arith, 1=Logic)| Breadboard Bus $M$ (DIP SW3 Pin 4 / ESP32 G37 / MCU A4) |
| **9** | $\overline{F}_0$ | Result Output Bit 0 | Arduino UNO R4 WiFi Pin **D2** & Voltage Divider 1 Input |
| **10** | $\overline{F}_1$ | Result Output Bit 1 | Arduino UNO R4 WiFi Pin **D3** & Voltage Divider 2 Input |
| **11** | $\overline{F}_2$ | Result Output Bit 2 | Arduino UNO R4 WiFi Pin **D4** & Voltage Divider 3 Input |
| **12** | **GND** | **Ground (0V Reference)** | **Breadboard Common Star GND Bus** |
| **13** | $\overline{F}_3$ | Result Output Bit 3 | Arduino UNO R4 WiFi Pin **D5** & Voltage Divider 4 Input |
| **14** | $X$ | Carry Propagate ($P$) | UNCONNECTED / NC (Used only with 74LS182 lookahead) |
| **15** | $Y$ | Carry Generate ($\overline{G}$) | UNCONNECTED / NC (Used only with 74LS182 lookahead) |
| **16** | $\overline{C}_{n+4}$| **Carry-Out (C4 Cascade)** | **Arduino UNO R4 WiFi Pin D6 (Carry-In to Upper ALU)** |
| **17** | $A=B$ | Open-Collector Equality Out | $10\text{ k}\Omega$ pull-up to $+5\text{V}$ (Optional test point) |
| **18** | $\overline{B}_3$ | Input B Bit 3 (Active-LOW) | Inverted from Bus $B_3$ (DIP SW2 Pin 4) |
| **19** | $\overline{A}_3$ | Input A Bit 3 (Active-LOW) | Inverted from Bus $A_3$ (DIP SW1 Pin 4) |
| **20** | $\overline{B}_2$ | Input B Bit 2 (Active-LOW) | Inverted from Bus $B_2$ (DIP SW2 Pin 3) |
| **21** | $\overline{A}_2$ | Input A Bit 2 (Active-LOW) | Inverted from Bus $A_2$ (DIP SW1 Pin 3) |
| **22** | $\overline{B}_1$ | Input B Bit 1 (Active-LOW) | Inverted from Bus $B_1$ (DIP SW2 Pin 2) |
| **23** | $\overline{A}_1$ | Input A Bit 1 (Active-LOW) | Inverted from Bus $A_1$ (DIP SW1 Pin 2) |
| **24** | **$V_{CC}$** | **Positive Supply (+5V)** | **Breadboard +5V Master Rail (from Minima 5V pin)** |

---

## 3. MCU2: Arduino UNO R4 WiFi (5V Upper ALU & 12×8 LED Matrix HUD)

* **Supply**: Powered exclusively via its own USB-C cable from PC/Laptop.
* **Logic Level**: 5V TTL Native (Can read and drive 74LS181N directly with 0 level shifters).
* **Onboard HUD**: Renders the complete 8-bit result on its built-in $12\times 8$ red LED matrix.

| Arduino Pin | Pin Function | Connected Hardware Source / Destination |
| :---: | :--- | :--- |
| **D0 (RX)** | Serial RX | Reserved for USB Serial Programming / Debugging (Do NOT connect) |
| **D1 (TX)** | Serial TX | Reserved for USB Serial Programming / Debugging (Do NOT connect) |
| **D2** | Input | 74LS181N Pin 9 ($\overline{F}_0$ / Lower Nibble Result Bit 0) |
| **D3** | Input | 74LS181N Pin 10 ($\overline{F}_1$ / Lower Nibble Result Bit 1) |
| **D4** | Input | 74LS181N Pin 11 ($\overline{F}_2$ / Lower Nibble Result Bit 2) |
| **D5** | Input | 74LS181N Pin 13 ($\overline{F}_3$ / Lower Nibble Result Bit 3) |
| **D6** | **Input (Carry In)** | **74LS181N Pin 16 ($\overline{C}_{n+4}$ / Hardware Carry Cascade C4)** |
| **D7** | Input | Operand A Upper Bit $A_4$ (DIP SW1 Pin 5 / ESP32 G6 via 4.7k) |
| **D8** | Input | Operand A Upper Bit $A_5$ (DIP SW1 Pin 6 / ESP32 G7 via 4.7k) |
| **D9** | Input | Operand A Upper Bit $A_6$ (DIP SW1 Pin 7 / ESP32 G8 via 4.7k) |
| **D10** | Input | Operand A Upper Bit $A_7$ (DIP SW1 Pin 8 / ESP32 G9 via 4.7k) |
| **D11** | Input | Operand B Upper Bit $B_4$ (DIP SW2 Pin 5 / ESP32 G14 via 4.7k) |
| **D12** | Input | Operand B Upper Bit $B_5$ (DIP SW2 Pin 6 / ESP32 G15 via 4.7k) |
| **D13** | Input | Operand B Upper Bit $B_6$ (DIP SW2 Pin 7 / ESP32 G16 via 4.7k) |
| **A0 (D14)**| Input | Operand B Upper Bit $B_7$ (DIP SW2 Pin 8 / ESP32 G17 via 4.7k) |
| **A1 (D15)**| Input | Opcode Bit $S_0$ (DIP SW3 Pin 1 / ESP32 G18 via 4.7k) |
| **A2 (D16)**| Input | Opcode Bit $S_1$ (DIP SW3 Pin 2 / ESP32 G35 via 4.7k) |
| **A3 (D17)**| Input | Opcode Bit $S_2$ (DIP SW3 Pin 3 / ESP32 G36 via 4.7k) |
| **A4 (D18)**| Input | Mode Select $M$ (DIP SW3 Pin 4 / ESP32 G37 via 4.7k) |
| **A5 (D19)**| Input | Carry-In $C_0$ (DIP Switch / Breadboard GND) |
| **GND** | **Ground** | **Breadboard Common Star GND Bus** |
| **5V** | **5V Power** | **LEFT UNCONNECTED! (Do NOT bridge to breadboard 5V)** |

---

## 4. MCU3: ESP32-S3 N16R8 (Web Dashboard & 4.7kΩ Remote Override Injector)

* **Supply**: Powered via independent USB-C cable.
* **Logic Level**: 3.3V CMOS.
* **Injection**: Drives 19 bus lines through $4.7\text{ k}\Omega$ current-limiting override resistors.
* **Telemetry**: Reads 9 bus lines through $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ voltage dividers.

### 4.1 Injection Outputs (19x $4.7\text{ k}\Omega$ Series Resistors)
| ESP32 GPIO | Net Name | Destination via $4.7\text{ k}\Omega$ Resistor |
| :---: | :--- | :--- |
| **GPIO 1** | INJ_A0 | Breadboard Bus $A_0$ (74LS181 Pin 2 & SW1 Pin 1) |
| **GPIO 2** | INJ_A1 | Breadboard Bus $A_1$ (74LS181 Pin 23 & SW1 Pin 2) |
| **GPIO 4** | INJ_A2 | Breadboard Bus $A_2$ (74LS181 Pin 21 & SW1 Pin 3) |
| **GPIO 5** | INJ_A3 | Breadboard Bus $A_3$ (74LS181 Pin 19 & SW1 Pin 4) |
| **GPIO 6** | INJ_A4 | Breadboard Bus $A_4$ (Arduino D7 & SW1 Pin 5) |
| **GPIO 7** | INJ_A5 | Breadboard Bus $A_5$ (Arduino D8 & SW1 Pin 6) |
| **GPIO 8** | INJ_A6 | Breadboard Bus $A_6$ (Arduino D9 & SW1 Pin 7) |
| **GPIO 9** | INJ_A7 | Breadboard Bus $A_7$ (Arduino D10 & SW1 Pin 8) |
| **GPIO 10** | INJ_B0 | Breadboard Bus $B_0$ (74LS181 Pin 1 & SW2 Pin 1) |
| **GPIO 11** | INJ_B1 | Breadboard Bus $B_1$ (74LS181 Pin 22 & SW2 Pin 2) |
| **GPIO 12** | INJ_B2 | Breadboard Bus $B_2$ (74LS181 Pin 20 & SW2 Pin 3) |
| **GPIO 13** | INJ_B3 | Breadboard Bus $B_3$ (74LS181 Pin 18 & SW2 Pin 4) |
| **GPIO 14** | INJ_B4 | Breadboard Bus $B_4$ (Arduino D11 & SW2 Pin 5) |
| **GPIO 15** | INJ_B5 | Breadboard Bus $B_5$ (Arduino D12 & SW2 Pin 6) |
| **GPIO 16** | INJ_B6 | Breadboard Bus $B_6$ (Arduino D13 & SW2 Pin 7) |
| **GPIO 17** | INJ_B7 | Breadboard Bus $B_7$ (Arduino A0 & SW2 Pin 8) |
| **GPIO 18** | INJ_S0 | Breadboard Bus $S_0$ (Opcode Bit 0 / SUB) |
| **GPIO 35** | INJ_S1 | Breadboard Bus $S_1$ (Opcode Bit 1) |
| **GPIO 36** | INJ_S2 | Breadboard Bus $S_2$ (Opcode Bit 2) |
| **GPIO 37** | INJ_MODE | Breadboard Bus $M$ (Mode Select: 0=Arith, 1=Logic) |

### 4.2 Telemetry Sense Inputs (9x $1.0\text{ k}\Omega / 2.2\text{ k}\Omega$ Voltage Dividers)
| ESP32 GPIO | Monitored 5V Signal | Voltage Divider Tap Source |
| :---: | :--- | :--- |
| **GPIO 38** | $Y_0$ (Bit 0) | Tap between $1.0\text{ k}\Omega$ from 74LS181 Pin 9 and $2.2\text{ k}\Omega$ to GND |
| **GPIO 39** | $Y_1$ (Bit 1) | Tap between $1.0\text{ k}\Omega$ from 74LS181 Pin 10 and $2.2\text{ k}\Omega$ to GND |
| **GPIO 40** | $Y_2$ (Bit 2) | Tap between $1.0\text{ k}\Omega$ from 74LS181 Pin 11 and $2.2\text{ k}\Omega$ to GND |
| **GPIO 41** | $Y_3$ (Bit 3) | Tap between $1.0\text{ k}\Omega$ from 74LS181 Pin 13 and $2.2\text{ k}\Omega$ to GND |
| **GPIO 42** | $Y_4$ (Bit 4) | Tap between $1.0\text{ k}\Omega$ from Arduino D7 upper bus and $2.2\text{ k}\Omega$ to GND |
| **GPIO 47** | $Y_5$ (Bit 5) | Tap between $1.0\text{ k}\Omega$ from Arduino D8 upper bus and $2.2\text{ k}\Omega$ to GND |
| **GPIO 48** | $Y_6$ (Bit 6) | Tap between $1.0\text{ k}\Omega$ from Arduino D9 upper bus and $2.2\text{ k}\Omega$ to GND |
| **GPIO 21** | $Y_7$ (Bit 7) | Tap between $1.0\text{ k}\Omega$ from Arduino D10 upper bus and $2.2\text{ k}\Omega$ to GND |
| **GPIO 3** | $C_8$ (Final Carry)| Tap between $1.0\text{ k}\Omega$ from Arduino C8 telemetry and $2.2\text{ k}\Omega$ to GND |

---

## 5. Input DIP Switches & Pull-Down Resistor Networks

* **SW1 (8-Position DIP)**: Operand A ($A_0..A_7$).
  * One side of all 8 switches connects to the Breadboard $+5\text{V}$ Master Rail.
  * The other side connects to the respective bus line ($A_0..A_7$), with each line tied to GND via a $10\text{ k}\Omega$ pull-down resistor.
* **SW2 (8-Position DIP)**: Operand B ($B_0..B_7$).
  * One side of all 8 switches connects to $+5\text{V}$.
  * The other side connects to $B_0..B_7$, with each line tied to GND via a $10\text{ k}\Omega$ pull-down resistor.
* **SW3 (4-Position DIP)**: Opcode ($S_0, S_1, S_2$) and Mode ($M$).
  * One side connects to $+5\text{V}$.
  * The other side connects to $S_0, S_1, S_2, M$, with each tied to GND via a $10\text{ k}\Omega$ resistor.
