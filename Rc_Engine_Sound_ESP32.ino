/* RC engine sound simulator for Arduino ESP32. Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****

   Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
   converter code by bitluni (send him a high five if you like the code)

*/

const float codeVersion = 1.5; // Software revision.

//
// =======================================================================================================
// SETTINGS (ADJUST THEM BEFORE CODE UPLOAD)
// =======================================================================================================
//

// All the required vehicle specific settings are done in Adjustments.h!
#include "Adjustments.h" // <<------- ADJUSTMENTS TAB

//#define DEBUG // can slow down the playback loop! Comment it out, if not needed

//
// =======================================================================================================
// LIRBARIES & HEADER FILES (TABS ABOVE)
// =======================================================================================================
//

#include "curves.h" // load nonlinear throttle curve arrays
#include <statusLED.h> // https://github.com/TheDIYGuy999/statusLED <<------- Install it!

//
// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES (Do not play around here)
// =======================================================================================================
//

// Pin assignment and wiring instructions
#define THROTTLE_PIN 13 // connect to RC receiver throttle channel (caution, max. 3.3V, 10kOhm series resistor recommended!)
#define STEERING_PIN 14 // Steering angle RC signal input
#define HORN_PIN 12 // This input is triggering the horn, if connected to VCC or PWM pulse length above threshold (see variable pwmHornTrigger" in Adjustments.h)

#define TAILLIGHT_PIN 4 // Red tail- & brake-lights
#define HEADLIGHT_PIN 5 // White headllights
#define INDICATOR_LEFT_PIN 0 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 2 // Orange right indicator (turn signal) light
#define BEACONS_LIGHTS2_PIN 21 // Blue beacons light
#define BEACONS_LIGHTS_PIN 33 // Blue beacons light
#define REVERSING_LIGHT_PIN 32 // White reversing light

#define DAC1 25 // connect pin25 to a 10kOhm resistor
#define DAC2 26 // connect pin26 to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 10kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
// of a PAM8403 amplifier and allows to adjust the volume. This way, two speakers can be used.

// Status LED objects
statusLED tailLight(false); // "false" = output not inverted
statusLED headLight(false);
statusLED indicatorL(false);
statusLED indicatorR(false);
statusLED beaconLights2(false);
statusLED beaconLights(false);
statusLED reversingLight(false);

// Define global variables
volatile uint8_t engineState = 0; // 0 = off, 1 = starting, 2 = running, 3 = stopping

volatile uint8_t soundNo = 0; // 0 = horn, 1 = additional sound 1

volatile boolean engineOn = false;              // Signal for engine on / off
volatile boolean hornOn = false;                // Signal for horn on / off
volatile boolean sound1On = false;              // Signal for additional sound 1  on / off
volatile boolean reversingSoundOn = false;      // active during backing up

volatile boolean lightsOn = false;             // Lights on

volatile boolean airBrakeTrigger = false;       // Trigger for air brake noise
volatile boolean EngineWasAboveIdle = false;    // Engine RPM was above idle
volatile boolean slowingDown = false;           // Engine is slowing down

volatile boolean hornSwitch = false;            // Switch state for horn on / off triggering
volatile boolean sound1Switch = false;          // Switch state for additional sound 1 triggering

boolean indicatorLon = false;                   // Left indicator
boolean indicatorRon = false;                   // Right indicator

uint32_t  currentThrottle = 0;                  // 0 - 500
uint32_t pulseWidth[3];                // Current RC signal pulse width [0] = throttle, [1] = steering, [2] = additional signal

uint16_t pulseMaxNeutral[3];                    // PWM signal configuration storage variables
uint16_t pulseMinNeutral[3];
uint16_t pulseMax[3];
uint16_t pulseMin[3];
uint16_t pulseMaxLimit[3];
uint16_t pulseMinLimit[3];

volatile boolean pulseAvailable;                // RC signal pulses are coming in

uint16_t pulseZero[3];                           // Usually 1500 (range 1000 - 2000us) Autocalibration active, if "engineManualOnOff" = "false"
uint16_t pulseLimit = 700; // pulseZero +/- this value (700)

const int32_t maxRpm = 500;                     // always 500
const int32_t minRpm = 0;                       // always 0
int32_t currentRpm = 0;                         // 0 - 500 (signed required!)
volatile uint32_t currentRpmScaled;

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
// INTERRUPT FOR VARIABLE SPEED PLAYBACK (Engine sound, brake sound)
// =======================================================================================================
//

void IRAM_ATTR variablePlaybackTimer() {

  static uint32_t attenuatorMillis;
  static uint32_t curEngineSample;              // Index of currently loaded engine sample
  static uint32_t curBrakeSample;               // Index of currently loaded brake sound sample
  static uint32_t curStartSample;               // Index of currently loaded start sample
  static uint16_t attenuator;                   // Used for volume adjustment during engine switch off
  static uint16_t speedPercentage;              // slows the engine down during shutdown
  static uint32_t a, b;                         // Two input signals for mixer: a = engine, b = additional sound

  portENTER_CRITICAL_ISR(&variableTimerMux);

  switch (engineState) {

    case 0: // Engine off ----
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      a = 128; // volume = zero
      if (engineOn) engineState = 1;
      break;

    case 1: // Engine start ----
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curStartSample < startSampleCount) {
        a = (int)startSamples[curStartSample] + 128;
        curStartSample ++;
      }
      else {
        curStartSample = 0;
        engineState = 2;
      }
      break;

    case 2: // Engine running ----
      variableTimerTicks = currentRpmScaled;  // our variable sampling rate!
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      // Engine sound
      if (curEngineSample < sampleCount) {
        a = (int)(samples[curEngineSample] * idleVolumePercentage / 100) + 128;
        curEngineSample ++;
      }
      else {
        curEngineSample = 0;
      }

      // Air brake release sound, triggered after stop
      if (airBrakeTrigger) {
        if (curBrakeSample < brakeSampleCount) {
          b = (int)brakeSamples[curBrakeSample] + 128;
          curBrakeSample ++;
        }
        else {
          airBrakeTrigger = false;
          EngineWasAboveIdle = false;
        }
      }
      else {
        b = 0; // Ensure full engine volume
        curBrakeSample = 0; // ensure, next sound will start @ first sample
      }


      if (!engineOn) {
        speedPercentage = 100;
        attenuator = 1;
        engineState = 3;
      }
      break;

    case 3: // Engine stop ----
      variableTimerTicks = 4000000 / sampleRate * speedPercentage / 100; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curEngineSample < sampleCount) {
        a = (int)(samples[curEngineSample] * idleVolumePercentage / 100 / attenuator) + 128;
        curEngineSample ++;
      }
      else {
        curEngineSample = 0;
      }

      // fade engine sound out
      if (millis() - attenuatorMillis > 100) { // Every 50ms
        attenuatorMillis = millis();
        attenuator ++; // attenuate volume
        speedPercentage += 20; // make it slower (10)
      }

      if (attenuator >= 50 || speedPercentage >= 500) { // 50 & 500
        a = 128;
        speedPercentage = 100;
        engineState = 4;
      }
      break;

    case 4: // brake air sound after engine is off ----
      variableTimerTicks = 4000000 / brakeSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curBrakeSample < brakeSampleCount) {
        b = (int)brakeSamples[curBrakeSample] + 128;
        curBrakeSample ++;
      }
      else {
        curBrakeSample = 0;
        engineState = 0;
      }
      break;

  } // end of switch case

  dacWrite(DAC1, (int) (a + b - a * b / 255)); // Write mixed output signals to DAC: http://www.vttoth.com/CMS/index.php/technical-notes/68

  portEXIT_CRITICAL_ISR(&variableTimerMux);
}

//
// =======================================================================================================
// INTERRUPT FOR FIXED SPEED PLAYBACK (Horn etc., played in parallel with engine sound)
// =======================================================================================================
//

void IRAM_ATTR fixedPlaybackTimer() {

  static uint32_t curHornSample;                // Index of currently loaded horn sample
  static uint32_t curSound1Sample;              // Index of currently loaded sound 1 sample
  static uint32_t curReversingSample;           // Index of currently loaded sound 1 sample
  static uint32_t a, b;                         // Two input signals for mixer: a = horn, b = reversing sound

  portENTER_CRITICAL_ISR(&fixedTimerMux);

  switch (soundNo) {

    case 0: // Horn "a" ----
      fixedTimerTicks = 4000000 / hornSampleRate; // our fixed sampling rate
      timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true
      curSound1Sample = 0;

      if (hornOn) {
        if (curHornSample < hornSampleCount) {
          a =  (int)hornSamples[curHornSample] + 128;
          curHornSample ++;
        }
        else {
          curHornSample = 0;
          a = 128;
          if (!hornSwitch) hornOn = false; // Latch required to prevent it from popping
        }
      }
      break;

    case 1: // Sound 1 "a" ----
      fixedTimerTicks = 4000000 / sound1SampleRate; // our fixed sampling rate
      timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true
      curHornSample = 0;

      if (sound1On) {
        if (curSound1Sample < sound1SampleCount) {
          a = (int)sound1Samples[curSound1Sample] + 128;
          curSound1Sample ++;
        }
        else {
          curSound1Sample = 0;
          a = 128;
          if (!sound1Switch) sound1On = false; // Latch required to prevent it from popping
        }
      }
      break;

  } // end of switch case

  // Reversing beep sound "b" (can play in parallel with sound "a") ----
  if (reversingSoundOn) {
    fixedTimerTicks = 4000000 / reversingSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curReversingSample < reversingSampleCount) {
      b = (int)reversingSamples[curReversingSample] + 128;
      curReversingSample ++;
    }
    else {
      curReversingSample = 0;
    }
    b = b * reversingvolumePercentage / 100; // Reversing volume
  }
  else {
    curReversingSample = 0;
    b = 0;
  }

  dacWrite(DAC2, (int) (a + b - a * b / 255)); // Write mixed output signals to DAC: http://www.vttoth.com/CMS/index.php/technical-notes/68

  portEXIT_CRITICAL_ISR(&fixedTimerMux);
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

  // Watchdog timers need to be disabled, if task 1 is running without delay(1)
  disableCore0WDT();
  disableCore1WDT();

  // Pin modes
  pinMode(THROTTLE_PIN, INPUT_PULLDOWN);
  pinMode(STEERING_PIN, INPUT_PULLDOWN);
  pinMode(HORN_PIN, INPUT_PULLDOWN);

  // LED Setup
  tailLight.begin(TAILLIGHT_PIN, 1, 500); // Timer 1, 500Hz
  headLight.begin(HEADLIGHT_PIN, 2, 500); // Timer 2, 500Hz
  indicatorL.begin(INDICATOR_LEFT_PIN, 3, 500); // Timer 3, 500Hz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 4, 500); // Timer 4, 500Hz
  beaconLights2.begin(BEACONS_LIGHTS_PIN, 5, 500); // Timer 5, 500Hz
  beaconLights.begin(BEACONS_LIGHTS2_PIN, 6, 500); // Timer 5, 500Hz
  reversingLight.begin(REVERSING_LIGHT_PIN, 7, 500); // Timer 6, 500Hz

#ifdef DEBUG
  // Serial setup
  Serial.begin(115200);
#endif

  // DAC
  dacWrite(DAC1, 128); // 128 = center / neutral position = 1.65V
  dacWrite(DAC2, 128);

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

  // Read RC signals for the first time (used for offset calculations)
  getRcSignals();

  // then compute the RC channel offsets
  if (!engineManualOnOff) pulseZero[0] = pulseWidth[0]; // store throttle offset (only, if "engineManualOnOff" inactive)
  else pulseZero[0] = 1500;

  if (indicators) pulseZero[1] = pulseWidth[1]; // store steering offset (only, if "indicators" active)
  else pulseZero[1] = 1500;

  pulseZero[2] = 1500; // This channel is controlled by a 3 position switch, so we don't want auto calibration!

  // Calculate RC signal ranges for all channels (0, 1, 2)
  for (uint8_t i = 0; i <= 2; i++) {
    //uint8_t i = 0;
    pulseMaxNeutral[i] = pulseZero[i] + pulseNeutral;
    pulseMinNeutral[i] = pulseZero[i] - pulseNeutral;
    pulseMax[i] = pulseZero[i] + pulseSpan;
    pulseMin[i] = pulseZero[i] - pulseSpan;
    pulseMaxLimit[i] = pulseZero[i] + pulseLimit;
    pulseMinLimit[i] = pulseZero[i] - pulseLimit;
  }
}

//
// =======================================================================================================
// GET RC SIGNALS
// =======================================================================================================
//

void getRcSignals() {
  // measure RC signal mark space ratio
  pulseWidth[0] = pulseIn(THROTTLE_PIN, HIGH, 50000); // Throttle
  
  if (indicators) pulseWidth[1] = pulseIn(STEERING_PIN, HIGH, 50000); // Steering
  else pulseWidth[1] = 1500;
  
  if (pwmHornTrigger) pulseWidth[2] = pulseIn(HORN_PIN, HIGH, 50000); // Additional sound trigger (RC signal with 3 positions)
}

//
// =======================================================================================================
// HORN TRIGGERING, ADDITIONAL SOUND TRIGGERING
// =======================================================================================================
//

void triggerHorn() {
  if (pwmHornTrigger) { // PWM RC signal mode --------------------------------------------

    // detect horn trigger ( impulse length > 1700us) -------------
    if (pulseWidth[2] > (pulseMaxNeutral[2] + 180)) {
      hornSwitch = true;
      soundNo = 0;  // 0 = horn
    }
    else hornSwitch = false;


    // detect sound 1 trigger ( impulse length < 1300us) ----------
    if (pulseWidth[2] < (pulseMinNeutral[2] - 180)) {
      sound1Switch = true;
      soundNo = 1;  // 1 = sound 1
    }
    else sound1Switch = false;

  }
  else { // High level triggering mode ---------------------------------------------------

    // detect horn trigger (constant high level)
    if (digitalRead(HORN_PIN)) {
      hornSwitch = true;
      soundNo = 0;  // 0 = horn
    }
    else hornSwitch = false;
  }

  // Latches (required to prevent sound seams from popping) --------------------------------

  if (hornSwitch) hornOn = true;
  if (sound1Switch) sound1On = true;

}

//
// =======================================================================================================
// INDICATOR (TURN SIGNAL) TRIGGERING
// =======================================================================================================
//

void triggerIndicators() {
  // detect left indicator trigger ( impulse length > 1700us) -------------
  if (pulseWidth[1] > (pulseMaxNeutral[1] + 20)) indicatorLon = true;
  if (pulseWidth[1] < pulseMaxNeutral[1]) indicatorLon = false;


  // detect right indicator trigger ( impulse length < 1300us) ----------
  if (pulseWidth[1] < (pulseMinNeutral[1] - 20)) indicatorRon = true;
  if (pulseWidth[1] > pulseMinNeutral[1]) indicatorRon = false;
}

//
// =======================================================================================================
// MAP PULSEWIDTH TO THROTTLE
// =======================================================================================================
//

void mapThrottle() {

  static unsigned long reversingMillis;

  // Input is around 1000 - 2000us, output 0-500 for forward and backwards

  // check if the pulsewidth looks like a servo pulse
  if (pulseWidth[0] > pulseMinLimit[0] && pulseWidth[0] < pulseMaxLimit[0]) {
    if (pulseWidth[0] < pulseMin[0]) pulseWidth[0] = pulseMin[0]; // Constrain the value
    if (pulseWidth[0] > pulseMax[0]) pulseWidth[0] = pulseMax[0];

    // calculate a throttle value from the pulsewidth signal
    if (pulseWidth[0] > pulseMaxNeutral[0]) currentThrottle = map(pulseWidth[0], pulseMaxNeutral[0], pulseMax[0], 0, 500);
    else if (pulseWidth[0] < pulseMinNeutral[0]) currentThrottle = map(pulseWidth[0], pulseMinNeutral[0], pulseMin[0], 0, 500);
    else currentThrottle = 0;
  }


  // reversing sound trigger signal
  if (reverseSoundMode == 1) {
    if (pulseWidth[0] <= pulseMaxNeutral[0]) {
      reversingMillis = millis();
    }

    if (millis() - reversingMillis > 200) {
      reversingSoundOn = true;
    }
    else reversingSoundOn = false;
  }

  if (reverseSoundMode == 2) {
    if (pulseWidth[0] >= pulseMinNeutral[0]) {
      reversingMillis = millis();
    }

    if (millis() - reversingMillis > 200) {
      reversingSoundOn = true;
    }
    else reversingSoundOn = false;
  }

  if (reverseSoundMode == 0) {
    reversingSoundOn = false;
  }

}


//
// =======================================================================================================
// ENGINE MASS SIMULATION
// =======================================================================================================
//

void engineMassSimulation() {

  static int32_t  mappedThrottle = 0;
  static unsigned long throtMillis;
  static unsigned long printMillis;

  if (millis() - throtMillis > 2) { // Every 2ms
    throtMillis = millis();

    // compute rpm curves
    if (shifted) mappedThrottle = reMap(curveShifting, currentThrottle);
    else mappedThrottle = reMap(curveLinear, currentThrottle);


    // Accelerate engine
    if (mappedThrottle > (currentRpm + acc) && (currentRpm + acc) < maxRpm && engineState == 2) {
      if (!airBrakeTrigger) { // No acceleration, if brake release noise still playing
        currentRpm += acc;
        if (currentRpm > maxRpm) currentRpm = maxRpm;
      }
    }

    // Decelerate engine
    if (mappedThrottle < currentRpm) {
      currentRpm -= dec;
      if (currentRpm < minRpm) currentRpm = minRpm;
    }


    // Speed (sample rate) output
    currentRpmScaled = map(currentRpm, minRpm, maxRpm, maxSampleInterval, minSampleInterval);
  }

  // Brake light trigger
  if (mappedThrottle < (currentRpm - 200)) slowingDown = true;
  if (mappedThrottle >= (currentRpm - 10)) slowingDown = false;

  // Print debug infos
#ifdef DEBUG // can slow down the playback loop!
  if (millis() - printMillis > 1000) { // Every 1000ms
    printMillis = millis();

    Serial.println("CH0");
    Serial.println(pulseWidth[0]);
    Serial.println(pulseMinNeutral[0]);
    Serial.println(pulseMaxNeutral[0]);
    Serial.println("CH1");
    Serial.println(pulseWidth[1]);
    Serial.println(pulseMinNeutral[1]);
    Serial.println(pulseMaxNeutral[1]);
    Serial.println("CH2");
    Serial.println(pulseWidth[2]);
    Serial.println(pulseMinNeutral[2]);
    Serial.println(pulseMaxNeutral[2]);
    Serial.println(" ");
    Serial.println(currentThrottle);
    Serial.println(mappedThrottle);
    Serial.println(currentRpm);
    Serial.println(currentRpmScaled);
    Serial.println(engineState);
    Serial.println(" ");
    Serial.println(loopTime);
    Serial.println(" ");
    Serial.println(airBrakeTrigger);
    Serial.println(EngineWasAboveIdle);
  }
#endif
}

//
// =======================================================================================================
// SWITCH ENGINE ON OR OFF, AIR BRAKE TRIGGERING
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

    if (millis() - idleDelayMillis > 10000) {
      lightsOn = false; // after delay, switch light off
    }

    // air brake noise trigggering
    if (millis() - idleDelayMillis > 1000) {
      if (EngineWasAboveIdle) {
        airBrakeTrigger = true; // after delay, trigger air brake noise
      }
    }

    // Engine start detection
    if (currentThrottle > 100 && !airBrakeTrigger) {
      engineOn = true;
      lightsOn = true;
      EngineWasAboveIdle = true;
    }
  }
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

void led() {

  // Reversing light ----
  if (reversingSoundOn) reversingLight.on();
  else reversingLight.off();

  // Beacons (blue light) ----
  if (hornOn) {
    if (doubleFlashBlueLight) {
      beaconLights.flash(30, 80, 380, 2); // Simulate double flash lights
      beaconLights2.flash(30, 80, 400, 2); // Simulate double flash lights
    }
    else {
      beaconLights.flash(30, 400, 0, 0); // Simulate rotating beacon lights with short flashes
      beaconLights2.flash(30, 420, 0, 0); // Simulate rotating beacon lights with short flashes
    }
  }
  else {
    beaconLights.off();
    beaconLights2.off();
  }

  // Headlights, tail lights ----
  if (lightsOn) {
    headLight.on();
    if (slowingDown) tailLight.on();
    else tailLight.pwm(50);
  }
  else {
    headLight.off();
    tailLight.off();
  }

  // Indicators (turn signals) ----
  if (indicatorLon) indicatorL.flash(375, 375, 0, 0); // Left indicator
  else indicatorL.off();

  if (indicatorRon) indicatorR.flash(375, 375, 0, 0); // Right indicator
  else indicatorR.off();

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

  // measure RC signals mark space ratio
  getRcSignals();

  // Horn triggering
  triggerHorn();

  // Indicator (turn signal) triggering
  triggerIndicators();
}

//
// =======================================================================================================
// 1st MAIN TASK, RUNNING ON CORE 0
// =======================================================================================================
//

void Task1code(void *pvParameters) {
  for (;;) {

    // Map pulsewidth to throttle
    mapThrottle();

    // Simulate engine mass, generate RPM signal
    engineMassSimulation();

    // Switch engine on or off
    engineOnOff();

    // LED control
    led();

    loopTime = loopDuration(); // measure loop time
  }
}
