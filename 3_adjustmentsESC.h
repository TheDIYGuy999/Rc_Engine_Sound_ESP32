/* Recommended internal ESC & settings (use programming card) for TAMIYA trucks with 3 speed transmission:

ESC: HOBBYWING 1080 QUICRUN WP Crawler Brushed with the following settings:
1: 3
2: 1
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
14: 1 (be careful here!!)
15: 1 (change it, important)

Motor: 540 size, 35 turns, stock pinion
*/

// ESC SETTINGS ******************************************************************************************************

// Drive direction adjustment:
//#define ESC_DIR // uncomment this, if your motor is spinning in the wrong direction

// Top speed adjusment:
const int16_t escPulseSpan = 500; // 500 = full ESC power available, 1000 half ESC power available etc. (1200 for TAMIYA trucks with 35T motor)
