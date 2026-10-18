#pragma once
#include <Arduino.h>

// ============================================================================
// DECA Minor Project: Discrete 74HCT Gate-Level ALU Testbench Web Dashboard
// Single-page application embedded in ESP32 Flash (PROGMEM)
// ============================================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DECA 8-Bit/4-Bit Discrete Gate-Level ALU</title>
  <style>
    :root {
      --bg: #090d16;
      --card-bg: rgba(19, 27, 46, 0.85);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #38bdf8;
      --accent-glow: rgba(56, 189, 248, 0.35);
      --green: #10b981;
      --green-glow: rgba(16, 185, 129, 0.4);
      --amber: #f59e0b;
      --amber-glow: rgba(245, 158, 11, 0.35);
      --red: #ef4444;
      --red-glow: rgba(239, 68, 68, 0.4);
      --led-off: #1e293b;
      --led-off-border: #334155;
      --text: #f1f5f9;
      --muted: #94a3b8;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, monospace; }
    body { background: var(--bg); color: var(--text); padding: 18px; min-height: 100vh; display: flex; flex-direction: column; align-items: center; }
    .container { max-width: 1080px; width: 100%; display: flex; flex-direction: column; gap: 18px; }

    header { display: flex; justify-content: space-between; align-items: center; flex-wrap: wrap; gap: 12px; padding-bottom: 12px; border-bottom: 1px solid var(--card-border); }
    .header-text h1 { font-size: 1.55rem; color: #fff; letter-spacing: 0.5px; }
    .header-text p { font-size: 0.85rem; color: var(--muted); margin-top: 3px; }
    .conn-badge { display: inline-flex; align-items: center; gap: 8px; padding: 6px 14px; border-radius: 999px; font-size: 0.78rem; font-weight: 600; background: rgba(16, 185, 129, 0.12); color: var(--green); border: 1px solid rgba(16, 185, 129, 0.3); }
    .conn-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--green); box-shadow: 0 0 10px var(--green); }

    .status-banner {
      background: linear-gradient(135deg, rgba(56, 189, 248, 0.12), rgba(16, 185, 129, 0.08));
      border: 1px solid rgba(56, 189, 248, 0.25);
      border-radius: 12px;
      padding: 14px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 12px;
    }
    .status-left { display: flex; flex-direction: column; gap: 4px; }
    .status-title { font-size: 1rem; font-weight: 700; color: var(--accent); }
    .status-desc { font-size: 0.82rem; color: var(--muted); }
    .status-pill { padding: 6px 14px; border-radius: 20px; font-size: 0.82rem; font-weight: 700; background: rgba(56, 189, 248, 0.18); color: var(--accent); border: 1px solid var(--accent); }

    .grid-2 { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 16px; }
    .card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: 14px; padding: 18px; backdrop-filter: blur(12px); box-shadow: 0 8px 24px rgba(0,0,0,0.3); }
    .card h2 { font-size: 0.92rem; color: var(--muted); text-transform: uppercase; letter-spacing: 1px; margin-bottom: 12px; display: flex; justify-content: space-between; align-items: center; }

    /* LED Bit Visualizer */
    .led-bus { display: flex; gap: 6px; justify-content: center; align-items: center; margin: 12px 0; }
    .nibble-divider { width: 2px; height: 38px; background: rgba(255, 255, 255, 0.15); margin: 0 4px; }
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
    .led.active-amber { background: #f59e0b; border-color: #fde68a; box-shadow: 0 0 14px #f59e0b, inset 0 -2px 4px rgba(255,255,255,0.7); }

    .readout-strip {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      background: rgba(0, 0, 0, 0.4);
      border-radius: 8px;
      padding: 8px 12px;
      margin-top: 10px;
      font-family: monospace;
      text-align: center;
    }
    .readout-label { font-size: 0.65rem; color: var(--muted); text-transform: uppercase; margin-bottom: 2px; }
    .readout-val { font-size: 1.05rem; font-weight: 700; color: #fff; }

    /* Control Panel */
    .ctrl-section { display: flex; flex-direction: column; gap: 14px; }
    .override-switch-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 12px 16px;
      background: rgba(0, 0, 0, 0.35);
      border-radius: 10px;
      border: 1px solid rgba(255, 255, 255, 0.08);
    }
    .switch-label { font-weight: 600; font-size: 0.95rem; display: flex; flex-direction: column; gap: 2px; }
    .switch-subtext { font-size: 0.75rem; color: var(--muted); font-weight: normal; }

    /* Toggle Switch */
    .switch { position: relative; display: inline-block; width: 52px; height: 28px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #334155; transition: .25s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .25s; border-radius: 50%; }
    input:checked + .slider { background-color: var(--accent); }
    input:checked + .slider:before { transform: translateX(24px); }

    .ctrl-row { display: grid; grid-template-columns: 80px 1fr 70px; align-items: center; gap: 12px; }
    .ctrl-row label { font-size: 0.85rem; font-weight: 600; color: var(--muted); }
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
    .ctrl-row select { cursor: pointer; }

    .btn-group { display: flex; gap: 10px; margin-top: 6px; }
    .btn {
      flex: 1;
      background: var(--accent);
      color: #090d16;
      border: none;
      padding: 10px 16px;
      border-radius: 8px;
      font-size: 0.88rem;
      font-weight: 700;
      cursor: pointer;
      transition: all 0.2s;
    }
    .btn:hover { filter: brightness(1.15); box-shadow: 0 0 14px var(--accent-glow); }
    .btn-secondary { background: rgba(255, 255, 255, 0.08); color: #fff; border: 1px solid var(--card-border); }
    .btn-secondary:hover { background: rgba(255, 255, 255, 0.15); box-shadow: none; }

    /* History Table */
    .history-card { overflow-x: auto; }
    .history-table { width: 100%; border-collapse: collapse; font-family: monospace; font-size: 0.82rem; }
    .history-table th { text-align: left; padding: 8px 12px; color: var(--muted); border-bottom: 1px solid var(--card-border); font-size: 0.72rem; text-transform: uppercase; }
    .history-table td { padding: 7px 12px; border-bottom: 1px solid rgba(255, 255, 255, 0.04); }
    .history-table tr:hover { background: rgba(255, 255, 255, 0.03); }
    .badge-op { display: inline-block; padding: 2px 8px; border-radius: 4px; font-weight: 700; background: rgba(56, 189, 248, 0.15); color: var(--accent); }

    footer { text-align: center; font-size: 0.75rem; color: var(--muted); margin-top: 10px; }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <div class="header-text">
        <h1>Discrete 74HCT Gate-Level ALU Testbench</h1>
        <p>DECA Minor Project · 8-Bit/4-Bit Reconfigurable Datapath · Hardware Verifier: Arduino UNO R4</p>
      </div>
      <div class="conn-badge" id="conn-status">
        <span class="conn-dot" id="conn-dot"></span>
        <span id="conn-text">WebSocket: Connecting...</span>
      </div>
    </header>

    <!-- Operational Mode Banner -->
    <div class="status-banner">
      <div class="status-left">
        <div class="status-title" id="mode-title">MODE: 8-BIT CASCADED ALU</div>
        <div class="status-desc" id="mode-desc">Gate-level ripple-carry adder carry chain active between nibbles (74HCT08 Carry Gate enabled)</div>
      </div>
      <div class="status-pill" id="input-source-pill">INPUT: PHYSICAL DIP SWITCHES</div>
    </div>

    <!-- Output Bus Hero Card -->
    <div class="card" style="border-color: rgba(16, 185, 129, 0.35); background: linear-gradient(180deg, rgba(16, 185, 129, 0.06), var(--card-bg));">
      <h2>
        <span>Hardware ALU Output Bus (Y7 .. Y0)</span>
        <span style="color: var(--green); font-family: monospace; font-size: 1.1rem;" id="out-hex">0x00</span>
      </h2>
      <div class="led-bus">
        <!-- Upper Nibble (Y7..Y4) -->
        <div class="led-cell"><div class="led" id="y7"></div><span>Y7</span></div>
        <div class="led-cell"><div class="led" id="y6"></div><span>Y6</span></div>
        <div class="led-cell"><div class="led" id="y5"></div><span>Y5</span></div>
        <div class="led-cell"><div class="led" id="y4"></div><span>Y4</span></div>
        <div class="nibble-divider"></div>
        <!-- Lower Nibble (Y3..Y0) -->
        <div class="led-cell"><div class="led" id="y3"></div><span>Y3</span></div>
        <div class="led-cell"><div class="led" id="y2"></div><span>Y2</span></div>
        <div class="led-cell"><div class="led" id="y1"></div><span>Y1</span></div>
        <div class="led-cell"><div class="led" id="y0"></div><span>Y0</span></div>
      </div>
      <div class="readout-strip">
        <div><div class="readout-label">Binary</div><div class="readout-val" id="out-bin">0000 0000</div></div>
        <div><div class="readout-label">Decimal</div><div class="readout-val" id="out-dec">0</div></div>
        <div><div class="readout-label">Hexadecimal</div><div class="readout-val" id="out-hex-full">0x00</div></div>
      </div>
    </div>

    <!-- Input Operands & Opcode Status Grid -->
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
          <div><div class="readout-label">Hex</div><div class="readout-val" id="disp-a-hex2">0x00</div></div>
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
          <div><div class="readout-label">Hex</div><div class="readout-val" id="disp-b-hex2">0x00</div></div>
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
        <div class="readout-strip">
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

          <!-- Opcode Selector -->
          <div class="ctrl-row">
            <label for="ctrl-op">Opcode:</label>
            <select id="ctrl-op" onchange="sendCurrentInputs()">
              <option value="0">000: ADD (A + B Gate-Level Adder)</option>
              <option value="1">001: SUB (A - B 2's Complement)</option>
              <option value="2">010: AND (A & B via 74HCT08)</option>
              <option value="3">011: OR  (A | B via 74HCT32)</option>
              <option value="4">100: XOR (A ^ B via 74HCT86)</option>
              <option value="5">101: NOT (~A via 74HCT04)</option>
              <option value="6">110: SHL (A << 1 Left Shift)</option>
              <option value="7">111: SHR (A >> 1 Right Shift)</option>
            </select>
            <button class="btn btn-secondary" style="padding: 6px;" onclick="toggleCtrlMode()" id="ctrl-mode-btn">8-Bit</button>
          </div>
        </div>
      </div>
    </div>

    <!-- Operation History Card -->
    <div class="card history-card">
      <h2>
        <span>Operation History Telemetry Log</span>
        <button class="btn btn-secondary" style="padding: 3px 10px; font-size: 0.72rem;" onclick="clearHistory()">Clear</button>
      </h2>
      <table class="history-table">
        <thead>
          <tr>
            <th>Time</th>
            <th>Mode</th>
            <th>Opcode</th>
            <th>Operand A</th>
            <th>Operand B</th>
            <th>ALU Output (Y)</th>
            <th>Source</th>
          </tr>
        </thead>
        <tbody id="history-body">
          <tr><td colspan="7" style="text-align: center; color: var(--muted); padding: 16px;">Waiting for telemetry...</td></tr>
        </tbody>
      </table>
    </div>

    <footer>
      Digital Electronics & Computer Architecture (DECA) Minor Project · Discrete 74HCT Logic Gate ALU Architecture
    </footer>
  </div>

  <script>
    const OP_NAMES = ["ADD", "SUB", "AND", "OR", "XOR", "NOT", "SHL", "SHR"];
    const OP_DESCS = ["A + B", "A - B (2's C)", "A & B", "A | B", "A ^ B", "~A", "A << 1", "A >> 1"];

    let ws = null;
    let isOverride = false;
    let ctrlMode = 1; // 1 = 8-bit, 0 = 4-bit
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

    // Format binary with spaces per nibble
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
      // 1. Output Bus
      updateLeds('y', data.y, 8, 'active-green');
      const hexY = '0x' + (data.y & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('out-hex').textContent = hexY;
      document.getElementById('out-hex-full').textContent = hexY;
      document.getElementById('out-dec').textContent = data.y;
      document.getElementById('out-bin').textContent = formatBin8(data.y);

      // 2. Operand A
      updateLeds('a', data.a, 8, 'active-blue');
      const hexA = '0x' + (data.a & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('disp-a-hex').textContent = hexA;
      document.getElementById('disp-a-hex2').textContent = hexA;
      document.getElementById('disp-a-dec').textContent = data.a;
      document.getElementById('disp-a-bin').textContent = formatBin8(data.a);

      // 3. Operand B
      updateLeds('b', data.b, 8, 'active-blue');
      const hexB = '0x' + (data.b & 0xFF).toString(16).toUpperCase().padStart(2, '0');
      document.getElementById('disp-b-hex').textContent = hexB;
      document.getElementById('disp-b-hex2').textContent = hexB;
      document.getElementById('disp-b-dec').textContent = data.b;
      document.getElementById('disp-b-bin').textContent = formatBin8(data.b);

      // 4. Opcode
      updateLeds('s', data.opcode, 3, 'active-amber');
      const opName = OP_NAMES[data.opcode] || "UNKNOWN";
      document.getElementById('disp-op-name').textContent = opName;
      document.getElementById('disp-op-bin').textContent = (data.opcode & 7).toString(2).padStart(3, '0');
      document.getElementById('disp-op-desc').textContent = OP_DESCS[data.opcode] || "";
      document.getElementById('disp-op-val').textContent = data.opcode;

      // 5. Mode Banner
      if (data.mode === 1) {
        document.getElementById('mode-title').textContent = "MODE: 8-BIT CASCADED ALU";
        document.getElementById('mode-desc').textContent = "Gate-level ripple-carry adder carry chain active between nibbles (74HCT08 Carry Gate enabled)";
      } else {
        document.getElementById('mode-title').textContent = "MODE: 4-BIT DUAL INDEPENDENT MODE";
        document.getElementById('mode-desc').textContent = "Carry chain gated OFF; lower and upper nibble operate as independent 4-bit ALUs";
      }

      // 6. Override source pill
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
        document.getElementById('ctrl-mode-btn').textContent = ctrlMode ? "8-Bit" : "4-Bit";
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
      ctrlMode = (ctrlMode === 1) ? 0 : 1;
      document.getElementById('ctrl-mode-btn').textContent = ctrlMode ? "8-Bit" : "4-Bit";
      if (isOverride) sendCurrentInputs();
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
      const entry = {
        time: timeStr,
        mode: data.mode ? "8-Bit" : "4-Bit",
        op: OP_NAMES[data.opcode] || "UNK",
        a: '0x' + (data.a & 0xFF).toString(16).toUpperCase().padStart(2, '0') + ` (${data.a})`,
        b: '0x' + (data.b & 0xFF).toString(16).toUpperCase().padStart(2, '0') + ` (${data.b})`,
        y: '0x' + (data.y & 0xFF).toString(16).toUpperCase().padStart(2, '0') + ` (${data.y})`,
        source: data.override ? "Remote Override" : "Physical DIP"
      };

      historyList.unshift(entry);
      if (historyList.length > MAX_HISTORY) historyList.pop();

      renderHistory();
    }

    function renderHistory() {
      const tbody = document.getElementById('history-body');
      if (historyList.length === 0) {
        tbody.innerHTML = '<tr><td colspan="7" style="text-align: center; color: var(--muted); padding: 16px;">Waiting for telemetry...</td></tr>';
        return;
      }

      let html = '';
      for (const item of historyList) {
        html += `<tr>
          <td style="color: var(--muted);">${item.time}</td>
          <td><span style="color: ${item.mode === '8-Bit' ? 'var(--accent)' : 'var(--amber)'};">${item.mode}</span></td>
          <td><span class="badge-op">${item.op}</span></td>
          <td>${item.a}</td>
          <td>${item.b}</td>
          <td style="color: var(--green); font-weight: bold;">${item.y}</td>
          <td style="color: var(--muted);">${item.source}</td>
        </tr>`;
      }
      tbody.innerHTML = html;
    }

    function clearHistory() {
      historyList.length = 0;
      renderHistory();
    }

    // Fallback Polling if WebSocket fails
    setInterval(() => {
      if (!ws || ws.readyState !== WebSocket.OPEN) {
        fetch('/api/telemetry')
          .then(r => r.json())
          .then(data => applyTelemetry(data))
          .catch(() => {});
      }
    }, 250);

    // Boot
    window.addEventListener('DOMContentLoaded', () => {
      connectWs();
    });
  </script>
</body>
</html>
)rawliteral";
