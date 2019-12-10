/* RC engine sound simulator for Arduino ESP32. Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****

   Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
   converter code by bitluni (send him a high five if you like the code)

*/

const float codeVersion = 0.6; // Software revision.

//
// =======================================================================================================
// SETTINGS (ADJUST THEM BEFORE CODE UPLOAD)
// =======================================================================================================
//

// All the required vehicle specific settings are done in Adjustments.h!
#include "Adjustments.h" // <<------- ADJUSTMENTS TAB

#define DEBUG // can slow down the playback loop! Comment it out, if not needed

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
#define HORN_PIN 12 // This input is triggering the horn, if connected to VCC or PWM pulse length above threshold (see variable pwmHornTrigger" in Adjustments.h)

#define DAC1 25 // connect pin25 to a 10kOhm resistor
#define DAC2 26 // connect pin26 to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 10kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
// of a PAM8403 amplifier and allows to adjust the volume. This way, two speakers can be used.

// Define global variables
volatile uint8_t engineState = 0; // 0 = off, 1 = starting, 2 = running, 3 = stopping

volatile boolean engineOn = false;              // Signal for engine on / off
volatile boolean hornOn = false;                // Signal for horn on / off

uint32_t  currentThrottle = 0;                  // 0 - 500
volatile uint32_t pulseWidth = 0;                // Current RC signal pulse width
volatile boolean pulseAvailable;                // RC signal pulses are coming in

const int32_t maxRpm = 500;                     // always 500
const int32_t minRpm = 0;                       // always 0
int32_t currentRpm = 0;                         // 0 - 500 (signed required!)
volatile uint32_t currentRpmScaled;

uint16_t pulseMaxNeutral;                        // PWM throttle configuration storage variables
uint16_t pulseMinNeutral;
uint16_t pulseMax;
uint16_t pulseMin;
uint16_t pulseMaxLimit;
uint16_t pulseMinLimit;

// Our main tasks
TaskHandle_t Task1;

// Loop time (for debug)
uint16_t loopTime;

// Sampling intervals for interrupt timer (adjusted according to your sound file sampling rate)
uint32_t maxSampleInterval = 4000000 / sampleRate;
uint32_t minSampleInterval = 4000000 / sampleRate / TOP_SPEED_MULTIPLIER;

// Interrupt timer for variable sample rate playback (engine sound)
hw_timer_t * variableTimer = NULL;
portMUX_TYPE variableTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t variableTimerTicks = maxSampleInterval;

// Interrupt timer for fixed sample rate playback (horn etc., playing in parallel with engine sound)
hw_timer_t * fixedTimer = NULL;
portMUX_TYPE fixedTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t fixedTimerTicks = maxSampleInterval;

//
// =======================================================================================================
// INTERRUPT FOR VARIABLE SPEED PLAYBACK (Engine sound)
// =======================================================================================================
//

void IRAM_ATTR variablePlaybackTimer() {

  static uint32_t curEngineSample;              // Index of currently loaded engine sample
  static uint32_t curStartSample;               // Index of currently loaded start sample
  static uint16_t attenuator;                   // Used for volume adjustment during engine switch off

  portENTER_CRITICAL_ISR(&variableTimerMux);

  switch (engineState) {

    case 0: // off ----
      fixedTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      dacWrite(DAC1, 128); // volume = zero
      if (engineOn) engineState = 1;
      break;

    case 1: // starting ----
      fixedTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curStartSample < startSampleCount) {
        dacWrite(DAC1, (int)startSamples[curStartSample] + 128);
        curStartSample ++;
      }
      else {
        curStartSample = 0;
        engineState = 2;
      }
      break;

    case 2: // running ----
      variableTimerTicks = currentRpmScaled;  // our variable sampling rate!
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curEngineSample < sampleCount) {
        dacWrite(DAC1, (int)(samples[curEngineSample] * idleVolumePercentage / 100) + 128);
        curEngineSample ++;
      }
      else {
        curEngineSample = 0;
      }

      if (!engineOn) {
        attenuator = 1;
        engineState = 3;
      }
      break;

    case 3: // stopping ----
      fixedTimerTicks = 4000000 / sampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curEngineSample < sampleCount) {
        dacWrite(DAC1, (int)(samples[curEngineSample] * idleVolumePercentage / 100 / attenuator) + 128);
        curEngineSample ++;
      }
      else {
        curEngineSample = 0;
        attenuator += 5; // fade engine sound out
      }
      if (attenuator >= 20) {  // 3 - 20
        dacWrite(DAC1, 128);
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

  static uint32_t curHornSample;                // Index of currently loaded horn sample


  portENTER_CRITICAL_ISR(&fixedTimerMux);

  fixedTimerTicks = 4000000 / hornSampleRate; // our fixed sampling rate
  timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

  if (hornOn) {
    if (curHornSample < hornSampleCount) {
      dacWrite(DAC2, (int)hornSamples[curHornSample] + 128);
      curHornSample ++;
    }
    else {
      curHornSample = 0;
      hornOn = false;
      dacWrite(DAC2, 128);
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
  pinMode(THROTTLE_PIN, INPUT_PULLDOWN);
  pinMode(HORN_PIN, INPUT_PULLDOWN);

#ifdef DEBUG
  // Serial setup
  Serial.begin(115200);
#endif

  // DAC
  dacWrite(DAC1, 128);
  dacWrite(DAC2, 128);

  // Watchdog timers need to be disabled, if task 1 is running without delay(1)
  disableCore0WDT();
  disableCore1WDT();

  // Task 1 setup (running on core 0)
  TaskHandle_t Task1;
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    100000,       /* Stack size of task (10000) */
    NULL,        /* parameter of the task */
    5,           /* priority of the task (1 = low, 3 = medium, 5 = highest)*/
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */

  // Interrupt timer for variable sample rate playback
  variableTimer = timerBegin(0, 20, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 20 -> 250 ns = 0.25 us, countUp
  timerAttachInterrupt(variableTimer, &variablePlaybackTimer, true); // edge (not level) triggered
  timerAlarmWrite(variableTimer, variableTimerTicks, true); // autoreload true
  timerAlarmEnable(variableTimer); // enable

  // Interrupt timer for fixed sample rate playback
  fixedTimer = timerBegin(1, 20, true);  // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 20 -> 250 ns = 0.25 us, countUp
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
  pulseWidth = pulseIn(THROTTLE_PIN, HIGH, 50000);
}

//
// =======================================================================================================
// HORN TRIGGERING
// =======================================================================================================
//

void triggerHorn() {
  if (pwmHornTrigger) {
    // detect horn trigger ( impulse length > 1700us)
    if (pulseIn(HORN_PIN, HIGH, 50000) > 1700) hornOn = true;
  }
  else {
    // detect horn trigger (constant high level)
    if (digitalRead(HORN_PIN)) hornOn = true;
  }
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

  static uint32_t  mappedThrottle = 0;
  static unsigned long throtMillis;
  static unsigned long printMillis;

  if (millis() - throtMillis > 2) { // Every 2ms
    throtMillis = millis();

    // compute rpm curves
    if (shifted) mappedThrottle = reMap(curveShifting, currentThrottle);
    else mappedThrottle = reMap(curveLinear, currentThrottle);


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
    currentRpmScaled = map(currentRpm, minRpm, maxRpm, maxSampleInterval, minSampleInterval);
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
    Serial.println(" ");
    Serial.println(loopTime);
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
// LOOP TIME MEASUREMENT
// =======================================================================================================
//

unsigned long loopDuration() {
  static unsigned long timerOld;
  unsigned long loopTime;
  unsigned long timer = millis();
  loopTime = timer - timerOld;
  timerOld = timer;
  return loopTime;
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

    static unsigned long pulseReadMillis;

    // *** Read PWM signals only every 100ms (very important, otherwise pulseRead() uses too much cycle time!! *** //
    if (millis() - pulseReadMillis > 100) {
      pulseReadMillis = millis();
      // measure RC signal mark space ratio
      getRcSignal();

      // Horn triggering
      triggerHorn();
    }

    // Map pulsewidth to throttle
    mapThrottle();

    // Simulate engine mass, generate RPM signal
    engineMassSimulation();

    // Switch engine on or off
    engineOnOff();

    loopTime = loopDuration(); // measure loop time

    //delay(1); // 1ms delay required, otherwise ESP32 keeps rebooting, if watchdog timers are active...
  }
}
