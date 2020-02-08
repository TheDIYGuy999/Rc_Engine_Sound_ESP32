/* RC engine sound simulator for Arduino ESP32. Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****

   Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
   converter code by bitluni (send him a high five, if you like the code)

*/

const float codeVersion = 2.5; // Software revision.

//
// =======================================================================================================
// SETTINGS (ADJUST THEM BEFORE CODE UPLOAD)
// =======================================================================================================
//

// All the required vehicle specific settings are done in Adjustments.h!
#include "Adjustments.h" // <<------- ADJUSTMENTS TAB

// DEBUG options can slow down the playback loop! Only comment them out for debugging
//#define DEBUG // uncomment it for general debugging informations
//#define SERIAL_DEBUG // uncomment it to debug the serial command interface on pin 36
#define DRIVE_STATE_DEBUG // uncomment it to debug the drive state statemachine

// TODO = Things to clean up!

//
// =======================================================================================================
// LIRBARIES & HEADER FILES (TABS ABOVE)
// =======================================================================================================
//

#include "curves.h" // load nonlinear throttle curve arrays
#include <statusLED.h> // https://github.com/TheDIYGuy999/statusLED <<------- Install the newest version!

//
// =======================================================================================================
// PIN ASSIGNMENTS & GLOBAL VARIABLES (Do not play around here)
// =======================================================================================================
//
// Pin assignment and wiring instructions

// ------------------------------------------------------------------------------------
// Use a 330Ohm resistor in series with all I/O pins! allows to drive LED directly and
// provides short circuit protection. Also works on the serial Rx pin "VP" (36)
// ------------------------------------------------------------------------------------

// Serial command pins (active, if "SERIAL_COMMUNICATION" in Adjustments.h is not commented out)
// see "sendSerialCommands()" in Micro RC Receiver code: https://github.com/TheDIYGuy999/Micro_RC_Receiver
// This is still experimental! It works, but the sound quality is not perfect.
#define COMMAND_RX 36 // pin 36, labelled with "VP", connect it to "Micro RC Receiver" pin "TXO"
#define COMMAND_TX 39 // pin 39, labelled with "VN", only used as a dummy, not connected

// PPM signal pin (multiple channel input with only one wire)
#define PPM_PIN 34

// RC signal pins (active, if "SERIAL_COMMUNICATION" is commented out)
// Channel numbers may be different on your recveiver!
#define SERVO1_PIN 13 // connect to RC receiver servo output channel 1 (aileron, steering)
#define SERVO2_PIN 12 // connect to RC receiver servo output channel 2 (elevator, 3 pos. switch)
#define SERVO3_PIN 14 // connect to RC receiver servo output channel 3 (throttle)
#define SERVO4_PIN 27 // connect to RC receiver servo output channel 4 (rudder, pot)

#define ESC_OUT_PIN 33 // connect crawler  ESC here (experimental, use it at your own risk!)

#define HEADLIGHT_PIN 0 // White headllights
#define TAILLIGHT_PIN 15 // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4 // Orange right indicator (turn signal) light
#define FOGLIGHT_PIN 16 // (RX2) Fog lights
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define ROOFLIGHT_PIN 5 // Roof lights
#define SIDELIGHT_PIN 18 // Side lights

#define BEACON_LIGHT2_PIN 19 // Blue beacons light
#define BEACON_LIGHT1_PIN 21 // Blue beacons light

#define BRAKELIGHT_PIN 32 // Upper brake lights

#define SHAKER_MOTOR_PIN 23 // Shaker motor (shaking truck while idling)

#define DAC1 25 // connect pin25 (do not change the pin) to a 10kOhm resistor
#define DAC2 26 // connect pin26 (do not change the pin) to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 10kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
// of a PAM8403 amplifier and allows to adjust the volume. This way, two speakers can be used.

// Status LED objects (also used for PWM shaker motor and ESC control)
statusLED headLight(false); // "false" = output not inversed
statusLED tailLight(false);
statusLED indicatorL(false);
statusLED indicatorR(false);
statusLED fogLight(false);
statusLED reversingLight(false);
statusLED roofLight(false);
statusLED sideLight(false);
statusLED beaconLight1(false);
statusLED beaconLight2(false);
statusLED brakeLight(false);
statusLED shakerMotor(false);
statusLED escOut(false);

// Define global variables

boolean serialInit = false;

volatile boolean failSafe = false;              // Triggered in emergency situations like: serial signal lost etc.
volatile int8_t ppmFailsafeCounter = 0;

volatile uint8_t engineState = 0; // 0 = off, 1 = starting, 2 = running, 3 = stopping

volatile uint8_t soundNo = 0; // 0 = horn, 1 = siren, 2 = sound1

volatile boolean engineOn = false;              // Signal for engine on / off
volatile boolean engineStart = false;           // Active, if engine is starting up
volatile boolean engineRunning = false;         // Active, if engine is running
volatile boolean engineStop = false;            // Active, if engine is shutting down
volatile boolean hornOn = false;                // Signal for horn on / off
volatile boolean sirenOn = false;               // Signal for siren  on / off
volatile boolean sound1On = false;              // Signal for sound1  on / off
volatile boolean reversingSoundOn = false;      // active during backing up TODO
volatile boolean indicatorSoundOn = false;      // active, if indicator bulb is on

volatile boolean lightsOn = false;              // Lights on

volatile boolean airBrakeTrigger = false;       // Trigger for air brake noise
volatile boolean EngineWasAboveIdle = false;    // Engine RPM was above idle
//volatile boolean slowingDown = false;           // Engine is slowing down TODO

volatile boolean escIsBraking = false;          // ESC is in a braking state
volatile boolean escInReverse = false;          // ESC is driving or braking backwards
int8_t driveState = 0;                          // for ESC state machine

volatile boolean hornSwitch = false;            // Switch state for horn triggering
volatile boolean sirenSwitch = false;           // Switch state for siren triggering
volatile boolean sound1Switch = false;          // Switch state for sound1 triggering

boolean indicatorLon = false;                   // Left indicator
boolean indicatorRon = false;                   // Right indicator

uint32_t currentThrottle = 0;                   // 0 - 500
boolean throttleReverse;                        // false = forward, true = reverse
uint32_t pulseWidth[4];                         // Current RC signal pulse width [0] = steering, [1] = 3p. switch, [2] = throttle, [4] = pot

uint16_t pulseMaxNeutral[4];                    // PWM input signal configuration storage variables
uint16_t pulseMinNeutral[4];
uint16_t pulseMax[4];
uint16_t pulseMin[4];
uint16_t pulseMaxLimit[4];
uint16_t pulseMinLimit[4];

int16_t escPulseMax;                           // ESC calibration variables
int16_t escPulseMin;

volatile boolean pulseAvailable;                // RC signal pulses are coming in

uint16_t pulseZero[4];                           // Usually 1500 (range 1000 - 2000us) Autocalibration active, if "engineManualOnOff" = "false"
uint16_t pulseLimit = 700; // pulseZero +/- this value (700)

int32_t axis1;                                  // Temporary variables for serial command parsing (for signals from "Micro RC" receiver)
int32_t axis2;                                  // See: https://github.com/TheDIYGuy999/Micro_RC_Receiver
int32_t axis3;
int32_t axis4;
int32_t pot1;
boolean mode1;
boolean mode2;
boolean momentary1;
boolean hazard;
boolean left;
boolean right;

const int32_t maxRpm = 500;                     // always 500
const int32_t minRpm = 0;                       // always 0
int32_t currentRpm = 0;                         // 0 - 500 (signed required!)
volatile uint32_t currentRpmScaled = 0;
volatile uint8_t throttleDependentVolume = 0;   // engine volume according to throttle position
volatile uint8_t throttleDependentTurboVolume = 0;   // turbo volume according to throttle position

// Our main tasks
TaskHandle_t Task1;

// Loop time (for debug)
uint16_t loopTime;

// PPM signal processing variables
volatile int valuesInt[NUM_OF_CHL + 1] = {0}; // Input values
volatile int valuesBuf[NUM_OF_CHL + 1] = {0}; // Buffered values
volatile byte counter = NUM_OF_CHL;
volatile byte average  = NUM_OF_AVG;
volatile boolean ready = false;
volatile unsigned long timelast;
unsigned long timelastloop;

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
  static uint32_t curTurboSample;              // Index of currently loaded turbo sample
  static uint32_t curBrakeSample;               // Index of currently loaded brake sound sample
  static uint32_t curStartSample;               // Index of currently loaded start sample
  static uint16_t attenuator;                   // Used for volume adjustment during engine switch off
  static uint16_t speedPercentage;              // slows the engine down during shutdown
  static int32_t a, b, c;                         // Two input signals for mixer: a = engine, b = additional sound, c = turbo sound

  portENTER_CRITICAL_ISR(&variableTimerMux);

  switch (engineState) {

    case 0: // Engine off ----
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      a = 128; // volume = zero
      if (engineOn) {
        engineState = 1;
        engineStart = true;
      }
      break;

    case 1: // Engine start ----
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curStartSample < startSampleCount) {
        a = (startSamples[curStartSample] * throttleDependentVolume / 100 * startVolumePercentage / 100) + 128;
        curStartSample ++;
      }
      else {
        curStartSample = 0;
        engineState = 2;
        engineStart = false;
        engineRunning = true;
        airBrakeTrigger = true;
      }
      break;

    case 2: // Engine running ----
      variableTimerTicks = currentRpmScaled;  // our variable sampling rate!
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      // Engine sound
      if (curEngineSample < sampleCount) {
        a = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100) + 128;
        curEngineSample ++;
      }
      else {
        curEngineSample = 0;
      }

      // Turbo sound
      if (curTurboSample < turboSampleCount) {
        c = (turboSamples[curTurboSample] * throttleDependentTurboVolume / 100 * turboVolumePercentage / 100) + 128;
        curTurboSample ++;
      }
      else {
        curTurboSample = 0;
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
        b = 0; // Ensure full engine volume, so 0!!
        curBrakeSample = 0; // ensure, next sound will start @ first sample
      }


      if (!engineOn) {
        speedPercentage = 100;
        attenuator = 1;
        engineState = 3;
        engineStop = true;
        engineRunning = false;
      }
      break;

    case 3: // Engine stop ----
      variableTimerTicks = 4000000 / sampleRate * speedPercentage / 100; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curEngineSample < sampleCount) {
        a = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100 / attenuator) + 128;
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
        engineStop = false;
      }
      break;

    case 4: // brake bleeding air sound after engine is off ----
      variableTimerTicks = 4000000 / brakeSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curBrakeSample < brakeSampleCount) {
        b = (brakeSamples[curBrakeSample] * brakeVolumePercentage / 100) + 128;
        curBrakeSample ++;
      }
      else {
        curBrakeSample = 0;
        engineState = 0;
      }
      break;

  } // end of switch case

  //dacWrite(DAC1, (int) (a + b - a * b / 255)); // Write mixed output signals to DAC: http://www.vttoth.com/CMS/index.php/technical-notes/68
  //dacWrite(DAC1, (int) ((a + b) / 2)); // Write mixed output signals to DAC
  //dacWrite(DAC1, (int) (constrain((a + (b / 2)), 0, 255))); // Write mixed output signals to DAC
  dacWrite(DAC1, (int) (constrain(((a * 8 / 10) + (b / 2) + (c / 5)), 0, 255))); // Write mixed output signals to DAC


  portEXIT_CRITICAL_ISR(&variableTimerMux);
}

//
// =======================================================================================================
// INTERRUPT FOR FIXED SPEED PLAYBACK (Horn etc., played in parallel with engine sound)
// =======================================================================================================
//

void IRAM_ATTR fixedPlaybackTimer() {

  static uint32_t curHornSample;                // Index of currently loaded horn sample
  static uint32_t curSirenSample;               // Index of currently loaded siren sample
  static uint32_t curSound1Sample;              // Index of currently loaded sound1 sample
  static uint32_t curReversingSample;           // Index of currently loaded reversing beep sample
  static uint32_t curIndicatorSample;           // Index of currently loaded indicator tick sample
  static int32_t a, b, b1, b2;                 // Two input signals for mixer: a = horn or siren, b = reversing sound, indicator sound

  portENTER_CRITICAL_ISR(&fixedTimerMux);

  switch (soundNo) {

    // Group "a" (never more than one active at a time) ----------------------------------------------

    case 0: // Horn "a" ----
      fixedTimerTicks = 4000000 / hornSampleRate; // our fixed sampling rate
      timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true
      curSirenSample = 0;
      curSound1Sample = 0;

      if (hornOn) {
        if (curHornSample < hornSampleCount) {
          a =  (hornSamples[curHornSample] * hornVolumePercentage / 100) + 128;
          curHornSample ++;
        }
        else {
          curHornSample = 0;
          a = 128;
          if (!hornSwitch) hornOn = false; // Latch required to prevent it from popping
        }
      }
      break;

    case 1: // Siren "a" ----
      fixedTimerTicks = 4000000 / sirenSampleRate; // our fixed sampling rate
      timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true
      curHornSample = 0;
      curSound1Sample = 0;

      if (sirenOn) {
        if (curSirenSample < sirenSampleCount) {
          a = (sirenSamples[curSirenSample] * sirenVolumePercentage / 100) + 128;
          curSirenSample ++;
        }
        else {
          curSirenSample = 0;
          a = 128;
          if (!sirenSwitch) sirenOn = false; // Latch required to prevent it from popping
        }
      }
      break;

    case 2: // Sound 1 "a" ----
      fixedTimerTicks = 4000000 / sound1SampleRate; // our fixed sampling rate
      timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true
      curSirenSample = 0;
      curHornSample = 0;

      if (sound1On) {
        if (curSound1Sample < sound1SampleCount) {
          a = (sound1Samples[curSound1Sample] * sound1VolumePercentage / 100) + 128;
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

  // Group "b" (multiple sounds are mixed together) ----------------------------------------------

  // Reversing beep sound "b1" ----
  if (engineRunning && escInReverse) {
    //if (reversingSoundOn && !escIsBraking) { TODO
    fixedTimerTicks = 4000000 / reversingSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curReversingSample < reversingSampleCount) {
      b1 = (reversingSamples[curReversingSample] * reversingVolumePercentage / 100) + 128;
      curReversingSample ++;
    }
    else {
      curReversingSample = 0;
    }
  }
  else {
    curReversingSample = 0;
    b1 = 128;
  }

  // Indicator tick sound "b2" ----
  if (indicatorSoundOn) {
    fixedTimerTicks = 4000000 / indicatorSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curIndicatorSample < indicatorSampleCount) {
      b2 = (indicatorSamples[curIndicatorSample] * indicatorVolumePercentage / 100) + 128;
      curIndicatorSample ++;
    }
    else {
      curIndicatorSample = 0;
      indicatorSoundOn = false;
    }
  }
  else {
    curIndicatorSample = 0;
    b2 = 128;
  }

  // Mixing "b1" + "b2" together ----
  //b = (b1 + b2 - b1 * b2 / 255);
  b = b1 + b2 / 2;

  // DAC output (groups a + b mixed together) ----------------------------------------------------

  //dacWrite(DAC2, (int) (a + b - a * b / 255)); // Write mixed output signals to DAC: http://www.vttoth.com/CMS/index.php/technical-notes/68
  dacWrite(DAC2, (int) ((a + b) / 2)); // Write mixed output signals to DAC

  portEXIT_CRITICAL_ISR(&fixedTimerMux);
}

//
// =======================================================================================================
// PPM SIGNAL READ INTERRUPT
// =======================================================================================================
//

void IRAM_ATTR readPpm() {
  unsigned long timenew = micros();
  unsigned long timediff = timenew - timelast;
  timelast = timenew;

  // reset ppm failsafe trigger signals
  ppmFailsafeCounter = 0;
  failSafe = false;

  if (timediff > 2500) {  // Synch gap detected:
    valuesInt[NUM_OF_CHL] = valuesInt[NUM_OF_CHL] + timediff; // add time
    counter = 0;
    if (average == NUM_OF_AVG) {
      for (int i = 0; i < NUM_OF_CHL + 1; i++) {
        valuesBuf[i] = valuesInt[i] / average;
        valuesInt[i] = 0;
      }
      average = 0;
      ready = true;
    }
    average++;
  }
  else {
    if (counter < NUM_OF_CHL) {
      valuesInt[counter] = valuesInt[counter] + timediff;
      counter++;
    }
  }
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
  pinMode(SERVO1_PIN, INPUT_PULLDOWN);
  pinMode(SERVO2_PIN, INPUT_PULLDOWN);
  pinMode(SERVO3_PIN, INPUT_PULLDOWN);
  pinMode(SERVO4_PIN, INPUT_PULLDOWN);

  pinMode(PPM_PIN, INPUT_PULLDOWN);

  // LED & shaker motor setup (note, that we only have timers from 0 - 15)
  headLight.begin(HEADLIGHT_PIN, 1, 500); // Timer 1, 500Hz
  tailLight.begin(TAILLIGHT_PIN, 2, 500); // Timer 2, 500Hz
  indicatorL.begin(INDICATOR_LEFT_PIN, 3, 500); // Timer 3, 500Hz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 4, 500); // Timer 4, 500Hz
  fogLight.begin(FOGLIGHT_PIN, 5, 500); // Timer 5, 500Hz
  reversingLight.begin(REVERSING_LIGHT_PIN, 6, 500); // Timer 6, 500Hz
  roofLight.begin(ROOFLIGHT_PIN, 7, 500); // Timer 7, 500Hz
  sideLight.begin(SIDELIGHT_PIN, 8, 500); // Timer 8, 500Hz
  beaconLight1.begin(BEACON_LIGHT1_PIN, 9, 500); // Timer 9, 500Hz
  beaconLight2.begin(BEACON_LIGHT2_PIN, 10, 500); // Timer 10, 500Hz

  brakeLight.begin(BRAKELIGHT_PIN, 11, 500); // Timer 11, 500Hz

  shakerMotor.begin(SHAKER_MOTOR_PIN, 13, 500); // Timer 13, 500Hz

  escOut.begin(ESC_OUT_PIN, 15, 50, 16); // Timer 15, 50Hz, 16bit (experimental)

  // Serial setup
  Serial.begin(115200); // USB serial

#ifdef SERIAL_COMMUNICATION
  Serial2.begin(115200, SERIAL_8N1, COMMAND_RX, COMMAND_TX);
#endif

  // PPM Setup
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), readPpm, RISING);
  timelast = micros();
  timelastloop = timelast;

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
    1,           /* priority of the task (1 = low, 3 = medium, 5 = highest)*/
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
  while (millis() <= 1000);

  // Read RC signals for the first time (used for offset calculations)
#if defined SERIAL_COMMUNICATION
  while (!serialInit) { // We need to read the entire buffer, so we do it until the end mark was detected!
    readSerialCommands(); // serial communication (pin 36)
  }
#elif defined PPM_COMMUNICATION
  readPpmCommands();
#else
  // measure RC signals mark space ratio
  readRcSignals();
#endif

  // then compute the RC channel offsets:

  // CH1
  if (indicators) pulseZero[0] = pulseWidth[0]; // store steering offset (only, if "indicators" active)
  else pulseZero[0] = 1500;

  // CH2
  pulseZero[1] = 1500; // This channel is controlled by a 3 position switch, so we don't want auto calibration!

  // CH3
  if (!engineManualOnOff) pulseZero[2] = pulseWidth[2]; // store throttle offset (only, if "engineManualOnOff" inactive)
  else pulseZero[2] = 1500;

  // CH4
  pulseZero[3] = 1500; // This channel is controlled by a potentiometer, so we don't want auto calibration!

  // Calculate RC signal ranges for all channels (0, 1, 2, 3)
  for (uint8_t i = 0; i <= 3; i++) {
    // Input signals
    pulseMaxNeutral[i] = pulseZero[i] + pulseNeutral;
    pulseMinNeutral[i] = pulseZero[i] - pulseNeutral;
    pulseMax[i] = pulseZero[i] + pulseSpan;
    pulseMin[i] = pulseZero[i] - pulseSpan;
    pulseMaxLimit[i] = pulseZero[i] + pulseLimit;
    pulseMinLimit[i] = pulseZero[i] - pulseLimit;
  }
  // ESC output
  escPulseMax = pulseZero[2] + escPulseSpan;
  escPulseMin = pulseZero[2] - escPulseSpan;
}

//
// =======================================================================================================
// READ SERIAL COMMMANDS (only compatible with my "Micro RC" receiver)
// =======================================================================================================
// See: https://github.com/TheDIYGuy999/Micro_RC_Receiver
//      https://forum.arduino.cc/index.php?topic=288234.0

const int32_t numChars = 256;
char receivedChars[numChars];

void readSerialCommands() {
  static unsigned long lastSerialRcv;
  static boolean recvInProgress = false;
  static byte index = 0;
  char startMarker = '<'; // Indicates the begin of our data
  char endMarker = '>'; // Indicates the end of our data
  char currentChar; // The currently read character

  if (millis() - lastSerialRcv > 300) failSafe = true; // Set failsafe mode, if serial command watchdog was triggered
  else failSafe = false;

  if (Serial2.available() > 0) {
    currentChar = Serial2.read();
    lastSerialRcv = millis();

    if (recvInProgress == true) {
      if (currentChar != endMarker) { // End marker not yet detected
        receivedChars[index] = currentChar;
        index++;
        if (index >= numChars) {
          index = numChars - 1;
        }
      }
      else { // End marker detected
        receivedChars[index] = '\0'; // terminate the string, if end marker detected
        recvInProgress = false;
        index = 0;
        parseSerialCommands(); // Call parsing sub function
      }
    }

    if (currentChar == startMarker) { // Start marker detected
      recvInProgress = true;
    }
  }

  // Falisafe for RC signals
  failsafeRcSignals();

}

// Parsing sub function ----
void parseSerialCommands() {

  char * strtokIindex;
  char delimiter[2] = "\n"; // used to separate the variables (generated by "println" on the receiver)

  // split the data into its parts
  // order see "sendSerialCommands()" in Micro RC Receiver code: https://github.com/TheDIYGuy999/Micro_RC_Receiver
  strtokIindex = strtok(receivedChars, delimiter);
  axis1 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  axis2 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  axis3 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  axis4 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  pot1 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  mode1 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  mode2 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  momentary1 = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  hazard = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  left = atoi(strtokIindex);
  strtokIindex = strtok(NULL, delimiter);
  right = atoi(strtokIindex);

  // Convert signals to servo pulses in ms
  pulseWidth[0] = map(axis1, 0, 100, 1000, 2000); // CH1 Steering
  pulseWidth[1] = map(axis2, 0, 100, 1000, 2000); // CH2
  pulseWidth[2] = map(axis3, 0, 100, 1000, 2000); // CH3 Throttle
  pulseWidth[3] = map(pot1, 0, 100, 1000, 2000); // Pot1 Horn

  // Invert RC signals
  invertRcSignals();

  serialInit = true; // first serial data block was processed
}

//
// =======================================================================================================
// READ PPM MULTI CHANNEL COMMMANDS (compatible with many receivers)
// =======================================================================================================
//

void readPpmCommands() {
  pulseWidth[0] = valuesBuf[0]; // CH1 Steering
  pulseWidth[1] = 1500; // CH2
  pulseWidth[2] = valuesBuf[1]; // CH3 Throttle
  pulseWidth[3] = valuesBuf[2]; // Pot1 Horn

  // Invert RC signals
  invertRcSignals();

  // Falisafe for RC signals
  failsafeRcSignals();
}

//
// =======================================================================================================
// PRINT SERIAL DATA
// =======================================================================================================
//

void showParsedData() {
  static unsigned long lastSerialTime;
#ifdef SERIAL_DEBUG
  if (millis() - lastSerialTime > 300) { // Print the data every 300ms
    lastSerialTime = millis();
    Serial.print("axis 1 ");
    Serial.println(axis1);
    Serial.print("axis 2 ");
    Serial.println(axis2);
    Serial.print("axis 3 ");
    Serial.println(axis3);
    Serial.print("axis 4 ");
    Serial.println(axis4);
    Serial.print("pot 1 ");
    Serial.println(pot1);
    Serial.print("mode 1 ");
    Serial.println(mode1);
    Serial.print("mode 2 ");
    Serial.println(mode2);
    Serial.print("momenrary 1 ");
    Serial.println(momentary1);
    Serial.print("hazard ");
    Serial.println(hazard);
    Serial.print("left ");
    Serial.println(left);
    Serial.print("right ");
    Serial.println(right);
    Serial.print("loop time ");
    Serial.println(loopTime);
    Serial.println("");
  }
#endif
}

//
// =======================================================================================================
// READ PWM RC SIGNALS
// =======================================================================================================
//

void readRcSignals() {
  // measure RC signal pulsewidth:

  // CH1 Steering
  if (indicators) pulseWidth[0] = pulseIn(SERVO1_PIN, HIGH, 50000);
  else pulseWidth[0] = 1500;

  // CH2 (not used, gearbox servo)
  pulseWidth[1] = pulseIn(SERVO2_PIN, HIGH, 50000);

  // CH3 Throttle
  pulseWidth[2] = pulseIn(SERVO3_PIN, HIGH, 50000);
  if (pulseWidth[2] == 0) failSafe = true; // 0, if timeout (signal loss)
  else failSafe = false;

  // CH4 Additional sound trigger (RC signal with 3 positions)
  if (pwmSoundTrigger) pulseWidth[3] = pulseIn(SERVO4_PIN, HIGH, 50000);
  else pulseWidth[3] = 1500;

  // Invert RC signals
  invertRcSignals();

  // Falisafe for RC signals
  failsafeRcSignals();
}

//
// =======================================================================================================
// INVERT RC SIGNALS (if your signals are inverted)
// =======================================================================================================
//

void invertRcSignals() {
  if (INDICATOR_DIR) pulseWidth[0] = map(pulseWidth[0], 0, 3000, 3000, 0); // invert steering direction
}

//
// =======================================================================================================
// RC SIGNAL FAILSAFE POSITIONS (if serial signal lost)
// =======================================================================================================
//

void failsafeRcSignals() {

  // PPM signal surveillance (serial & PWM communication does not need any actions here) --------
#if defined PPM_COMMUNICATION
  static unsigned long ppmFailsafeMillis;

  if (millis() - ppmFailsafeMillis > 50) { // Every 50ms
    ppmFailsafeMillis = millis();

    if (ppmFailsafeCounter < 10) ppmFailsafeCounter ++ ; //it will be reset in the ppm interrupt
  }
  if (ppmFailsafeCounter > 5) failSafe = true;
#endif

  // Failsafe actions --------
  if (failSafe) pulseWidth[2] = pulseZero[2]; // Throttle to zero position!
}

//
// =======================================================================================================
// HORN TRIGGERING, SIREN TRIGGERING, SOUND1 TRIGGERING BY CH4 (POT)
// =======================================================================================================
//

void triggerHorn() {
  if (pwmSoundTrigger) { // PWM RC signal mode --------------------------------------------

    // detect horn trigger ( impulse length > 1700us) -------------
    if (pulseWidth[3] > (pulseMaxNeutral[3] + 180) && pulseWidth[3] < pulseMaxLimit[3]) {
      hornSwitch = true;
      //sirenSwitch = false;
      soundNo = 0;  // 0 = horn
    }
    else hornSwitch = false;

    // detect siren trigger ( impulse length < 1300us) ----------
    if (pulseWidth[3] < (pulseMinNeutral[3] - 180) && pulseWidth[3] > pulseMinLimit[3]) {
      sirenSwitch = true;
      //hornSwitch = false;
      soundNo = 1;  // 1 = siren
    }
    else sirenSwitch = false;

    // Sound 1 triggered via momentary1 button (Micro RC in serial mode only) ---------
    if (momentary1  && !hornSwitch  && !sirenSwitch) {
      sound1Switch = true;
      soundNo = 2; // 2 = sound1
    }
    else sound1Switch = false;

  }
  else { // High level triggering mode ---------------------------------------------------

    // detect horn trigger (constant high level)
    if (digitalRead(SERVO4_PIN)) {
      hornSwitch = true;
      soundNo = 0;  // 0 = horn
    }
    else hornSwitch = false;
  }

  // Latches (required to prevent sound seams from popping) --------------------------------

  if (hornSwitch) {
    hornOn = true;
    sirenOn = false;
    sound1On = false;
  }
  if (sirenSwitch) {
    sirenOn = true;
    hornOn = false;
    sound1On = false;
  }
  if (sound1Switch) {
    sound1On = true;
    sirenOn = false;
    hornOn = false;
  }
}

//
// =======================================================================================================
// INDICATOR (TURN SIGNAL) TRIGGERING BY CH1 (STEERING)
// =======================================================================================================
//

void triggerIndicators() {

  // detect left indicator trigger ( impulse length > 1700us) -------------
  if (pulseWidth[0] > (pulseMaxNeutral[0] + 30) && pulseWidth[0] < pulseMaxLimit[0]) indicatorLon = true;
  if (pulseWidth[0] < pulseMaxNeutral[0]) indicatorLon = false;


  // detect right indicator trigger ( impulse length < 1300us) ----------
  if (pulseWidth[0] < (pulseMinNeutral[0] - 30) && pulseWidth[0] > pulseMinLimit[0]) indicatorRon = true;
  if (pulseWidth[0] > pulseMinNeutral[0]) indicatorRon = false;
}

//
// =======================================================================================================
// MAP PULSEWIDTH TO THROTTLE CH3
// =======================================================================================================
//

void mapThrottle() {

  static unsigned long reversingMillis; // TODO

  // Input is around 1000 - 2000us, output 0-500 for forward and backwards

  // check if the pulsewidth looks like a servo pulse
  if (pulseWidth[2] > pulseMinLimit[2] && pulseWidth[2] < pulseMaxLimit[2]) {
    if (pulseWidth[2] < pulseMin[2]) pulseWidth[2] = pulseMin[2]; // Constrain the value
    if (pulseWidth[2] > pulseMax[2]) pulseWidth[2] = pulseMax[2];

    // calculate a throttle value from the pulsewidth signal
    if (pulseWidth[2] > pulseMaxNeutral[2]) {
      currentThrottle = map(pulseWidth[2], pulseMaxNeutral[2], pulseMax[2], 0, 500);
      throttleReverse = false;
    }
    else if (pulseWidth[2] < pulseMinNeutral[2]) {
      currentThrottle = map(pulseWidth[2], pulseMinNeutral[2], pulseMin[2], 0, 500);
      throttleReverse = true;
    }
    else {
      currentThrottle = 0;
    }
  }

  // Calculate throttle dependent engine volume
  if (!escIsBraking && engineRunning) throttleDependentVolume = map(currentThrottle, 0, 500, engineIdleVolumePercentage, 100);
  else throttleDependentVolume = engineIdleVolumePercentage;

  // Calculate engine rpm dependent turbo volume
  if (!escIsBraking && engineRunning) throttleDependentTurboVolume = map(currentRpm, 0, 500, turboIdleVolumePercentage, 100);
  else throttleDependentTurboVolume = turboIdleVolumePercentage;

  // reversing sound trigger signal (TODO)
  /*if (reverseSoundMode == 1) {
    if (pulseWidth[2] <= pulseMaxNeutral[2]) {
      reversingMillis = millis();
    }

    if (millis() - reversingMillis > 200) {
      reversingSoundOn = true;
    }
    else reversingSoundOn = false;
    }

    if (reverseSoundMode == 2) {
    if (pulseWidth[2] >= pulseMinNeutral[2]) {
      reversingMillis = millis();
    }

    if (millis() - reversingMillis > 200) {
      reversingSoundOn = true;
    }
    else reversingSoundOn = false;
    }

    if (reverseSoundMode == 0) {
    reversingSoundOn = false;
    }*/
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
    if (mappedThrottle > (currentRpm + acc) && (currentRpm + acc) < maxRpm && engineState == 2 && !escIsBraking && engineRunning) {
      if (!airBrakeTrigger) { // No acceleration, if brake release noise still playing
        currentRpm += acc;
        if (currentRpm > maxRpm) currentRpm = maxRpm;
      }
    }

    // Decelerate engine
    if (mappedThrottle < currentRpm || escIsBraking) {
      currentRpm -= dec;
      if (currentRpm < minRpm) currentRpm = minRpm;
    }


    // Speed (sample rate) output
    currentRpmScaled = map(currentRpm, minRpm, maxRpm, maxSampleInterval, minSampleInterval);
  }

  // Brake light trigger TODO
  /*if (mappedThrottle < (currentRpm - 200)) slowingDown = true;
    if (mappedThrottle >= (currentRpm - 10)) slowingDown = false;*/

  // Print debug infos
#ifdef DEBUG // can slow down the playback loop!
  if (millis() - printMillis > 1000) { // Every 1000ms
    printMillis = millis();

    Serial.println("CH1");
    Serial.println(pulseWidth[0]);
    Serial.println(pulseMinNeutral[0]);
    Serial.println(pulseMaxNeutral[0]);
    Serial.println("CH2");
    Serial.println(pulseWidth[1]);
    Serial.println(pulseMinNeutral[1]);
    Serial.println(pulseMaxNeutral[1]);
    Serial.println("CH3");
    Serial.println(pulseWidth[2]);
    Serial.println(pulseMinNeutral[2]);
    Serial.println(pulseMaxNeutral[2]);
    Serial.println("CH4");
    Serial.println(pulseWidth[3]);
    Serial.println(pulseMinNeutral[3]);
    Serial.println(pulseMaxNeutral[3]);
    Serial.println(" ");
    /*Serial.println(currentThrottle);
    Serial.println(mappedThrottle);
    Serial.println(currentRpm);
    Serial.println(currentRpmScaled);
    Serial.println(engineState);
    Serial.println(" ");
    Serial.println(loopTime);
    Serial.println(" ");
    Serial.println(airBrakeTrigger);
    Serial.println(EngineWasAboveIdle);
    Serial.println(throttleDependentVolume);
    Serial.println(sound1On);
    Serial.println(soundNo);
    Serial.print("PPM Failsafe Counter ");
    Serial.println(ppmFailsafeCounter);
    Serial.print("failSafe ");
    Serial.println(failSafe);*/

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
    if (currentThrottle > 80 || driveState != 0) idleDelayMillis = millis(); // reset delay timer, if throttle not in neutral

    if (millis() - idleDelayMillis > 15000) {
      engineOn = false; // after delay, switch engine off
    }

    if (millis() - idleDelayMillis > 10000) {
      lightsOn = false; // after delay, switch light off
    }

    // air brake noise trigggering TODO
    /*if (millis() - idleDelayMillis > 1000) {
      if (EngineWasAboveIdle) {
        airBrakeTrigger = true; // after delay, trigger air brake noise
      }
      }*/

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
  //if (reversingSoundOn) reversingLight.on();
  if (engineRunning && escInReverse) reversingLight.on();
  else reversingLight.off();

  // Beacons (blue light) ----
  if (sirenOn) {
    if (doubleFlashBlueLight) {
      beaconLight2.flash(30, 80, 380, 2); // Simulate double flash lights
      beaconLight1.flash(30, 80, 400, 2); // Simulate double flash lights
    }
    else {
      beaconLight2.flash(30, 400, 0, 0); // Simulate rotating beacon lights with short flashes
      beaconLight1.flash(30, 420, 0, 0); // Simulate rotating beacon lights with short flashes
    }
  }
  else {
    beaconLight2.off();
    beaconLight1.off();
  }

  // Headlights, tail lights ----
  if (lightsOn) {
    headLight.on();
    if (escIsBraking) {
      tailLight.on();  // Taillights (full brightness)
      brakeLight.on(); // Brakelight on
    }
    else {
      tailLight.pwm(50); // Taillights (reduced brightness)
      brakeLight.off(); // Brakelight off
    }
  }
  else {
    headLight.off();
    tailLight.off();
    brakeLight.off();
  }

  if (!hazard) {
    // Indicators (turn signals, blinkers) ----
    if (indicatorLon) {
      if (indicatorL.flash(375, 375, 0, 0)) indicatorSoundOn = true; // Left indicator
    }
    else indicatorL.off();

    if (indicatorRon) {
      if (indicatorR.flash(375, 375, 0, 0)) indicatorSoundOn = true; // Right indicator
    }
    else indicatorR.off();
  }
  else { // Hazard lights on, if no connection to transmitter (serial control mode only)
    if (indicatorL.flash(375, 375, 0, 0)) indicatorSoundOn = true;
    indicatorR.flash(375, 375, 0, 0);
  }

  // Foglights (serial control mode only) ----
  if (lightsOn && !mode2) fogLight.on();
  else fogLight.off();

  // Roof lights (serial control mode only) ----
  if (lightsOn && !mode1) roofLight.on();
  else roofLight.off();

  // Sidelights ----
  if (engineOn) sideLight.on();
  else sideLight.off();

}

//
// =======================================================================================================
// SHAKER (simulates engine vibrations)
// =======================================================================================================
//

void shaker() {
  int32_t shakerRpm;

  // Set desired shaker rpm
  if (engineRunning) shakerRpm = map(currentRpm, minRpm, maxRpm, shakerIdle, shakerFullThrottle);
  if (engineStart) shakerRpm = shakerStart;
  if (engineStop) shakerRpm = shakerStop;

  // Shaker on
  if (engineRunning || engineStart || engineStop) shakerMotor.pwm(shakerRpm);
  else shakerMotor.off();
}


//
// =======================================================================================================
// ESC CONTROL
// =======================================================================================================
//

// If you connect your ESC to pin 33, the vehicle inertia is simulated. Direct brake (crawler) ESC required
// *** WARNING!! Do it at your own risk!! There is a falisafe function in case, the signal input from the
// receiver is lost, but if the ESP32 crashes, the vehicle could get out of control!! ***

void esc() {
  static uint32_t escPulseWidth = 1500;
  static uint32_t escSignal;
  static unsigned long escMillis;
  static unsigned long lastStateTime;
  static int8_t pulse; // -1 = reverse, 0 = neutral, 1 = forward
  static int8_t escPulse; // -1 = reverse, 0 = neutral, 1 = forward
  static int8_t driveRampRate;
  static int8_t brakeRampRate;

  if (millis() - escMillis > escRampTime) { // About very 2 - 6ms
    escMillis = millis();

    // calulate throttle dependent brake steps
    brakeRampRate = map (currentThrottle, 0, 500, 1, escBrakeSteps);

    // Emergency ramp rates for falisafe
    if (failSafe) {
      brakeRampRate = escBrakeSteps;
      driveRampRate = escBrakeSteps;
    }
    else driveRampRate = 1;

    // Comparators
    if (pulseWidth[2] > pulseMaxNeutral[2] && pulseWidth[2] < pulseMaxLimit[2]) pulse = 1; // 1 = Forward
    else if (pulseWidth[2] < pulseMinNeutral[2] && pulseWidth[2] > pulseMinLimit[2]) pulse = -1; // -1 = Backwards
    else pulse = 0; // 0 = Neutral

    if (escPulseWidth > pulseMaxNeutral[2] && escPulseWidth < pulseMaxLimit[2]) escPulse = 1; // 1 = Forward
    else if (escPulseWidth < pulseMinNeutral[2] && escPulseWidth > pulseMinLimit[2]) escPulse = -1; // -1 = Backwards
    else escPulse = 0; // 0 = Neutral

#ifdef DRIVE_STATE_DEBUG
    if (millis() - lastStateTime > 300) { // Print the data every 300ms
      lastStateTime = millis();
      Serial.println(driveState);
      Serial.println(pulse);
      Serial.println(escPulse);
      Serial.println(escPulseMin);
      Serial.println(escPulseMax);
      Serial.println(brakeRampRate);
      Serial.println("");
    }
#endif

    // Drive state state machine
    switch (driveState) {

      case 0: // Standing still ---------------------------------------------------------------------
        escIsBraking = false;
        escInReverse = false;
        escPulseWidth = pulseZero[2];  // ESC to neutral position

        if (pulse == 1 && engineRunning) driveState = 1; // Driving forward
        if (pulse == -1 && engineRunning) driveState = 3; // Driving backwards
        break;

      case 1: // Driving forward ---------------------------------------------------------------------
        escIsBraking = false;
        escInReverse = false;
        if (escPulseWidth < pulseWidth[2]) escPulseWidth += driveRampRate;
        if (escPulseWidth > pulseWidth[2] && escPulseWidth > pulseZero[2]) escPulseWidth -= driveRampRate;

        if (pulse == -1 && escPulse == 1) driveState = 2; // Braking forward
        if (pulse == 0 && escPulse == 0) driveState = 0; // standing still
        break;

      case 2: // Braking forward ---------------------------------------------------------------------
        escIsBraking = true;
        escInReverse = false;
        if (escPulseWidth > pulseZero[2]) escPulseWidth -= brakeRampRate; // brake with variable deceleration

        if (pulse == 0 && escPulse == 1) {
          driveState = 1; // Driving forward
          airBrakeTrigger = true;
        }
        if (pulse == 0 && escPulse == 0) {
          driveState = 0; // standing still
          airBrakeTrigger = true;
        }
        break;

      case 3: // Driving backwards ---------------------------------------------------------------------
        escIsBraking = false;
        escInReverse = true;
        if (escPulseWidth > pulseWidth[2]) escPulseWidth -= driveRampRate;
        if (escPulseWidth < pulseWidth[2] && escPulseWidth < pulseZero[2]) escPulseWidth += driveRampRate;

        if (pulse == 1 && escPulse == -1) driveState = 4; // Braking backwards
        if (pulse == 0 && escPulse == 0) driveState = 0; // standing still
        break;

      case 4: // Braking backwards ---------------------------------------------------------------------
        escIsBraking = true;
        escInReverse = true;
        if (escPulseWidth < pulseZero[2]) escPulseWidth += brakeRampRate; // brake with variable deceleration

        if (pulse == 0 && escPulse == -1) {
          driveState = 3; // Driving backwards
          airBrakeTrigger = true;
        }
        if (pulse == 0 && escPulse == 0) {
          driveState = 0; // standing still
          airBrakeTrigger = true;
        }
        break;

    } // End of state machine


    // ESC control
    escSignal = map(escPulseWidth, escPulseMin, escPulseMax, 3278, 6553); // 1 - 2ms (5 - 10% pulsewidth of 65534)
    escOut.pwm(escSignal);
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

#if defined  SERIAL_COMMUNICATION
  readSerialCommands(); // Serial communication (pin 36)
  showParsedData();
#elif defined PPM_COMMUNICATION
  readPpmCommands(); // PPM communication (pin 34)
#else
  // measure RC signals mark space ratio
  readRcSignals();
#endif

  // Horn triggering
  triggerHorn();

  // Indicator (turn signal) triggering
  triggerIndicators();
}

//
// =======================================================================================================
// 1st MAIN TASK, RUNNING ON CORE 0 (Interrupts are running on this core as well)
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

    // Shaker control
    shaker();

    // ESC control (Crawler ESC with direct brake on pin 33)
    esc();

    // measure loop time
    loopTime = loopDuration();
  }
}
