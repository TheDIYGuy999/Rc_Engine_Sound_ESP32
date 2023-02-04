#include <Arduino.h>

/* TRAILER SETTINGS ************************************************************************************************

   The trailer is using ESP-NOW. This means it's communicating, using the 2.4GHz antenna in the ESP32
   You can use a 2nd, identical controller in your trailer. Of course, it doesn't need an amplifier or a receiver.
*/

//#define TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT // wireless trailer lights are swiched off, if truck mounted trailer presence switch on pin 32 is closed.

// MAC addresses for communication with trailers:
// Only addresses with low multicast bit are valid: http://sqa.fyicenter.com/1000208_MAC_Address_Validator.html
// This means, that the least significant bit of the first byte from the left always needs to be zero!
// Also, the second last significant bit of this byte should  always be high = locally administrated.
// So, it is recommended to use 0xFE for the first byte from the left.
// Use hex calculator for verification!
// This results in the following numbering schematic recommendation:
// ***********************************************************************************************************************************
// Always FE : Country number (phone country code) : Region number (phone area code) : User Number 1 : User Number 2 : Trailer Number
// ***********************************************************************************************************************************
// Please use your own addresses, otherwise you could get in trouble and you are suddenly controlling the trailer of someone else! <<------------- NOTE!

// IMPORTANT!! Replace the addresses below with your trailers MAC addresses!! --------------------------

// Note: these options can be changed, using the wireless configuration via 192.168.4.1

// Trailer 1
bool defaultUseTrailer1 = true; // Use Trailer 1 or not
//uint8_t defaultBroadcastAddress1[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
//uint8_t defaultBroadcastAddress1[] = {0xAC, 0x67, 0xB2, 0x37, 0xF3, 0x7C}; // SCANIA S1  (this address is causing engine revving issues, if used in combination with the trailer 2 Scania address!)
//uint8_t defaultBroadcastAddress1[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x1E, 0x10}; // US flatbed
//uint8_t defaultBroadcastAddress1[] = {0xAC, 0x67, 0xB2, 0x12, 0x30, 0x28}; // white board
//uint8_t defaultBroadcastAddress1[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266
uint8_t defaultBroadcastAddress1[] = {0xFE, 0x41, 0x41, 0x09, 0x99, 0x01}; // Don't use it, TheDIYGuy999 custom address only!

// Trailer 2
bool defaultUseTrailer2 = false; // Use Trailer 2 or not
//uint8_t defaultBroadcastAddress2[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
//uint8_t defaultBroadcastAddress2[] = {0x34, 0x86, 0x5D, 0x3A, 0xF1, 0x5C}; //SCANIA S2
//uint8_t defaultBroadcastAddress2[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266
uint8_t defaultBroadcastAddress2[] = {0xFE, 0x41, 0x41, 0x09, 0x99, 0x02}; // Don't use it, TheDIYGuy999 custom address only!

// Trailer 3
bool defaultUseTrailer3 = false; // Use Trailer 3 or not
//uint8_t defaultBroadcastAddress3[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
//uint8_t defaultBroadcastAddress3[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266
uint8_t defaultBroadcastAddress3[] = {0xFE, 0x41, 0x41, 0x09, 0x99, 0x03}; // Don't use it, TheDIYGuy999 custom address only!
