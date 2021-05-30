/*
  Blink LED's
  This program intends to check the LED's on the RC engine sound & LED controller for Arduino ESP32: https://github.com/TheDIYGuy999/Rc_Engine_Sound created by TheDIYGuy999
  Turns secventiualy the 12 LED's on for one second, then start from the begining
  Written by Skorpishor https://www.rc-modellbau-portal.de/index.php?threads/esp32-arduino-rc-sound-und-licht-controller.7183/post-330603
*/

#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4 // Orange right indicator (turn signal) light
#define FOGLIGHT_PIN 16 // (16 = RX2) Fog lights
#define ROOFLIGHT_PIN 5 // Roof lights (high beam, if "define SEPARATE_FULL_BEAM")
#define SIDELIGHT_PIN 18 // Side lights (connect roof ligthts here, if "define SEPARATE_FULL_BEAM")
#define BEACON_LIGHT2_PIN 19 // Blue beacons light
#define BEACON_LIGHT1_PIN 21 // Blue beacons light
#define HEADLIGHT_PIN 3 // 3 = RX0 pin, (1 = TX0 is not usable) white headllights
#define CABLIGHT_PIN 22 // Cabin lights
#define TAILLIGHT_PIN 15 // Red tail- & brake-lights (combined)
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define BRAKELIGHT_PIN 32 // Upper brake lights

int LedPins [12] = {INDICATOR_LEFT_PIN , INDICATOR_RIGHT_PIN, FOGLIGHT_PIN, ROOFLIGHT_PIN, SIDELIGHT_PIN, BEACON_LIGHT2_PIN, BEACON_LIGHT1_PIN, HEADLIGHT_PIN, CABLIGHT_PIN , TAILLIGHT_PIN,  REVERSING_LIGHT_PIN,  BRAKELIGHT_PIN};

#define SHAKER_MOTOR_PIN 23

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  for ( int i = 0; i < 12; i++)
  {
    pinMode(LedPins[i], OUTPUT);
  }
}

// the loop function runs over and over again forever
void loop() {
  for ( int i = 0; i < 12; i++)
  {
    digitalWrite(LedPins[i], HIGH);
    delay(1000);
    digitalWrite(LedPins[i], LOW);
  }
}
