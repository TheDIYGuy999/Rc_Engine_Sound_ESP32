/* RC engine sound & LED controller for Arduino ESP32. Written by TheDIYGuy999
    Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****
    ESP32 macOS Big Sur fix see: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/BigSurFix.md

   NEW: Visual Studio Code IDE support added (you need to install PlatformIO)
   Arduino IDE is supported as before, but stuff was renamed and moved to different folders!
*/

const float codeVersion = 0.1; // Software revision.

//
// =======================================================================================================
// LIRBARIES & HEADER FILES, REQUIRED ESP32 BOARD DEFINITION
// =======================================================================================================
//

// Libraries (you have to install all of them in the "Arduino sketchbook"/libraries folder)
// !! Do NOT install the libraries in the sketch folder.
// No manual library download is required in Visual Studio Code IDE (see platformio.ini)
#include <statusLED.h> // https://github.com/TheDIYGuy999/statusLED <<------- required for LED control

// No need to install these, they come with the ESP32 board definition
#include <esp_now.h>
#include <WiFi.h>

// The following tasks are not required for Visual Studio Code IDE! ----
// Install ESP32 board according to: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
// Warning: do not use Espressif ESP32 board definition v1.05, its causing crash & reboot loops! Use v1.04 instead.
// Adjust board settings according to: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/pictures/settings.png

// Make sure to remove -master from your sketch folder name

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

#define TAILLIGHT_PIN 15 // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4 // Orange right indicator (turn signal) light
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define SIDELIGHT_PIN 18 // Side lights

// Objects *************************************************************************************
// Status LED objects -----
statusLED tailLight(false); // "false" = output not inversed
statusLED indicatorL(false);
statusLED indicatorR(false);
statusLED reversingLight(false);
statusLED sideLight(false);

// Global variables **********************************************************************

esp_now_peer_info_t peerInfo; // This MUST be global!! Transmission is not working otherwise!

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
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
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
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower (WIFI_POWER_MINUS_1dBm);
  WiFi.disconnect();

  // Print MAC address (this is the required MAC address in the sender)

  Serial.println("Trailer MAC address (you need to add it in '10_adjustmentsTrailer.h' in the sound controller): ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
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

  // LED setup (note, that we only have timers from 0 - 15)
  tailLight.begin(TAILLIGHT_PIN, 2, 20000); // Timer 2, 20kHz
  sideLight.begin(SIDELIGHT_PIN, 3, 20000); // Timer 3, 20kHz
  reversingLight.begin(REVERSING_LIGHT_PIN, 4, 20000); // Timer 4, 20kHz
  indicatorL.begin(INDICATOR_LEFT_PIN, 5, 20000); // Timer 5, 20kHz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 6, 20000); // Timer 6, 20kHz
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

void led() {

  tailLight.pwm(trailerData.tailLight);
  sideLight.pwm(trailerData.sideLight);
  reversingLight.pwm(trailerData.reversingLight);
  indicatorL.pwm(trailerData.indicatorL);
  indicatorR.pwm(trailerData.indicatorR);
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop() {
 // No loop required!
}
