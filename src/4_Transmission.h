#include <Arduino.h>

/* General TRANSMISSION SETTINGS ************************************************************************************************************
 *  
 * Most transmission settings like automatic, double clutch etc. are done in the vehicle configuration files in the /vehicles/ directory.
 * 
 */

// The following transmission modes are ignored in "automatic" or "doubleClutch" vehicles ==================================================
// Never uncomment more than one! If no option is defined, you can use a real 3 speed transmission, for example from TAMIYA

// #define VIRTUAL_3_SPEED allows to simulate a 3 speed shifing transmission, if your vehicle doesn't have a real one.
// Gears are virtually shifted, using the 3 position switch. Example: your crawler has a 2 speed transmission, which is used as off road reducer,
// but no real 3 speed shifting transmission. Don't uncomment if for vehicles with electric or hydrostatic drive or automatic transmissions!
// Also don't use it for STEAM_LOCOMOTIVE_MODE
//#define VIRTUAL_3_SPEED

//#define VIRTUAL_16_SPEED_SEQUENTIAL will enable a sequencial transmission, shifted by up / down impulses via 3 position switch
//#define VIRTUAL_16_SPEED_SEQUENTIAL // This is still experimental and not working properly! Don't use it.

// Additional transmission options =========================================================================================================
// Automatic transmission with overdrive (lower RPM in top gear, gear ratio lower than 1:1, 4 & 6 speed only)
// Also usable in combination with VIRTUAL_3_SPEED. The 4th gear is switched automatically in this case, if driving in 3rd gear @ full throttle
#define OVERDRIVE // Don't use it for: doubleClutch. Not working with SEMI_AUTOMATIC, but you can leave it on in this case.

// In some cases we want a different reverse acceleration for automatic transmission vehicles.
uint16_t automaticReverseAccelerationPercentage = 100;

// Low range percentage is used for MODE1_SHIFTING (off road reducer)
uint16_t lowRangePercentage = 58;// WPL 2 speed ratios = 29:1, 17:1 = 58% in low range. You may want to change this for other 2 speed transmissions

// Transmission controls options ===========================================================================================================
// #define SEMI_AUTOMATIC This will simulate a semi automatic transmission. Shifting is not controlled by the 3 position switch in this mode!
//#define SEMI_AUTOMATIC // Works for VIRTUAL_3_SPEED or real 3 speed transmission. Don't select this @ the same time as VIRTUAL_16_SPEED_SEQUENTIAL

//#define MODE1_SHIFTING The 2 speed transmission is shifted by the "Mode 1" button instead of the 3 position switch.
// This is often used in WPL vehicles with 2 speed transmission, used as off road reducer, shifted while driving slowly in order to engage properly.
//#define MODE1_SHIFTING

//#define TRANSMISSION_NEUTRAL Allows to put the transmission in neutral. This can't be used, if the "Mode 1" button is used for other stuff!
// You can leave it on, if defined MODE1_SHIFTING. It is disabled automatically in this case.
#define TRANSMISSION_NEUTRAL

// Clutch options ==========================================================================================================================
uint16_t maxClutchSlippingRpm = 250; // The clutch will never slip above this limit! (about 250) 500 for vehicles like locomotives
// and the Kirovets tractor with hydrostatic or electric drive! Mainly required for "VIRTUAL_3_SPEED" mode

//#define DOUBLE_CLUTCH // Double-clutch (Zwischengas) Enable this for older manual transmission trucks without synchronised gears

//#define HIGH_SLIPPINGPOINT // Clutch will engage @ higher RPM, if defined. Comment this out for heavy vehicles like semi trucks
