#pragma once
#include <Arduino.h>

// ============================================================================
// DECA Minor Project: Dual-Mode Hybrid Discrete ALU Processor Web Dashboard
// Single-page application embedded in ESP32 Flash (PROGMEM)
// Author: Shubh Kashyap | Course: Digital Electronics & Computer Architecture
// ============================================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DECA Dual-Mode Hybrid ALU Processor Dashboard</title>
  <style>
    :root {
      --bg: #090d16;
      --card-bg: rgba(19, 27, 46, 0.88);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #38bdf8;
      --accent-glow: rgba(56, 189, 248, 0.35);
      --green: #10b981;
      --green-glow: rgba(16, 185, 129, 0.4);
      --amber: #f59e0b;
      --amber-glow: rgba(245, 158, 11, 0.35);
      --red: #ef4444;
      --red-glow: rgba(239, 68, 68, 0.4);
      --purple: #a855f7;
      --purple-glow: rgba(168, 85, 247, 0.35);
      --led-off: #1e293b;
      --led-off-border: #334155;
      --text: #f1f5f9;
      --muted: #94a3b8;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, monospace; }
    body { background: var(--bg); color: var(--text); padding: 18px; min-height: 100vh; display: flex; flex-direction: column; align-items: center; }
    .container { max-width: 1120px; width: 100%; display: flex; flex-direction: column; gap: 16px; }

    header { display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 12px; padding-bottom: 12px; border-bottom: 1px solid var(--card-border); }
    .header-text h1 { font-size: 1.55rem; color: #fff; letter-spacing: 0.5px; }
    .header-text p { font-size: 0.85rem; color: var(--muted); margin-top: 3px; }
    .conn-badge { display: inline-flex; align-items: center; gap: 8px; padding: 6px 14px; border-radius: 999px; font-size: 0.78rem; font-weight: 600; background: rgba(16, 185, 129, 0.12); color: var(--green); border: 1px solid rgba(16, 185, 129, 0.3); }
    .conn-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--green); box-shadow: 0 0 10px var(--green); }

    /* Operational Mode Banner */
    .status-banner {
      background: linear-gradient(135deg, rgba(56, 189, 248, 0.12), rgba(168, 85, 247, 0.10));
      border: 1px solid rgba(56, 189, 248, 0.3);
      border-radius: 12px;
      padding: 14px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 12px;
    }
    .status-banner.mode1 {
      background: linear-gradient(135deg, rgba(168, 85, 247, 0.16), rgba(16, 185, 129, 0.12));
      border-color: rgba(168, 85, 247, 0.45);
    }
    .status-left { display: flex; flex-direction: column; gap: 4px; }
    .status-title { font-size: 1.05rem; font-weight: 800; color: var(--accent); letter-spacing: 0.5px; }
    .status-title.mode1 { color: var(--purple); }
    .status-desc { font-size: 0.83rem; color: var(--muted); line-height: 1.35; }
    .pill-group { display: flex; gap: 8px; flex-wrap: wrap; }
    .status-pill { padding: 6px 14px; border-radius: 20px; font-size: 0.78rem; font-weight: 700; background: rgba(56, 189, 248, 0.18); color: var(--accent); border: 1px solid var(--accent); }

    /* Silicon Match & Verification Dual Card */
    .verification-strip {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
      gap: 14px;
    }
    .verify-box {
      border-radius: 12px;
      padding: 14px 18px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 10px;
      background: rgba(16, 185, 129, 0.10);
      border: 1px solid rgba(16, 185, 129, 0.35);
      transition: all 0.2s ease-in-out;
    }
    .verify-box.fail {
      background: rgba(239, 68, 68, 0.12);
      border-color: rgba(239, 68, 68, 0.45);
    }
    .verify-box.neutral {
      background: rgba(56, 189, 248, 0.08);
      border-color: rgba(56, 189, 248, 0.25);
    }
    .vb-title { font-size: 0.76rem; text-transform: uppercase; color: var(--muted); font-weight: 700; letter-spacing: 0.5px; }
    .vb-status { font-size: 1.05rem; font-weight: 800; display: flex; align-items: center; gap: 8px; }
    .vb-status.pass { color: var(--green); }
    .vb-status.fail { color: var(--red); }
    .vb-status.info { color: var(--accent); }
    .vb-sub { font-size: 0.80rem; font-family: monospace; color: var(--muted); margin-top: 3px; }

    .grid-2 { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 16px; }
    .card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: 14px; padding: 18px; backdrop-filter: blur(12px); box-shadow: 0 8px 24px rgba(0,0,0,0.3); }
    .card h2 { font-size: 0.90rem; color: var(--muted); text-transform: uppercase; letter-spacing: 1px; margin-bottom: 12px; display: flex; justify-content: space-between; align-items: center; }

    /* LED Bit Visualizer */
    .led-bus-header { display: flex; justify-content: space-between; font-size: 0.72rem; color: var(--muted); padding: 0 10px; text-transform: uppercase; }
    .led-bus { display: flex; gap: 6px; justify-content: center; align-items: center; margin: 10px 0; }
    .nibble-divider { width: 3px; height: 42px; background: rgba(56, 189, 248, 0.4); margin: 0 6px; border-radius: 2px; }
    .nibble-divider.comparator { background: rgba(168, 85, 247, 0.6); box-shadow: 0 0 8px var(--purple-glow); }
    .led-cell { display: flex; flex-direction: column; align-items: center; gap: 5px; font-size: 0.68rem; color: var(--muted); }
    .led {
      width: 26px;
      height: 26px;
      border-radius: 50%;
      background: var(--led-off);
      border: 2px solid var(--led-off-border);
      transition: all 0.12s ease-in-out;
      box-shadow: inset 0 2px 5px rgba(0,0,0,0.6);
    }
    .led.active-blue { background: #38bdf8; border-color: #bae6fd; box-shadow: 0 0 14px #38bdf8, inset 0 -2px 4px rgba(255,255,255,0.7); }
    .led.active-green { background: #10b981; border-color: #a7f3d0; box-shadow: 0 0 14px #10b981, inset 0 -2px 4px rgba(255,255,255,0.7); }
    .led.active-purple { background: #a855f7; border-color: #e9d5ff; box-shadow: 0 0 14px #a855f7, inset 0 -2px 4px rgba(255,255,255,0.7); }
    .led.active-amber { background: #f59e0b; border-color: #fde68a; box-shadow: 0 0 14px #f59e0b, inset 0 -2px 4px rgba(255,255,255,0.7); }

    .readout-strip {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      background: rgba(0, 0, 0, 0.45);
      border-radius: 8px;
      padding: 8px 12px;
      margin-top: 10px;
      font-family: monospace;
      text-align: center;
      gap: 4px;
    }
    .readout-label { font-size: 0.65rem; color: var(--muted); text-transform: uppercase; margin-bottom: 2px; }
    .readout-val { font-size: 0.98rem; font-weight: 700; color: #fff; }

    /* Control Panel */
    .ctrl-section { display: flex; flex-direction: column; gap: 13px; }
    .override-switch-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 12px 16px;
      background: rgba(0, 0, 0, 0.35);
      border-radius: 10px;
      border: 1px solid rgba(255, 255, 255, 0.08);
    }
    .switch-label { font-weight: 600; font-size: 0.92rem; display: flex; flex-direction: column; gap: 2px; }
    .switch-subtext { font-size: 0.74rem; color: var(--muted); font-weight: normal; }

    /* Toggle Switch */
    .switch { position: relative; display: inline-block; width: 50px; height: 26px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #334155; transition: .25s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 18px; width: 18px; left: 4px; bottom: 4px; background-color: white; transition: .25s; border-radius: 50%; }
    input:checked + .slider { background-color: var(--accent); }
    input:checked + .slider:before { transform: translateX(24px); }

    .ctrl-row { display: grid; grid-template-columns: 80px 1fr 70px; align-items: center; gap: 10px; }
    .ctrl-row label { font-size: 0.84rem; font-weight: 600; color: var(--muted); }
    .ctrl-row input[type=range] { accent-color: var(--accent); cursor: pointer; width: 100%; }
    .ctrl-row input[type=number], .ctrl-row select {
      background: rgba(0, 0, 0, 0.5);
      border: 1px solid var(--card-border);
      color: #fff;
      padding: 6px 10px;
      border-radius: 6px;
      font-size: 0.88rem;
      font-family: monospace;
      outline: none;
      width: 100%;
    }

    .preset-chips { display: flex; gap: 6px; flex-wrap: wrap; margin-top: 4px; }
    .chip { font-size: 0.72rem; padding: 4px 10px; background: rgba(255, 255, 255, 0.06); border: 1px solid var(--card-border); border-radius: 6px; cursor: pointer; color: var(--muted); transition: all 0.15s; }
    .chip:hover { background: rgba(56, 189, 248, 0.15); color: var(--accent); border-color: var(--accent); }

    /* History Table */
    .history-card { overflow-x: auto; }
    .history-table { width: 100%; border-collapse: collapse; font-family: monospace; font-size: 0.80rem; }
    .history-table th { text-align: left; padding: 8px 10px; color: var(--muted); border-bottom: 1px solid var(--card-border); font-size: 0.70rem; text-transform: uppercase; }
    .history-table td { padding: 6px 10px; border-bottom: 1px solid rgba(255, 255, 255, 0.04); }
    .history-table tr:hover { background: rgba(255, 255, 255, 0.03); }
    .badge-op { display: inline-block; padding: 2px 7px; border-radius: 4px; font-weight: 700; background: rgba(56, 189, 248, 0.15); color: var(--accent); }
    .badge-pass { display: inline-block; padding: 2px 7px; border-radius: 4px; font-weight: 700; background: rgba(16, 185, 129, 0.15); color: var(--green); }
    .badge-fail { display: inline-block; padding: 2px 7px; border-radius: 4px; font-weight: 700; background: rgba(239, 68, 68, 0.15); color: var(--red); }
    .badge-silicon { display: inline-block; padding: 2px 7px; border-radius: 4px; font-weight: 700; background: rgba(168, 85, 247, 0.15); color: var(--purple); }

    footer { text-align: center; font-size: 0.74rem; color: var(--muted); margin-top: 10px; padding-bottom: 10px; }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <div class="header-text">
        <h1>DECA Dual-Mode Hybrid ALU Processor Dashboard</h1>
        <p>Custom 4-Bit ALU + 74LS181N + 74HCT85 Comparator · Author: Shubh Kashyap · Golden Model: Arduino R4 WiFi</p>
      </div>
      <div class="conn-badge" id="conn-status">
        <span class="conn-dot" id="conn-dot"></span>
        <span id="conn-text">WebSocket: Connecting...</span>
      </div>
    </header>

    <!-- Operational Mode Banner -->
    <div class="status-banner" id="mode-banner">
      <div class="status-left">
        <div class="status-title" id="mode-title">MODE 0: 8-BIT CASCADED COMPUTE</div>
        <div class="status-desc" id="mode-desc">
          Lower Nibble Y[3:0]: Custom 4-Bit ALU (74LS83N + Gates) | Upper Nibble Y[7:4]: 74LS181N ALU | Carry C4 cascades into /Cn
        </div>
      </div>
      <div class="pill-group">
        <div class="status-pill" id="input-source-pill">INPUT: PHYSICAL DIP SWITCHES</div>
      </div>
    </div>

    <!-- Verification & Silicon Match Strip -->
    <div class="verification-strip">
      <!-- IC12 (74HCT85) Hardware Silicon Match Box -->
      <div class="verify-box" id="box-7485">
        <div>
          <div class="vb-title">IC12 (74HCT85) Silicon Equivalence</div>
          <div class="vb-status pass" id="status-7485">
            <span>●</span>
            <span id="text-7485">74HCT85 PIN 6 (A=B): MATCH</span>
          </div>
          <div class="vb-sub" id="detail-7485">Custom Y[3:0] (0x0) == 74LS181 F[3:0] (0x0)</div>
        </div>
        <div id="badge-7485" class="badge-silicon">HARDWARE COMPARATOR</div>
      </div>

      <!-- Software Golden Model Verifier Box -->
      <div class="verify-box" id="box-golden">
        <div>
          <div class="vb-title">Arduino R4 Software Golden Model</div>
          <div class="vb-status pass" id="status-golden">
            <span>●</span>
            <span id="text-golden">GOLDEN REFERENCE: PASS</span>
          </div>
          <div class="vb-sub" id="detail-golden">Expected: 0x00 | Actual HW: 0x00</div>
        </div>
        <div id="badge-golden" class="badge-pass">100% EQUIVALENCE</div>
      </div>
    </div>

    <!-- Hardware ALU Output Bus Hero Card -->
    <div class="card" style="border-color: rgba(16, 185, 129, 0.35); background: linear-gradient(180deg, rgba(16, 185, 129, 0.05), var(--card-bg));">
      <h2>
        <span>Hardware ALU Output Bus (Y7 .. Y0)</span>
        <span style="color: var(--green); font-family: monospace; font-size: 1.15rem;" id="out-hex">0x00</span>
      </h2>
      <div class="led-bus-header">
        <span>Upper Nibble: Commercial 74LS181N (F3..F0)</span>
        <span>Lower Nibble: Custom 4-Bit ALU (Y3..Y0)</span>
      </div>
      <div class="led-bus">
        <!-- Upper Nibble (Y7..Y4) Commercial 74LS181 -->
        <div class="led-cell"><div class="led" id="y7"></div><span>Y7 (F3)</span></div>
        <div class="led-cell"><div class="led" id="y6"></div><span>Y6 (F2)</span></div>
        <div class="led-cell"><div class="led" id="y5"></div><span>Y5 (F1)</span></div>
        <div class="led-cell"><div class="led" id="y4"></div><span>Y4 (F0)</span></div>
        <!-- Center Divider between Nibbles -->
        <div class="nibble-divider" id="bus-divider"></div>
        <!-- Lower Nibble (Y3..Y0) Custom 4-Bit ALU -->
        <div class="led-cell"><div class="led" id="y3"></div><span>Y3</span></div>
        <div class="led-cell"><div class="led" id="y2"></div><span>Y2</span></div>
        <div class="led-cell"><div class="led" id="y1"></div><span>Y1</span></div>
        <div class="led-cell"><div class="led" id="y0"></div><span>Y0</span></div>
      </div>
      <div class="readout-strip">
        <div><div class="readout-label">Binary</div><div class="readout-val" id="out-bin">0000 0000</div></div>
        <div><div class="readout-label">Decimal</div><div class="readout-val" id="out-dec">0</div></div>
        <div><div class="readout-label">Custom Y[3:0]</div><div class="readout-val" id="out-custom-nibble" style="color: var(--accent);">0x0 (0)</div></div>
        <div><div class="readout-label">74LS181 F[3:0]</div><div class="readout-val" id="out-181-nibble" style="color: var(--purple);">0x0 (0)</div></div>
      </div>
    </div>

    <!-- Input Operands Grid -->
    <div class="grid-2">
      <!-- Operand A Card -->
      <div class="card">
        <h2>
          <span>Operand A Bus (A7 .. A0)</span>
          <span style="color: var(--accent); font-family: monospace;" id="disp-a-hex">0x00</span>
        </h2>
        <div class="led-bus">
          <div class="led-cell"><div class="led" id="a7"></div><span>A7</span></div>
          <div class="led-cell"><div class="led" id="a6"></div><span>A6</span></div>
          <div class="led-cell"><div class="led" id="a5"></div><span>A5</span></div>
          <div class="led-cell"><div class="led" id="a4"></div><span>A4</span></div>
          <div class="nibble-divider"></div>
          <div class="led-cell"><div class="led" id="a3"></div><span>A3</span></div>
          <div class="led-cell"><div class="led" id="a2"></div><span>A2</span></div>
          <div class="led-cell"><div class="led" id="a1"></div><span>A1</span></div>
          <div class="led-cell"><div class="led" id="a0"></div><span>A0</span></div>
        </div>
        <div class="readout-strip">
          <div><div class="readout-label">Binary</div><div class="readout-val" id="disp-a-bin">0000 0000</div></div>
          <div><div class="readout-label">Decimal</div><div class="readout-val" id="disp-a-dec">0</div></div>
          <div><div class="readout-label">A[7:4] 181</div><div class="readout-val" id="disp-a-high">0x0</div></div>
          <div><div class="readout-label">A[3:0] Custom</div><div class="readout-val" id="disp-a-low">0x0</div></div>
        </div>
      </div>

      <!-- Operand B Card -->
      <div class="card">
        <h2>
          <span>Operand B Bus (B7 .. B0)</span>
          <span style="color: var(--accent); font-family: monospace;" id="disp-b-hex">0x00</span>
        </h2>
        <div class="led-bus">
          <div class="led-cell"><div class="led" id="b7"></div><span>B7</span></div>
          <div class="led-cell"><div class="led" id="b6"></div><span>B6</span></div>
          <div class="led-cell"><div class="led" id="b5"></div><span>B5</span></div>
          <div class="led-cell"><div class="led" id="b4"></div><span>B4</span></div>
          <div class="nibble-divider"></div>
          <div class="led-cell"><div class="led" id="b3"></div><span>B3</span></div>
          <div class="led-cell"><div class="led" id="b2"></div><span>B2</span></div>
          <div class="led-cell"><div class="led" id="b1"></div><span>B1</span></div>
          <div class="led-cell"><div class="led" id="b0"></div><span>B0</span></div>
        </div>
        <div class="readout-strip">
          <div><div class="readout-label">Binary</div><div class="readout-val" id="disp-b-bin">0000 0000</div></div>
          <div><div class="readout-label">Decimal</div><div class="readout-val" id="disp-b-dec">0</div></div>
          <div><div class="readout-label">B[7:4] 181</div><div class="readout-val" id="disp-b-high">0x0</div></div>
          <div><div class="readout-label">B[3:0] Custom</div><div class="readout-val" id="disp-b-low">0x0</div></div>
        </div>
      </div>
    </div>

    <!-- Opcode & Remote Control Panel Grid -->
    <div class="grid-2">
      <!-- Opcode Monitor Card -->
      <div class="card">
        <h2>
          <span>Opcode Select (S2 .. S0)</span>
          <span class="badge-op" id="disp-op-name">ADD</span>
        </h2>
        <div class="led-bus">
          <div class="led-cell"><div class="led" id="s2"></div><span>S2 (MSB)</span></div>
          <div class="led-cell"><div class="led" id="s1"></div><span>S1</span></div>
          <div class="led-cell"><div class="led" id="s0"></div><span>S0 (LSB)</span></div>
        </div>
        <div class="readout-strip" style="grid-template-columns: repeat(3, 1fr);">
          <div><div class="readout-label">Select Bits</div><div class="readout-val" id="disp-op-bin">000</div></div>
          <div><div class="readout-label">Operation</div><div class="readout-val" id="disp-op-desc">A + B</div></div>
          <div><div class="readout-label">Opcode Val</div><div class="readout-val" id="disp-op-val">0</div></div>
        </div>
      </div>

      <!-- Remote Input Override Panel -->
      <div class="card" style="border-color: rgba(56, 189, 248, 0.35);">
        <h2>
          <span>Remote Input Injection (Hybrid Control)</span>
          <span style="font-size: 0.72rem; color: var(--accent);">4.7kΩ Resistor Injection</span>
        </h2>
        <div class="ctrl-section">
          <div class="override-switch-bar">
            <div class="switch-label">
              <span>Enable Remote Override</span>
              <span class="switch-subtext">Overrides physical breadboard DIP switches</span>
            </div>
            <label class="switch">
              <input type="checkbox" id="override-toggle" onchange="toggleOverride()">
              <span class="slider"></span>
            </label>
          </div>

          <!-- Input A Control -->
          <div class="ctrl-row">
            <label for="ctrl-a-slider">Input A:</label>
            <input type="range" id="ctrl-a-slider" min="0" max="255" value="0" oninput="syncControlA(this.value)">
            <input type="number" id="ctrl-a-num" min="0" max="255" value="0" oninput="syncControlA(this.value)">
          </div>

          <!-- Input B Control -->
          <div class="ctrl-row">
            <label for="ctrl-b-slider">Input B:</label>
            <input type="range" id="ctrl-b-slider" min="0" max="255" value="0" oninput="syncControlB(this.value)">
            <input type="number" id="ctrl-b-num" min="0" max="255" value="0" oninput="syncControlB(this.value)">
          </div>

          <!-- Opcode Selector & Mode Toggle -->
          <div class="ctrl-row">
            <label for="ctrl-op">Opcode:</label>
            <select id="ctrl-op" onchange="sendCurrentInputs()">
              <option value="0">000: ADD (Addition)</option>
              <option value="1">001: SUB (Subtraction 2's C)</option>
              <option value="2">010: AND (Bitwise AND)</option>
              <option value="3">011: OR  (Bitwise OR)</option>
              <option value="4">100: XOR (Bitwise XOR)</option>
              <option value="5">101: NOT (Bitwise NOT)</option>
              <option value="6">110: SHL (Logical Shift Left)</option>
              <option value="7">111: SHR (Logical Shift Right)</option>
            </select>
            <button class="chip" style="padding: 7px 10px; font-weight: 700;" onclick="toggleCtrlMode()" id="ctrl-mode-btn">Mode 0</button>
          </div>

          <!-- Quick Test Presets -->
          <div class="preset-chips">
            <span style="font-size: 0.70rem; color: var(--muted); align-self: center;">Presets:</span>
            <button class="chip" onclick="applyPreset(5, 3, 0, 0)">M0: 5 + 3</button>
            <button class="chip" onclick="applyPreset(120, 45, 0, 0)">M0: 120 + 45</button>
            <button class="chip" onclick="applyPreset(100, 40, 1, 0)">M0: 100 - 40</button>
            <button class="chip" onclick="applyPreset(7, 3, 0, 1)">M1: Verify 7+3</button>
            <button class="chip" onclick="applyPreset(9, 6, 4, 1)">M1: Verify XOR</button>
            <button class="chip" onclick="applyPreset(12, 5, 2, 1)">M1: Verify AND</button>
          </div>
        </div>
      </div>
    </div>

    <!-- Operation History Card -->
    <div class="card history-card">
      <h2>
        <span>Real-Time Silicon Verification Telemetry Log</span>
        <button class="chip" style="padding: 3px 10px;" onclick="clearHistory()">Clear</button>
      </h2>
      <table class="history-table">
        <thead>
          <tr>
            <th>Time</th>
            <th>Mode</th>
            <th>Opcode</th>
            <th>A</th>
            <th>B</th>
            <th>Custom Y[3:0]</th>
            <th>74LS181 F[3:0]</th>
            <th>Full Y[7:0]</th>
            <th>7485 HW Match</th>
            <th>Golden Exp</th>
            <th>Status</th>
          </tr>
        </thead>
        <tbody id="history-body">
          <tr><td colspan="11" style="text-align: center; color: var(--muted); padding: 16px;">Waiting for telemetry...</td></tr>
        </tbody>
      </table>
    </div>

    <footer>
      Digital Electronics & Computer Architecture (DECA) Minor Project · Author: Shubh Kashyap<br>
      Heterogeneous 3-Board Infrastructure: ESP32-S3 (Brain) · Arduino UNO R4 WiFi (Verifier) · Arduino UNO R4 Minima (Power Rail)
    </footer>
  </div>

  <script>
    const OP_NAMES = ["ADD", "SUB", "AND", "OR", "XOR", "NOT", "SHL", "SHR"];
    const OP_DESCS = ["A + B", "A - B", "A & B", "A | B", "A ^ B", "~A", "A << 1", "A >> 1"];

    let ws = null;
    let isOverride = false;
    let ctrlMode = 0; // 0 = 8-bit Cascaded, 1 = Silicon Verification
    const historyList = [];
    const MAX_HISTORY = 20;

    // Helper: Update an array of LEDs based on integer value
    function updateLeds(prefix, value, count, activeClass) {
      for (let i = 0; i < count; i++) {
        const bit = (value >> i) & 1;
        const el = document.getElementById(prefix + i);
        if (el) {
          el.className = bit ? ("led " + activeClass) : "led";
        }
      }
    }

    // Format binary with space between nibbles
    function formatBin8(val) {
      const s = (val & 0xFF).toString(2).padStart(8, '0');
      return s.substring(0, 4) + ' ' + s.substring(4);
    }

    // Connect WebSocket
    function connectWs() {
      const proto = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
      const host = window.location.hostname || '192.168.4.1';
      const url = `${proto}//${host}:81/`;

      ws = new WebSocket(url);

      ws.onopen = () => {
        document.getElementById('conn-status').style.background = 'rgba(16, 185, 129, 0.12)';
        document.getElementById('conn-status').style.color = 'var(--green)';
        document.getElementById('conn-status').style.borderColor = 'rgba(16, 185, 129, 0.3)';
        document.getElementById('conn-dot').style.background = 'var(--green)';
        document.getElementById('conn-dot').style.boxShadow = '0 0 10px var(--green)';
        document.getElementById('conn-text').textContent = 'WebSocket: Live (100ms)';
      };

      ws.onclose = () => {
        document.getElementById('conn-status').style.background = 'rgba(239, 68, 68, 0.12)';
        document.getElementById('conn-status').style.color = 'var(--red)';
        document.getElementById('conn-status').style.borderColor = 'rgba(239, 68, 68, 0.3)';
        document.getElementById('conn-dot').style.background = 'var(--red)';
        document.getElementById('conn-dot').style.boxShadow = '0 0 10px var(--red)';
        document.getElementById('conn-text').textContent = 'WebSocket: Reconnecting...';
        setTimeout(connectWs, 1500);
      };

      ws.onerror = (err) => {
        ws.close();
      };

      ws.onmessage = (evt) => {
        try {
          const data = JSON.parse(evt.data);
          if (data.type === 'telemetry') {
            applyTelemetry(data);
          }
        } catch (e) {
          console.error("WS Parse error", e);
        }
      };
    }

    // Apply Telemetry to UI
    let lastA = -1, lastB = -1, lastOp = -1, lastMode = -1, lastY = -1;

    function applyTelemetry(data) {
      // 1. Output Bus Y[7:0]
      // Lower nibble (Y3..Y0) is Custom ALU, Upper nibble (Y7..Y4) is 74LS181
      for (let i = 0; i < 4; i++) {
        const bit = (data.y >> i) & 1;
        const el = document.getElementById('y' + i);
        if (el) el.className = bit ? "led active-green" : "led";
      }
      for (let i = 4; i < 8; i++) {
        const bit = (data.y >> i) & 1;
        const el = document.getElementById('y' + i);
        if (el) el.className = bit ? (data.mode === 1 ? "led active-purple" : "led active-green") : "led";
      }

      const hexY = '0x' + (data.y & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('out-hex').textContent = hexY;
      document.getElementById('out-dec').textContent = data.y;
      document.getElementById('out-bin').textContent = formatBin8(data.y);

      const customNibble = data.y & 0x0F;
      const ls181Nibble = (data.y >> 4) & 0x0F;
      document.getElementById('out-custom-nibble').textContent = `0x${customNibble.toString(16).toUpperCase()} (${customNibble})`;
      document.getElementById('out-181-nibble').textContent = `0x${ls181Nibble.toString(16).toUpperCase()} (${ls181Nibble})`;

      // 2. Operand A Bus
      updateLeds('a', data.a, 8, 'active-blue');
      const hexA = '0x' + (data.a & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('disp-a-hex').textContent = hexA;
      document.getElementById('disp-a-dec').textContent = data.a;
      document.getElementById('disp-a-bin').textContent = formatBin8(data.a);
      document.getElementById('disp-a-high').textContent = '0x' + ((data.a >> 4) & 0x0F).toString(16).toUpperCase();
      document.getElementById('disp-a-low').textContent = '0x' + (data.a & 0x0F).toString(16).toUpperCase();

      // 3. Operand B Bus
      updateLeds('b', data.b, 8, 'active-blue');
      const hexB = '0x' + (data.b & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('disp-b-hex').textContent = hexB;
      document.getElementById('disp-b-dec').textContent = data.b;
      document.getElementById('disp-b-bin').textContent = formatBin8(data.b);
      document.getElementById('disp-b-high').textContent = '0x' + ((data.b >> 4) & 0x0F).toString(16).toUpperCase();
      document.getElementById('disp-b-low').textContent = '0x' + (data.b & 0x0F).toString(16).toUpperCase();

      // 4. Opcode Bus
      updateLeds('s', data.opcode, 3, 'active-amber');
      const opName = OP_NAMES[data.opcode] || "UNK";
      document.getElementById('disp-op-name').textContent = opName;
      document.getElementById('disp-op-bin').textContent = (data.opcode & 7).toString(2).padStart(3, '0');
      document.getElementById('disp-op-desc').textContent = OP_DESCS[data.opcode] || "";
      document.getElementById('disp-op-val').textContent = data.opcode;

      // 5. Operational Mode Banner
      const modeBanner = document.getElementById('mode-banner');
      const modeTitle = document.getElementById('mode-title');
      const modeDesc = document.getElementById('mode-desc');
      const busDivider = document.getElementById('bus-divider');

      if (data.mode === 0) {
        modeBanner.className = "status-banner";
        modeTitle.className = "status-title";
        modeTitle.textContent = "MODE 0: 8-BIT CASCADED COMPUTE";
        modeDesc.textContent = "Lower Nibble Y[3:0]: Custom 4-Bit ALU (74LS83N + Gates) | Upper Nibble Y[7:4]: Commercial 74LS181N ALU | Carry C4 cascades into /Cn";
        busDivider.className = "nibble-divider";
      } else {
        modeBanner.className = "status-banner mode1";
        modeTitle.className = "status-title mode1";
        modeTitle.textContent = "MODE 1: REAL-TIME SILICON VERIFICATION";
        modeDesc.textContent = "Custom 4-Bit ALU Y[3:0] vs Commercial 74LS181N F[3:0] evaluated in real-time hardware via IC12 (74HCT85 Magnitude Comparator Pin 6 A=B)";
        busDivider.className = "nibble-divider comparator";
      }

      // 6. 74HCT85 Silicon Match Box
      const box7485 = document.getElementById('box-7485');
      const status7485 = document.getElementById('status-7485');
      const text7485 = document.getElementById('text-7485');
      const detail7485 = document.getElementById('detail-7485');

      if (data.mode === 0) {
        box7485.className = "verify-box neutral";
        status7485.className = "vb-status info";
        text7485.textContent = "74HCT85 COMPARATOR: IDLE IN MODE 0";
        detail7485.textContent = "Datapath configured for 8-bit cascaded compute (Nibbles chained via carry)";
      } else {
        if (data.hw_7485_pass) {
          box7485.className = "verify-box";
          status7485.className = "vb-status pass";
          text7485.textContent = "74HCT85 PIN 6 (A=B): MATCH (PASS)";
          detail7485.textContent = `Custom Y[3:0] (0x${customNibble.toString(16).toUpperCase()}) == 74LS181 F[3:0] (0x${ls181Nibble.toString(16).toUpperCase()})`;
        } else {
          box7485.className = "verify-box fail";
          status7485.className = "vb-status fail";
          text7485.textContent = "74HCT85 PIN 6 (A=B): MISMATCH (FAIL)";
          detail7485.textContent = `Custom Y[3:0] (0x${customNibble.toString(16).toUpperCase()}) != 74LS181 F[3:0] (0x${ls181Nibble.toString(16).toUpperCase()})`;
        }
      }

      // 7. Software Golden Model Box
      const boxGolden = document.getElementById('box-golden');
      const statusGolden = document.getElementById('status-golden');
      const textGolden = document.getElementById('text-golden');
      const detailGolden = document.getElementById('detail-golden');
      const expHex = '0x' + (data.expected & 0xFF).toString(16).toUpperCase().padStart(2, '0');

      if (data.golden_match) {
        boxGolden.className = "verify-box";
        statusGolden.className = "vb-status pass";
        textGolden.textContent = "GOLDEN REFERENCE: PASS (MATCH)";
        detailGolden.textContent = `Expected: ${expHex} (${data.expected}) | Actual HW: ${hexY} (${data.y})`;
      } else {
        boxGolden.className = "verify-box fail";
        statusGolden.className = "vb-status fail";
        textGolden.textContent = "GOLDEN REFERENCE: MISMATCH (FAIL)";
        detailGolden.textContent = `Expected: ${expHex} (${data.expected}) != Actual HW: ${hexY} (${data.y})`;
      }

      // 8. Override Source Pill
      const pill = document.getElementById('input-source-pill');
      if (data.override) {
        pill.textContent = "INPUT: REMOTE OVERRIDE (ESP32)";
        pill.style.background = "rgba(245, 158, 11, 0.2)";
        pill.style.borderColor = "var(--amber)";
        pill.style.color = "var(--amber)";
      } else {
        pill.textContent = "INPUT: PHYSICAL DIP SWITCHES";
        pill.style.background = "rgba(56, 189, 248, 0.18)";
        pill.style.borderColor = "var(--accent)";
        pill.style.color = "var(--accent)";
      }

      // Sync Controls if NOT currently overriding
      if (!isOverride) {
        document.getElementById('ctrl-a-slider').value = data.a;
        document.getElementById('ctrl-a-num').value = data.a;
        document.getElementById('ctrl-b-slider').value = data.b;
        document.getElementById('ctrl-b-num').value = data.b;
        document.getElementById('ctrl-op').value = data.opcode;
        ctrlMode = data.mode;
        document.getElementById('ctrl-mode-btn').textContent = ctrlMode ? "Mode 1 (Verify)" : "Mode 0 (8-Bit)";
      }

      // Record to history if state changed
      if (data.a !== lastA || data.b !== lastB || data.opcode !== lastOp || data.mode !== lastMode || data.y !== lastY) {
        addHistoryEntry(data);
        lastA = data.a;
        lastB = data.b;
        lastOp = data.opcode;
        lastMode = data.mode;
        lastY = data.y;
      }
    }

    // Sync Slider and Number Input
    function syncControlA(val) {
      val = Math.max(0, Math.min(255, parseInt(val) || 0));
      document.getElementById('ctrl-a-slider').value = val;
      document.getElementById('ctrl-a-num').value = val;
      if (isOverride) sendCurrentInputs();
    }

    function syncControlB(val) {
      val = Math.max(0, Math.min(255, parseInt(val) || 0));
      document.getElementById('ctrl-b-slider').value = val;
      document.getElementById('ctrl-b-num').value = val;
      if (isOverride) sendCurrentInputs();
    }

    function toggleCtrlMode() {
      ctrlMode = (ctrlMode === 0) ? 1 : 0;
      document.getElementById('ctrl-mode-btn').textContent = ctrlMode ? "Mode 1 (Verify)" : "Mode 0 (8-Bit)";
      if (isOverride) sendCurrentInputs();
    }

    function applyPreset(a, b, op, mode) {
      document.getElementById('override-toggle').checked = true;
      isOverride = true;
      document.getElementById('ctrl-a-slider').value = a;
      document.getElementById('ctrl-a-num').value = a;
      document.getElementById('ctrl-b-slider').value = b;
      document.getElementById('ctrl-b-num').value = b;
      document.getElementById('ctrl-op').value = op;
      ctrlMode = mode;
      document.getElementById('ctrl-mode-btn').textContent = ctrlMode ? "Mode 1 (Verify)" : "Mode 0 (8-Bit)";
      sendCurrentInputs();
    }

    // Toggle Override Switch
    function toggleOverride() {
      isOverride = document.getElementById('override-toggle').checked;
      if (isOverride) {
        sendCurrentInputs();
      } else {
        if (ws && ws.readyState === WebSocket.OPEN) {
          ws.send(JSON.stringify({ cmd: "release_override" }));
        }
      }
    }

    // Send Input Command
    let sendDebounce = null;
    function sendCurrentInputs() {
      if (!isOverride) return;
      clearTimeout(sendDebounce);
      sendDebounce = setTimeout(() => {
        const payload = {
          cmd: "set_input",
          a: parseInt(document.getElementById('ctrl-a-num').value) || 0,
          b: parseInt(document.getElementById('ctrl-b-num').value) || 0,
          opcode: parseInt(document.getElementById('ctrl-op').value) || 0,
          mode: ctrlMode
        };
        if (ws && ws.readyState === WebSocket.OPEN) {
          ws.send(JSON.stringify(payload));
        }
      }, 30);
    }

    // History Table Management
    function addHistoryEntry(data) {
      const now = new Date();
      const timeStr = now.toTimeString().split(' ')[0] + '.' + String(now.getMilliseconds()).padStart(3, '0');
      const customNib = data.y & 0x0F;
      const ls181Nib = (data.y >> 4) & 0x0F;

      const entry = {
        time: timeStr,
        mode: data.mode === 0 ? "Mode 0" : "Mode 1",
        op: OP_NAMES[data.opcode] || "UNK",
        a: '0x' + (data.a & 0xFF).toString(16).toUpperCase().padStart(2, '0'),
        b: '0x' + (data.b & 0xFF).toString(16).toUpperCase().padStart(2, '0'),
        customNib: '0x' + customNib.toString(16).toUpperCase(),
        ls181Nib: '0x' + ls181Nib.toString(16).toUpperCase(),
        fullY: '0x' + (data.y & 0xFF).toString(16).toUpperCase().padStart(2, '0'),
        hw7485: data.mode === 1 ? (data.hw_7485_pass ? "MATCH" : "MISMATCH") : "N/A",
        goldenExp: '0x' + (data.expected & 0xFF).toString(16).toUpperCase().padStart(2, '0'),
        pass: data.pass
      };

      historyList.unshift(entry);
      if (historyList.length > MAX_HISTORY) historyList.pop();

      renderHistory();
    }

    function renderHistory() {
      const tbody = document.getElementById('history-body');
      if (historyList.length === 0) {
        tbody.innerHTML = '<tr><td colspan="11" style="text-align: center; color: var(--muted); padding: 16px;">Waiting for telemetry...</td></tr>';
        return;
      }

      let html = '';
      for (const item of historyList) {
        const statusBadge = item.pass
          ? '<span class="badge-pass">PASS</span>'
          : '<span class="badge-fail">FAIL</span>';

        const hwBadge = item.hw7485 === 'MATCH'
          ? '<span class="badge-silicon">MATCH</span>'
          : (item.hw7485 === 'MISMATCH' ? '<span class="badge-fail">MISMATCH</span>' : '<span style="color:var(--muted);">N/A</span>');

        html += `<tr>
          <td style="color: var(--muted);">${item.time}</td>
          <td><span style="color: ${item.mode === 'Mode 0' ? 'var(--accent)' : 'var(--purple)'}; font-weight:700;">${item.mode}</span></td>
          <td><span class="badge-op">${item.op}</span></td>
          <td>${item.a}</td>
          <td>${item.b}</td>
          <td style="color: var(--accent); font-weight: bold;">${item.customNib}</td>
          <td style="color: var(--purple); font-weight: bold;">${item.ls181Nib}</td>
          <td style="color: var(--green); font-weight: bold;">${item.fullY}</td>
          <td>${hwBadge}</td>
          <td style="color: var(--muted);">${item.goldenExp}</td>
          <td>${statusBadge}</td>
        </tr>`;
      }
      tbody.innerHTML = html;
    }

    function clearHistory() {
      historyList.length = 0;
      renderHistory();
    }

    // Fallback Polling if WebSocket drops
    setInterval(() => {
      if (!ws || ws.readyState !== WebSocket.OPEN) {
        fetch('/api/telemetry')
          .then(r => r.json())
          .then(data => applyTelemetry(data))
          .catch(() => {});
      }
    }, 300);

    // Boot
    window.addEventListener('DOMContentLoaded', () => {
      connectWs();
    });
  </script>
</body>
</html>
)rawliteral";
