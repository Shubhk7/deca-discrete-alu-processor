# Plain PCB (Perfboard / Zero-PCB) Transfer & Soldering Guide
## How to Migrate from Breadboard to a Permanent Hardware Circuit

Migrating a digital circuit from a solderless breadboard to a plain perfboard (also known as a general-purpose dot-matrix PCB or Zero-PCB) turns a temporary prototype into a rugged, permanent engineering showcase.

Follow these rules to ensure your board works on the first power-up without damaged ICs or hours of desoldering.

---

## 1. The #1 Golden Rule: ALWAYS Use DIP IC Sockets

> [!CAUTION]
> **NEVER solder TTL ICs directly onto the perfboard!**
> * Prolonged heat from a soldering iron (above 260°C for >3 seconds) can permanently degrade or destroy internal bipolar junctions.
> * If a wiring mistake shorts an output pin, you cannot easily replace a soldered IC without damaging the copper pads.
> * **Always solder IC Sockets** (14-pin and 16-pin DIP sockets) to the board first. The actual chips are pressed into the sockets only after all soldering and voltage tests are 100% complete.

---

## 2. Floorplanning & Component Layout

Before heating up your soldering iron, place all components onto the perfboard without soldering to check spacing.

```
+-------------------------------------------------------------------------+
| [POWER JACK]  [PWR LED]        +5V POWER BUS RAIL                       |
|                                                                         |
|  +-------+     +----------+     +----------+     +-------------------+  |
|  | DIP   |     | 74LS283  |     | 74LS08   |     | 74LS151 (MUX 0)   |  |
|  | SW (A)|     | (Adder)  |     | (AND)    |     | 74LS151 (MUX 1)   |  |
|  +-------+     +----------+     +----------+     | 74LS151 (MUX 2)   |  |
|  | DIP   |     | 74LS86   |     | 74LS32   |     | 74LS151 (MUX 3)   |  |
|  | SW (B)|     | (XOR)    |     | (OR)     |     +-------------------+  |
|  +-------+     +----------+     +----------+               |            |
|  | OPCODE|                      | 74LS04   |         [4x LEDs + Res]    |
|  | SWITCH|                      | (NOT)    |         (Result Y3..Y0)    |
|  +-------+                      +----------+                            |
|                                                                         |
|                                GND BUS RAIL                             |
+-------------------------------------------------------------------------+
```

### Layout Guidelines:
1. **Signal Flow Direction (Left to Right)**:
   * **Left edge**: Power terminal/jack, DIP switches, and pull-down resistor arrays.
   * **Middle section**: Arithmetic unit (Adder, XOR) and logic gates (AND, OR, NOT).
   * **Right section**: The 4 Multiplexer ICs and the final output LEDs.
2. **Orientation**: Align all IC sockets with their notch/pin 1 facing the same direction (e.g., all pointing Left or Up). This avoids accidentally inserting an IC backwards.
3. **Keep Spacing**: Leave at least 2 to 3 rows of holes between IC sockets so you have room to route wires and solder decoupling capacitors.

---

## 3. Power Grid & Decoupling (Solder First!)

1. **Power Bus Rails**:
   * Solder bare tinned copper wire or thick solid wire along the top for $+5\text{ V}$ and along the bottom for $\text{GND}$.
   * Distribute power to every IC socket:
     * Red insulated wire to Pin 14 (for 14-pin ICs) or Pin 16 (for 16-pin ICs).
     * Black insulated wire to Pin 7 (for 14-pin ICs) or Pin 8 (for 16-pin ICs).
2. **Decoupling Capacitors ($0.1\ \mu\text{F}$ Ceramic "104")**:
   * Solder a $0.1\ \mu\text{F}$ capacitor directly between the $V_{CC}$ and $\text{GND}$ pins of **every single IC socket**.
   * Keep the capacitor leads as short as possible.
   > **Why?** TTL gates draw sharp current spikes during switching. Without decoupling capacitors, power rail noise can cause phantom switching and glitches.

---

## 4. Wiring Technique: Point-to-Point Routing

For a clean, professional underside on a perfboard:

* **Recommended Wire**: Use **30 AWG Kynar (Wire-Wrap) wire** or thin solid-core insulated hookup wire.
  * Thin insulation melts cleanly with a light touch or strips easily.
  * Wires lay flat against the board and do not create bulky tangles.
* **Color-Coding**:
  * **Red**: $+5\text{ V}$ Power
  * **Black**: Ground (0V)
  * **Blue**: Data Lines (Operands $A$ and $B$)
  * **Yellow**: Opcode Select Lines ($S_2, S_1, S_0$)
  * **Green**: Arithmetic Carry & Status signals
* **Routing Tip**: Route wires along the natural grid channels on the solder side. Strip only 2–3 mm of insulation at each end, insert into the hole beside the target pin, and bridge with a tiny drop of solder.

---

## 5. Pre-Power Testing Checklist (Do NOT Skip!)

Before inserting any IC into its socket:

1. **Visual Inspection**:
   * Check the underside under good light with a magnifying glass for **solder bridges** (accidental solder touching adjacent pins).
2. **Continuity & Short-Circuit Check (Multimeter)**:
   * Set multimeter to Continuity / Resistance ($\Omega$) mode.
   * Place probes between $+5\text{ V}$ rail and $\text{GND}$ rail.
   * **Result must NOT beep** (infinite resistance or > 1 kΩ). If it beeps, you have a dead short—find and fix it before applying power!
3. **Socket Voltage Verification (The Smoke Test)**:
   * Connect your 5V power supply to the empty board.
   * Check the Power LED is lit.
   * With multimeter in DC Voltage mode, touch black probe to GND and measure:
     * Pin 14 of 14-pin sockets: Must read **+4.8 V to +5.2 V**.
     * Pin 16 of 16-pin sockets: Must read **+4.8 V to +5.2 V**.
     * Pin 7 / Pin 8: Must read **0.0 V**.
4. **Insert the ICs**:
   * **TURN OFF POWER**.
   * Carefully insert each IC into its corresponding socket, verifying the notch orientation.
   * Power on and run the verification test cases!
