#include <Arduino.h>

/* TRAILER SETTINGS ************************************************************************************************
 *  
 * The trailer is using ESP-NOW. This means it's communicating, using the 2.4GHz antenna in the ESP32
 * You can use a 2nd, identical controller in your trailer. Of course, it doesn't need an amplifier or a receiver.
 */

//#define WIRELESS_TRAILER // Disable it, if no wireless trailer is used. It may cause noise in the speaker, if no trailer is detected.

// Replace this with your trailers MAC address
//uint8_t broadcastAddress[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74}; // rec
uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x12, 0x30, 0x28}; // white board
//uint8_t broadcastAddress[] = {0xA0, 0x20, 0xA6, 0x10, 0x46, 0x3B}; // D1 Mini ESP8266
