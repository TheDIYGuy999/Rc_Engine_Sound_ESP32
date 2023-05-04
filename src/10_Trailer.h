#include <Arduino.h>

/* TRAILER SETTINGS ************************************************************************************************

   The trailer is using ESP-NOW. This means it's communicating, using the 2.4GHz antenna in the ESP32
   You can use a 2nd, identical controller in your trailer. Of course, it doesn't need an amplifier or a receiver.
*/

// #define TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT // wireless trailer lights are swiched off, if truck mounted trailer presence switch on pin 32 is closed.

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
uint8_t defaultBroadcastAddress1[] = {0xFE, 0x49, 0x01, 0x00, 0x00, 0x01}; // Generic

// Trailer 2
bool defaultUseTrailer2 = false; // Use Trailer 2 or not
uint8_t defaultBroadcastAddress2[] = {0xFE, 0x49, 0x01, 0x00, 0x00, 0x02}; // Generic

// Trailer 3
bool defaultUseTrailer3 = false; // Use Trailer 3 or not
uint8_t defaultBroadcastAddress3[] = {0xFE, 0x49, 0x01, 0x00, 0x00, 0x03}; // Generic
