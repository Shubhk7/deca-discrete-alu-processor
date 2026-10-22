"""
# !/usr/bin/env python3
DECA Dual-Mode 8-Bit Discrete & Silicon ALU Processor — Complete Schemdraw Generator
Draws all 12 DIP ICs (IC1..IC12), 3 MCUs (Minima, R4 WiFi, ESP32-S3), Hybrid Switch
Networks (20x 10k + 19x 4.7k), 9x Voltage Dividers (1.0k/2.2k), Decoupling Caps,
and 12 Status LEDs in a clean 4-column schematic layout.
"""

import os
import schemdraw
import schemdraw.elements as elm


def pt(ic, anchor_name):
    """Safely retrieves the (x, y) connection point of a pin on an elm.Ic."""
    return ic.absanchors[anchor_name]


def build_deca_alu_schematic():
    with schemdraw.Drawing(show=False) as d:
        # Use single valid font family 'monospace' to prevent matplotlib findfont warnings
        d.config(unit=2.2, fontsize=9.0, font='monospace', lw=1.6)

        # Standardized Color Palette matching the wiring guide
        C_VCC   = '#e5534b'  # Red (+5V Master)
        C_GND   = '#768390'  # Gray/Black (Common Star GND)
        C_BUSA  = '#d29922'  # Yellow (Operand A Bus)
        C_BUSB  = '#3fb950'  # Green (Operand B Bus)
        C_SUB   = '#bc8cff'  # Violet (Opcode S[2:0] / SUB)
        C_SUM   = '#58a6ff'  # Blue (Adder Sum & Output Bus Y[7:0])
        C_CARRY = '#f0883e'  # Orange (Carry Chain C4 -> /Cn)
        C_LOGIC = '#a5d6ff'  # Cyan (Internal Gate Outputs)
        C_MATCH = '#56d364'  # Bright Green (Equivalence Match)

        # =====================================================================
        # COLUMN 1 (x = 0): POWER STATION, HYBRID SWITCHES & ESP32-S3 BRAIN
        # =====================================================================

        # --- 1A. MCU1: Arduino UNO R4 Minima Power Station ---
        minima_pins = [
            elm.IcPin(name='USB-C 5V In', side='L', pin='USB', anchorname='USB'),
            elm.IcPin(name='COMMON_GND',  side='R', pin='GND', anchorname='GND'),
            elm.IcPin(name='+5V_MASTER',  side='R', pin='5V',  anchorname='V5'),
        ]
        minima = d.add(
            elm.Ic(pins=minima_pins, size=(4.4, 2.2), pinspacing=0.8)
            .at((0, 24.5))
            .label('MCU1: Arduino R4 Minima\n(Clean +5V / 1.2A Station)', loc='top', fontsize=9.5)
            .color(C_VCC)
        )

        # +5V Rail, 2x 100uF Bulk Electrolytic, and 14x 0.1uF Ceramic Decoupling Bank
        d.add(elm.Line().right(1.5).at(pt(minima, 'V5')).color(C_VCC).label('+5V_MASTER', loc='top'))
        vcc_node1 = d.here
        d.add(elm.Dot().at(vcc_node1).color(C_VCC))
        d.add(elm.Capacitor(polar=True).down(1.6).at(vcc_node1).label('2x 100uF\nBulk', loc='bot').color(C_BUSA))
        gnd_node1 = d.here
        d.add(elm.Line().right(2.2).at(vcc_node1).color(C_VCC))
        vcc_node2 = d.here
        d.add(elm.Capacitor().down(1.6).at(vcc_node2).label('14x 0.1uF\nBypass (104)', loc='bot').color(C_BUSA))
        gnd_node2 = d.here
        d.add(elm.Wire('|-').at(pt(minima, 'GND')).to(gnd_node1).color(C_GND))
        d.add(elm.Line().at(gnd_node1).to(gnd_node2).color(C_GND))
        d.add(elm.Ground().at(gnd_node1).color(C_GND).label('COMMON_GND (Star Hub)', loc='right'))

        # --- 1B. Hybrid Input Stage Topology (SW1, SW2, SW3 + 20x 10k + 19x 4.7k) ---
        d.add(elm.Vdd().at((0.5, 20.5)).color(C_VCC).label('+5V_MASTER'))
        d.add(elm.Switch().right(2.4).at((0.5, 20.5)).label('SW1(A0..7), SW2(B0..7)\nSW3(S0..2, MODE)', loc='top').color(C_BUSA))
        hybrid_junction = d.here
        d.add(elm.Dot().at(hybrid_junction).color(C_BUSA))
        d.add(elm.Resistor().down(1.8).at(hybrid_junction).label('20x 10k\nPull-Down', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))
        d.add(elm.Line().right(1.8).at(hybrid_junction).color(C_BUSA).label('To Bus Nodes\n(NET_Ax, Bx, Sx)', loc='right'))

        # --- 1C. MCU3: ESP32-S3 N16R8 (Remote Injector & WebSocket Telemetry) ---
        esp_pins = [
            # Left side: Telemetry sense inputs (via 9x 1k/2.2k dividers) & Power isolation
            elm.IcPin(name='Sense Match',  side='L', pin='G3',           anchorname='G3'),
            elm.IcPin(name='Sense Y[7:4]', side='L', pin='G42,47,48,21', anchorname='GY_HI'),
            elm.IcPin(name='Sense Y[3:0]', side='L', pin='G38-41',       anchorname='GY_LO'),
            elm.IcPin(name='COMMON_GND',   side='L', pin='GND',          anchorname='GND'),
            elm.IcPin(name='5V (NC!)',     side='L', pin='VIN',          anchorname='VIN'),
            # Right side: Contiguous 4.7k Override Injection Outputs
            elm.IcPin(name='MODE',         side='R', pin='G37',          anchorname='G_MD'),
            elm.IcPin(name='S[2:0] (SUB)', side='R', pin='G36,35,18',    anchorname='G_OP'),
            elm.IcPin(name='B[7:4]',       side='R', pin='G14-17',       anchorname='G_BHI'),
            elm.IcPin(name='B[3:0]',       side='R', pin='G10-13',       anchorname='G_BLO'),
            elm.IcPin(name='A[7:4]',       side='R', pin='G6-9',         anchorname='G_AHI'),
            elm.IcPin(name='A[3:0]',       side='R', pin='G1,2,4,5',     anchorname='G_ALO'),
        ]
        esp32 = d.add(
            elm.Ic(pins=esp_pins, size=(4.8, 7.2), pinspacing=0.95)
            .at((0, 8.2))
            .label('MCU3: ESP32-S3 N16R8\n(SoftAP "DECA-ALU" + Injector)', loc='top', fontsize=9.5)
            .color(C_SUM)
        )
        d.add(elm.Ground().at(pt(esp32, 'GND')).color(C_GND))

        # 19x 4.7k Override Resistors on ESP32 Injection Buses
        for anc, net_lbl, col in [
            ('G_ALO', 'NET_A[3:0]',          C_BUSA),
            ('G_AHI', 'NET_A[7:4]',          C_BUSA),
            ('G_BLO', 'NET_B[3:0]',          C_BUSB),
            ('G_BHI', 'NET_B[7:4]',          C_BUSB),
            ('G_OP',  'NET_S[2:0] (S0=SUB)', C_SUB),
            ('G_MD',  'NET_MODE',            C_CARRY),
        ]:
            d.add(
                elm.Resistor()
                .right(2.4)
                .at(pt(esp32, anc))
                .label('4.7k', loc='top', fontsize=8.5)
                .label(net_lbl, loc='right', fontsize=8.5)
                .color(col)
            )

        # --- 1D. 5V-to-3.3V Level Shifter Subcircuit (9 Pairs: 1.0k / 2.2k) ---
        d.add(elm.Dot().at((0.5, 4.8)).color(C_SUM).label('5V Bus (Y[7:0] + Match)', loc='left'))
        d.add(elm.Resistor().right(2.2).at((0.5, 4.8)).label('9x 1.0k\nSeries', loc='top').color(C_SUM))
        div_mid = d.here
        d.add(elm.Dot().at(div_mid).color(C_MATCH))
        d.add(elm.Resistor().down(1.6).at(div_mid).label('9x 2.2k\nShunt', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))
        d.add(elm.Line().right(1.5).at(div_mid).color(C_MATCH).label('3.43V Safe -> ESP32 G38..21, G3', loc='right'))

        # =====================================================================
        # COLUMN 2 (x = 13.0): CUSTOM 4-BIT DISCRETE ALU CORE (IC1 – IC6)
        # =====================================================================

        # --- IC2: 74HC86N (#1) Subtraction B-Inverter (14-Pin DIP) ---
        ic2_pins = [
            elm.IcPin(name='SUB (S0)', side='L', pin='2,5,10,13', anchorname='SUB'),
            elm.IcPin(name='B[3:0]',   side='L', pin='1,4,9,12',  anchorname='B_IN'),
            elm.IcPin(name='GND',      side='B', pin='7',         anchorname='GND'),
            elm.IcPin(name='VCC',      side='T', pin='14',        anchorname='VCC'),
            elm.IcPin(name="B'[3:0]",  side='R', pin='3,6,8,11',  anchorname='B_OUT'),
        ]
        ic2 = d.add(
            elm.Ic(pins=ic2_pins, size=(4.2, 2.6), pinspacing=0.85)
            .at((13.0, 23.8))
            .label('IC2: 74HC86N (#1)\nSub Inverter (B ^ SUB)', loc='top', fontsize=9.0)
            .color(C_LOGIC)
        )
        d.add(elm.Line().left(0.9).at(pt(ic2, 'B_IN')).color(C_BUSB).label('NET_B[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic2, 'SUB')).color(C_SUB).label('NET_S0 (SUB)', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic2, 'B_OUT')).color(C_LOGIC).label("NET_B'[3:0] -> IC1", loc='right'))

        # --- IC1: 74LS83N 4-Bit Binary Full Adder (16-Pin DIP, Non-Standard Power!) ---
        ic1_pins = [
            elm.IcPin(name='C0 (Cin)',   side='L', pin='13',        anchorname='C0'),
            elm.IcPin(name="B'[3:0]",    side='L', pin='11,7,4,16', anchorname='B_COND'),
            elm.IcPin(name='A[3:0]',     side='L', pin='10,8,3,15', anchorname='A_IN'),
            elm.IcPin(name='GND (P12!)', side='B', pin='12',        anchorname='GND'),
            elm.IcPin(name='VCC (P5!)',  side='T', pin='5',         anchorname='VCC'),
            elm.IcPin(name='C4 (Cout)',  side='R', pin='14',        anchorname='C4'),
            elm.IcPin(name='Sum[3:0]',   side='R', pin='9,6,2,1',   anchorname='SUM'),
        ]
        ic1 = d.add(
            elm.Ic(pins=ic1_pins, size=(4.2, 3.4), pinspacing=0.85)
            .at((13.0, 18.2))
            .label('IC1: 74LS83N Adder\n(! P5=+5V, P12=GND)', loc='top', fontsize=9.0)
            .color(C_VCC)
        )
        d.add(elm.Line().left(0.9).at(pt(ic1, 'A_IN')).color(C_BUSA).label('NET_A[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic1, 'B_COND')).color(C_LOGIC).label("NET_B'[3:0] (IC2)", loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic1, 'C0')).color(C_SUB).label('NET_S0 (SUB)', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic1, 'SUM')).color(C_SUM).label('NET_SUM[3:0] -> MUX D0,D1', loc='right'))
        d.add(elm.Line().right(0.9).at(pt(ic1, 'C4')).color(C_CARRY).label('NET_C4 -> IC6:11', loc='right'))

        # --- IC4: 74HCT08 Bitwise AND (14-Pin DIP) ---
        ic4_pins = [
            elm.IcPin(name='B[3:0]',   side='L', pin='2,5,10,13', anchorname='B_IN'),
            elm.IcPin(name='A[3:0]',   side='L', pin='1,4,9,12',  anchorname='A_IN'),
            elm.IcPin(name='GND',      side='B', pin='7',         anchorname='GND'),
            elm.IcPin(name='VCC',      side='T', pin='14',        anchorname='VCC'),
            elm.IcPin(name='AND[3:0]', side='R', pin='3,6,8,11',  anchorname='OUT'),
        ]
        ic4 = d.add(
            elm.Ic(pins=ic4_pins, size=(4.2, 2.4), pinspacing=0.8)
            .at((13.0, 13.6))
            .label('IC4: 74HCT08 (AND)', loc='top', fontsize=9.0)
            .color(C_BUSA)
        )
        d.add(elm.Line().left(0.9).at(pt(ic4, 'A_IN')).color(C_BUSA).label('NET_A[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic4, 'B_IN')).color(C_BUSB).label('NET_B[3:0]', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic4, 'OUT')).color(C_BUSA).label('NET_AND[3:0] -> MUX D2', loc='right'))

        # --- IC5: 74HCT32 Bitwise OR (14-Pin DIP) ---
        ic5_pins = [
            elm.IcPin(name='B[3:0]',  side='L', pin='2,5,10,13', anchorname='B_IN'),
            elm.IcPin(name='A[3:0]',  side='L', pin='1,4,9,12',  anchorname='A_IN'),
            elm.IcPin(name='GND',     side='B', pin='7',         anchorname='GND'),
            elm.IcPin(name='VCC',     side='T', pin='14',        anchorname='VCC'),
            elm.IcPin(name='OR[3:0]', side='R', pin='3,6,8,11',  anchorname='OUT'),
        ]
        ic5 = d.add(
            elm.Ic(pins=ic5_pins, size=(4.2, 2.4), pinspacing=0.8)
            .at((13.0, 9.4))
            .label('IC5: 74HCT32 (OR)', loc='top', fontsize=9.0)
            .color(C_SUB)
        )
        d.add(elm.Line().left(0.9).at(pt(ic5, 'A_IN')).color(C_BUSA).label('NET_A[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic5, 'B_IN')).color(C_BUSB).label('NET_B[3:0]', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic5, 'OUT')).color(C_SUB).label('NET_OR[3:0] -> MUX D3', loc='right'))

        # --- IC3: 74HC86N (#2) Bitwise XOR (14-Pin DIP) ---
        ic3_pins = [
            elm.IcPin(name='B[3:0]',   side='L', pin='2,5,10,13', anchorname='B_IN'),
            elm.IcPin(name='A[3:0]',   side='L', pin='1,4,9,12',  anchorname='A_IN'),
            elm.IcPin(name='GND',      side='B', pin='7',         anchorname='GND'),
            elm.IcPin(name='VCC',      side='T', pin='14',        anchorname='VCC'),
            elm.IcPin(name='XOR[3:0]', side='R', pin='3,6,8,11',  anchorname='OUT'),
        ]
        ic3 = d.add(
            elm.Ic(pins=ic3_pins, size=(4.2, 2.4), pinspacing=0.8)
            .at((13.0, 5.2))
            .label('IC3: 74HC86N (#2) (XOR)', loc='top', fontsize=9.0)
            .color(C_GND)
        )
        d.add(elm.Line().left(0.9).at(pt(ic3, 'A_IN')).color(C_BUSA).label('NET_A[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic3, 'B_IN')).color(C_BUSB).label('NET_B[3:0]', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic3, 'OUT')).color(C_GND).label('NET_XOR[3:0] -> MUX D4', loc='right'))

        # --- IC6: 74HCT04 Hex Inverter (~A[3:0] & Carry Inverter 5A->5Y) ---
        ic6_pins = [
            elm.IcPin(name='6A (GND)',     side='L', pin='13',      anchorname='G6A'),
            elm.IcPin(name='5A (C4 In)',   side='L', pin='11',      anchorname='C4_IN'),
            elm.IcPin(name='A[3:0]',       side='L', pin='1,3,5,9', anchorname='A_IN'),
            elm.IcPin(name='GND',          side='B', pin='7',       anchorname='GND'),
            elm.IcPin(name='VCC',          side='T', pin='14',      anchorname='VCC'),
            elm.IcPin(name='5Y (/C4 Out)', side='R', pin='10',      anchorname='CN_OUT'),
            elm.IcPin(name='~A[3:0]',      side='R', pin='2,4,6,8', anchorname='NOT_OUT'),
        ]
        ic6 = d.add(
            elm.Ic(pins=ic6_pins, size=(4.2, 3.0), pinspacing=0.8)
            .at((13.0, 0.5))
            .label('IC6: 74HCT04 Inverter\n(~A[3:0] + C4 -> /Cn)', loc='top', fontsize=9.0)
            .color(C_LOGIC)
        )
        d.add(elm.Line().left(0.9).at(pt(ic6, 'A_IN')).color(C_BUSA).label('NET_A[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic6, 'C4_IN')).color(C_CARRY).label('NET_C4 (IC1:14)', loc='left'))
        d.add(elm.Ground().at(pt(ic6, 'G6A')).color(C_GND))
        d.add(elm.Line().right(0.9).at(pt(ic6, 'NOT_OUT')).color(C_LOGIC).label('NET_NOT[3:0] -> MUX D5', loc='right'))
        d.add(elm.Line().right(0.9).at(pt(ic6, 'CN_OUT')).color(C_CARRY).label('NET_CN (/C4 -> IC11:7)', loc='right'))

        # =====================================================================
        # COLUMN 3 (x = 24.5): 4x 74HCT151 8:1 MULTIPLEXERS (IC7 – IC10)
        # =====================================================================

        mux_specs = [
            ('IC7',  0, 21.0, '9', 'GND (0-Fill)', 'NET_A1',     'NET_Y0 -> IC12:10, D2, G38'),
            ('IC8',  1, 14.2, '6', 'NET_A0',       'NET_A2',     'NET_Y1 -> IC12:12, D3, G39'),
            ('IC9',  2,  7.4, '2', 'NET_A1',       'NET_A3',     'NET_Y2 -> IC12:13, D4, G40'),
            ('IC10', 3,  0.5, '1', 'NET_A2',       'A4(M0)/GND', 'NET_Y3 -> IC12:15, D5, G41'),
        ]

        for ic_des, bit_i, y_coord, sum_p, shl_src, shr_src, y_dest in mux_specs:
            mpins = [
                elm.IcPin(name=f'D7 (SHR:{shr_src})',          side='L', pin='12',      anchorname='D7'),
                elm.IcPin(name=f'D6 (SHL:{shl_src})',          side='L', pin='13',      anchorname='D6'),
                elm.IcPin(name=f'D5 (~A{bit_i})',              side='L', pin='14',      anchorname='D5'),
                elm.IcPin(name=f'D4 (XOR{bit_i})',             side='L', pin='15',      anchorname='D4'),
                elm.IcPin(name=f'D3 (OR{bit_i})',              side='L', pin='1',       anchorname='D3'),
                elm.IcPin(name=f'D2 (AND{bit_i})',             side='L', pin='2',       anchorname='D2'),
                elm.IcPin(name=f'D0,D1 (Sum{bit_i}=P{sum_p})', side='L', pin='4,3',     anchorname='D01'),
                elm.IcPin(name='/E,GND',                       side='B', pin='7,8',     anchorname='GND'),
                elm.IcPin(name='VCC',                          side='T', pin='16',      anchorname='VCC'),
                elm.IcPin(name='S2,S1,S0',                     side='R', pin='9,10,11', anchorname='SEL'),
                elm.IcPin(name=f'Y{bit_i} Out',                side='R', pin='5',       anchorname='Y'),
            ]
            mux_ic = d.add(
                elm.Ic(pins=mpins, size=(5.0, 4.8), pinspacing=0.58)
                .at((24.5, y_coord))
                .label(f'{ic_des}: 74HCT151 (MUX Bit {bit_i})', loc='top', fontsize=9.0)
                .color(C_SUB)
            )
            d.add(elm.Ground().at(pt(mux_ic, 'GND')).color(C_GND))
            d.add(elm.Line().right(0.9).at(pt(mux_ic, 'Y')).color(C_SUM).label(y_dest, loc='right', fontsize=8.5))
            d.add(elm.Line().right(0.9).at(pt(mux_ic, 'SEL')).color(C_SUB).label('NET_S[2:0]', loc='right', fontsize=8.5))

        # =====================================================================
        # COLUMN 4 (x = 37.5): IC11 (74LS181N), IC12 (74HCT85), & MCU2 (R4 WIFI)
        # =====================================================================

        # --- IC11: 74LS181N 4-Bit Bipolar ALU (24-Pin DIP) ---
        ic11_pins = [
            elm.IcPin(name='/Cn (Carry In)',              side='L', pin='7',          anchorname='CN'),
            elm.IcPin(name='M (Mode)',                    side='L', pin='8',          anchorname='M'),
            elm.IcPin(name='S3,S2,S1,S0',                 side='L', pin='3,4,5,6',    anchorname='SEL'),
            elm.IcPin(name='/B[3:0] (M0:B7..4/M1:B3..0)', side='L', pin='1,22,20,18', anchorname='B_IN'),
            elm.IcPin(name='/A[3:0] (M0:A7..4/M1:A3..0)', side='L', pin='2,23,21,19', anchorname='A_IN'),
            elm.IcPin(name='GND',                         side='B', pin='12',         anchorname='GND'),
            elm.IcPin(name='VCC (+5V)',                   side='T', pin='24',         anchorname='VCC'),
            elm.IcPin(name='/Cn+4 (Cout8)',               side='R', pin='16',         anchorname='COUT'),
            elm.IcPin(name='/F[3:0] (Y7..Y4)',            side='R', pin='9,10,11,13', anchorname='F_OUT'),
        ]
        ic11 = d.add(
            elm.Ic(pins=ic11_pins, size=(5.6, 4.6), pinspacing=0.75)
            .at((37.5, 20.5))
            .label('IC11: 74LS181N 4-Bit Silicon ALU\n(Mode 0: Upper Nibble | Mode 1: Benchmark)', loc='top', fontsize=9.0)
            .color(C_CARRY)
        )
        d.add(elm.Line().left(0.9).at(pt(ic11, 'A_IN')).color(C_BUSA).label('SPDT_A', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic11, 'B_IN')).color(C_BUSB).label('SPDT_B', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic11, 'CN')).color(C_CARRY).label('SPDT_CARRY (/C4 or +5V)', loc='left'))
        d.add(elm.Line().right(0.9).at(pt(ic11, 'F_OUT')).color(C_SUM).label('NET_Y[7:4] -> IC12, D6..D9', loc='right'))

        # Amber Carry-Out LED on IC11 Pin 16
        d.add(elm.Resistor().right(1.6).at(pt(ic11, 'COUT')).label('330R', loc='top').color(C_CARRY))
        d.add(elm.LED().right(1.4).label('Amber Cout8 LED', loc='top').color(C_BUSA))
        d.add(elm.Ground().color(C_GND))

        # --- IC12: 74HCT85 4-Bit Magnitude Comparator (16-Pin DIP) ---
        ic12_pins = [
            elm.IcPin(name='I_A<B, I_A>B (GND)',   side='L', pin='2,4',         anchorname='EXP_LO'),
            elm.IcPin(name='I_A=B (+5V!)',         side='L', pin='3',           anchorname='EXP_HI'),
            elm.IcPin(name='Word B (F_181[3:0])',  side='L', pin='9,11,14,1',   anchorname='WB'),
            elm.IcPin(name='Word A (Y_cust[3:0])', side='L', pin='10,12,13,15', anchorname='WA'),
            elm.IcPin(name='GND',                  side='B', pin='8',           anchorname='GND'),
            elm.IcPin(name='VCC (+5V)',            side='T', pin='16',          anchorname='VCC'),
            elm.IcPin(name='Q_A=B (Match)',        side='R', pin='6',           anchorname='MATCH'),
        ]
        ic12 = d.add(
            elm.Ic(pins=ic12_pins, size=(5.6, 3.8), pinspacing=0.8)
            .at((37.5, 13.0))
            .label('IC12: 74HCT85 Comparator\n(Real-Time 17ns Hardware Equivalence)', loc='top', fontsize=9.0)
            .color(C_MATCH)
        )
        d.add(elm.Vdd().at(pt(ic12, 'EXP_HI')).color(C_VCC))
        d.add(elm.Ground().at(pt(ic12, 'EXP_LO')).color(C_GND))
        d.add(elm.Line().left(0.9).at(pt(ic12, 'WA')).color(C_SUM).label('NET_Y[3:0] (IC7..10)', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(ic12, 'WB')).color(C_SUM).label('NET_Y[7:4] (IC11)', loc='left'))

        # Green Silicon Match LED on IC12 Pin 6
        d.add(elm.Line().right(1.0).at(pt(ic12, 'MATCH')).color(C_MATCH).label('NET_EQUIV_PASS', loc='top'))
        match_node = d.here
        d.add(elm.Dot().at(match_node).color(C_MATCH))
        d.add(elm.Resistor().right(1.6).at(match_node).label('330R', loc='top').color(C_MATCH))
        d.add(elm.LED().right(1.4).label('Green Silicon Match LED', loc='top').color(C_MATCH))
        d.add(elm.Ground().color(C_GND))

        # --- MCU2: Arduino UNO R4 WiFi (Golden Model Oracle & 12x8 LED Matrix HUD) ---
        r4_pins = [
            elm.IcPin(name='In: S[2:0] + MODE',     side='L', pin='A0-A3', anchorname='A0_3'),
            elm.IcPin(name='In: HW Match (IC12:6)', side='L', pin='D12',   anchorname='D12'),
            elm.IcPin(name='In: Y[7:4] (74LS181)',  side='L', pin='D6-D9', anchorname='D6_9'),
            elm.IcPin(name='In: Y[3:0] (Custom)',   side='L', pin='D2-D5', anchorname='D2_5'),
            elm.IcPin(name='COMMON_GND',            side='L', pin='GND',   anchorname='GND'),
            elm.IcPin(name='5V (NC!)',              side='L', pin='5V',    anchorname='V5'),
            elm.IcPin(name='SW FAIL Out',           side='R', pin='D11',   anchorname='D11'),
            elm.IcPin(name='SW PASS Out',           side='R', pin='D10',   anchorname='D10'),
            elm.IcPin(name='12x8 Matrix HUD',       side='R', pin='HUD',   anchorname='HUD'),
        ]
        r4_wifi = d.add(
            elm.Ic(pins=r4_pins, size=(5.6, 5.2), pinspacing=0.75)
            .at((37.5, 4.0))
            .label('MCU2: Arduino UNO R4 WiFi\n(5V Golden Model Verifier + 12x8 HUD)', loc='top', fontsize=9.0)
            .color(C_BUSB)
        )
        d.add(elm.Ground().at(pt(r4_wifi, 'GND')).color(C_GND))
        d.add(elm.Line().left(0.9).at(pt(r4_wifi, 'D2_5')).color(C_SUM).label('NET_Y[3:0]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(r4_wifi, 'D6_9')).color(C_SUM).label('NET_Y[7:4]', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(r4_wifi, 'D12')).color(C_MATCH).label('NET_EQUIV_PASS', loc='left'))
        d.add(elm.Line().left(0.9).at(pt(r4_wifi, 'A0_3')).color(C_SUB).label('NET_S[2:0], MODE', loc='left'))

        # External Software PASS (D10) & FAIL (D11) LEDs with 220R Resistors
        d.add(elm.Resistor().right(1.6).at(pt(r4_wifi, 'D10')).label('220R', loc='top').color(C_BUSB))
        d.add(elm.LED().right(1.4).label('Green PASS LED', loc='top').color(C_BUSB))
        d.add(elm.Ground().color(C_GND))

        d.add(elm.Resistor().right(1.6).at(pt(r4_wifi, 'D11')).label('220R', loc='top').color(C_VCC))
        d.add(elm.LED().right(1.4).label('Red FAIL LED', loc='top').color(C_VCC))
        d.add(elm.Ground().color(C_GND))

    # Save output SVGs after the with-block finishes rendering the canvas
    script_dir = os.path.dirname(os.path.abspath(__file__))
    out_path_1 = os.path.join(script_dir, 'DECA_ALU_Schemdraw_Full.svg')
    out_path_2 = os.path.join(script_dir, 'CIRCUIT_DIAGRAM.svg')
    d.save(out_path_1)
    d.save(out_path_2)
    print(f"Successfully generated:\n  1. {out_path_1}\n  2. {out_path_2}")


if __name__ == '__main__':
    build_deca_alu_schematic()