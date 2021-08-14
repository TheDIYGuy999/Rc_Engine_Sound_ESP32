/* Recommended ESC & settings

*****************************************************************************
ESC: HOBBYWING 1080 QUICRUN WP Crawler Brushed with the following settings:
Vehicle: TAMIYA trucks with 3 speed transmission
Motor: 540 size, 35 turns, stock pinion

For more details refer to the ESC manual.
Uncommented settings are left on factory preset
1: 3
2: 1 (1=  LiPo, 2 = NIMH battery)
3: 3
4: 3
5: 4
6: 2
7: 9
8: 1 
9: 8 (change it, important)
10: 4
11: 4
12: 5
13: 5 (16KHz = less whining)
14: 1 (be careful here, this will change the BEC voltage!!)
15: 1 (change it, important)

*****************************************************************************
ESC: HOBBYWING QUICRUN FUSION motor / ESC combo with the following settings:
Vehicle: RGT EX86100 Jeep Crawler, stock pinion, 3S LiPo
Motor: Integrated brushless

Make sure you are uncommenting (remove //) "#define QUICRUN_FUSION" below!

For more details refer to the ESC manual.
Uncommented settings are left on factory preset
1: 2 (change it, 1 is jerky)
2: 1
3: 3
4: 1
5: 2 (change it, if Vehicle is driving in wrong direction)
6: 1 (be careful here, this will change the BEC voltage!!)
7: 5
8: 1 
9: 4 (change reverse speed as you prefer)

*****************************************************************************
HOBBYWING 1060 is working as well, but 1080 is still better

*****************************************************************************
AS-12/6RW EASY from Modellbau-Regler.de is recommended for smaller vehicles (WPL, MN Model etc.)

*****************************************************************************

*/

// ESC SETTINGS ******************************************************************************************************

// ESC type selection:
//#define QUICRUN_FUSION // Linearity compensation for HOBBYWING Quicrun Fusion

// Drive direction adjustment:
//#define ESC_DIR // uncomment this, if your motor is spinning in the wrong direction

// Top speed adjustment:
// Usually 500 ( = 1000 - 2000 microseconds output or -45° to 45° servo angle) Enlarge it, if your vehicle is too fast
// - The Hobbywing 1060 is reaching full throttle (forward) @ about 1800 Microseconds, so we need about 640 for full throttle
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 1600
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 650
// - Hobbywing 1040 ESC & stock 20T 540 motor in RGT EX86100 = 640
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 1200
// - Modellbau-Regler.de AS-12/6RW EASY ESC = 600
const int16_t escPulseSpan = 1200; // 500 = full ESC power available, 1000 half ESC power available etc. 

// Additional takeoff punch:
// Usually 0. Enlarge it, if your motor is too weak around neutral.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 80
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 10
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 0
// - Hobbywing 1080 ESC & 35T 540 motor for HERCULES HOBBY trucks with 3 speed transmission = 150
const int16_t escTakeoffPunch = 150; 

// Additional reverse speed (disconnect & reconnect battery after changing this setting):
// Usually 0. Enlarge it, if your reverse speed is too slow.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 220
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 150
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 0
// - Hobbywing 1080 ESC & 35T 540 motor for HERCULES HOBBY trucks with 3 speed transmission = 80
const int16_t escReversePlus = 80;
