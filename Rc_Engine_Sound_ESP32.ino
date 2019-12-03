/* RC engine sound simulator for Arduino ESP32. Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

   Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
   converter code by bitluni (send him a high five if you like the code)
   
   Todo:
   - Engine on off
   - more sounds
   - additional sounds like horn
*/

const float codeVersion = 0.2; // Software revision.

// All the required vehicle specific settings are done in settings.h!
#include "settings.h" // <<------- SETTINGS

#define DEBUG // can slow down the playback loop!

//
// =======================================================================================================
// LIRBARIES & HEADER FILES (TABS ABOVE)
// =======================================================================================================
//

#include "curves.h" // load nonlinear throttle curve arrays

//
// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES (Do not play around here)
// =======================================================================================================
//

// Pin assignment
#define THROTTLE_PIN 13 // connect to RC receiver throttle (caution, max. 3.3V!)
#define DAC1 25 // connect the PAM8403 amplifier to pin 25. A 10kOhm potentiometer is required for volume adjustment!

// Define global variables
volatile uint8_t engineState = 2; // 0 = off, 1 = starting, 2 = running, 3 = stopping

volatile boolean engineOn = false;              // Signal for engine on / off

volatile uint16_t curEngineSample;              // Index of currently loaded engine sample
volatile uint16_t curStartSample;               // Index of currently loaded start sample
volatile uint16_t curHornSample;                // Index of currently loaded horn sample

uint16_t  currentThrottle = 0;                  // 0 - 500
volatile int16_t pulseWidth = 0;                // Current RC signal pulse width
volatile boolean pulseAvailable;                // RC signal pulses are coming in

int16_t currentRpm = 0; // 0 - 500
//float currentRpmScaled;
volatile int16_t currentRpmScaled;

int16_t pulseMaxNeutral; // PWM throttle configuration storage variables
int16_t pulseMinNeutral;
int16_t pulseMax;
int16_t pulseMin;
int16_t pulseMaxLimit;
int16_t pulseMinLimit;

// Our main tasks
TaskHandle_t Task1;

// Interrupt timer for playback
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t timerTicks = 500000; // start 500000 * 1 us = 500 ms

// Sampling intervals for interrupt timer (adjusted according to your sound file sampling rate)
uint8_t maxSampleInterval = 1000000 / sampleRate;
uint8_t minSampleInterval = 13; //max((1000000 / sampleRate / TOP_SPEED_MULTIPLIER), 13);

//
// =======================================================================================================
// INTERRUPT FOR VARIABLE SPEED PLAYBACK
// =======================================================================================================
//

void IRAM_ATTR onTimer() {

  portENTER_CRITICAL_ISR(&timerMux);

  timerTicks = currentRpmScaled;  // 45 = idle, 13 is minimum, otherwise the ESP32 crashes! for 24000Hz
  timerAlarmWrite(timer, timerTicks, true); // // change timer ticks, autoreload true

  static unsigned long pos;

  pos ++;
  //if (pos < sampleCount) dacWrite(DAC1, (int)samples[pos] + 128);
  if (pos < sampleCount) curEngineSample = (int)samples[pos] + 128;
  else pos = 0;

  dacWrite(DAC1, curEngineSample);

  portEXIT_CRITICAL_ISR(&timerMux);
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {
  pinMode(THROTTLE_PIN, INPUT);
  Serial.begin(115200);

  // DAC
  dacWrite(DAC1, 128);

  //disableCore0WDT();
  //disableCore1WDT();

  // Task 1 setup (running on core 0)
  TaskHandle_t Task1;
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */

  // Interrupt timer for playback
  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered
  timerAlarmWrite(timer, timerTicks, true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmEnable(timer); // enable

  // wait for RC receiver to initialize
  delay(1000);

  // then compute the RC channel offset (only, if "engineManualOnOff" inactive)
  if (!engineManualOnOff) pulseZero = pulseWidth;

  // Calculate throttle range
  pulseMaxNeutral = pulseZero + pulseNeutral;
  pulseMinNeutral = pulseZero - pulseNeutral;
  pulseMax = pulseZero + pulseSpan;
  pulseMin = pulseZero - pulseSpan;
  pulseMaxLimit = pulseZero + pulseLimit;
  pulseMinLimit = pulseZero - pulseLimit;
}

//
// =======================================================================================================
// GET RC SIGNAL
// =======================================================================================================
//

void getRcSignal() {
  // measure RC signal mark space ratio
  pulseWidth = pulseIn(THROTTLE_PIN, HIGH, 100000);
}

//
// =======================================================================================================
// MAP PULSEWIDTH TO THROTTLE
// =======================================================================================================
//

void mapThrottle() {

  // Input is around 1000 - 2000us, output 0-500 for forward and backwards

  // check if the pulsewidth looks like a servo pulse
  if (pulseWidth > pulseMinLimit && pulseWidth < pulseMaxLimit) {
    if (pulseWidth < pulseMin) pulseWidth = pulseMin; // Constrain the value
    if (pulseWidth > pulseMax) pulseWidth = pulseMax;

    // calculate a throttle value from the pulsewidth signal
    if (pulseWidth > pulseMaxNeutral) currentThrottle = map(pulseWidth, pulseMaxNeutral, pulseMax, 0, 500);
    else if (pulseWidth < pulseMinNeutral) currentThrottle = map(pulseWidth, pulseMinNeutral, pulseMin, 0, 500);
    else currentThrottle = 0;
  }
}

//
// =======================================================================================================
// ENGINE MASS SIMULATION
// =======================================================================================================
//

void engineMassSimulation() {

  static int16_t  mappedThrottle = 0;
  //static int16_t currentRpm = 0; // 0 - 1023
  static unsigned long throtMillis;
  static unsigned long printMillis;

  //if (millis() - throtMillis > 1) { // Every 5ms
  //throtMillis = millis();

  // compute unlinear throttle curve
  //mappedThrottle = reMap(curveShifting, throttleAverage(currentThrottle));  // optional filter
  mappedThrottle = reMap(curveShifting, currentThrottle);

  // Accelerate engine
  if (mappedThrottle > (currentRpm + acc) && (currentRpm + acc) < maxRpm && engineState == 2) {
    currentRpm += acc;
    if (currentRpm > maxRpm) currentRpm = maxRpm;
  }

  // Decelerate engine
  if (mappedThrottle < currentRpm) {
    currentRpm -= dec;
    if (currentRpm < minRpm) currentRpm = minRpm;
  }

  // Speed (sample rate) output
  currentRpmScaled = map(currentRpm, 0, 500, maxSampleInterval, minSampleInterval);
  //}


  // Print debug infos
#ifdef DEBUG // can slow down the playback loop!
  if (millis() - printMillis > 200) { // Every 200ms
    printMillis = millis();

    Serial.println(currentThrottle);
    Serial.println(mappedThrottle);
    Serial.println(currentRpm);
    Serial.println(currentRpmScaled);
  }
#endif
}

// Smoothing sub function
int throttleAverage(int input) {
  static int raw[6];

  if (raw[0] == 0) {
    for (int i = 0; i <= 5; i++) {
      raw[i] = input; // Init array
    }
  }

  raw[5] = raw[4];
  raw[4] = raw[3];
  raw[3] = raw[2];
  raw[2] = raw[1];
  raw[1] = raw[0];
  raw[0] = input;
  int average = (raw[0] + raw[1] + raw[2] + raw[3] + raw[4] + raw[5]) / 6;
  return average;
}

//
// =======================================================================================================
// SWITCH ENGINE ON OR OFF
// =======================================================================================================
//

void engineOnOff() {

  static unsigned long pulseDelayMillis;
  static unsigned long idleDelayMillis;

  if (engineManualOnOff) { // Engine manually switched on or off depending on presence of servo pulses
    if (pulseAvailable) pulseDelayMillis = millis(); // reset delay timer, if pulses are available

    if (millis() - pulseDelayMillis > 100) {
      engineOn = false; // after delay, switch engine off
    }
    else engineOn = true;
  }
  else { // Engine automatically switched on or off depending on throttle position and 15s delay timne
    if (currentThrottle > 80) idleDelayMillis = millis(); // reset delay timer, if throttle not in neutral

    if (millis() - idleDelayMillis > 15000) {
      engineOn = false; // after delay, switch engine off
    }
    else {
      if (currentThrottle > 100) engineOn = true;
    }
  }
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop() {

  // Nothing going on here...
}

//
// =======================================================================================================
// 1st MAIN TASK, RUNNING ON CORE 0
// =======================================================================================================
//

void Task1code(void *pvParameters) {
  for (;;) {

    // measure RC signal mark space ratio
    getRcSignal();

    // Map pulsewidth to throttle
    mapThrottle();

    // Simulate engine mass, generate RPM signal
    engineMassSimulation();

    // Switch engine on or off
    engineOnOff();

    delay(1); // 1ms delay required, otherwise ESP32 keeps rebooting!
  }
}
