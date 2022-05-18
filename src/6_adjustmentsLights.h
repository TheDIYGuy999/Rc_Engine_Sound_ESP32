#include <Arduino.h>

/* General LIGHTS SETTINGS ************************************************************************************************
 *  
 * Most lights settings are done in the vehicle configuration files in the /vehicles/ directory.
 * 
 */

// Neopixel settings (experimental)--------------------------------------------------------------------------------------------------------------- 
#define NEOPIXEL_ENABLED // GPIO0 used for WS2812 Neopixel control, if defined!
#define NEOPIXEL_COUNT 8 // How many Neopixels? 8 for Knight Rider, 3 for B33lz3bub
#define NEOPIXEL_BRIGHTNESS  5 // Brightness (255 is the maximum)

// Choose the mode you want
//#define NEOPIXEL_DEMO // Demo
#define NEOPIXEL_KNIGHT_RIDER // Knight Rider scanner animation
//#define NEOPIXEL_B33LZ3BUB // B33lz3bub Austria animation: https://www.rc-modellbau-portal.de/index.php?threads/baubericht-mercedes-actros-1851-gigaspace-tamiya.14349/page-3


// The following options may already be defined in the vehicle file. Don't uncomment them in this case or you will get a redefinition error ------
//#define XENON_LIGHTS // Headlights will show a xenon bulb ignition flash, if defined
//#define SEPARATE_FULL_BEAM // The full beam is a separate bulb, connected to roof lights pin. Connect roof lights to side lights in this case
//#define INDICATOR_SIDE_MARKERS // The indicators are used as side markers as well. This is commonly used in US trucks.

// More light configurations ---------------------------------------------------------------------------------------------------------------------
//#define NO_CABLIGHTS // The cablights step in the lights sequence is skipped, if defined
//#define NO_FOGLIGHTS // The foglights step in the lights sequence is skipped, if defined
//#define THIRD_BRAKELIGHT // if defined, pin 32 is used for a third brake light, otherwise for a trailer presence switch (pulled to GND = no trailer attached)
#define FLICKERING_WHILE_CRANKING // The lights will flicker a bit during engine cranking, otherwise they are just a bit dimmed
#define HAZARDS_WHILE_5TH_WHEEL_UNLOCKED // Hazards on, if 5th wheel is unlocked

// Adjust the brightness of your lights here -----------------------------------------------------------------------------------------------------
// All brightness values 0 - 255
uint8_t cabLightsBrightness = 255; // Usually 255, 100 for Actros
uint8_t sideLightsBrightness = 200; // Usually 200, 100 for WPL C44, 50 for Landy, 100 for P407, 150 for Actros
uint8_t rearlightDimmedBrightness = 30; // tailligt brigthness, if not braking, about 30
uint8_t rearlightParkingBrightness = 0; // 0, if you want the taillights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
uint8_t headlightParkingBrightness = 0; // 0, if you want the headlights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
uint8_t reversingLightBrightness = 140; // Around 140, 50 for Landy
