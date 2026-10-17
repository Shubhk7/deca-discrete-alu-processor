# Minor Project Report: Dual-Mode 8-Bit Reconfigurable ALU with 74LS181 Golden Reference & ESP32-S3 HIL Testbench

**Course**: Digital Electronics & Computer Architecture (DECA)  
**Project Title**: Design, Simulation, and Hardware Prototyping of a Dual-Mode 8-Bit Reconfigurable Arithmetic Logic Unit (ALU) with Commercial IC Verification and IoT Telemetry  
**Author**: Shubh Kashyap  

---

## 1. Abstract

This project presents the design, software modeling, hardware implementation, and automated verification of an advanced **Dual-Mode Reconfigurable Arithmetic Logic Unit (ALU)**. The physical circuit bridges foundational gate-level digital design with industrial semiconductor validation methodologies. The hardware consists of a **Custom 4-Bit Discrete ALU** (constructed using a 74LS283 4-bit adder, 74LS86 XOR gates, 74LS08/32/04 logic gates, and four 74LS151 8:1 multiplexers) integrated alongside a commercial **74LS181 4-Bit ALU IC** and a **74LS85 4-Bit Magnitude Comparator**. 

A single mode control line switches the architecture between two distinct operating regimes:
1. **Mode 0 (8-Bit Compute Mode)**: Cascades the Custom 4-Bit ALU (handling the lower nibble $D_0–D_3$) with the 74LS181 (handling the upper nibble $D_4–D_7$) through carry propagation ($C_4 \to C_n$), forming an authentic 8-bit hybrid computer datapath driving an 8-bit output bus.
2. **Mode 1 (4-Bit Real-Time Hardware Verification Mode)**: Both ALUs execute identical instructions on matching 4-bit inputs in parallel. The 74LS85 comparator verifies equivalence in real time, driving visual Pass/Fail indicators.

An **ESP32-S3 N16R8 microcontroller** acts as an automated Hardware-in-the-Loop (HIL) testbench, hosting an asynchronous WebSockets telemetry dashboard over a self-contained Wi-Fi Access Point and executing an automated test pattern sweep of 2,048 vectors. The complete sequential datapath—including 8-bit registers, clocking, and status flags (Carry, Zero, Sign)—is modeled and simulated in Logisim Evolution.

---

## 2. System Architecture & Dual Operating Modes

```
                                [ MODE SWITCH ]
                           (0 = COMPUTE, 1 = VERIFY)
                                       |
                 +---------------------+---------------------+
                 |                                           |
                 v                                           v
       [ 74LS157 Multiplexer #1 ]                  [ 74LS157 Multiplexer #2 ]
       Mode 0: A[7..4] | Mode 1: A[3..0]           Mode 0: B[7..4] | Mode 1: B[3..0]
                 |                                           |
                 +---------------------+---------------------+
                                       |
                                       v
                             [ 74LS181 Inputs ]
                               (Upper Nibble /
                              Golden Reference)
                                       |
  [ Custom 4-Bit ALU ]                 |
  (Adder + Gates + MUX)                |
  Inputs: A[3..0], B[3..0]             |
           |                           |
           v                           v
     Lower Nibble Y[3..0]         Upper Nibble Y[7..4]
           |                           |
           +------------+--------------+
                        |
                        +-------------> [ 74LS85 Comparator ] (Mode 1: Equivalence)
                        |                      |
                        v                      v
               [ 8-Bit Output Bus ]      🟢 PASS / 🔴 FAIL
```

### 2.1 Mode 0: 8-Bit Hybrid Datapath (Compute Mode)
In Compute Mode, the system functions as an 8-bit ALU:
* The **Custom 4-Bit ALU** processes the lower 4 bits ($A_{3..0}, B_{3..0}$) and generates lower result bits $Y_{3..0}$ along with an intermediate carry-out $C_4$.
* The $C_4$ carry signal is routed directly to the active-low carry-in ($\overline{C_n}$) of the **74LS181**.
* The 74LS181 processes the upper 4 bits ($A_{7..4}, B_{7..4}$), producing upper result bits $Y_{7..4}$ and the final 8-bit carry-out flag $C_8$.
* The complete 8-bit word is driven onto the 8 output LEDs and captured in the accumulator register.

### 2.2 Mode 1: Real-Time Hardware-in-the-Loop Verification (Verify Mode)
In Verify Mode, the system mimics an industrial ASIC/VLSI test fixture:
* The input steering stage routes the identical 4-bit operands ($A_{3..0}, B_{3..0}$) and identical opcode to **both** ALUs simultaneously.
* The Custom ALU (Device Under Test) and the 74LS181 (Certified Golden Model) execute the operation in parallel.
* The 4-bit output of the Custom ALU ($Y_{3..0}$) and the 4-bit output of the 74LS181 ($F_{3..0}$) feed into the 74LS85 4-bit magnitude comparator.
* If both outputs match on all 4 bits, Pin 6 ($A=B$) goes HIGH, illuminating the **Green "PASS / VERIFIED" LED**. If any gate or timing discrepancy occurs, the **Red "MISMATCH" LED** is triggered.

---

## 3. Operation Truth Table & Opcode Mapping

A synchronized 3-bit Opcode ($S_2, S_1, S_0$) governs both execution units:

| Opcode | Operation | Custom ALU Source | 74LS181 Mode ($M$) | 74LS181 Select ($S_3 S_2 S_1 S_0$) | Carry-In ($\overline{C_n}$) |
| :---: | :--- | :--- | :---: | :---: | :---: |
| `000` | **ADD** ($A + B$) | 74LS283 Adder Sum | `0` (Arithmetic) | `1 0 0 1` | `1` (No carry) |
| `001` | **SUB** ($A - B$) | 74LS283 2's Comp Sub | `0` (Arithmetic) | `0 1 1 0` | `0` (Active carry) |
| `010` | **AND** ($A \cdot B$) | 74LS08 Quad AND | `1` (Logic) | `1 1 1 0` | X |
| `011` | **OR** ($A + B$) | 74LS32 Quad OR | `1` (Logic) | `1 0 1 1` | X |
| `100` | **XOR** ($A \oplus B$) | 74LS86 Quad XOR | `1` (Logic) | `1 0 0 1` | X |
| `101` | **NOT A** ($\overline{A}$) | 74LS04 Hex Inverter | `1` (Logic) | `0 0 0 0` | X |
| `110` | **INC A** ($A + 1$) | Incrementer logic | `0` (Arithmetic) | `1 1 1 1` | `0` (Active carry) |
| `111` | **CLEAR** (`0000`) | Tied to GND | `1` (Logic) | `1 1 0 0` | X |

---

## 4. Hardware Implementation & Bill of Materials

| Component | Part Number | Package | Quantity | Functional Purpose |
| :--- | :--- | :---: | :---: | :--- |
| **4-Bit Binary Adder** | 74LS283 | 16-pin DIP | 1 | Custom ALU arithmetic core |
| **Commercial 4-Bit ALU** | 74LS181 | 24-pin DIP | 1 | Upper nibble (Compute) / Golden reference (Verify) |
| **4-Bit Magnitude Comparator** | 74LS85 | 16-pin DIP | 1 | Hardware equivalence checking |
| **Quad 2-Input XOR** | 74LS86 | 14-pin DIP | 2 | IC #1: Subtraction inversion; IC #2: Bitwise XOR |
| **Quad 2-Input AND** | 74LS08 | 14-pin DIP | 1 | Bitwise AND logic |
| **Quad 2-Input OR** | 74LS32 | 14-pin DIP | 1 | Bitwise OR logic |
| **Hex Inverter** | 74LS04 | 14-pin DIP | 1 | Bitwise NOT logic |
| **8:1 Multiplexer** | 74LS151 | 16-pin DIP | 4 | Bit-slice output operation selection |
| **Microcontroller Testbench** | ESP32-S3 N16R8 | DevKit | 1 | Web telemetry, ATPG verification engine |
| **Bypass Capacitors** | Ceramic 104 | Radial | 12 | 0.1 µF power supply noise decoupling |

---

## 5. ESP32-S3 Hardware-in-the-Loop Testbench

The **ESP32-S3 N16R8** (240 MHz dual-core, 16 MB Flash, 8 MB Octal PSRAM) interfaces with the breadboard ALU:
1. **Level-Shifting Protection**: 74LS IC outputs operate at 5.0 V; simple resistor dividers ($1\text{ k}\Omega + 2\text{ k}\Omega$) condition all 8 output lines and status flags to safe 3.3 V logic levels for the ESP32 GPIOs.
2. **Wi-Fi Telemetry Server**: The ESP32 hosts a local Wi-Fi Access Point (`ALU-Testbench-AP`). Any browser connecting to `192.168.4.1` receives a responsive web dashboard streaming 8-bit bus states, binary/hex/dec conversions, and real-time PASS/FAIL badges.
3. **Automated Test Pattern Generator (ATPG)**: Clicking "Run 2,048-Vector Test Suite" triggers a high-speed verification sweep across all possible operand combinations ($16 \times 16 \times 8 = 2048$), comparing the hardware output against calculated results in under 50 ms.

---

## 6. Viva-Voce Questions & Model Answers

### Q1: What makes your ALU architecture "Dual-Mode"?
**Answer**: Our system features a hardware Mode Switch. In Mode 0 (Compute), the Custom 4-Bit ALU and the 74LS181 are cascaded in series via carry propagation ($C_4 \to C_n$) to form an 8-bit hybrid computer datapath. In Mode 1 (Verify), both ALUs operate in parallel on the same 4-bit inputs, feeding their outputs into a 74LS85 comparator to verify custom circuit correctness against the commercial standard.

### Q2: What is the purpose of the 74LS85 comparator in your circuit?
**Answer**: The 74LS85 performs real-time hardware equivalence checking between our custom discrete ALU and the 74LS181 golden reference model. When all 4 output bits match identically, Pin 6 ($A=B$) goes HIGH, illuminating the Green "PASS" LED. If any gate fails or an unexpected glitch occurs, Pin 6 drops LOW, triggering the Red "MISMATCH" LED.

### Q3: How does carry propagation work between the Custom ALU and the 74LS181?
**Answer**: The 74LS283 adder in the Custom ALU produces an active-high carry-out ($C_4$). The 74LS181 uses active-low carry inputs ($\overline{C_n}$) in active-high data mode. In addition mode, $C_4$ connects directly to enable multi-nibble carry ripple from bit 3 to bit 4, generating the final 8-bit carry flag ($C_8$).

### Q4: Why is the ESP32-S3 N16R8 ideal for this testbench?
**Answer**: The ESP32-S3 provides up to 45 GPIOs, allowing simultaneous parallel connection to all 8 result bits, operand monitoring lines, opcode selectors, and status flags without external shift registers. Its 16 MB Flash and dual-core processor allow Core 0 to handle high-speed pin telemetry while Core 1 runs the Wi-Fi AP and WebSockets server.

### Q5: Why did you prototype on breadboard before soldering onto a plain perfboard?
**Answer**: Breadboarding allows iterative verification of individual functional blocks (adder, logic gates, multiplexers) and debugging of signal paths with a logic probe or oscilloscope. Once verified, migrating to socketed perfboard eliminates intermittent jumper contact resistance and provides a permanent, vibration-resistant hardware instrument.
