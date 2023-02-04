#include <Arduino.h>

/* DASHBOARD SETTINGS ************************************************************************************************
 *  
 * for the Dashboard by Gamadril :https://github.com/Gamadril/Rc_Engine_Sound_ESP32
 * 
 * Before you can use it (not required, if Visual Studio Code is used instead of Arduino, see platformio.ini in this case):
 * create the folder TFT_eSPI_Setups inside your Arduino TFT_eSPI library directory
 * move/copy the file Setup43_ST7735_ESP32_80x160.h to TFT_eSPI_Setups directory
 * replace the line #include <User_Setup.h> in the file User_Setup_Select.h inside TFT_eSPI library folder with #include <TFT_eSPI_Setups/Setup43_ST7735_ESP32_80x160.h>
 * modify the Setup43_ST7735_ESP32_80x160.h file if you use other pins for the display connection
 */

//#define SPI_DASHBOARD // A 0.96" SPI LCD is used as dashboard: https://www.ebay.ch/itm/174458054566?hash=item289e82a7a6:g:LpAAAOSwtL1fdDtI 
/* WARNING:
 * Pins 18 (SCL), 19 (DC), 21 (RES) & 23 (SDA) are used for the dashboard in this case!
 * The dispay CS pin needs to be connected to GND.
 * Shaker, sidelights and both beacon flashers will not work!
*/

uint8_t dashRotation = 3; // 3 = normal, 1 = upside down
 
#define MAX_REAL_SPEED 110                 // max speed in km/h to show on the dashboard
int manualGearRatios[3] = {305, 165, 100}; // TAMIYA gear ratios: 1st 1:32.49, 2nd 1:17.66, 3rd 1:10.66, value div 100 (used for real shifting transmissions only!)
#define RPM_MAX 500                        // always 500
