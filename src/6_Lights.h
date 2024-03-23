#include <Arduino.h>

/* LIGHTS SETTINGS ******************************************************************************************************************************************
 *
 * All light settings are now done here and are not part of the vehicle file anymore
 * All #define settings are only adjustable during compile time and are not changeable afterwards!
 * 
 * The other ones can be changed here or on 192.168.4.1, if connected to your vehicles WiFi access point
 * Note, that the settings are only applied on a new or comletely erased ESP32. Afterwards they are stored in the EEPROM
 * They can also be applied by changing the "eeprom_id" in "0_GeneralSettings"
 */

// Neopixel settings ----------------------------------------------------------------------------------------------------------------------------------------
#define NEOPIXEL_ENABLED     // GPIO0 used for WS2812 Neopixel control, if defined! Always enabled, unless you have issues with the library!
#if defined NEOPIXEL_ENABLED // only selectable, if NEOPIXELS are in use ---
// #define NEOPIXEL_ON_CH4 // The CH 4 servo header is used as Neopixel output, if defined (BUS communication mode only, intended for RC trailer controller, no coupler servo)
#endif                          // ---
#define NEOPIXEL_COUNT 8        // How many Neopixels? 8 for Knight Rider, 3 for B33lz3bub
#define NEOPIXEL_BRIGHTNESS 127 // Brightness (255 is the maximum)
#define MAX_POWER_MILLIAMPS 100 // Maximum allowed Neopixel current draw in mA (100 recommended, depending on your 5V supply)

#define NEOPIXEL_HIGHBEAM // Neopixel bar is used as high beam as well, if defined. Also usable in combination with NEOPIXEL_KNIGHT_RIDER & NEOPIXEL_BLUELIGHT

// Choose the animation (neopixelMode) you want
/* 1 = Demo (don't use it)
 * 2 = Knight Rider scanner animation for 8 LED (use it in combination with "kittScanner.h" "siren" sound in your vehicle file)
 * 3 = Bluelight animation for 8 LED
 * 4 = Union Jack United Kingdom animation for 8 LED (use it in combination with "BritishNationalAnthemSiren.h" sound in your vehicle file)
 * 5 = B33lz3bub Austria animation for 3 LED: https://www.rc-modellbau-portal.de/index.php?threads/baubericht-mercedes-actros-1851-gigaspace-tamiya.14349/page-3
 */
uint8_t neopixelMode = 2; // See above

// These light settings are adjustabale during compile time only ---------------------------------------------------------------------------------------------
#define THIRD_BRAKELIGHT // if defined, pin 32 is used for a third brake light, otherwise for a trailer presence switch (pulled to GND = no trailer attached)

// These light options are adjustable on the configuration website and stored in the EEPROM -----------------------------------------------------------------
bool noCabLights = false;                 // The cablights step in the lights sequence is skipped, if true
bool noFogLights = false;                 // The foglights step in the lights sequence is skipped, if true
bool xenonLights = false;                 // Headlights will show a xenon bulb ignition flash, if true
bool flickeringWileCranking = false;      // if true: The lights will flicker a bit during engine cranking, otherwise they are just a bit dimmed
bool ledIndicators = false;               // indicators are switching "hard" (without fading like an incandescent bulb), if true (was LED_INDICATORS)
bool swap_L_R_indicators = false;         // If you want to swap L & R indicators
bool indicatorsAsSidemarkers = false;     // "US Mode" The indicators are used as side markers as well. This is commonly used in US trucks.
bool separateFullBeam = false;            // The full beam is a separate bulb, connected to roof lights pin. Connect roof lights to side lights in this case
bool flashingBlueLight = true;            // double flash blue lights if "true", "rotating" beacons if "false"
bool hazardsWhile5thWheelUnlocked = true; // Hazards on, if 5th wheel is unlocked

// Light brightness values 0 - 255
uint8_t cabLightsBrightness = 100;      // Usually 255, 100 for Actros & Ural
uint8_t sideLightsBrightness = 150;     // Usually 200, 100 for WPL C44, 50 for Landy, 100 for P407, 150 for Actros
uint8_t rearlightDimmedBrightness = 30; // tailligt brightness, if not braking, about 30
uint8_t rearlightParkingBrightness = 3; // 0, if you want the taillights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
uint8_t headlightParkingBrightness = 3; // 0, if you want the headlights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
uint8_t reversingLightBrightness = 140; // Around 140, 50 for Landy & Ural
uint8_t fogLightBrightness = 200;       // Around 200
