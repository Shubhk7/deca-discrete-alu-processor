# 4-Bit ALU with Register Bank & ESP32-S3 Testbench (DECA Minor Project)

This repository contains the complete engineering files, hardware guides, simulation models, firmware, and documentation for the **Digital Electronics & Computer Architecture (DECA)** minor project.

---

## Project Structure & Navigation

### 1. Hardware & Assembly Guides (`hardware_docs/`)
* [PINOUTS_AND_SCHEMATICS.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/PINOUTS_AND_SCHEMATICS.md): Exact IC pinout diagrams (74LS283, 74LS86, 74LS08, 74LS32, 74LS04, 74LS151), MUX input tables, and Boolean equations.
* [BREADBOARD_ASSEMBLY_GUIDE.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/BREADBOARD_ASSEMBLY_GUIDE.md): Modular, step-by-step breadboard wiring procedure with intermediate testing checklists.
* [PERFBOARD_SOLDERING_GUIDE.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/PERFBOARD_SOLDERING_GUIDE.md): Practical guide for migrating from breadboard to a plain dot-matrix PCB (using DIP IC sockets, power bus routing, and pre-power testing).

### 2. Software Simulation (`simulation/`)
* [alu_4bit_processor.circ](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/simulation/alu_4bit_processor.circ): Ready-to-load Logisim Evolution circuit file featuring the 4-bit ALU core, A & B registers, Accumulator register, Clock, Reset, Opcode selector, and Status Flags (Carry, Zero).

### 3. ESP32-S3 Testbench & Dashboard (`esp32_firmware/`)
* [esp32_firmware.ino](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/esp32_firmware.ino): Arduino C++ firmware running a Wi-Fi Access Point and WebSocket/HTTP server.
* [pin_definitions.h](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/pin_definitions.h): Safe pinout configuration specifically for the ESP32-S3 N16R8.
* [web_page.h](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/esp32_firmware/web_page.h): Self-contained, responsive cyber-styled web dashboard with real-time virtual LEDs, Hex/Dec converters, and an automated verification test runner.

### 4. Academic Documentation (`project_report/`)
* [DECA_ALU_PROJECT_REPORT.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/project_report/DECA_ALU_PROJECT_REPORT.md): Complete minor project report with abstract, truth tables, circuit derivations, test vectors, and Viva-Voce preparation Q&A.

---

## 3-Phase Solo Execution Plan

1. **Phase 1: Breadboard Prototyping**
   * Follow [BREADBOARD_ASSEMBLY_GUIDE.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/BREADBOARD_ASSEMBLY_GUIDE.md).
   * Assemble and test each module (Adder $\to$ Logic gates $\to$ MUXes) independently.
2. **Phase 2: Transfer to Plain Perfboard**
   * Follow [PERFBOARD_SOLDERING_GUIDE.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/hardware_docs/PERFBOARD_SOLDERING_GUIDE.md).
   * Solder IC sockets and test power rail voltages before plugging in the ICs.
3. **Phase 3: Logisim Simulation & Viva Presentation**
   * Open [alu_4bit_processor.circ](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/simulation/alu_4bit_processor.circ) in Logisim Evolution to demonstrate the clocked processor datapath.
   * Review the Viva questions in [DECA_ALU_PROJECT_REPORT.md](file:///c:/Users/SHUBH%20KASHYAP/OneDrive/Desktop/PROGRAMMING/DECA%20minor%20project/project_report/DECA_ALU_PROJECT_REPORT.md).
