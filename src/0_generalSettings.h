#include <Arduino.h>
#include <WiFi.h>

/* GENERAL SETTINGS *********************************************************************************************************************************
 *  Have a look at the serial monitor in order to check the current settings of your vehicle!
 *  The required login informations for the browser based configuration via 192.186.4.1 can also be found there.
 */

// Hardware settings -------------------------------------------------------------------------------------------------------------------------------
//#define WEMOS_D1_MINI_ESP32 // Software is running on ESP32 RC Trailer controller, if defined (Headlights on GPIO 22 instead of 3, no cab lights support)

// Debug settings -----------------------------------------------------------------------------------------------------------------------------------
// DEBUG options can slow down the playback loop! Only uncomment them for debugging, may slow down your system!
//#define DEBUG // More infos such as EEPROM dump on serial monitor, if defined
//#define CHANNEL_DEBUG // uncomment it for input signal & general debugging informations
//#define ESC_DEBUG // uncomment it to debug the ESC
//#define AUTO_TRANS_DEBUG // uncomment it to debug the automatic transmission
//#define MANUAL_TRANS_DEBUG // uncomment it to debug the manual transmission
//#define TRACKED_DEBUG // debugging tracked vehicle mode
//#define SERVO_DEBUG // uncomment it for servo calibration in BUS communication mode
//#define ESPNOW_DEBUG  // uncomment for additional ESP-NOW messages
//#define CORE_DEBUG // Don't use this!

// EEPROM settings ---------------------------------------------------------------------------------------------------------------------------------
uint8_t eeprom_id = 5; // change this id (between 1 and 255, compare with serial monitor), if you want to restore EEPROM defaults (executed if different) <<------------- NOTE!
//#define ERASE_EEPROM_ON_BOOT // EEPROM is completely overwritten, if defined! Never define it, vehicle will not work!
// only define it in order to clean up junk from old projects in your EEPROM

// Wireless settings -------------------------------------------------------------------------------------------------------------------------------
//#define ENABLE_WIRELESS // Define this, if you want to use an ESP-Now wireless trailer or the WiFi configuration via 192.168.4.1

/* Wifi & ESP-Now and ESP-Now transmission power: less power = less speaker noise & longer battery life. Valid options are:
WIFI_POWER_19_5dBm = 78     // full power
WIFI_POWER_19dBm = 76
WIFI_POWER_18_5dBm = 74
WIFI_POWER_17dBm = 68
WIFI_POWER_15dBm = 60
WIFI_POWER_13dBm = 52
WIFI_POWER_11dBm = 44
WIFI_POWER_8_5dBm = 34
WIFI_POWER_7dBm = 28         // recommended setting
WIFI_POWER_5dBm = 20
WIFI_POWER_2dBm = 8          // lowest setting, WiFi may become weak
*/ 
wifi_power_t cpType = WIFI_POWER_7dBm; // Only use values from above!

// WiFi settings (for vehicle configuration website, open 192.168.4.1 in your browser)--------------------------------------------------------------
// Note: if these credentials were changed, using the configuration website, 
// you can find the current ones in the serial monitor!
String default_ssid = "My_Truck"; // Select this network
String default_password = "123456789"; // Then enter this password

// Configuration website settings -------------------------------------------------------------------------------------------------------------------
#define USE_CSS // Simple, unformatted Website style is used, if not defined
#define MODERN_CSS // Use modern CSS with scaling for better adaption between different devices

// How to read Flash Memory (ignore it, for testing only): ------------------------------------------------------------------------------------------
// Read firmware from ESP32 in terminal below:
// ~/.platformio/packages/tool-esptoolpy/esptool.py -p /dev/cu.SLAB_USBtoUART -b 115200 read_flash 0x00000 0x400000 flash_dump.bin
// export PATH=$PATH:/path/to/python