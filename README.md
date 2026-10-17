# Dual-Mode 8-Bit Reconfigurable ALU with 74LS181 Golden Reference & ESP32-S3 HIL Testbench

**Digital Electronics & Computer Architecture (DECA) Minor Project**  
**Author**: Shubh Kashyap  

---

## 📊 Project Implementation Progress

| Milestone | Visual Progress Bar | Progress | Status |
| :--- | :--- | :---: | :---: |
| **Logisim Evolution 8-Bit Simulation** | `[████████████████████]` | **100%** | ✅ Ready to Run |
| **ESP32-S3 Firmware & Web Dashboard** | `[████████████████████]` | **100%** | ✅ Ready to Flash |
| **Hardware Schematics & Pinout Guides** | `[████████████████████]` | **100%** | ✅ Complete |
| **Project Report & Viva Cheat Sheet** | `[████████████████████]` | **100%** | ✅ Ready to Print |
| **Breadboard Hardware Assembly** | `[████░░░░░░░░░░░░░░░░]` | **20%** | 🛠️ In Progress |
| **Plain Perfboard Transfer & Soldering** | `[░░░░░░░░░░░░░░░░░░░░]` | **0%** | ⏳ Pending Assembly |

**Overall Project Readiness**: `[██████████████░░░░░░]` **70%** (Software, Simulation & Firmware 100% complete; awaiting physical wiring & soldering).

---

## 🚀 Architectural Overview

This project implements a **Dual-Mode Reconfigurable Hardware Architecture** that combines gate-level digital design with industrial ASIC verification:

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

### The Two Operational Modes:
1. **Mode 0: 8-Bit Hybrid Datapath (Compute Mode)**
   * **Custom 4-Bit ALU** (74LS283 + 74LS86/08/32/04 + 4× 74LS151) handles the **lower 4 bits** ($D_0–D_3$).
   * **74LS181 ALU IC** handles the **upper 4 bits** ($D_4–D_7$).
   * Carry propagates from Custom ($C_4$) $\to$ 74LS181 ($\overline{C_n}$), driving an authentic **8-bit parallel output bus**.
2. **Mode 1: Real-Time Hardware-in-the-Loop Verification (Verify Mode)**
   * Both ALUs execute the **exact same 4-bit operation** in parallel.
   * A **74LS85 4-Bit Magnitude Comparator** compares the Custom ALU against the 74LS181 in real-time.
   * **🟢 Green LED**: Lights up when Custom == 74LS181 (**PASS / VERIFIED**).
   * **🔴 Red LED**: Lights up on any logic or timing mismatch (**FAULT**).

---

## 📂 Repository File Index & Navigation

### 1. Hardware Prototyping & Schematics (`hardware_docs/`)
* [**PINOUTS_AND_SCHEMATICS.md**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/PINOUTS_AND_SCHEMATICS.md): Complete pinout diagrams for 74LS181, 74LS85, 74LS283, 74LS86, 74LS08, 74LS32, 74LS04, 74LS151, MUX mapping, and Boolean equations.
* [**BREADBOARD_ASSEMBLY_GUIDE.md**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/BREADBOARD_ASSEMBLY_GUIDE.md): Step-by-step modular breadboard wiring plan with checkpoint tests for each IC stage.
* [**PERFBOARD_SOLDERING_GUIDE.md**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/PERFBOARD_SOLDERING_GUIDE.md): Guide for transferring the circuit to a permanent dot-matrix PCB (using DIP sockets, bus rails, and pre-power smoke tests).

### 2. Software Simulation (`simulation/`)
* [**dual_mode_8bit_alu_processor.circ**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/simulation/dual_mode_8bit_alu_processor.circ): Logisim Evolution circuit model with 8-bit hybrid ALU datapath, 74LS85 equivalence checker, 8-bit register bank, clocking, and status flags.

### 3. ESP32-S3 HIL Testbench & Web Dashboard (`esp32_firmware/`)
* [**esp32_firmware.ino**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/esp32_firmware.ino): Arduino C++ firmware hosting a local Wi-Fi Access Point (`ALU-Testbench-AP`) and Web telemetry server.
* [**web_page.h**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/web_page.h): Modern, responsive web dashboard with live virtual LEDs, Hex/Dec converters, and 1-click 2,048-vector automated verification suite.
* [**pin_definitions.h**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/pin_definitions.h): Safe pinout mapping specifically for the ESP32-S3 N16R8.

### 4. Academic Documentation (`project_report/`)
* [**DECA_ALU_PROJECT_REPORT.md**](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/project_report/DECA_ALU_PROJECT_REPORT.md): Complete minor project report with abstract, block diagrams, truth tables, circuit derivations, and Viva-Voce Q&A with model answers.

---

## 🛠️ Complete Bill of Materials (BOM)

| Component | Part Number | Quantity | Functional Purpose |
| :--- | :--- | :---: | :--- |
| **Commercial 4-Bit ALU** | 74LS181 | 1 | Upper 4 bits (Compute) / Golden reference (Verify) |
| **4-Bit Binary Adder** | 74LS283 | 1 | Custom 4-Bit ALU arithmetic core |
| **4-Bit Magnitude Comparator** | 74LS85 | 1 | Hardware equivalence checking (PASS/FAIL) |
| **Quad 2-Input XOR** | 74LS86 | 2 | IC #1: Subtraction inversion; IC #2: Bitwise XOR |
| **Quad 2-Input AND** | 74LS08 | 1 | Bitwise AND logic |
| **Quad 2-Input OR** | 74LS32 | 1 | Bitwise OR logic |
| **Hex Inverter** | 74LS04 | 1 | Bitwise NOT logic |
| **8:1 Multiplexer** | 74LS151 | 4 | Bit-slice output selection (1 per bit) |
| **Microcontroller Testbench** | ESP32-S3 N16R8 | 1 | Wi-Fi telemetry & ATPG automated test engine |
| **DIP Switches** | 8-Position + 4-Position | 3 | Operands A, B, Opcode, and Mode Select |
| **LED Indicators** | 8× Blue/Green + 1× Green + 1× Red | 10 | 8-Bit result bus + PASS/FAIL status |
| **Bypass Capacitors** | 0.1 µF (104 Ceramic) | 12 | Power rail noise decoupling |
| **Resistors** | 10 kΩ (Pull-down) + 330 Ω (LEDs) | Assorted | Signal pull-down and LED current-limiting |

---

## 📋 Execution Roadmap

1. **Phase 1: Breadboard Prototyping**
   * Wire the Custom 4-Bit ALU core and test with temporary LEDs.
   * Add the 74LS181 and 74LS85 comparator.
   * Verify Mode 0 (8-bit compute) and Mode 1 (4-bit real-time verification).
2. **Phase 2: Transfer to Plain Perfboard**
   * Solder DIP IC sockets onto dot-matrix PCB.
   * Run power rails and solder 0.1 µF bypass capacitors.
   * Perform pre-power voltage checks before inserting the ICs.
3. **Phase 3: ESP32-S3 HIL Testbench & Demo**
   * Connect level-shifted output lines to the ESP32-S3.
   * Flash `esp32_firmware.ino` and open the web dashboard on phone/laptop.
   * Run the 2,048-vector automated verification suite for the examiner.
