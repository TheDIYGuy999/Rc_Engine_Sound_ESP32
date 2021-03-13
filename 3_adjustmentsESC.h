/* Recommended internal ESC & settings (use programming card) for TAMIYA trucks with 3 speed transmission:

ESC: HOBBYWING 1080 QUICRUN WP Crawler Brushed with the following settings:
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
13: 4
14: 1 (be careful here, this will change the BEC voltage!!)
15: 1 (change it, important)

Motor: 540 size, 35 turns, stock pinion

HOBBYWING 1060 is working as well, but 1080 is still better

*/

// ESC SETTINGS ******************************************************************************************************

// Drive direction adjustment:
//#define ESC_DIR // uncomment this, if your motor is spinning in the wrong direction

// Top speed adjustment:
// Usually 500 ( = 1000 - 2000 microseconds output or -45° to 45° servo angle) Enlarge it, if your vehicle is too fast
// - The Hobbywing 1060 is reaching full throttle (forward) @ about 1800 Microseconds, so we need about 640 for full throttle
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 1600
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 650
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 1200
const int16_t escPulseSpan = 1200; // 500 = full ESC power available, 1000 half ESC power available etc. 

// Additional takeoff punch:
// Usually 0. Enlarge it, if your motor is too weak around neutral.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 80
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 10
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 0
const int16_t escTakeoffPunch = 0; 

// Additional reverse speed (disconnect & reconnect battery after changing this setting):
// Usually 0. Enlarge it, if your reverse speed is too slow.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 220
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 150
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 0
const int16_t escReversePlus = 0;
