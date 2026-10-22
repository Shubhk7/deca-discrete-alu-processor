#!/usr/bin/env python3
"""
DECA Dual-Mode 8-Bit Discrete & Silicon ALU Processor — Complete Schemdraw Generator
Draws all 12 DIP ICs (IC1..IC12), 3 MCUs (Minima, R4 WiFi, ESP32-S3), Hybrid Switch
Networks (20x 10k + 19x 4.7k), 9x Voltage Dividers (1.0k/2.2k), Decoupling Caps,
and 12 Status LEDs with continuous orthographic bus trunks and solder dots.
"""

import os
import schemdraw
import schemdraw.elements as elm


def pt(ic, anchor_name):
    """Retrieves the (x, y) coordinates of a named pin on an elm.Ic."""
    return ic.absanchors[anchor_name]


def connect_to_bus(d, pin_pt, bus_x, color, dot=True):
    """Draws a horizontal wire from an IC pin to a vertical bus trunk at x = bus_x."""
    y = pin_pt[1]
    d.add(elm.Line().at(pin_pt).to((bus_x, y)).color(color))
    if dot:
        d.add(elm.Dot().at((bus_x, y)).color(color))


def build_deca_alu_schematic():
    with schemdraw.Drawing(show=False) as d:
        d.config(unit=2.4, fontsize=9.0, font='monospace', lw=1.8)

        # High-contrast engineering palette (crisp on white & light backgrounds)
        C_VCC   = '#cf222e'  # Deep Red (+5V_MASTER)
        C_GND   = '#334155'  # Dark Slate (COMMON_GND)
        C_BUSA  = '#b45309'  # Rich Amber/Gold (Operand A Bus)
        C_BUSB  = '#15803d'  # Forest Green (Operand B Bus)
        C_SUB   = '#7e22ce'  # Royal Purple (Opcode S[2:0] / SUB)
        C_SUM   = '#0369a1'  # Deep Ocean Blue (Adder Sum & Output Bus Y[7:0])
        C_CARRY = '#ea580c'  # Vivid Orange (Carry Chain C4 -> /Cn)
        C_LOGIC = '#0f766e'  # Deep Teal (Conditioned B' + ~A)
        C_XOR   = '#475569'  # Charcoal Blue (Bitwise XOR)
        C_AND   = '#92400e'  # Warm Brown (Bitwise AND)
        C_OR    = '#6d28d9'  # Deep Violet (Bitwise OR)
        C_MATCH = '#16a34a'  # Bright Emerald (Hardware Equivalence Match)

        # =====================================================================
        # COLUMN 1 (x = 0): POWER STATION, HYBRID SWITCHES & ESP32-S3 BRAIN
        # =====================================================================

        # --- 1A. MCU1: Arduino UNO R4 Minima Power Station ---
        minima_pins = [
            elm.IcPin(name='[USB-C] 5V In',       side='L', anchorname='USB'),
            elm.IcPin(name='COMMON_GND [GND]',    side='R', anchorname='GND'),
            elm.IcPin(name='+5V_MASTER [5V Pin]', side='R', anchorname='V5'),
        ]
        minima = d.add(
            elm.Ic(pins=minima_pins, size=(8.5, 2.8), pinspacing=1.1)
            .at((0, 34.0))
            .label('MCU1: ARDUINO UNO R4 MINIMA (Dedicated +5V / 1.2A Power Station)', loc='top', fontsize=10)
            .color(C_VCC)
        )
        d.add(elm.Line().left(1.2).at(pt(minima, 'USB')).color(C_VCC).label('USB-C 5V Host', loc='left'))

        # +5V Rail, 2x 100uF Bulk Electrolytic, and 14x 0.1uF Ceramic Decoupling Bank
        d.add(elm.Line().right(2.2).at(pt(minima, 'V5')).color(C_VCC).label('+5V_MASTER', loc='top'))
        vcc_node1 = d.here
        d.add(elm.Dot().at(vcc_node1).color(C_VCC))
        d.add(elm.Capacitor(polar=True).down(2.2).at(vcc_node1).label('2x 100uF\nBulk', loc='bot').color(C_BUSA))
        gnd_node1 = d.here
        d.add(elm.Line().right(3.2).at(vcc_node1).color(C_VCC))
        vcc_node2 = d.here
        d.add(elm.Capacitor().down(2.2).at(vcc_node2).label('14x 0.1uF\nBypass', loc='bot').color(C_BUSA))
        gnd_node2 = d.here
        d.add(elm.Wire('|-').at(pt(minima, 'GND')).to(gnd_node1).color(C_GND))
        d.add(elm.Line().at(gnd_node1).to(gnd_node2).color(C_GND))
        d.add(elm.Dot().at(gnd_node1).color(C_GND))
        d.add(elm.Ground().at(gnd_node1).color(C_GND).label('COMMON_GND (Star Hub)', loc='right'))

        # --- 1B. Hybrid Input Stage Topology (SW1, SW2, SW3 + 20x 10k + 19x 4.7k) ---
        sw_start = (0.5, 29.5)
        d.add(elm.Vdd().at(sw_start).color(C_VCC).label('+5V_MASTER', loc='top'))
        d.add(elm.Line().right(1.4).at(sw_start).color(C_VCC))
        d.add(elm.Switch().right(2.8).label('SW1(A0..7), SW2(B0..7)\nSW3(S0..2, MODE)', loc='top').color(C_BUSA))
        hybrid_jct = d.here
        d.add(elm.Dot().at(hybrid_jct).color(C_BUSA))
        d.add(elm.Resistor().down(2.2).at(hybrid_jct).label('20x 10k\nPull-Down', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))

        # Connect Hybrid Switch output directly to the Master Input Bus Trunks
        BUS_A_X    = 15.0
        BUS_B_X    = 16.0
        BUS_S_X    = 17.0
        BUS_MODE_X = 18.0
        d.add(elm.Line().at(hybrid_jct).to((BUS_A_X, hybrid_jct[1])).color(C_BUSA).label('Manual DIP Bus Feed', loc='top'))
        d.add(elm.Dot().at((BUS_A_X, hybrid_jct[1])).color(C_BUSA))

        # --- 1C. MCU3: ESP32-S3 N16R8 (Remote Injector & WebSocket Telemetry) ---
        esp_pins = [
            # Left side: Telemetry sense inputs & Power isolation
            elm.IcPin(name='[G3] Sense Match',           side='L', anchorname='G3'),
            elm.IcPin(name='[G42,47,48,21] Sense Y7..4', side='L', anchorname='GY_HI'),
            elm.IcPin(name='[G38..41] Sense Y3..0',      side='L', anchorname='GY_LO'),
            elm.IcPin(name='[GND] Star Ground',          side='L', anchorname='GND'),
            elm.IcPin(name='[5V/VIN] UNCONNECTED!',      side='L', anchorname='VIN'),
            # Right side: Contiguous 4.7k Override Injection Outputs
            elm.IcPin(name='Inject MODE [G37]',          side='R', anchorname='G_MD'),
            elm.IcPin(name='Inject S2..S0(SUB) [G36,35,18]', side='R', anchorname='G_OP'),
            elm.IcPin(name='Inject B[7:4] [G14..17]',    side='R', anchorname='G_BHI'),
            elm.IcPin(name='Inject B[3:0] [G10..13]',    side='R', anchorname='G_BLO'),
            elm.IcPin(name='Inject A[7:4] [G6..9]',      side='R', anchorname='G_AHI'),
            elm.IcPin(name='Inject A[3:0] [G1,2,4,5]',   side='R', anchorname='G_ALO'),
        ]
        esp32 = d.add(
            elm.Ic(pins=esp_pins, size=(9.2, 8.2), pinspacing=1.15)
            .at((0, 14.5))
            .label('MCU3: ESP32-S3 N16R8 (SoftAP "DECA-ALU" + 4.7k Override Injector)', loc='top', fontsize=10)
            .color(C_SUM)
        )
        d.add(elm.Ground().at(pt(esp32, 'GND')).color(C_GND))

        # --- 1D. 5V-to-3.3V Level Shifter Network (9 Pairs: 1.0k Series + 2.2k Shunt) ---
        div_in_pt = (-4.5, 9.5)
        d.add(elm.Dot().at(div_in_pt).color(C_SUM).label('From 5V Bus\n(NET_Y[7:0] + Match)', loc='left'))
        d.add(elm.Resistor().right(2.6).at(div_in_pt).label('9x 1.0k\nSeries', loc='top').color(C_SUM))
        div_mid = d.here
        d.add(elm.Dot().at(div_mid).color(C_MATCH))
        d.add(elm.Resistor().down(2.0).at(div_mid).label('9x 2.2k\nShunt', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))
        # Wire 3.43V divider output directly into ESP32 Sense inputs
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'GY_LO')).color(C_SUM))
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'GY_HI')).color(C_SUM))
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'G3')).color(C_MATCH).label('3.43V Safe CMOS', loc='bot'))

        # =====================================================================
        # VERTICAL MASTER INPUT BUS TRUNKS (Between Col 1 and Col 2)
        # =====================================================================
        d.add(elm.Line().at((BUS_A_X, 1.5)).to((BUS_A_X, 35.5)).color(C_BUSA).label('BUS_A (A[7:0])', loc='top'))
        d.add(elm.Line().at((BUS_B_X, 5.5)).to((BUS_B_X, 34.5)).color(C_BUSB).label('BUS_B (B[7:0])', loc='top'))
        d.add(elm.Line().at((BUS_S_X, 1.0)).to((BUS_S_X, 33.5)).color(C_SUB).label('BUS_S (S[2:0]/SUB)', loc='top'))
        d.add(elm.Line().at((BUS_MODE_X, 0.5)).to((BUS_MODE_X, 32.5)).color(C_CARRY).label('BUS_MODE', loc='top'))

        # Connect ESP32 19x 4.7k Override Resistors directly onto the Master Bus Trunks
        for anc, target_bus_x, col in [
            ('G_ALO', BUS_A_X,    C_BUSA),
            ('G_AHI', BUS_A_X,    C_BUSA),
            ('G_BLO', BUS_B_X,    C_BUSB),
            ('G_BHI', BUS_B_X,    C_BUSB),
            ('G_OP',  BUS_S_X,    C_SUB),
            ('G_MD',  BUS_MODE_X, C_CARRY),
        ]:
            pin_c = pt(esp32, anc)
            res = d.add(elm.Resistor().right(3.2).at(pin_c).label('4.7k', loc='top', fontsize=8.5).color(col))
            connect_to_bus(d, res.end, target_bus_x, col, dot=True)

        # =====================================================================
        # COLUMN 2 (x = 20.5): CUSTOM 4-BIT DISCRETE ALU CORE (IC1 – IC6)
        # =====================================================================
        COL2_X = 20.5
        IC_W2  = 8.6

        # Vertical Intermediate Function Bus Trunks (Between Col 2 and Col 3)
        BUS_SUM_X = 31.0
        BUS_AND_X = 31.8
        BUS_OR_X  = 32.6
        BUS_XOR_X = 33.4
        BUS_NOT_X = 34.2

        d.add(elm.Line().at((BUS_SUM_X, 3.0)).to((BUS_SUM_X, 29.0)).color(C_SUM).label('SUM[3:0]', loc='top'))
        d.add(elm.Line().at((BUS_AND_X, 3.0)).to((BUS_AND_X, 28.0)).color(C_AND).label('AND[3:0]', loc='top'))
        d.add(elm.Line().at((BUS_OR_X,  3.0)).to((BUS_OR_X,  27.0)).color(C_OR).label('OR[3:0]', loc='top'))
        d.add(elm.Line().at((BUS_XOR_X, 3.0)).to((BUS_XOR_X, 26.0)).color(C_XOR).label('XOR[3:0]', loc='top'))
        d.add(elm.Line().at((BUS_NOT_X, 3.0)).to((BUS_NOT_X, 25.0)).color(C_LOGIC).label('~A[3:0]', loc='top'))

        # --- IC2: 74HC86N (#1) Subtraction B-Inverter (14-Pin DIP) ---
        ic2_pins = [
            elm.IcPin(name='[P2,5,10,13] SUB (S0)', side='L', anchorname='SUB'),
            elm.IcPin(name='[P1,4,9,12] B[3:0]',    side='L', anchorname='B_IN'),
            elm.IcPin(name='GND [P7] | VCC [P14]',  side='R', anchorname='PWR'),
            elm.IcPin(name="B'[3:0] [P3,6,8,11]",   side='R', anchorname='B_OUT'),
        ]
        ic2 = d.add(
            elm.Ic(pins=ic2_pins, size=(IC_W2, 3.2), pinspacing=1.1)
            .at((COL2_X, 32.0))
            .label('IC2: 74HC86N (#1) — Sub Inverter (B ^ SUB)', loc='top', fontsize=9.5)
            .color(C_LOGIC)
        )
        connect_to_bus(d, pt(ic2, 'B_IN'), BUS_B_X, C_BUSB)
        connect_to_bus(d, pt(ic2, 'SUB'),  BUS_S_X, C_SUB)

        # --- IC1: 74LS83N 4-Bit Binary Full Adder (16-Pin DIP, Non-Standard Power!) ---
        ic1_pins = [
            elm.IcPin(name='[P13] C0 (Cin=SUB)',     side='L', anchorname='C0'),
            elm.IcPin(name="[P11,7,4,16] B'[3:0]",   side='L', anchorname='B_COND'),
            elm.IcPin(name='[P10,8,3,15] A[3:0]',    side='L', anchorname='A_IN'),
            elm.IcPin(name='! P5=+5V, P12=GND !',    side='R', anchorname='PWR'),
            elm.IcPin(name='C4 (Cout) [P14]',        side='R', anchorname='C4'),
            elm.IcPin(name='Sum[3:0] [P9,6,2,1]',    side='R', anchorname='SUM'),
        ]
        ic1 = d.add(
            elm.Ic(pins=ic1_pins, size=(IC_W2, 4.2), pinspacing=1.1)
            .at((COL2_X, 25.5))
            .label('IC1: 74LS83N 4-Bit Adder (! PIN 5 = +5V, PIN 12 = GND !)', loc='top', fontsize=9.5)
            .color(C_VCC)
        )
        connect_to_bus(d, pt(ic1, 'A_IN'), BUS_A_X, C_BUSA)
        connect_to_bus(d, pt(ic1, 'C0'),   BUS_S_X, C_SUB)
        connect_to_bus(d, pt(ic1, 'SUM'),  BUS_SUM_X, C_SUM)

        # Wire IC2 B'[3:0] directly into IC1 B'[3:0]
        b_out_pt = pt(ic2, 'B_OUT')
        b_in_pt  = pt(ic1, 'B_COND')
        d.add(elm.Line().right(1.0).at(b_out_pt).color(C_LOGIC))
        d.add(elm.Line().to((b_out_pt[0] + 1.0, b_in_pt[1] + 2.3)).color(C_LOGIC))
        d.add(elm.Line().to((COL2_X - 0.8, b_in_pt[1] + 2.3)).color(C_LOGIC).label("NET_B'[3:0] (IC2 -> IC1)", loc='top', fontsize=8.5))
        d.add(elm.Line().to((COL2_X - 0.8, b_in_pt[1])).color(C_LOGIC))
        d.add(elm.Line().to(b_in_pt).color(C_LOGIC))

        # --- IC4: 74HCT08 Bitwise AND (14-Pin DIP) ---
        ic4_pins = [
            elm.IcPin(name='[P2,5,10,13] B[3:0]',  side='L', anchorname='B_IN'),
            elm.IcPin(name='[P1,4,9,12] A[3:0]',   side='L', anchorname='A_IN'),
            elm.IcPin(name='GND [P7] | VCC [P14]', side='R', anchorname='PWR'),
            elm.IcPin(name='AND[3:0] [P3,6,8,11]', side='R', anchorname='OUT'),
        ]
        ic4 = d.add(
            elm.Ic(pins=ic4_pins, size=(IC_W2, 3.0), pinspacing=1.1)
            .at((COL2_X, 20.2))
            .label('IC4: 74HCT08 — Quad 2-Input Bitwise AND (A . B)', loc='top', fontsize=9.5)
            .color(C_AND)
        )
        connect_to_bus(d, pt(ic4, 'A_IN'), BUS_A_X,   C_BUSA)
        connect_to_bus(d, pt(ic4, 'B_IN'), BUS_B_X,   C_BUSB)
        connect_to_bus(d, pt(ic4, 'OUT'),  BUS_AND_X, C_AND)

        # --- IC5: 74HCT32 Bitwise OR (14-Pin DIP) ---
        ic5_pins = [
            elm.IcPin(name='[P2,5,10,13] B[3:0]',  side='L', anchorname='B_IN'),
            elm.IcPin(name='[P1,4,9,12] A[3:0]',   side='L', anchorname='A_IN'),
            elm.IcPin(name='GND [P7] | VCC [P14]', side='R', anchorname='PWR'),
            elm.IcPin(name='OR[3:0] [P3,6,8,11]',  side='R', anchorname='OUT'),
        ]
        ic5 = d.add(
            elm.Ic(pins=ic5_pins, size=(IC_W2, 3.0), pinspacing=1.1)
            .at((COL2_X, 15.0))
            .label('IC5: 74HCT32 — Quad 2-Input Bitwise OR (A + B)', loc='top', fontsize=9.5)
            .color(C_OR)
        )
        connect_to_bus(d, pt(ic5, 'A_IN'), BUS_A_X,  C_BUSA)
        connect_to_bus(d, pt(ic5, 'B_IN'), BUS_B_X,  C_BUSB)
        connect_to_bus(d, pt(ic5, 'OUT'),  BUS_OR_X, C_OR)

        # --- IC3: 74HC86N (#2) Bitwise XOR (14-Pin DIP) ---
        ic3_pins = [
            elm.IcPin(name='[P2,5,10,13] B[3:0]',  side='L', anchorname='B_IN'),
            elm.IcPin(name='[P1,4,9,12] A[3:0]',   side='L', anchorname='A_IN'),
            elm.IcPin(name='GND [P7] | VCC [P14]', side='R', anchorname='PWR'),
            elm.IcPin(name='XOR[3:0] [P3,6,8,11]', side='R', anchorname='OUT'),
        ]
        ic3 = d.add(
            elm.Ic(pins=ic3_pins, size=(IC_W2, 3.0), pinspacing=1.1)
            .at((COL2_X, 9.8))
            .label('IC3: 74HC86N (#2) — Quad 2-Input Bitwise XOR (A ^ B)', loc='top', fontsize=9.5)
            .color(C_XOR)
        )
        connect_to_bus(d, pt(ic3, 'A_IN'), BUS_A_X,   C_BUSA)
        connect_to_bus(d, pt(ic3, 'B_IN'), BUS_B_X,   C_BUSB)
        connect_to_bus(d, pt(ic3, 'OUT'),  BUS_XOR_X, C_XOR)

        # --- IC6: 74HCT04 Hex Inverter (~A[3:0] + Carry Inverter 5A->5Y) ---
        ic6_pins = [
            elm.IcPin(name='[P13] 6A (Tied GND)',   side='L', anchorname='G6A'),
            elm.IcPin(name='[P11] 5A (C4 Carry In)',side='L', anchorname='C4_IN'),
            elm.IcPin(name='[P1,3,5,9] A[3:0]',     side='L', anchorname='A_IN'),
            elm.IcPin(name='GND [P7] | VCC [P14]',  side='R', anchorname='PWR'),
            elm.IcPin(name='5Y (/C4 Out) [P10]',    side='R', anchorname='CN_OUT'),
            elm.IcPin(name='~A[3:0] [P2,4,6,8]',    side='R', anchorname='NOT_OUT'),
        ]
        ic6 = d.add(
            elm.Ic(pins=ic6_pins, size=(IC_W2, 4.2), pinspacing=1.1)
            .at((COL2_X, 3.2))
            .label('IC6: 74HCT04 — Hex Inverter (~A[3:0] + Carry Invert C4 -> /Cn)', loc='top', fontsize=9.5)
            .color(C_LOGIC)
        )
        connect_to_bus(d, pt(ic6, 'A_IN'),    BUS_A_X,   C_BUSA)
        connect_to_bus(d, pt(ic6, 'NOT_OUT'), BUS_NOT_X, C_LOGIC)
        d.add(elm.Ground().at(pt(ic6, 'G6A')).color(C_GND))

        # Route IC1 Pin 14 (C4) down into IC6 Pin 11 (5A)
        c4_pt = pt(ic1, 'C4')
        c4_in = pt(ic6, 'C4_IN')
        d.add(elm.Line().right(1.1).at(c4_pt).color(C_CARRY).label('NET_C4', loc='top', fontsize=8.5))
        d.add(elm.Line().to((c4_pt[0] + 1.1, c4_in[1] - 1.8)).color(C_CARRY))
        d.add(elm.Line().to((COL2_X - 0.8, c4_in[1] - 1.8)).color(C_CARRY))
        d.add(elm.Line().to((COL2_X - 0.8, c4_in[1])).color(C_CARRY))
        d.add(elm.Line().to(c4_in).color(C_CARRY))

        # =====================================================================
        # COLUMN 3 (x = 36.5): 4x 74HCT151 8:1 MULTIPLEXERS (IC7 – IC10)
        # =====================================================================
        COL3_X = 36.5
        IC_W3  = 9.0

        # Vertical Output Bus Trunks (Between Col 3 and Col 4)
        BUS_YCUST_X = 48.0
        BUS_Y181_X  = 49.0
        BUS_MATCH_X = 50.0

        d.add(elm.Line().at((BUS_YCUST_X, 4.0)).to((BUS_YCUST_X, 33.5)).color(C_SUM).label('NET_Y[3:0]', loc='top'))
        d.add(elm.Line().at((BUS_Y181_X,  4.0)).to((BUS_Y181_X,  32.5)).color(C_SUM).label('NET_Y[7:4]', loc='top'))
        d.add(elm.Line().at((BUS_MATCH_X, 4.0)).to((BUS_MATCH_X, 20.0)).color(C_MATCH).label('EQUIV_PASS', loc='top'))

        mux_specs = [
            ('IC7',  0, 27.5, '9', 'GND(0)', 'NET_A1'),
            ('IC8',  1, 19.0, '6', 'NET_A0', 'NET_A2'),
            ('IC9',  2, 10.5, '2', 'NET_A1', 'NET_A3'),
            ('IC10', 3,  2.0, '1', 'NET_A2', 'A4/GND'),
        ]

        for ic_des, bit_i, y_coord, sum_p, shl_src, shr_src in mux_specs:
            mpins = [
                elm.IcPin(name=f'[P12,13] D7({shr_src}), D6({shl_src})', side='L', anchorname='SHF'),
                elm.IcPin(name=f'[P14] D5 (~A{bit_i})',                  side='L', anchorname='D5'),
                elm.IcPin(name=f'[P15] D4 (XOR{bit_i})',                 side='L', anchorname='D4'),
                elm.IcPin(name=f'[P1] D3 (OR{bit_i})',                   side='L', anchorname='D3'),
                elm.IcPin(name=f'[P2] D2 (AND{bit_i})',                  side='L', anchorname='D2'),
                elm.IcPin(name=f'[P4,3] D0,D1 (Sum{bit_i}=IC1:P{sum_p})',side='L', anchorname='D01'),
                elm.IcPin(name='VCC [P16] | /E,GND [P7,8]',              side='R', anchorname='PWR'),
                elm.IcPin(name='S2,S1,S0 [P9,10,11]',                    side='R', anchorname='SEL'),
                elm.IcPin(name=f'Y{bit_i} Out [P5]',                     side='R', anchorname='Y'),
            ]
            mux_ic = d.add(
                elm.Ic(pins=mpins, size=(IC_W3, 6.4), pinspacing=0.95)
                .at((COL3_X, y_coord))
                .label(f'{ic_des}: 74HCT151 — 8:1 Operation MUX (Bit {bit_i}: Y{bit_i})', loc='top', fontsize=9.5)
                .color(C_SUB)
            )
            # Connect MUX left inputs directly to the vertical Function Bus Trunks
            connect_to_bus(d, pt(mux_ic, 'D01'), BUS_SUM_X, C_SUM)
            connect_to_bus(d, pt(mux_ic, 'D2'),  BUS_AND_X, C_AND)
            connect_to_bus(d, pt(mux_ic, 'D3'),  BUS_OR_X,  C_OR)
            connect_to_bus(d, pt(mux_ic, 'D4'),  BUS_XOR_X, C_XOR)
            connect_to_bus(d, pt(mux_ic, 'D5'),  BUS_NOT_X, C_LOGIC)
            connect_to_bus(d, pt(mux_ic, 'SHF'), BUS_A_X,   C_BUSA)
            # Connect MUX output Y to vertical BUS_YCUST_X trunk
            connect_to_bus(d, pt(mux_ic, 'Y'),   BUS_YCUST_X, C_SUM)
            d.add(elm.Line().right(0.9).at(pt(mux_ic, 'SEL')).color(C_SUB).label('BUS_S[2:0]', loc='right', fontsize=8.5))

        # =====================================================================
        # COLUMN 4 (x = 52.5): IC11 (74LS181N), IC12 (74HCT85), & MCU2 (R4 WIFI)
        # =====================================================================
        COL4_X = 52.5
        IC_W4  = 9.4

        # --- IC11: 74LS181N 4-Bit Bipolar ALU (24-Pin DIP) ---
        ic11_pins = [
            elm.IcPin(name='[P7] /Cn (Active-LOW Carry)',    side='L', anchorname='CN'),
            elm.IcPin(name='[P3..6,8] S3..S0 + Mode M',      side='L', anchorname='SEL'),
            elm.IcPin(name='[P1,22,20,18] /B3..0 (M0/M1)',   side='L', anchorname='B_IN'),
            elm.IcPin(name='[P2,23,21,19] /A3..0 (M0/M1)',   side='L', anchorname='A_IN'),
            elm.IcPin(name='VCC [P24] | GND [P12]',          side='R', anchorname='PWR'),
            elm.IcPin(name='/Cn+4 (Cout8) [P16]',            side='R', anchorname='COUT'),
            elm.IcPin(name='/F[3:0] (Y7..Y4) [P9,10,11,13]', side='R', anchorname='F_OUT'),
        ]
        ic11 = d.add(
            elm.Ic(pins=ic11_pins, size=(IC_W4, 5.2), pinspacing=1.1)
            .at((COL4_X, 27.5))
            .label('IC11: 74LS181N 4-Bit Silicon ALU (M0: Upper Nibble | M1: Benchmark)', loc='top', fontsize=9.5)
            .color(C_CARRY)
        )
        # Route IC6 Pin 10 (/C4) carry cascade all the way to IC11 Pin 7 (/Cn)
        cn_out_pt = pt(ic6, 'CN_OUT')
        cn_in_pt  = pt(ic11, 'CN')
        d.add(elm.Line().at(cn_out_pt).to((46.8, cn_out_pt[1])).color(C_CARRY).label('NET_CN (/C4 Cascade)', loc='bot'))
        d.add(elm.Line().to((46.8, cn_in_pt[1])).color(C_CARRY))
        d.add(elm.Line().to(cn_in_pt).color(C_CARRY))

        # Route IC11 /F[3:0] to vertical BUS_Y181_X trunk
        f_pt = pt(ic11, 'F_OUT')
        d.add(elm.Line().right(1.0).at(f_pt).color(C_SUM))
        d.add(elm.Line().to((f_pt[0] + 1.0, f_pt[1] + 1.8)).color(C_SUM))
        d.add(elm.Line().to((BUS_Y181_X, f_pt[1] + 1.8)).color(C_SUM).label('NET_Y[7:4]', loc='top', fontsize=8.5))
        d.add(elm.Dot().at((BUS_Y181_X, f_pt[1] + 1.8)).color(C_SUM))

        # Amber Carry-Out LED on IC11 Pin 16
        d.add(elm.Resistor().right(2.0).at(pt(ic11, 'COUT')).label('330R', loc='top').color(C_CARRY))
        d.add(elm.LED().right(1.8).label('Amber Cout8', loc='top').color(C_BUSA))
        d.add(elm.Ground().color(C_GND))

        # --- IC12: 74HCT85 4-Bit Magnitude Comparator (16-Pin DIP) ---
        ic12_pins = [
            elm.IcPin(name='[P2,4] I_A<B, I_A>B (GND)',     side='L', anchorname='EXP_LO'),
            elm.IcPin(name='[P3] I_A=B (Tied +5V!)',        side='L', anchorname='EXP_HI'),
            elm.IcPin(name='[P9,11,14,1] Word B (F_181)',   side='L', anchorname='WB'),
            elm.IcPin(name='[P10,12,13,15] Word A (Y_cust)',side='L', anchorname='WA'),
            elm.IcPin(name='VCC [P16] | GND [P8]',          side='R', anchorname='PWR'),
            elm.IcPin(name='Q_A=B Match Out [P6]',          side='R', anchorname='MATCH'),
        ]
        ic12 = d.add(
            elm.Ic(pins=ic12_pins, size=(IC_W4, 5.0), pinspacing=1.1)
            .at((COL4_X, 16.5))
            .label('IC12: 74HCT85 Magnitude Comparator (17ns Silicon Equivalence)', loc='top', fontsize=9.5)
            .color(C_MATCH)
        )
        connect_to_bus(d, pt(ic12, 'WA'), BUS_YCUST_X, C_SUM)
        connect_to_bus(d, pt(ic12, 'WB'), BUS_Y181_X,  C_SUM)
        d.add(elm.Vdd().at(pt(ic12, 'EXP_HI')).color(C_VCC).label('+5V', loc='left'))
        d.add(elm.Ground().at(pt(ic12, 'EXP_LO')).color(C_GND))

        # Green Silicon Match LED on IC12 Pin 6 & feed into BUS_MATCH_X
        m_pt = pt(ic12, 'MATCH')
        d.add(elm.Line().right(1.2).at(m_pt).color(C_MATCH))
        m_jct = d.here
        d.add(elm.Dot().at(m_jct).color(C_MATCH))
        d.add(elm.Resistor().right(2.0).at(m_jct).label('330R', loc='top').color(C_MATCH))
        d.add(elm.LED().right(1.8).label('Green Match LED', loc='top').color(C_MATCH))
        d.add(elm.Ground().color(C_GND))
        # Tap match signal back to vertical BUS_MATCH_X trunk
        d.add(elm.Line().at(m_jct).to((m_jct[0], m_pt[1] - 2.6)).color(C_MATCH))
        d.add(elm.Line().to((BUS_MATCH_X, m_pt[1] - 2.6)).color(C_MATCH).label('NET_EQUIV_PASS', loc='bot', fontsize=8.5))
        d.add(elm.Dot().at((BUS_MATCH_X, m_pt[1] - 2.6)).color(C_MATCH))

        # --- MCU2: Arduino UNO R4 WiFi (Golden Model Oracle & 12x8 LED Matrix HUD) ---
        r4_pins = [
            elm.IcPin(name='[GND] Star Ground | [5V] NC!',  side='L', anchorname='GND'),
            elm.IcPin(name='[A0..A3] S[2:0] + MODE In',     side='L', anchorname='A0_3'),
            elm.IcPin(name='[D12] HW Match In (IC12:P6)',   side='L', anchorname='D12'),
            elm.IcPin(name='[D6..D9] Y[7:4] (74LS181 In)',  side='L', anchorname='D6_9'),
            elm.IcPin(name='[D2..D5] Y[3:0] (Custom In)',   side='L', anchorname='D2_5'),
            elm.IcPin(name='SW FAIL LED Out [D11]',         side='R', anchorname='D11'),
            elm.IcPin(name='SW PASS LED Out [D10]',         side='R', anchorname='D10'),
            elm.IcPin(name='Onboard 12x8 Matrix [HUD]',     side='R', anchorname='HUD'),
        ]
        r4_wifi = d.add(
            elm.Ic(pins=r4_pins, size=(IC_W4, 6.2), pinspacing=1.1)
            .at((COL4_X, 3.5))
            .label('MCU2: ARDUINO UNO R4 WIFI (5V Golden Model Verifier + 12x8 HUD)', loc='top', fontsize=9.5)
            .color(C_BUSB)
        )
        connect_to_bus(d, pt(r4_wifi, 'D2_5'), BUS_YCUST_X, C_SUM)
        connect_to_bus(d, pt(r4_wifi, 'D6_9'), BUS_Y181_X,  C_SUM)
        connect_to_bus(d, pt(r4_wifi, 'D12'),  BUS_MATCH_X, C_MATCH)
        connect_to_bus(d, pt(r4_wifi, 'A0_3'), BUS_MODE_X,  C_CARRY)
        d.add(elm.Ground().at(pt(r4_wifi, 'GND')).color(C_GND))

        # External Software PASS (D10) & FAIL (D11) LEDs with 220R Resistors
        d.add(elm.Resistor().right(2.0).at(pt(r4_wifi, 'D10')).label('220R', loc='top').color(C_BUSB))
        d.add(elm.LED().right(1.8).label('Green PASS LED', loc='top').color(C_BUSB))
        d.add(elm.Ground().color(C_GND))

        d.add(elm.Resistor().right(2.0).at(pt(r4_wifi, 'D11')).label('220R', loc='top').color(C_VCC))
        d.add(elm.LED().right(1.8).label('Red FAIL LED', loc='top').color(C_VCC))
        d.add(elm.Ground().color(C_GND))

    # Save output SVGs in the same directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    out_path_1 = os.path.join(script_dir, 'DECA_ALU_Schemdraw_Full.svg')
    out_path_2 = os.path.join(script_dir, 'CIRCUIT_DIAGRAM.svg')
    d.save(out_path_1)
    d.save(out_path_2)
    print(f"Successfully generated clean, connected schematics:\n  1. {out_path_1}\n  2. {out_path_2}")


if __name__ == '__main__':
    build_deca_alu_schematic()