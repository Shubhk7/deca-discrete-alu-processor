#!/usr/bin/env python3
"""
DECA Dual 4-Bit Cascaded 8-Bit Hybrid ALU Processor — Schemdraw Vector Generator
Draws the complete hybrid architecture:
  - MCU1: Arduino UNO R4 Minima (Dedicated Clean +5V / 1.2A Power Station)
  - MCU3: ESP32-S3 N16R8 (SoftAP Web Dashboard, 19x 4.7k Injectors, 9x Dividers)
  - IC1:  74LS181N 4-Bit Silicon ALU (DIP-24 Bipolar Computing Core)
  - MCU2: Arduino UNO R4 WiFi (5V Upper 4-Bit ALU Engine + 12x8 LED Matrix HUD)
  - Direct Hardware Carry Cascade: 74LS181 Pin 16 (/Cn+4) -> Arduino Pin D6
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


def build_hybrid_cascade_schematic():
    with schemdraw.Drawing(show=False) as d:
        d.config(unit=2.4, fontsize=9.0, font='monospace', lw=1.8)

        # High-contrast engineering palette
        C_VCC   = '#cf222e'  # Deep Red (+5V_MASTER)
        C_GND   = '#334155'  # Dark Slate (COMMON_GND)
        C_BUSA  = '#b45309'  # Rich Amber/Gold (Operand A Bus)
        C_BUSB  = '#15803d'  # Forest Green (Operand B Bus)
        C_SUB   = '#7e22ce'  # Royal Purple (Opcode S[2:0] / SUB)
        C_SUM   = '#0369a1'  # Deep Ocean Blue (ALU Outputs Y[7:0])
        C_CARRY = '#ea580c'  # Vivid Orange (Hardware Carry Cascade Link)
        C_HUD   = '#16a34a'  # Bright Emerald (12x8 LED Matrix HUD)

        # =====================================================================
        # COLUMN 1 (x = 0): POWER STATION, HYBRID SWITCHES + ESP32-S3 BRAIN
        # =====================================================================

        # --- 1A. MCU1: Arduino UNO R4 Minima Power Station ---
        minima_pins = [
            elm.IcPin(name='[USB-C] 5V In',       side='L', anchorname='USB'),
            elm.IcPin(name='COMMON_GND [GND]',    side='R', anchorname='GND'),
            elm.IcPin(name='+5V_MASTER [5V Pin]', side='R', anchorname='V5'),
        ]
        minima = d.add(
            elm.Ic(pins=minima_pins, size=(8.5, 2.8), pinspacing=1.1)
            .at((0, 32.0))
            .label('MCU1: ARDUINO UNO R4 MINIMA (Dedicated +5V / 1.2A Power Station)', loc='top', fontsize=10)
            .color(C_VCC)
        )
        d.add(elm.Line().left(1.2).at(pt(minima, 'USB')).color(C_VCC).label('USB-C 5V Host', loc='left'))

        # +5V Rail, Bulk 100uF Electrolytic, and Decoupling
        d.add(elm.Line().right(2.2).at(pt(minima, 'V5')).color(C_VCC).label('+5V_MASTER', loc='top'))
        vcc_node1 = d.here
        d.add(elm.Dot().at(vcc_node1).color(C_VCC))
        d.add(elm.Capacitor(polar=True).down(2.2).at(vcc_node1).label('100uF\nBulk', loc='bot').color(C_BUSA))
        gnd_node1 = d.here
        d.add(elm.Line().right(3.2).at(vcc_node1).color(C_VCC))
        vcc_node2 = d.here
        d.add(elm.Capacitor().down(2.2).at(vcc_node2).label('0.1uF\nBypass', loc='bot').color(C_BUSA))
        gnd_node2 = d.here
        d.add(elm.Wire('|-').at(pt(minima, 'GND')).to(gnd_node1).color(C_GND))
        d.add(elm.Line().at(gnd_node1).to(gnd_node2).color(C_GND))
        d.add(elm.Dot().at(gnd_node1).color(C_GND))
        d.add(elm.Ground().at(gnd_node1).color(C_GND).label('COMMON_GND (Star Hub)', loc='right'))

        # --- 1B. Hybrid Input Stage (SW1, SW2, SW3 + 20x 10k Pull-Downs) ---
        sw_start = (0.5, 27.5)
        d.add(elm.Vdd().at(sw_start).color(C_VCC).label('+5V_MASTER', loc='top'))
        d.add(elm.Line().right(1.4).at(sw_start).color(C_VCC))
        d.add(elm.Switch().right(2.8).label('SW1(A0..7), SW2(B0..7)\nSW3(S0..2, MODE)', loc='top').color(C_BUSA))
        hybrid_jct = d.here
        d.add(elm.Dot().at(hybrid_jct).color(C_BUSA))
        d.add(elm.Resistor().down(2.2).at(hybrid_jct).label('20x 10k\nPull-Down', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))

        BUS_A_X    = 14.0
        BUS_B_X    = 15.0
        BUS_S_X    = 16.0
        BUS_MODE_X = 17.0
        d.add(elm.Line().at(hybrid_jct).to((BUS_A_X, hybrid_jct[1])).color(C_BUSA).label('DIP Switch Bus Feed', loc='top'))
        d.add(elm.Dot().at((BUS_A_X, hybrid_jct[1])).color(C_BUSA))

        # --- 1C. MCU3: ESP32-S3 N16R8 (SoftAP Dashboard + 19x 4.7k Injectors) ---
        esp_pins = [
            elm.IcPin(name='[G3] Sense C8',               side='L', anchorname='G3'),
            elm.IcPin(name='[G42,47,48,21] Sense Y7..4',  side='L', anchorname='GY_HI'),
            elm.IcPin(name='[G38..41] Sense Y3..0',       side='L', anchorname='GY_LO'),
            elm.IcPin(name='[GND] Star Ground',           side='L', anchorname='GND'),
            elm.IcPin(name='[5V/VIN] UNCONNECTED!',       side='L', anchorname='VIN'),
            elm.IcPin(name='Inject MODE [G37]',           side='R', anchorname='G_MD'),
            elm.IcPin(name='Inject S2..S0 [G36,35,18]',   side='R', anchorname='G_OP'),
            elm.IcPin(name='Inject B[7:4] [G14..17]',     side='R', anchorname='G_BHI'),
            elm.IcPin(name='Inject B[3:0] [G10..13]',     side='R', anchorname='G_BLO'),
            elm.IcPin(name='Inject A[7:4] [G6..9]',       side='R', anchorname='G_AHI'),
            elm.IcPin(name='Inject A[3:0] [G1,2,4,5]',    side='R', anchorname='G_ALO'),
        ]
        esp32 = d.add(
            elm.Ic(pins=esp_pins, size=(9.2, 8.2), pinspacing=1.15)
            .at((0, 13.5))
            .label('MCU3: ESP32-S3 N16R8 (SoftAP "DECA-ALU-HYBRID" + 4.7k Injector)', loc='top', fontsize=10)
            .color(C_SUM)
        )
        d.add(elm.Ground().at(pt(esp32, 'GND')).color(C_GND))

        # --- 1D. 5V-to-3.3V Voltage Divider Network (9 Pairs: 1.0k / 2.2k) ---
        div_in_pt = (-4.5, 9.0)
        d.add(elm.Dot().at(div_in_pt).color(C_SUM).label('From 5V Output Bus\n(Y[7:0] + C8)', loc='left'))
        d.add(elm.Resistor().right(2.6).at(div_in_pt).label('9x 1.0k\nSeries', loc='top').color(C_SUM))
        div_mid = d.here
        d.add(elm.Dot().at(div_mid).color(C_HUD))
        d.add(elm.Resistor().down(2.0).at(div_mid).label('9x 2.2k\nShunt', loc='bot').color(C_GND))
        d.add(elm.Ground().color(C_GND))
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'GY_LO')).color(C_SUM))
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'GY_HI')).color(C_SUM))
        d.add(elm.Wire('|-').at(div_mid).to(pt(esp32, 'G3')).color(C_HUD).label('3.43V Safe CMOS', loc='bot'))

        # =====================================================================
        # VERTICAL MASTER INPUT BUS TRUNKS
        # =====================================================================
        d.add(elm.Line().at((BUS_A_X, 1.5)).to((BUS_A_X, 33.5)).color(C_BUSA).label('BUS_A (A[7:0])', loc='top'))
        d.add(elm.Line().at((BUS_B_X, 5.5)).to((BUS_B_X, 32.5)).color(C_BUSB).label('BUS_B (B[7:0])', loc='top'))
        d.add(elm.Line().at((BUS_S_X, 1.0)).to((BUS_S_X, 31.5)).color(C_SUB).label('BUS_S (S[2:0])', loc='top'))
        d.add(elm.Line().at((BUS_MODE_X, 0.5)).to((BUS_MODE_X, 30.5)).color(C_CARRY).label('BUS_MODE', loc='top'))

        # Connect ESP32 Override Resistors directly into Bus Trunks
        for anc, target_bus_x, col in [
            ('G_ALO', BUS_A_X,    C_BUSA),
            ('G_AHI', BUS_A_X,    C_BUSA),
            ('G_BLO', BUS_B_X,    C_BUSB),
            ('G_BHI', BUS_B_X,    C_BUSB),
            ('G_OP',  BUS_S_X,    C_SUB),
            ('G_MD',  BUS_MODE_X, C_CARRY),
        ]:
            pin_c = pt(esp32, anc)
            res = d.add(elm.Resistor().right(3.0).at(pin_c).label('4.7k', loc='top', fontsize=8.5).color(col))
            connect_to_bus(d, res.end, target_bus_x, col, dot=True)

        # =====================================================================
        # COLUMN 2 (x = 21.0): IC1 — 74LS181N 4-BIT SILICON ALU (DIP-24)
        # =====================================================================
        COL2_X = 21.0
        IC_W   = 9.4

        ic11_pins = [
            elm.IcPin(name='[P7] /Cn (+5V = No Cin)',        side='L', anchorname='CN'),
            elm.IcPin(name='[P3..6,8] S3..S0 + Mode M',      side='L', anchorname='SEL'),
            elm.IcPin(name='[P1,22,20,18] /B3..0 (Lower B)', side='L', anchorname='B_IN'),
            elm.IcPin(name='[P2,23,21,19] /A3..0 (Lower A)', side='L', anchorname='A_IN'),
            elm.IcPin(name='VCC [P24] | GND [P12]',          side='R', anchorname='PWR'),
            elm.IcPin(name='/Cn+4 (C4 Cascade) [P16]',       side='R', anchorname='COUT'),
            elm.IcPin(name='/F[3:0] (Y3..Y0) [P9,10,11,13]', side='R', anchorname='F_OUT'),
        ]
        ic181 = d.add(
            elm.Ic(pins=ic11_pins, size=(IC_W, 5.8), pinspacing=1.1)
            .at((COL2_X, 23.5))
            .label('IC1: 74LS181N 4-Bit Silicon ALU\n(Computes Lower Nibble Y[3:0] in < 15ns)', loc='top', fontsize=9.5)
            .color(C_CARRY)
        )
        connect_to_bus(d, pt(ic181, 'A_IN'), BUS_A_X, C_BUSA)
        connect_to_bus(d, pt(ic181, 'B_IN'), BUS_B_X, C_BUSB)
        connect_to_bus(d, pt(ic181, 'SEL'),  BUS_S_X, C_SUB)
        d.add(elm.Vdd().at(pt(ic181, 'CN')).color(C_VCC).label('+5V (No Cin)', loc='left'))

        # Vertical Output Bus Trunks
        BUS_YLO_X = 33.5
        d.add(elm.Line().at((BUS_YLO_X, 4.0)).to((BUS_YLO_X, 28.0)).color(C_SUM).label('Y_181[3:0]', loc='top'))

        connect_to_bus(d, pt(ic181, 'F_OUT'), BUS_YLO_X, C_SUM)

        # =====================================================================
        # COLUMN 3 (x = 36.5): MCU2 — ARDUINO UNO R4 WIFI (ARM UPPER ALU + HUD)
        # =====================================================================
        COL3_X = 36.5

        r4_pins = [
            elm.IcPin(name='[GND] Star Ground | [5V] NC!',  side='L', anchorname='GND'),
            elm.IcPin(name='[A1..A4] S[2:0] + Mode M In',   side='L', anchorname='S_MD'),
            elm.IcPin(name='[D11..D13,A0] B[7:4] Upper In', side='L', anchorname='B_HI'),
            elm.IcPin(name='[D7..D10] A[7:4] Upper In',     side='L', anchorname='A_HI'),
            elm.IcPin(name='[D2..D5] Y[3:0] (74LS181 In)',  side='L', anchorname='Y_LO_IN'),
            elm.IcPin(name='[D6] C4 Cascade (74LS181:16)',  side='L', anchorname='C4_IN'),
            elm.IcPin(name='C8 Final Carry Out Telemetry',  side='R', anchorname='C8_OUT'),
            elm.IcPin(name='Y[7:4] Upper Nibble Out',       side='R', anchorname='Y_HI_OUT'),
            elm.IcPin(name='Onboard 12x8 LED Matrix HUD',   side='R', anchorname='MATRIX_HUD'),
        ]
        r4_wifi = d.add(
            elm.Ic(pins=r4_pins, size=(IC_W, 7.2), pinspacing=1.1)
            .at((COL3_X, 15.0))
            .label('MCU2: ARDUINO UNO R4 WIFI (32-Bit ARM @ 48MHz)\nUpper 4-Bit ALU Engine + Built-in 12x8 Matrix HUD', loc='top', fontsize=9.5)
            .color(C_BUSB)
        )
        d.add(elm.Ground().at(pt(r4_wifi, 'GND')).color(C_GND))
        connect_to_bus(d, pt(r4_wifi, 'A_HI'),    BUS_A_X,   C_BUSA)
        connect_to_bus(d, pt(r4_wifi, 'B_HI'),    BUS_B_X,   C_BUSB)
        connect_to_bus(d, pt(r4_wifi, 'S_MD'),    BUS_S_X,   C_SUB)
        connect_to_bus(d, pt(r4_wifi, 'Y_LO_IN'), BUS_YLO_X, C_SUM)

        # Direct Hardware Carry Cascade Link (74LS181 Pin 16 -> Arduino Pin D6)
        c4_out_pt = pt(ic181, 'COUT')
        c4_in_pt  = pt(r4_wifi, 'C4_IN')
        d.add(elm.Line().right(1.4).at(c4_out_pt).color(C_CARRY).label('C4 Cascade', loc='top'))
        c4_jct = d.here
        d.add(elm.Dot().at(c4_jct).color(C_CARRY))
        d.add(elm.Line().to((c4_jct[0], c4_in_pt[1])).color(C_CARRY))
        d.add(elm.Line().to(c4_in_pt).color(C_CARRY))

        # Visual 12x8 LED Matrix Display Box on R4 WiFi
        hud_pt = pt(r4_wifi, 'MATRIX_HUD')
        d.add(elm.Line().right(1.5).at(hud_pt).color(C_HUD))
        hud_box_origin = (hud_pt[0] + 1.5, hud_pt[1] - 2.0)
        d.add(
            elm.Ic(pins=[
                elm.IcPin(name='C0, C4, C8',  side='L', anchorname='C_STATUS'),
                elm.IcPin(name='Y[7:4] Dots', side='B', anchorname='Y_HI_DOTS'),
                elm.IcPin(name='Y[3:0] Dots', side='B', anchorname='Y_LO_DOTS'),
                elm.IcPin(name='Opcode HUD',  side='T', anchorname='OP_HUD'),
            ], size=(6.0, 4.0), pinspacing=1.2)
            .at(hud_box_origin)
            .label('ONBOARD 12x8 RED LED MATRIX\n(Virtual Display HUD: No Discrete LEDs)', loc='top', fontsize=9.0)
            .color(C_HUD)
        )

    # Save output SVGs
    script_dir = os.path.dirname(os.path.abspath(__file__))
    out_path_1 = os.path.join(script_dir, 'DECA_ALU_Hybrid_Cascade.svg')
    out_path_2 = os.path.join(script_dir, 'CIRCUIT_DIAGRAM.svg')
    d.save(out_path_1)
    d.save(out_path_2)
    print(f"Successfully generated hybrid cascade circuit diagrams:\n  1. {out_path_1}\n  2. {out_path_2}")


if __name__ == '__main__':
    build_hybrid_cascade_schematic()
