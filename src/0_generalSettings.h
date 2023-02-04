#include <Arduino.h>

/* GENERAL SETTINGS ************************************************************************************************
 *  Have a look at the serial monitor in order to check the current settings of your vehicle!
 *  The required login informations for the browser based configuration via 192.186.4.1 can also be found there.
 */

// Debug settings -----------------------------------------------------------------------------------------------
// DEBUG options can slow down the playback loop! Only uncomment them for debugging, may slow down your system!
#define DEBUG // More infos such as EEPROM dump on serial monitor, if defined
//#define CHANNEL_DEBUG // uncomment it for input signal & general debugging informations
//#define ESC_DEBUG // uncomment it to debug the ESC
//#define AUTO_TRANS_DEBUG // uncomment it to debug the automatic transmission
//#define MANUAL_TRANS_DEBUG // uncomment it to debug the manual transmission
//#define TRACKED_DEBUG // debugging tracked vehicle mode
//#define SERVO_DEBUG // uncomment it for servo calibration in BUS communication mode
//#define ESPNOW_DEBUG  // uncomment for additional ESP-NOW messages
//#define CORE_DEBUG // Don't use this!

// EEPROM settings ----------------------------------------------------------------------------------------------
uint8_t eeprom_id = 2; // change this id (between 1 and 255, compare with serial monitor), if you want to restore EEPROM defaults (only executed 1x) <<------------- NOTE!
//#define ERASE_EEPROM_ON_BOOT // EEPROM is completely overwritten, if defined! Never define it, vehicle will not work!
// only define it in order to clean up junk from old projects in your EEPROM

// Wireless settings --------------------------------------------------------------------------------------------
#define ENABLE_WIRELESS // Define this, if you want to use an ESP-Now wireless trailer or the WiFi configuration via 192.168.4.1

// WiFi settings (for vehicle configuration website, open 192.168.4.1 in your browser)----------------------------
// Note: if these credentials were changed, using the configuration website, 
// you can find the current ones in the serial monitor!
String default_ssid = "My_Truck"; // Select this network
String default_password = "123456789"; // Then enter this password

// Configuration website settings --------------------------------------------------------------------------------
#define USE_CSS // Simple Website style is used, if not defined
