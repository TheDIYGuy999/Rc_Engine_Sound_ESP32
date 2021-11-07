#include <Arduino.h>

/* General TRANSMISSION SETTINGS ************************************************************************************************
 *  
 * Most transmission settings like automatic, double clutch etc. are done in the vehicle configuration files in the /vehicles/ directory.
 * 
 */

// #define VIRTUAL_3_SPEED allows to simulate a 3 speed shifing transmission, if your vehicle doesn't have a real one.
// Gears are shifted, using the 3 position switch. Example: your crawler has a 2 speed transmission, which is used as off road reducer,
// but no real 3 speed shifting transmission. Don't uncomment if for vehicles with electric or hydrostatic drive or automatic transmissions!
//#define VIRTUAL_3_SPEED
//#define VIRTUAL_16_SPEED_SEQUENTIAL // This is still experimental!

//#define TRANSMISSION_NEUTRAL Allows to put the transmission in neutral. This can't be used, if the "Mode 1" button is used for other stuff!
//#define TRANSMISSION_NEUTRAL

//#define MODE1_SHIFTING The 2 speed transmission is shifted by the "Mode 1" button instead of the 3 position switch (Micro RC remote only).
// This is often used in WPL vehicles with 2 speed transmission, used as off road reducer
//#define MODE1_SHIFTING

uint16_t maxClutchSlippingRpm = 300; // The clutch will never slip above this limit! (about 300) 500 for vehicles like locomotives
// and the Kirovets tractor with hydrostatic or electric drive! Mainly required for "VIRTUAL_3_SPEED" mode

// In some cases we want a faster reverse acceleration for automatic transmission vehicles. Around 170% for Landy
uint16_t automaticReverseAccelerationPercentage = 100;

// Automatic transmission with overdrive (lower RPM in top gear, gear ratio lower than 1:1, 4 & 6 speed only)
#define OVERDRIVE // This is usually on, but don't use it for double clutch transmissions!
