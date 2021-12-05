#include <Arduino.h>

/* General LIGHTS SETTINGS ************************************************************************************************
 *  
 * Most lights settings are done in the vehicle configuration files in the /vehicles/ directory.
 * 
 */
//#define NEOPIXEL_LED // GPIO0 used for WS2812 Neopixel control, if defined!
 

// The following options may already be defined in the vehicle file. Don't uncomment them in this case or you will get a redefinition error ------
//#define XENON_LIGHTS // Headlights will show a xenon bulb ignition flash, if defined
//#define SEPARATE_FULL_BEAM // The full beam is a separate bulb, connected to roof lights pin. Connect roof lights to side lights in this case
//#define INDICATOR_SIDE_MARKERS // The indicators are used as side markers as well. This is commonly used in US trucks.
#define FLICKERING_WHILE_CRANKING // The lights will flicker a bit during engine cranking, otherwise they are just a bit dimmed

// More light configurations ---------------------------------------------------------------------------------------------------------------------
//#define NO_CABLIGHTS // The cablights step in the lights sequence is skipped, if defined
//#define NO_FOGLIGHTS // The foglights step in the lights sequence is skipped, if defined
#define THIRD_BRAKLELIGHT // if defined, pin 32 is used for a third brake light, otherwise for a trailer presence switch

// Adjust the brightness of your lights here -----------------------------------------------------------------------------------------------------
// All brightness values 0 - 255
uint8_t cabLightsBrightness = 255; // Usually 255, 100 for Actros
uint8_t sideLightsBrightness = 200; // Usually 200, 100 for WPL C44, 50 for Landy, 100 for P407, 150 for Actros
uint8_t rearlightDimmedBrightness = 30; // tailligt brigthness, if not braking, about 30
uint8_t rearlightParkingBrightness = 0; // 0, if you want the taillights being off, if side lights are on, or about 5 if you want them on
uint8_t headlightParkingBrightness = 0; // 0, if you want the headlights being off, if side lights are on, or about 5 if you want them on
uint8_t reversingLightBrightness = 140; // Around 140, 50 for Landy
