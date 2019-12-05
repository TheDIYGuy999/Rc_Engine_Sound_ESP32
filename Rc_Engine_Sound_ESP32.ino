/* RC engine sound simulator for Arduino ESP32. Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****

   Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
   converter code by bitluni (send him a high five if you like the code)

*/

const float codeVersion = 0.3; // Software revision.

//
// =======================================================================================================
// SETTINGS (ADJUST THEM BEFORE CODE UPLOAD)
// =======================================================================================================
//

// All the required vehicle specific settings are done in settings.h!
#include "Adjustments.h" // <<------- ADJUSTMENTS TAB

//#define DEBUG // can slow down the playback loop! Comment it out, if not needed

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

// Pin assignment and wiring instructions
#define THROTTLE_PIN 13 // connect to RC receiver throttle channel (caution, max. 3.3V, 10kOhm series resistor recommended!)
#define HORN_SWITCH 12 // This input is triggering the horn, if connected to VCC

#define DAC1 25 // connect pin25 to a 10kOhm resistor
#define DAC2 26 // connect pin26 to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 10kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
// of a PAM8403 amplifier and allows to adjust the volume. This way, two speakers can be used.

// Define global variables
volatile uint8_t engineState = 0; // 0 = off, 1 = starting, 2 = running, 3 = stopping

volatile boolean engineOn = false;              // Signal for engine on / off
volatile boolean hornOn = false;                // Signal for horn on / off

uint16_t  currentThrottle = 0;                  // 0 - 500
volatile int16_t pulseWidth = 0;                // Current RC signal pulse width
volatile boolean pulseAvailable;                // RC signal pulses are coming in

const int16_t maxRpm = 500;                     // always 500
const int16_t minRpm = 0;                       // always 0
int16_t currentRpm = 0;                         // 0 - 500
volatile int16_t currentRpmScaled;

int16_t pulseMaxNeutral;                        // PWM throttle configuration storage variables
int16_t pulseMinNeutral;
int16_t pulseMax;
int16_t pulseMin;
int16_t pulseMaxLimit;
int16_t pulseMinLimit;

// Our main tasks
TaskHandle_t Task1;

// Interrupt timer for variable sample rate playback (engine sound)
hw_timer_t * variableTimer = NULL;
portMUX_TYPE variableTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t variableTimerTicks = 50; // start 50 * 1 us = 50 us

// Interrupt timer for fixed sample rate playback (horn etc., playing in parallel with engine sound)
hw_timer_t * fixedTimer = NULL;
portMUX_TYPE fixedTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t fixedTimerTicks = 50; // start 50 * 1 us = 50 us

// Sampling intervals for interrupt timer (adjusted according to your sound file sampling rate)
uint8_t maxSampleInterval = 1000000 / sampleRate;
uint8_t minSampleInterval = 1000000 / sampleRate / TOP_SPEED_MULTIPLIER;

//
// =======================================================================================================
// INTERRUPT FOR VARIABLE SPEED PLAYBACK (Engine sound)
// =======================================================================================================
//

void IRAM_ATTR variablePlaybackTimer() {

  static uint16_t curEngineSample;              // Index of currently loaded engine sample
  static uint16_t curStartSample;               // Index of currently loaded start sample
  static uint16_t attenuator;                   // Used for volume adjustment during engine switch off

  portENTER_CRITICAL_ISR(&variableTimerMux);

  switch (engineState) {

    case 0: // off ----
      fixedTimerTicks = 1000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      dacWrite(DAC1, 128); // volume = zero
      if (engineOn) engineState = 1;
      break;

    case 1: // starting ----
      fixedTimerTicks = 1000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      curStartSample ++;
      if (curStartSample < startSampleCount) dacWrite(DAC1, (int)startSamples[curStartSample] + 128);
      else {
        curStartSample = 0;
        engineState = 2;
      }
      break;

    case 2: // running ----
      variableTimerTicks = currentRpmScaled;  // our variable sampling rate!
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      curEngineSample ++;
      if (curEngineSample < sampleCount) dacWrite(DAC1, (int)samples[curEngineSample] + 128);
      else {
        curEngineSample = 0;
        attenuator = 1;
      }

      if (!engineOn) {
        engineState = 3;
      }
      break;

    case 3: // stopping ----
      fixedTimerTicks = 1000000 / sampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      curEngineSample ++;
      if (curEngineSample < sampleCount) dacWrite(DAC1, (int)(samples[curEngineSample] / attenuator) + 128);
      else {
        curEngineSample = 0;
        attenuator += 5; // fade engine sound out
      }
      if (attenuator >= 20) {  // 3 - 20
        engineOn = false;
        if (!engineOn) engineState = 0; // Important: ensure, that engine is off, before we go back to "starting"!!
      }
      break;

  } // end of switch case

  portEXIT_CRITICAL_ISR(&variableTimerMux);
}

//
// =======================================================================================================
// INTERRUPT FOR FIXED SPEED PLAYBACK (Horn etc., played in parallel with engine sound)
// =======================================================================================================
//

void IRAM_ATTR fixedPlaybackTimer() {

  static uint16_t curHornSample;                // Index of currently loaded horn sample


  portENTER_CRITICAL_ISR(&fixedTimerMux);

  fixedTimerTicks = 1000000 / hornSampleRate; // our fixed sampling rate
  timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

  if (hornOn) {
    curHornSample ++;
    if (curHornSample < hornSampleCount) dacWrite(DAC2, (int)hornSamples[curHornSample] + 128);
    else {
      curHornSample = 0;
      hornOn = false;
    }
  }

  portEXIT_CRITICAL_ISR(&fixedTimerMux);
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

  // Pin modes
  pinMode(THROTTLE_PIN, INPUT);
  pinMode(HORN_SWITCH, INPUT_PULLDOWN);

  // Serial setup
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

  // Interrupt timer for variable sample rate playback
  variableTimer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(variableTimer, &variablePlaybackTimer, true); // edge (not level) triggered
  timerAlarmWrite(variableTimer, variableTimerTicks, true); // autoreload true
  timerAlarmEnable(variableTimer); // enable

  // Interrupt timer for fixed sample rate playback
  fixedTimer = timerBegin(1, 80, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(fixedTimer, &fixedPlaybackTimer, true); // edge (not level) triggered
  timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // autoreload true
  timerAlarmEnable(fixedTimer); // enable

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
// HORN TRIGGERING
// =======================================================================================================
//

void triggerHorn() {
  // detect horn input
  if (digitalRead(HORN_SWITCH)) hornOn = true;
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
    static unsigned long throtMillis;
    static unsigned long printMillis;

    if (millis() - throtMillis > 2) { // Every 2ms
      throtMillis = millis();

      // compute unlinear throttle curve
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
    }


    // Print debug infos
#ifdef DEBUG // can slow down the playback loop!
    if (millis() - printMillis > 200) { // Every 200ms
      printMillis = millis();

      Serial.println(currentThrottle);
      Serial.println(mappedThrottle);
      Serial.println(currentRpm);
      Serial.println(currentRpmScaled);
      Serial.println(engineState);
    }
#endif
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

      // Horn triggering
      triggerHorn();

      // Map pulsewidth to throttle
      mapThrottle();

      // Simulate engine mass, generate RPM signal
      engineMassSimulation();

      // Switch engine on or off
      engineOnOff();

      delay(1); // 1ms delay required, otherwise ESP32 keeps rebooting!
    }
  }
