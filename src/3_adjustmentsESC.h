#include <Arduino.h>

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
  AS-12/6RW EASY from Modellbau-Regler.de is suitable for smaller vehicles (WPL, MN Model etc.),
  but does not have the best drag brake. Better use an RZ7886 motor driver IC (see below)
*****************************************************************************

*/

// ESC SETTINGS ******************************************************************************************************

// General options
//#define QUICRUN_FUSION // Linearity compensation for HOBBYWING Quicrun Fusion motor / ESC combo
//#define ESC_DIR // uncomment this, if your motor is spinning in the wrong direction

/* RZ7886 motor driver IC instead of ESC. Not in combination with #define THIRD_BRAKELIGHT or #define TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT
  Wiring:
  Pin 1 to 33 "ESC"
  Pin 2 to 32
  Pin 3 to GND (0V)
  Pin 4 to Battery +
  Pin 5 & 6 to motor +
  Pin 7 & 8 to motor -
  Notes:
  - Make sure to have enough copper area around motor + and - for IC heatsinking!
  - Connect a 100 - 470uF electrolytic cap and a 100nF tantalum or ceramic cap across pins 2 - 3
*/
//#define RZ7886_DRIVER_MODE // An RZ7886 motor driver is used instead of a standard RC Crawler Type ESC. suitable for motors up to 370 size, for example WPL vehicles.
const uint16_t RZ7886_FREQUENCY = 500; // 500 Hz is recommended. It is not audible, if virtual engine sound is running. Higher frequencies may overheat the driver IC!
const uint8_t RZ7886_DRAGBRAKE_DUTY = 100; // 0 - 100%. 100% = max. brake power while standing still. 100% is recommended for crawlers.

// Top speed adjustment:
// Usually 500 ( = 1000 - 2000 microseconds output or -45° to 45° servo angle) Enlarge it, if your vehicle is too fast
// - The Hobbywing 1060 is reaching full throttle (forward) @ about 1800 Microseconds, so we need about 640 for full throttle
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 1600
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 650
// - Hobbywing 1040 ESC & stock 20T 540 motor in RGT EX86100 = 640
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 1200
// - Hobbywing 1080 ESC & RBR/C 370 motor in Carson Mercedes racing truck (top speed = 160km/h) = 900
// - Modellbau-Regler.de AS-12/6RW EASY ESC = 600
// - Meccano Dumper = 500
const uint16_t escPulseSpan = 500; // 500 = full ESC power available, 1000 half ESC power available etc.

// Additional takeoff punch:
// Usually 0. Enlarge it, if your motor is too weak around neutral.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 80
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 10
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 40 (was 0)
// - Hobbywing 1080 ESC & 35T 540 motor for HERCULES HOBBY trucks with 3 speed transmission = 150
// - Hobbywing 1080 ESC & RBR/C 370 motor in Carson Mercedes racing truck = 50
// - Meccano Dumper = 0
const uint16_t escTakeoffPunch = 0;

// Additional reverse speed (disconnect & reconnect battery after changing this setting):
// Usually 0. Enlarge it, if your reverse speed is too slow.
// - Hobbywing 1060 ESC & RBR/C 370 motor & 2 speed shifting transmission = 220
// - Hobbywing 1060 ESC & stock WPL 370 motor & 2 speed shifting transmission = 150
// - Hobbywing 1080 ESC & 35T 540 motor for TAMIYA trucks with 3 speed transmission = 0
// - Hobbywing 1080 ESC & 35T 540 motor for HERCULES HOBBY trucks with 3 speed transmission = 80
// - Meccano Dumper = 0
const uint16_t escReversePlus = 0;

// Brake margin: (Experimental!)
// This setting prevents the ESC to go completely back to zero / neutral as long as the braker trigger is pulled.
// This prevents the vehicle from rolling back as long as brake is applied. 0 = no effect, ca. 20 = strong effect.
// How it works? Prevents the ESC from entering the "drag brake range"
// Warning: vehicle may be unable to stop, if too high, especially when driving downhill!
const uint16_t brakeMargin = 10; // For RZ7886 motor driver and 370 motor = 10

// Battery low discharge protection (only for boards with voltage divider resistors):
//#define BATTERY_PROTECTION // This will disable the ESC output, if the battery cutout voltage is reached
const float CUTOFF_VOLTAGE = 3.3; // Usually 3.3 V per LiPo cell. NEVER below 3.2 V!
const float FULLY_CHARGED_VOLTAGE = 4.2; // Usually 4.2 V per LiPo cell, NEVER above!
const uint16_t RESISTOR_TO_BATTTERY_PLUS = 9650; // 10000 Ohms (9900 for switch mode test board, 9650 for Ural)
const uint16_t RESISTOR_TO_GND = 1000; // 1000 Ohms
const float DIODE_DROP = 0.0; // About 0.34V for SS34 diode (0 for Ural)
volatile int outOfFuelVolumePercentage = 80; // Adjust the message volume in %
// Select the out of fuel message you want:
//#include "vehicles/sounds/OutOfFuelEnglish.h"
#include "vehicles/sounds/OutOfFuelGerman.h"
//#include "vehicles/sounds/OutOfFuelFrench.h"
//#include "vehicles/sounds/OutOfFuelDutch.h"
//#include "vehicles/sounds/OutOfFuelSpanish.h"
//#include "vehicles/sounds/OutOfFuelPortuguese.h"
//#include "vehicles/sounds/OutOfFuelJapanese.h"
//#include "vehicles/sounds/OutOfFuelChinese.h"
//#include "vehicles/sounds/OutOfFuelTurkish.h"
//#include "vehicles/sounds/OutOfFuelRussian.h"
