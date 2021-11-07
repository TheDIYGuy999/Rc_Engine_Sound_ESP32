#include <Arduino.h>

/* TRAILER SETTINGS ************************************************************************************************
 *  
 * The trailer is using ESP NOW. This means, it's communicating, using the 2.4GHz antenna in the ESP32
 */

#define WIRELESS_TRAILER 

// Replace this with your trailers MAC Address
uint8_t broadcastAddress[] = {0x4C, 0xEB, 0xD6, 0x7C, 0x02, 0x74};
