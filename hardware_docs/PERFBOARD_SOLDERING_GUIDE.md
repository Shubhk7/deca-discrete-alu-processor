# Plain PCB (Perfboard / Zero-PCB) Transfer & Soldering Guide
## Dual-Mode 8-Bit ALU with 74LS181 Golden Verifier

Transferring your verified breadboard circuit to a permanent perfboard (zero-PCB) creates a professional, road-tested piece of hardware. This guide details the floorplan, socket list, and soldering procedure for the full **Dual-Mode 8-Bit Architecture**.

---

## 1. Sockets & Hardware Parts Required

> [!CAUTION]
> **NEVER solder TTL ICs directly to the perfboard!** Always solder DIP sockets first.

* **DIP IC Sockets**:
  * 1× **24-Pin DIP Socket** (Skinny 0.3" pitch) $\to$ for **74LS181** (Commercial ALU)
  * 1× **16-Pin DIP Socket** $\to$ for **74LS283** (4-Bit Adder)
  * 4× **16-Pin DIP Sockets** $\to$ for **74LS151** (8:1 Multiplexers)
  * 1× **16-Pin DIP Socket** $\to$ for **74LS85** (Magnitude Comparator)
  * 2× **16-Pin DIP Sockets** (Optional) $\to$ for **74LS157** (Mode Steer MUXes)
  * 2× **14-Pin DIP Sockets** $\to$ for **74LS86** (XOR Gates)
  * 1× **14-Pin DIP Socket** $\to$ for **74LS08** (AND Gates)
  * 1× **14-Pin DIP Socket** $\to$ for **74LS32** (OR Gates)
  * 1× **14-Pin DIP Socket** $\to$ for **74LS04** (NOT Gates)
* **Board Dimensions**: 1 large perfboard (e.g., $12\text{ cm} \times 18\text{ cm}$ or $15\text{ cm} \times 20\text{ cm}$) or two medium perfboards linked with header pins.
* **Passives**: 0.1 µF ceramic capacitors (one per socket), 10 kΩ pull-down resistor SIP arrays, 330 Ω LED resistors.
* **Indicators**: 8× Blue/Green LEDs ($Y_7..Y_0$), 1× Green LED ("PASS"), 1× Red LED ("FAIL").

---

## 2. Perfboard Floorplan

Plan the component placement with all signals flowing cleanly from **Left (Inputs) $\to$ Center (Processing) $\to$ Right (Telemetry & Outputs)**:

```
+------------------------------------------------------------------------------------------------+
| [5V DC JACK] [POWER SWITCH & LED]              +5V POWER BUS RAIL                              |
|                                                                                                |
| +--------+  +-------------------+  +-------------------+  +-----------------+  +-------------+ |
| | DIP SW |  | 74LS283 (Adder)   |  | 74LS08 (AND Gate) |  | 74LS151 (MUX 0) |  | [8-Bit LED  | |
| | (A7..0)|  | 74LS86 (XOR #1)   |  | 74LS32 (OR Gate)  |  | 74LS151 (MUX 1) |  |  Bus Y7..0] | |
| +--------+  +-------------------+  +-------------------+  | 74LS151 (MUX 2) |  +-------------+ |
| | DIP SW |                                                | 74LS151 (MUX 3) |                  |
| | (B7..0)|  +-------------------------------------------+ +-----------------+  +-------------+ |
| +--------+  | 74LS181 (Commercial 4-Bit ALU)            |                      | 🟢 PASS LED | |
| | OPCODE |  | (Upper 4 bits in Compute / Reference in   |  +-----------------+ | 🔴 FAIL LED | |
| | & MODE |  |  Verify Mode)                             |  | 74LS85 (4-Bit   | +-------------+ |
| +--------+  +-------------------------------------------+  |  Comparator)    |                 |
|                                                            +-----------------+                 |
|                                                GND BUS RAIL                                    |
+------------------------------------------------------------------------------------------------+
```

---

## 3. Step-by-Step Soldering Execution

### Step 1: Power Rails & Sockets First
1. Solder bare tinned bus wire along the top for $+5\text{ V}$ and along the bottom for $\text{GND}$.
2. Insert all DIP sockets with their notch pointing towards the **LEFT**.
3. Bend two corner pins of each socket slightly outward on the solder side to hold them mechanically before soldering.
4. Solder all socket pins cleanly.
5. Solder a **$0.1\ \mu\text{F}$ decoupling capacitor directly across the $V_{CC}$ and $\text{GND}$ pins** of every socket.

### Step 2: Input Controls & Pull-Downs
1. Mount the 8-bit DIP switches for Operands A and B, plus the control switches.
2. Solder SIP resistor arrays (or 10 kΩ resistors) from each switch line to $\text{GND}$.
3. Route input lines to their respective sockets using insulated 30 AWG Kynar wire.

### Step 3: Arithmetic & Logic Interconnections
1. Solder the XOR subtraction gating around the 74LS283 adder.
2. Route the 4 candidate function results ($A+B$, $A-B$, AND, OR, XOR, NOT, CLEAR) into the data inputs of the four 74LS151 MUX sockets.
3. Wire the 74LS181 inputs and control lines.
4. Wire the Custom ALU outputs ($Y_3..Y_0$) and 74LS181 outputs ($F_3..F_0$) to the 74LS85 comparator inputs.

### Step 4: Output Indicators
1. Solder the 8 output LEDs with 330 Ω current-limiting resistors to GND.
2. Solder the Green "PASS" LED to 74LS85 Pin 6 ($A=B$).
3. Solder the Red "FAIL" LED to the inverted output of Pin 6.

---

## 4. The Pre-Power "Smoke Test" Checklist

Before placing any IC into a socket:

1. **Short Circuit Check**: Measure resistance between $+5\text{ V}$ and $\text{GND}$ rails with a multimeter. It must read open or $>1\text{ k}\Omega$.
2. **Voltage Verification**: Plug in 5V power with all sockets empty:
   * Measure Pin 24 of the 74LS181 socket: Must read $+5.0\text{ V}$.
   * Measure Pin 16 of all 16-pin sockets: Must read $+5.0\text{ V}$.
   * Measure Pin 14 of all 14-pin sockets: Must read $+5.0\text{ V}$.
   * Measure all Ground pins: Must read $0.0\text{ V}$.
3. **Insert Chips**: Disconnect power, carefully press all ICs into their sockets matching the notch orientation, and power on!
