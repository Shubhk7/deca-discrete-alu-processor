#pragma once
#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>DECA Hybrid 8-Bit Cascaded ALU Dashboard</title>
  <style>
    :root {
      --bg: #0d1117;
      --card-bg: #161b22;
      --border: #30363d;
      --accent-blue: #58a6ff;
      --accent-green: #3fb950;
      --accent-orange: #f0883e;
      --accent-purple: #bc8cff;
      --accent-red: #f85149;
      --text: #c9d1d9;
      --text-muted: #8b949e;
      --font-mono: 'SFMono-Regular', Consolas, 'Liberation Mono', Menlo, monospace;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background-color: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
      padding: 16px;
      line-height: 1.5;
    }
    .container { max-width: 1040px; margin: 0 auto; }
    header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-bottom: 16px;
      border-bottom: 1px solid var(--border);
      margin-bottom: 20px;
      flex-wrap: wrap;
      gap: 12px;
    }
    h1 { font-size: 1.4rem; color: #fff; font-weight: 700; }
    .badge {
      display: inline-flex;
      align-items: center;
      padding: 4px 10px;
      border-radius: 20px;
      font-size: 0.8rem;
      font-weight: 600;
      background: rgba(88, 166, 255, 0.15);
      color: var(--accent-blue);
      border: 1px solid rgba(88, 166, 255, 0.3);
    }
    .badge.connected {
      background: rgba(63, 185, 80, 0.15);
      color: var(--accent-green);
      border-color: rgba(63, 185, 80, 0.3);
    }
    .badge.override {
      background: rgba(240, 136, 62, 0.15);
      color: var(--accent-orange);
      border-color: rgba(240, 136, 62, 0.3);
    }
    .architecture-banner {
      background: linear-gradient(135deg, rgba(88, 166, 255, 0.1) 0%, rgba(188, 140, 255, 0.1) 100%);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 14px 18px;
      margin-bottom: 20px;
      display: flex;
      justify-content: space-around;
      text-align: center;
      flex-wrap: wrap;
      gap: 12px;
    }
    .arch-step { flex: 1; min-width: 180px; }
    .arch-title { font-size: 0.75rem; text-transform: uppercase; color: var(--text-muted); letter-spacing: 0.5px; }
    .arch-val { font-size: 1.1rem; font-weight: bold; font-family: var(--font-mono); margin-top: 4px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 16px; margin-bottom: 20px; }
    .card {
      background-color: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 16px;
    }
    .card-title {
      font-size: 0.95rem;
      font-weight: 600;
      margin-bottom: 14px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      color: #fff;
    }
    .bit-row {
      display: flex;
      gap: 6px;
      margin: 12px 0;
      justify-content: center;
    }
    .bit-chip {
      display: flex;
      flex-direction: column;
      align-items: center;
      width: 32px;
    }
    .bit-dot {
      width: 26px;
      height: 26px;
      border-radius: 6px;
      background: #21262d;
      border: 1px solid #30363d;
      display: flex;
      align-items: center;
      justify-content: center;
      font-family: var(--font-mono);
      font-size: 0.8rem;
      font-weight: bold;
      color: var(--text-muted);
      transition: all 0.15s ease;
    }
    .bit-dot.on {
      background: var(--accent-blue);
      border-color: #79c0ff;
      color: #040d21;
      box-shadow: 0 0 10px rgba(88, 166, 255, 0.6);
    }
    .bit-dot.on-orange {
      background: var(--accent-orange);
      border-color: #ffa657;
      color: #210d04;
      box-shadow: 0 0 10px rgba(240, 136, 62, 0.6);
    }
    .bit-dot.on-purple {
      background: var(--accent-purple);
      border-color: #d2a8ff;
      color: #1a0421;
      box-shadow: 0 0 10px rgba(188, 140, 255, 0.6);
    }
    .bit-lbl { font-size: 0.65rem; color: var(--text-muted); margin-top: 4px; font-family: var(--font-mono); }
    .val-display {
      font-family: var(--font-mono);
      font-size: 1.3rem;
      text-align: center;
      margin-top: 8px;
      color: #fff;
      font-weight: 700;
    }
    .controls { display: flex; flex-direction: column; gap: 12px; }
    .control-group { display: flex; flex-direction: column; gap: 6px; }
    label { font-size: 0.85rem; color: var(--text-muted); display: flex; justify-content: space-between; }
    input[type="range"] {
      width: 100%;
      accent-color: var(--accent-blue);
      height: 6px;
      background: #21262d;
      border-radius: 3px;
    }
    select, button, input[type="number"] {
      background: #21262d;
      border: 1px solid var(--border);
      color: #fff;
      padding: 8px 12px;
      border-radius: 6px;
      font-size: 0.9rem;
      font-family: inherit;
    }
    button {
      cursor: pointer;
      font-weight: 600;
      background: #238636;
      border-color: rgba(240, 246, 252, 0.1);
      transition: all 0.2s;
    }
    button:hover { background: #2ea043; }
    button.btn-override-active {
      background: #da3633;
    }
    button.btn-override-active:hover { background: #f85149; }
    .btn-secondary {
      background: #21262d;
      color: var(--text);
    }
    .btn-secondary:hover { background: #30363d; }
    .quick-vectors {
      display: flex;
      flex-wrap: wrap;
      gap: 6px;
      margin-top: 8px;
    }
    .quick-vectors button {
      padding: 4px 8px;
      font-size: 0.75rem;
      background: #30363d;
    }
    .status-box {
      background: #0d1117;
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 10px;
      font-family: var(--font-mono);
      font-size: 0.8rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <div>
        <h1>DECA Dual 4-Bit Cascaded 8-Bit ALU</h1>
        <div style="font-size: 0.8rem; color: var(--text-muted); margin-top: 2px;">
          74LS181N Bipolar Silicon ALU × Arduino UNO R4 WiFi ARM MCU Cascade
        </div>
      </div>
      <div style="display: flex; gap: 8px;">
        <span id="ws-badge" class="badge">WebSocket Disconnected</span>
        <span id="mode-badge" class="badge">DIP Switch Mode</span>
      </div>
    </header>

    <div class="architecture-banner">
      <div class="arch-step">
        <div class="arch-title">Nibble 0 (Bits 3..0)</div>
        <div class="arch-val" style="color: var(--accent-orange);">74LS181N Silicon</div>
        <div style="font-size: 0.7rem; color: var(--text-muted);">~15ns Pure Bipolar TTL</div>
      </div>
      <div class="arch-step" style="border-left: 1px solid var(--border); border-right: 1px solid var(--border);">
        <div class="arch-title">Hardware Carry Cascade</div>
        <div class="arch-val" id="cascade-c4" style="color: var(--accent-purple);">C4 = 0</div>
        <div style="font-size: 0.7rem; color: var(--text-muted);">Pin 16 (/Cn+4) &rarr; Pin D6</div>
      </div>
      <div class="arch-step">
        <div class="arch-title">Nibble 1 (Bits 7..4)</div>
        <div class="arch-val" style="color: var(--accent-blue);">Arduino UNO R4 WiFi</div>
        <div style="font-size: 0.7rem; color: var(--text-muted);">32-Bit ARM Cortex-M4</div>
      </div>
      <div class="arch-step" style="border-left: 1px solid var(--border);">
        <div class="arch-title">Visual HUD</div>
        <div class="arch-val" style="color: var(--accent-green);">12×8 LED Matrix</div>
        <div style="font-size: 0.7rem; color: var(--text-muted);">No Discrete LEDs Needed</div>
      </div>
    </div>

    <div class="grid">
      <!-- 8-BIT OUTPUT BUS Y[7:0] -->
      <div class="card" style="grid-column: 1 / -1;">
        <div class="card-title">
          <span>8-Bit Hybrid Output Bus (Y[7:0])</span>
          <span id="pass-indicator" class="badge" style="background: rgba(63,185,80,0.15); color: var(--accent-green);">100% MATCH</span>
        </div>
        <div class="bit-row">
          <!-- Upper Nibble (Arduino R4 WiFi) -->
          <div class="bit-chip"><div id="y7" class="bit-dot">0</div><span class="bit-lbl">Y7</span></div>
          <div class="bit-chip"><div id="y6" class="bit-dot">0</div><span class="bit-lbl">Y6</span></div>
          <div class="bit-chip"><div id="y5" class="bit-dot">0</div><span class="bit-lbl">Y5</span></div>
          <div class="bit-chip"><div id="y4" class="bit-dot">0</div><span class="bit-lbl">Y4</span></div>
          <div style="width: 14px; border-left: 2px dashed #484f58; margin: 0 4px;"></div>
          <!-- Lower Nibble (74LS181N Silicon) -->
          <div class="bit-chip"><div id="y3" class="bit-dot on-orange">0</div><span class="bit-lbl">Y3</span></div>
          <div class="bit-chip"><div id="y2" class="bit-dot on-orange">0</div><span class="bit-lbl">Y2</span></div>
          <div class="bit-chip"><div id="y1" class="bit-dot on-orange">0</div><span class="bit-lbl">Y1</span></div>
          <div class="bit-chip"><div id="y0" class="bit-dot on-orange">0</div><span class="bit-lbl">Y0</span></div>
        </div>
        <div class="val-display" id="y-hex-val">0x00 (0)</div>
      </div>

      <!-- INPUT MONITOR / INJECTOR: OPERAND A -->
      <div class="card">
        <div class="card-title">
          <span>Operand A [7:0]</span>
          <span id="a-val-badge" class="badge">0x00</span>
        </div>
        <div class="bit-row" id="a-bits">
          <div class="bit-chip"><div id="a7" class="bit-dot">0</div><span class="bit-lbl">A7</span></div>
          <div class="bit-chip"><div id="a6" class="bit-dot">0</div><span class="bit-lbl">A6</span></div>
          <div class="bit-chip"><div id="a5" class="bit-dot">0</div><span class="bit-lbl">A5</span></div>
          <div class="bit-chip"><div id="a4" class="bit-dot">0</div><span class="bit-lbl">A4</span></div>
          <div class="bit-chip"><div id="a3" class="bit-dot">0</div><span class="bit-lbl">A3</span></div>
          <div class="bit-chip"><div id="a2" class="bit-dot">0</div><span class="bit-lbl">A2</span></div>
          <div class="bit-chip"><div id="a1" class="bit-dot">0</div><span class="bit-lbl">A1</span></div>
          <div class="bit-chip"><div id="a0" class="bit-dot">0</div><span class="bit-lbl">A0</span></div>
        </div>
        <div class="control-group">
          <label>Remote Override Slider <span id="a-slider-val">0</span></label>
          <input type="range" id="a-slider" min="0" max="255" value="0" disabled>
        </div>
      </div>

      <!-- INPUT MONITOR / INJECTOR: OPERAND B -->
      <div class="card">
        <div class="card-title">
          <span>Operand B [7:0]</span>
          <span id="b-val-badge" class="badge">0x00</span>
        </div>
        <div class="bit-row" id="b-bits">
          <div class="bit-chip"><div id="b7" class="bit-dot">0</div><span class="bit-lbl">B7</span></div>
          <div class="bit-chip"><div id="b6" class="bit-dot">0</div><span class="bit-lbl">B6</span></div>
          <div class="bit-chip"><div id="b5" class="bit-dot">0</div><span class="bit-lbl">B5</span></div>
          <div class="bit-chip"><div id="b4" class="bit-dot">0</div><span class="bit-lbl">B4</span></div>
          <div class="bit-chip"><div id="b3" class="bit-dot">0</div><span class="bit-lbl">B3</span></div>
          <div class="bit-chip"><div id="b2" class="bit-dot">0</div><span class="bit-lbl">B2</span></div>
          <div class="bit-chip"><div id="b1" class="bit-dot">0</div><span class="bit-lbl">B1</span></div>
          <div class="bit-chip"><div id="b0" class="bit-dot">0</div><span class="bit-lbl">B0</span></div>
        </div>
        <div class="control-group">
          <label>Remote Override Slider <span id="b-slider-val">0</span></label>
          <input type="range" id="b-slider" min="0" max="255" value="0" disabled>
        </div>
      </div>

      <!-- OPCODE & REMOTE INJECTION CONTROLS -->
      <div class="card" style="grid-column: 1 / -1;">
        <div class="card-title">
          <span>Remote Override Controls (via 19x 4.7k&Omega; Override Resistors)</span>
          <button id="btn-toggle-override" onclick="toggleOverride()">Enable Remote Override</button>
        </div>
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 12px; margin-top: 10px;">
          <div class="control-group">
            <label>ALU Operation</label>
            <select id="op-select" onchange="sendOverride()" disabled>
              <option value="0">000: ADD (A + B)</option>
              <option value="1">001: SUB (A - B)</option>
              <option value="2">010: AND (A & B)</option>
              <option value="3">011: OR  (A | B)</option>
              <option value="4">100: XOR (A ^ B)</option>
              <option value="5">101: NOT (~A)</option>
              <option value="6">110: SHL (A << 1)</option>
              <option value="7">111: SHR (A >> 1)</option>
            </select>
          </div>
          <div class="control-group">
            <label>Mode Select</label>
            <select id="mode-select" onchange="sendOverride()" disabled>
              <option value="0">Arithmetic Mode (M=0)</option>
              <option value="1">Logic Mode (M=1)</option>
            </select>
          </div>
        </div>

        <div class="quick-vectors">
          <span style="font-size: 0.75rem; color: var(--text-muted); align-self: center; margin-right: 4px;">Quick Test:</span>
          <button class="btn-secondary" onclick="setQuick(15, 1, 0, 0)">15 + 1</button>
          <button class="btn-secondary" onclick="setQuick(127, 1, 0, 0)">127 + 1 (Nibble Carry)</button>
          <button class="btn-secondary" onclick="setQuick(255, 1, 0, 0)">255 + 1 (Full 8-Bit Overflow)</button>
          <button class="btn-secondary" onclick="setQuick(85, 170, 4, 1)">0x55 ^ 0xAA (XOR)</button>
          <button class="btn-secondary" onclick="setQuick(50, 20, 1, 0)">50 - 20 (SUB)</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    let ws;
    let overrideActive = false;

    function initWebSocket() {
      const wsUrl = 'ws://' + window.location.hostname + ':81/';
      ws = new WebSocket(wsUrl);

      ws.onopen = function() {
        document.getElementById('ws-badge').textContent = 'WebSocket Active (100ms)';
        document.getElementById('ws-badge').classList.add('connected');
      };

      ws.onclose = function() {
        document.getElementById('ws-badge').textContent = 'Disconnected (Retrying...)';
        document.getElementById('ws-badge').classList.remove('connected');
        setTimeout(initWebSocket, 2000);
      };

      ws.onmessage = function(event) {
        try {
          const d = JSON.parse(event.data);
          updateTelemetry(d);
        } catch(e) {}
      };
    }

    function updateBits(prefix, val) {
      for (let i = 0; i < 8; i++) {
        const bit = (val >> i) & 1;
        const el = document.getElementById(prefix + i);
        if (el) {
          if (bit) {
            el.classList.add('on');
            el.textContent = '1';
          } else {
            el.classList.remove('on');
            el.textContent = '0';
          }
        }
      }
    }

    function updateTelemetry(d) {
      // Inputs
      updateBits('a', d.a);
      updateBits('b', d.b);
      document.getElementById('a-val-badge').textContent = '0x' + d.a.toString(16).toUpperCase().padStart(2, '0') + ' (' + d.a + ')';
      document.getElementById('b-val-badge').textContent = '0x' + d.b.toString(16).toUpperCase().padStart(2, '0') + ' (' + d.b + ')';

      // Output Bus
      updateBits('y', d.y);
      document.getElementById('y-hex-val').textContent = '0x' + d.y.toString(16).toUpperCase().padStart(2, '0') + ' (' + d.y + ')';

      // Cascade Carry
      document.getElementById('cascade-c4').textContent = 'C4 = ' + (d.c4 ? '1 (Active)' : '0');

      // Sliders if not actively overriding
      if (!overrideActive) {
        document.getElementById('a-slider').value = d.a;
        document.getElementById('b-slider').value = d.b;
        document.getElementById('a-slider-val').textContent = d.a;
        document.getElementById('b-slider-val').textContent = d.b;
        document.getElementById('op-select').value = d.op;
        document.getElementById('mode-select').value = d.m;
      }
    }

    function toggleOverride() {
      overrideActive = !overrideActive;
      const btn = document.getElementById('btn-toggle-override');
      const badge = document.getElementById('mode-badge');
      const inputs = ['a-slider', 'b-slider', 'op-select', 'mode-select'];

      if (overrideActive) {
        btn.textContent = 'Release Override (Back to DIP)';
        btn.classList.add('btn-override-active');
        badge.textContent = 'Remote WebSocket Mode';
        badge.classList.add('override');
        inputs.forEach(id => document.getElementById(id).disabled = false);
        sendOverride();
      } else {
        btn.textContent = 'Enable Remote Override';
        btn.classList.remove('btn-override-active');
        badge.textContent = 'DIP Switch Mode';
        badge.classList.remove('override');
        inputs.forEach(id => document.getElementById(id).disabled = true);
        ws.send(JSON.stringify({ cmd: 'release_override' }));
      }
    }

    function sendOverride() {
      if (!overrideActive || !ws || ws.readyState !== WebSocket.OPEN) return;
      const a = parseInt(document.getElementById('a-slider').value);
      const b = parseInt(document.getElementById('b-slider').value);
      const op = parseInt(document.getElementById('op-select').value);
      const m = parseInt(document.getElementById('mode-select').value);

      document.getElementById('a-slider-val').textContent = a;
      document.getElementById('b-slider-val').textContent = b;

      ws.send(JSON.stringify({
        cmd: 'set_input',
        a: a,
        b: b,
        op: op,
        m: m
      }));
    }

    document.getElementById('a-slider').oninput = sendOverride;
    document.getElementById('b-slider').oninput = sendOverride;

    function setQuick(a, b, op, m) {
      if (!overrideActive) toggleOverride();
      document.getElementById('a-slider').value = a;
      document.getElementById('b-slider').value = b;
      document.getElementById('op-select').value = op;
      document.getElementById('mode-select').value = m;
      sendOverride();
    }

    window.onload = initWebSocket;
  </script>
</body>
</html>
)rawliteral";
