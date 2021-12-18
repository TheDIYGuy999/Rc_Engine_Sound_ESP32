/* Trailer for RC engine sound & LED controller for Arduino ESP32. Written by TheDIYGuy999

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****
    ESP32 macOS Big Sur fix see: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/BigSurFix.md
*/

const float codeVersion = 0.3; // Software revision.

//
// =======================================================================================================
// ! ! I M P O R T A N T ! !   ALL USER SETTINGS ARE DONE IN THE FOLLOWING TABS, WHICH ARE DISPLAYED ABOVE
// (ADJUST THEM BEFORE CODE UPLOAD), DO NOT CHANGE ANYTHING IN THIS TAB EXCEPT THE DEBUG OPTIONS
// =======================================================================================================
//

// All the required user settings are done in the following .h files:
#include "7_adjustmentsServos.h"        // <<------- Servo output related adjustments

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
#include "driver/mcpwm.h"  // for servo PWM output
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
// Pin assignment and wiring instructions ******************************************************

// ------------------------------------------------------------------------------------
// Use a 330Ohm resistor in series with all I/O pins! allows to drive LED directly and
// provides short circuit protection. Also works on the serial Rx pin "VP" (36)
// ------------------------------------------------------------------------------------

#define TAILLIGHT_PIN 15 // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4 // Orange right indicator (turn signal) light
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define SIDELIGHT_PIN 18 // Side lights

#define SERVO_1_PIN 13 // Servo CH1 legs
#define SERVO_2_PIN 12 // Servo CH2 ramps
#define SERVO_3_PIN 14 // Servo CH3 beacon control
#define SERVO_4_PIN 27 // Servo CH4 spare servo channel

#define FIFTH_WHEEL_DETECTION_PIN 32 // This NO switch is closed, if the trailer is coupled to the 5th wheel. Between Pin 32 and GND

// Objects *************************************************************************************
// Status LED objects -----
statusLED tailLight(false); // "false" = output not inversed
statusLED indicatorL(false);
statusLED indicatorR(false);
statusLED reversingLight(false);
statusLED sideLight(false);

// Global variables ****************************************************************************

esp_now_peer_info_t peerInfo; // This MUST be global!! Transmission is not working otherwise!

typedef struct struct_message { // This is the data packet
  uint8_t tailLight;
  uint8_t sideLight;
  uint8_t reversingLight;
  uint8_t indicatorL;
  uint8_t indicatorR;
  bool legsUp;
  bool legsDown;
  bool rampsUp;
  bool rampsDown;
  bool beaconsOn;
} struct_message;

// Create a struct_message called trailerData
struct_message trailerData;

bool trailerCoupled;

//
// =======================================================================================================
// ESP NOW TRAILER DATA RECEIVED CALLBACK
// =======================================================================================================
//

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&trailerData, incomingData, sizeof(trailerData));

  //led();

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
  Serial.print("Legs up: ");
  Serial.println(trailerData.legsUp);
  Serial.print("Legs down: ");
  Serial.println(trailerData.legsDown);
  Serial.print("Ramps up: ");
  Serial.println(trailerData.rampsUp);
  Serial.print("Ramps down: ");
  Serial.println(trailerData.rampsDown);
  Serial.print("Beacons on: ");
  Serial.println(trailerData.beaconsOn);

  Serial.println();
}

//
// =======================================================================================================
// mcpwm SETUP (1x during startup)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void setupMcpwm() {
  // 1. set our servo output pins
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_1_PIN);    //Set legs as PWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, SERVO_2_PIN);    //Set ramps as PWM0B
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, SERVO_4_PIN);    //Set beacon as PWM1A
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, SERVO_3_PIN);    //Set spare servo as PWM1B

  // 2. configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = SERVO_FREQUENCY;     //frequency usually = 50Hz, some servos may run smoother @ 100Hz
  pwm_config.cmpr_a = 0;                      //duty cycle of PWMxa = 0
  pwm_config.cmpr_b = 0;                      //duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;   // 0 = not inverted, 1 = inverted

  // 3. configure channels with settings above
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B
  mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &pwm_config);    //Configure PWM1A & PWM1B
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

  pinMode(FIFTH_WHEEL_DETECTION_PIN, INPUT_PULLUP);

  // ESP NOW setup
  setupEspNow();

  // LED setup (note, that we only have timers from 0 - 15)
  tailLight.begin(TAILLIGHT_PIN, 2, 20000); // Timer 2, 20kHz
  sideLight.begin(SIDELIGHT_PIN, 3, 20000); // Timer 3, 20kHz
  reversingLight.begin(REVERSING_LIGHT_PIN, 4, 20000); // Timer 4, 20kHz
  indicatorL.begin(INDICATOR_LEFT_PIN, 5, 20000); // Timer 5, 20kHz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 6, 20000); // Timer 6, 20kHz

  tailLight.pwm(255);
  sideLight.pwm(255);
  reversingLight.pwm(255);
  indicatorL.pwm(255);
  indicatorR.pwm(255);

  delay(500); // LED test 0.5s (all on)

  tailLight.pwm(0);
  sideLight.pwm(0);
  reversingLight.pwm(0);
  indicatorL.pwm(0);
  indicatorR.pwm(0);

  setupMcpwm(); // mcpwm servo output setup
}

//
// =======================================================================================================
// 5TH WHEEL DETECTION SWITCH
// =======================================================================================================
//

void switchDetect() {
  static unsigned long switchMillis;

  if (!digitalRead(FIFTH_WHEEL_DETECTION_PIN)) {
    switchMillis = millis(); // if coupled
    //Serial.println("coupled");
  }

  trailerCoupled = (millis() - switchMillis <= 1000); // 1s delay, if not coupled
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

void led() {

  if (trailerCoupled) {
    tailLight.pwm(trailerData.tailLight);
    sideLight.pwm(trailerData.sideLight);
    reversingLight.pwm(trailerData.reversingLight);
    indicatorL.pwm(trailerData.indicatorL);
    indicatorR.pwm(trailerData.indicatorR);
  }
  else {
    tailLight.pwm(0);
    sideLight.pwm(0);
    reversingLight.pwm(0);
    indicatorL.pwm(0);
    indicatorR.pwm(0);
  }
}

//
// =======================================================================================================
// ROTATING BEACON CONTROL (disconnect USB & battery after upload for correct beacon function)
// =======================================================================================================
//

bool beaconControl(uint8_t pulses) {

  /* Beacons: "RC DIY LED Rotating Beacon Light Flash For 1/10 Truck Crawler Toy"
      from: https://www.ebay.ch/itm/303979210629
      States (every servo signal change from 1000 to 2000us will switch to the next state):
      0 rotating beacon slow
      1 Rotating beacon slow
      2 4x flash
      3 endless flash
      4 off

  */

  static unsigned long pulseMillis;
  static unsigned long pulseWidth = CH3L;
  static uint8_t i;

  if (millis() - pulseMillis > 40) { // Every 40ms (this is the required minimum)
    pulseMillis = millis();
    if (pulseWidth == CH3L) {
      pulseWidth = CH3R;
    }
    else {
      pulseWidth = CH3L;
      i++;
    }
    mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, pulseWidth);
  }

  if (i >= pulses) {
    i = 0;
    return true;
  }
  else return false;
}

//
// =======================================================================================================
// MCPWM SERVO RC SIGNAL OUTPUT (BUS communication mode only)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void mcpwmOutput() {

  // Legs servo CH1 (active, if 5th wheel is unlocked, use horn pot) **********************

#ifdef LEGS_ESC_MODE // ESC mode
  static uint16_t legsServoMicros = CH1L;
  if (trailerData.legsDown) legsServoMicros = CH1L; // down
  else if (trailerData.legsUp) legsServoMicros = CH1R; // up
  else legsServoMicros = CH1C; // off

#else // Servo mode  
  static uint16_t legsServoMicrosTarget = CH1L;
  static uint16_t legsServoMicros = CH1L;
  static unsigned long legsDelayMicros;
  if (micros() - legsDelayMicros > LEGS_RAMP_TIME) {
    legsDelayMicros = micros();
    if (trailerData.legsDown) legsServoMicrosTarget = CH1L; // down
    else if (trailerData.legsUp) legsServoMicrosTarget = CH1R; // up
    else legsServoMicrosTarget = legsServoMicros; // stop
    if (legsServoMicros < legsServoMicrosTarget) legsServoMicros ++;
    if (legsServoMicros > legsServoMicrosTarget) legsServoMicros --;
  }
#endif
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, legsServoMicros);

  // Ramps servo CH2 (active, if hazards are on, use horn pot) *****************************
  static uint16_t rampsServoMicrosTarget = CH2R;
  static uint16_t rampsServoMicros = CH2R;
  static unsigned long rampsDelayMicros;
  if (micros() - rampsDelayMicros > RAMPS_RAMP_TIME) {
    rampsDelayMicros = micros();
    if (trailerData.rampsDown) rampsServoMicrosTarget = CH2L; // down
    else if (trailerData.rampsUp) rampsServoMicrosTarget = CH2R; // up
    else rampsServoMicrosTarget = rampsServoMicros; // stop
    if (rampsServoMicros < rampsServoMicrosTarget) rampsServoMicros ++;
    if (rampsServoMicros > rampsServoMicrosTarget) rampsServoMicros --;
  }
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, rampsServoMicros);

  // Beacon control CH3 (use horn / blue light pot)******************************************
  // Init (5 pulses are required to shut beacons off after power on)
  static bool blueLightInit;
  if (!blueLightInit) {
    if (beaconControl(5)) blueLightInit = true;
  }

  // Switching modes
  static uint16_t beaconServoMicros;
  static bool lockRotating, lockOff;
  if (blueLightInit) {
    if (trailerData.beaconsOn && !lockRotating) { // Rotating mode on (1 pulse)
      if (beaconControl(1)) {
        lockRotating = true;
        lockOff = false;
      }
    }
    if (!trailerData.beaconsOn && !lockOff && lockRotating) { // Off (4 pulses)
      if (beaconControl(4)) {
        lockOff = true;
        lockRotating = false;
      }
    }
  }
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop() {

  switchDetect();
  mcpwmOutput();
  led();
}
