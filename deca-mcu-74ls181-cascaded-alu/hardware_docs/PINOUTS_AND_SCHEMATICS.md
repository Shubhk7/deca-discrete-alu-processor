# Pinouts & Electrical Schematics Reference
## DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU (74LS181N × Arduino R4 WiFi)

---

## 1. 74LS181N 4-Bit Arithmetic Logic Unit (24-Pin DIP)

The **74LS181N** is a high-speed Schottky TTL arithmetic logic unit that performs 16 arithmetic and 16 logic operations on two 4-bit words ($A$ and $B$).

```
                         74LS181N PINOUT (TOP VIEW)
                               ┌───---───┐
         (Input B0)   /B0    1 │●        │ 24   VCC    (+5V Master Rail)
         (Input A0)   /A0    2 │         │ 23   /A1    (Input A1)
         (Select S3)   S3    3 │         │ 22   /B1    (Input B1)
         (Select S2)   S2    4 │         │ 21   /A2    (Input A2)
         (Select S1)   S1    5 │  74LS181│ 20   /B2    (Input B2)
         (Select S0)   S0    6 │  DIP-24 │ 19   /A3    (Input A3)
        (Carry In)    /Cn    7 │         │ 18   /B3    (Input B3)
         (Mode Select) M     8 │         │ 17   A=B    (Equality Out - Open Col)
        (Result F0)   /F0    9 │         │ 16   /Cn+4  (Carry Out C4 Cascade)
        (Result F1)   /F1   10 │         │ 15   Y      (Carry Generate /G)
        (Result F2)   /F2   11 │         │ 14   X      (Carry Propagate P)
            (Ground)  GND   12 │         │ 13   /F3    (Result F3)
                               └─────────┘
```

> [!IMPORTANT]
> **Key Electrical Pin Characteristics**:
> - **Power Pins**: **Pin 24 is $+5\text{V}$ $V_{CC}$**, and **Pin 12 is $\text{GND}$**.
> - **Carry Cascade Pin**: **Pin 16 is $\overline{C}_{n+4}$** (Carry-Out). In arithmetic mode, this pin outputs active-LOW carry, which connects directly to Arduino R4 digital pin `D6`.
> - **Data Inversion**: When operating with standard active-HIGH data inputs, $A$ and $B$ are applied to $\overline{A}_i$ and $\overline{B}_i$, and results are read from $\overline{F}_i$.

---

## 2. 74LS181 Function Table (Active-HIGH Logic)

With inputs $A$ and $B$ active-HIGH, outputs $F$ active-HIGH:

| $S_3$ | $S_2$ | $S_1$ | $S_0$ | Logic ($M=1$) | Arithmetic ($M=0$, $\overline{C}_n = 1$, No Carry) | Arithmetic ($M=0$, $\overline{C}_n = 0$, With Carry) |
| :---: | :---: | :---: | :---: | :--- | :--- | :--- |
| 0 | 0 | 0 | 0 | $F = \overline{A}$ (NOT A) | $F = A$ | $F = A \text{ plus } 1$ |
| 0 | 1 | 1 | 0 | $F = A \oplus B$ (XOR) | $F = A \text{ minus } B \text{ minus } 1$ | **$F = A \text{ minus } B$ (SUB)** |
| 1 | 0 | 0 | 1 | $F = \overline{A \oplus B}$ | **$F = A \text{ plus } B$ (ADD)** | $F = A \text{ plus } B \text{ plus } 1$ |
| 1 | 0 | 1 | 1 | $F = A \cdot B$ (AND) | $F = (A + \overline{B}) \text{ plus } A$ | $F = (A + \overline{B}) \text{ plus } A \text{ plus } 1$ |
| 1 | 1 | 1 | 0 | $F = A + B$ (OR) | $F = (A + B) \text{ plus } A$ | $F = (A + B) \text{ plus } A \text{ plus } 1$ |
| 1 | 1 | 1 | 1 | $F = A$ | $F = A \text{ minus } 1$ | $F = A$ |

---

## 3. Microcontroller Pinout Reference

### 3.1 Arduino UNO R4 WiFi (5V Native I/O)

```
                       ARDUINO UNO R4 WIFI (TOP VIEW)
                             ┌───---───┐
                       [D13] │         │ [D12]
                       [D11] │         │ [D10]
                        [D9] │         │ [D8]
                        [D7] │         │ [D6]  <-- C4 Carry Cascade In (from 74LS181:16)
                        [D5] │         │ [D4]  <-- Y2 In (from 74LS181:11)
                        [D3] │         │ [D2]  <-- Y0 In (from 74LS181:9)
                        [TX] │         │ [RX]
                             │         │
                        [A0] │         │ [A1]  <-- S0 Opcode In
                        [A2] │         │ [A3]  <-- S2 Opcode In
                        [A4] │         │ [A5]  <-- C0 Carry In
                       [VIN] │         │ [5V]  <-- LEFT UNCONNECTED!
                       [GND] │         │ [GND] <-- Tied to Common Star Ground
                             └─────────┘
```

### 3.2 ESP32-S3 N16R8 (3.3V CMOS)

```
                            ESP32-S3 N16R8
                             ┌───---───┐
                     [3.3V]  │         │ [GND] <-- Tied to Common Star Ground
                     [GPIO1] │         │ [G48] <-- Y6 Sense (via divider)
                     [GPIO2] │         │ [G47] <-- Y5 Sense (via divider)
                     [GPIO4] │         │ [G42] <-- Y4 Sense (via divider)
                     [GPIO5] │         │ [G41] <-- Y3 Sense (via divider)
                     [GPIO6] │         │ [G40] <-- Y2 Sense (via divider)
                     [GPIO7] │         │ [G39] <-- Y1 Sense (via divider)
                     [GPIO8] │         │ [G38] <-- Y0 Sense (via divider)
                     [GPIO9] │         │ [G37] <-- Mode M Override
                    [GPIO10] │         │ [G36] <-- S2 Opcode Override
                    [GPIO11] │         │ [G35] <-- S1 Opcode Override
                    [GPIO12] │         │ [G18] <-- S0 Opcode Override
                    [GPIO13] │         │ [G17] <-- B7 Override
                    [GPIO14] │         │ [G16] <-- B6 Override
                    [GPIO15] │         │ [G15] <-- B5 Override
                    [GPIO16] │         │ [G14] <-- B4 Override
                             └─────────┘
```

---

## 4. Voltage Level Translation Subcircuits

### 4.1 Input Override Subcircuit (4.7kΩ Resistor)
```
  [ESP32 3.3V GPIO] ───[ 4.7kΩ Override ]───┬───► [74LS181 Input Pin / Bus Line]
                                            │
                                            ├───[ 10kΩ Pull-Down ]─── (GND)
                                            │
                                     [ DIP Switch ]─── (+5V Rail)
```
* **DIP Switch ON**: Logic HIGH is $+5\text{V}$. The $4.7\text{ k}\Omega$ resistor limits any current into the ESP32 pin to $< 0.4\text{ mA}$.
* **DIP Switch OFF**: When ESP32 is set to OUTPUT HIGH ($3.3\text{V}$), the voltage at the node is:
  $$V_{\text{node}} = 3.3\text{V} \times \frac{10\text{ k}\Omega}{4.7\text{ k}\Omega + 10\text{ k}\Omega} = 2.24\text{V} > V_{IH(\min)} = 2.0\text{V}$$
  This reliably drives the 74LS181 TTL inputs into a valid logic HIGH!

### 4.2 Output Telemetry Voltage Divider (1.0kΩ / 2.2kΩ)
```
  [5V Bus Line (Y0..Y7, C8)] ───[ 1.0kΩ Series ]───┬───► [ESP32 Telemetry Sense Pin]
                                                   │
                                                   └───[ 2.2kΩ Shunt ]─── (GND)
```
$$\text{Voltage into ESP32} = 5.0\text{V} \times \frac{2.2\text{ k}\Omega}{1.0\text{ k}\Omega + 2.2\text{ k}\Omega} = 3.4375\text{V}$$
Safe for the ESP32-S3 3.3V GPIO input buffer, safely below the $3.6\text{V}$ absolute maximum rating.
