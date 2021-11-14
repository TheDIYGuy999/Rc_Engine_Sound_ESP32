/* Trailer for RC engine sound & LED controller for Arduino ESP8266. Written by TheDIYGuy999*/

const float codeVersion = 0.1; // Software revision.

//
// =======================================================================================================
// LIRBARIES & HEADER FILES, REQUIRED ESP32 BOARD DEFINITION
// =======================================================================================================
//

// No need to install these, they come with the ESP32 board definition
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Install ESP8266 board according to: https://github.com/esp8266/Arduino
// Upload speed : 921600
// CPU Frequency: 160 MHz

//
// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES (Do not play around here)
// =======================================================================================================
//
// Pin assignment and wiring instructions ****************************************************************

// ------------------------------------------------------------------------------------
// Use a 330Ohm resistor in series with all I/O pins! allows to drive LED directly and
// provides short circuit protection. Also works on the serial Rx pin "VP" (36)
// ------------------------------------------------------------------------------------

#define TAILLIGHT_PIN 1 // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light (onboard LED)
#define INDICATOR_RIGHT_PIN 3 // Orange right indicator (turn signal) light
#define REVERSING_LIGHT_PIN 4 // (TX2) White reversing light
#define SIDELIGHT_PIN 5 // Side lights

// Global variables **********************************************************************

typedef struct struct_message { // This is the data packet
  uint8_t tailLight;
  uint8_t sideLight;
  uint8_t reversingLight;
  uint8_t indicatorL;
  uint8_t indicatorR;
} struct_message;

// Create a struct_message called trailerData
struct_message trailerData;

//
// =======================================================================================================
// ESP NOW TRAILER DATA RECEIVED CALLBACK
// =======================================================================================================
//

// callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&trailerData, incomingData, sizeof(trailerData));

  led();

  Serial.print("Tailllight: ");
  Serial.println(trailerData.tailLight);
  Serial.print("Sidelight: ");
  Serial.println(trailerData.sideLight);
  Serial.print("Reversing light: ");
  Serial.println(trailerData.reversingLight);
  Serial.print("Indicator L: ");
  Serial.println(trailerData.indicatorL);
  Serial.print("Indicator R: ");
  Serial.println(trailerData.indicatorR);
  Serial.println();
}

//
// =======================================================================================================
// ESP NOW SETUP FOR WIRELESS TRAILER CONTROL
// =======================================================================================================
//

void setupEspNow() {

  WiFi.disconnect();
  ESP.eraseConfig();
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);


  // Print MAC address (this is the required MAC address in the sender)

  Serial.println("Trailer MAC address (you need to add it in '10_adjustmentsTrailer.h' in the sound controller): ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

  // Serial setup
  Serial.begin(115200); // USB serial (for DEBUG)

  // ESP NOW setup
  setupEspNow();
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

void led() {

  analogWrite(TAILLIGHT_PIN, map(trailerData.tailLight, 0, 255, 0, 1023));
  analogWrite(INDICATOR_LEFT_PIN, map(trailerData.indicatorL, 0, 255, 0, 1023));
  analogWrite(INDICATOR_RIGHT_PIN, map(trailerData.indicatorR, 0, 255, 0, 1023));
  analogWrite(REVERSING_LIGHT_PIN, map(trailerData.reversingLight, 0, 255, 0, 1023));
  analogWrite(SIDELIGHT_PIN, map(trailerData.sideLight, 0, 255, 0, 1023));
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop() {
  // No loop required!
}
