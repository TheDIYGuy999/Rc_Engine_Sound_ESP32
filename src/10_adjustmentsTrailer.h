#include <Arduino.h>

/* TRAILER SETTINGS ************************************************************************************************
 *  
 * The trailer is using ESP-NOW. This means it's communicating, using the 2.4GHz antenna in the ESP32
 * You can use a 2nd, identical controller in your trailer. Of course, it doesn't need an amplifier or a receiver.
 */

//#define WIRELESS_TRAILER // Disable it, if no wireless trailer is used. It may cause noise in the speaker, if no trailer is detected.
//#define TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT // wireless trailer lights are swiched off, if truck mounted trailer presence switch on pin 32 is closed.

// IMPORTANT!! Replace the addresses below with your trailers MAC addresses!! --------------------------
// Trailer 1
uint8_t broadcastAddress1[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
//uint8_t broadcastAddress1[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x1E, 0x10}; // US flatbed
//uint8_t broadcastAddress1[] = {0xAC, 0x67, 0xB2, 0x12, 0x30, 0x28}; // white board
//uint8_t broadcastAddress1[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266

// Trailer 2
//#define TRAILER_2 // Uncomment this, if you want to use a 2nd trailer
//uint8_t broadcastAddress2[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
uint8_t broadcastAddress2[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266

// Trailer 3 (EXPERIMENTAL, causing engine RPM issues!)
//#define TRAILER_3 // Uncomment this, if you want to use a 3rd trailer
//uint8_t broadcastAddress3[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // Bruder Low Loader
uint8_t broadcastAddress3[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266
