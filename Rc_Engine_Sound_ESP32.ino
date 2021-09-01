/* RC engine sound & LED controller for Arduino ESP32. Written by TheDIYGuy999
    Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

 *  ***** ESP32 CPU frequency must be set to 240MHz! *****
    ESP32 macOS Big Sur fix see: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/BigSurFix.md

   Sound files converted with: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/Audio2Header.html
   Original converter code by bitluni (send him a high five, if you like the code)

   Parts of automatic transmision code from Wombii's fork: https://github.com/Wombii/Rc_Engine_Sound_ESP32

   Dashboard, Neopixel and SUMD support by Gamadril: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
*/

const float codeVersion = 7.9; // Software revision.

//
// =======================================================================================================
// ERROR CODES (INDICATOR LIGHTS)
// =======================================================================================================
//
/* Constantly on = no SBUS signal (check "sbusInverted" true / false in "2_adjustmentsRemote.h")
   Number of blinks = this channel signal is not between 1400 and 1600 microseconds and can't be auto calibrated
   (check channel trim settings)
*/

//
// =======================================================================================================
// ! ! I M P O R T A N T ! !   ALL USER SETTINGS ARE DONE IN THE FOLLOWING TABS, WHICH ARE DISPLAYED ABOVE
// (ADJUST THEM BEFORE CODE UPLOAD), DO NOT CHANGE ANYTHING IN THIS TAB EXCEPT THE DEBUG OPTIONS
// =======================================================================================================
//

// All the required user settings are done in the following .h files:
#include "1_adjustmentsVehicle.h"       // <<------- Select the vehicle you want to simulate
#include "2_adjustmentsRemote.h"        // <<------- Remote control system related adjustments
#include "3_adjustmentsESC.h"           // <<------- ESC related adjustments
#include "4_adjustmentsTransmission.h"  // <<------- Transmission related adjustments
#include "5_adjustmentsShaker.h"        // <<------- Shaker related adjustments
#include "6_adjustmentsLights.h"        // <<------- Lights related adjustments
#include "7_adjustmentsServos.h"        // <<------- Servo output related adjustments
#include "8_adjustmentsSound.h"         // <<------- Sound related adjustments
#include "9_adjustmentsDashboard.h"     // <<------- Dashboard related adjustments

// DEBUG options can slow down the playback loop! Only uncomment them for debugging, may slow down your system!
#define CHANNEL_DEBUG // uncomment it for input signal debugging informations
//#define ESC_DEBUG // uncomment it to debug the ESC
//#define AUTO_TRANS_DEBUG // uncomment it to debug the automatic transmission
//#define MANUAL_TRANS_DEBUG // uncomment it to debug the manual transmission
//#define TRACKED_DEBUG // debugging tracked vehicle mode
//#define SERVO_DEBUG // uncomment it for servo calibration in BUS communication mode

// TODO = Things to clean up!

//
// =======================================================================================================
// LIRBARIES & HEADER FILES, REQUIRED ESP32 BOARD DEFINITION
// =======================================================================================================
//

// Header files
#include "headers/curves.h" // Nonlinear throttle curve arrays

// Libraries (you have to install all of them in the "Arduino sketchbook"/libraries folder)
// !! Do NOT install the libraries in the sketch folder.
#include <statusLED.h> // https://github.com/TheDIYGuy999/statusLED <<------- required for LED control
#include <SBUS.h>      // https://github.com/TheDIYGuy999/SBUS      <<------- you need to install my fork of this library!
#include <rcTrigger.h> // https://github.com/TheDIYGuy999/rcTrigger <<------- required for RC signal processing
#include <IBusBM.h>    // https://github.com/bmellink/IBusBM        <<------- required for IBUS interface
#include <TFT_eSPI.h>  // https://github.com/Bodmer/TFT_eSPI        <<------- required for LCD dashboard
#include <FastLED.h>   // https://github.com/FastLED/FastLED        <<------- required for Neopixel support

// Plugins (included, but not programmed by TheDIYGuy999)
#include "src/dashboard.h" // For LCD dashboard. See: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
#include "src/SUMD.h" // For Graupner SUMD interface. See: https://github.com/Gamadril/Rc_Engine_Sound_ESP32

// No need to install these, they come with the ESP32 board definition
#include "driver/rmt.h" // for PWM signal detection
#include "driver/mcpwm.h" // for servo PWM output

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

// Serial command pins for SBUS & IBUS -----
#define COMMAND_RX 36 // pin 36, labelled with "VP", connect it to "Micro RC Receiver" pin "TXO"
#define COMMAND_TX 39 // pin 39, labelled with "VN", only used as a dummy, not connected

// PPM signal pin (multiple channel input with only one wire) -----
#define PPM_PIN 36

// PWM RC signal pins (active, if no other communications profile is enabled) -----
// Channel numbers may be different on your recveiver!
//CH1: (steering)
//CH2: (gearbox) (left throttle in TRACKED_MODE)
//CH3: (throttle) (right throttle in TRACKED_MODE)
//CH4: (horn and bluelight / siren)
//CH5: (high / low beam, transmission neutral, jake brake etc.)
//CH6: (indicators, hazards)
#define PWM_CHANNELS_NUM 6 // Number of PWM signal input pins
const uint8_t PWM_CHANNELS[PWM_CHANNELS_NUM] = { 1, 2, 3, 4, 5, 6}; // Channel numbers
const uint8_t PWM_PINS[PWM_CHANNELS_NUM] = { 13, 12, 14, 27, 35, 34 }; // Input pin numbers

#define ESC_OUT_PIN 33 // connect crawler type ESC here (working fine, but use it at your own risk! Not supported in TRACKED_MODE) -----

#define STEERING_PIN 13 // CH1 output for steering servo (bus communication only)
#define SHIFTING_PIN 12 // CH2 output for shifting servo (bus communication only)
#define WINCH_PIN 14 // CH3 output for winch servo (bus communication only)
#define COUPLER_PIN 27 // CH4 output for coupler (5th. wheel) servo (bus communication only)

#ifdef PROTOTYPE_36 // switching headlight pin depending on the board variant (do not uncomment it, or it will cause boot issues!)
#define HEADLIGHT_PIN 0 // White headllights connected to pin D0, which only exists on the 36 pin ESP32 board (causes boot issues, if used!)
#else
#define HEADLIGHT_PIN 3 // 3 = RX0 pin, (1 = TX0 is not usable) white headllights
#endif

#define TAILLIGHT_PIN 15 // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2 // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4 // Orange right indicator (turn signal) light
#define FOGLIGHT_PIN 16 // (16 = RX2) Fog lights
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define ROOFLIGHT_PIN 5 // Roof lights (high beam, if "define SEPARATE_FULL_BEAM")
#define SIDELIGHT_PIN 18 // Side lights (connect roof ligthts here, if "define SEPARATE_FULL_BEAM")
#define BEACON_LIGHT2_PIN 19 // Blue beacons light
#define BEACON_LIGHT1_PIN 21 // Blue beacons light
#define CABLIGHT_PIN 22 // Cabin lights

#define RGB_LEDS_PIN 0 // Pin is used for WS2812 LED control

#if defined THIRD_BRAKLELIGHT
#define BRAKELIGHT_PIN 32 // Upper brake lights
#else
#define COUPLER_SWITCH_PIN 32 // switch for trailer coupler sound
#endif

#define SHAKER_MOTOR_PIN 23 // Shaker motor (shaking truck while idling and engine start / stop)

#define DAC1 25 // connect pin25 (do not change the pin) to a 10kOhm resistor
#define DAC2 26 // connect pin26 (do not change the pin) to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 10kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
// of a PAM8403 amplifier and allows to adjust the volume. This way, two speakers can be used.

// Objects *************************************************************************************
// Status LED objects (also used for PWM shaker motor and ESC control) -----
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
statusLED shakerMotor(false);
statusLED cabLight(false);
statusLED brakeLight(false);

statusLED escOut(false);

// rcTrigger objects -----
// Analog or 3 position switches (short / long pressed time)
rcTrigger functionR100u(200); // 200ms required!
rcTrigger functionR100d(100);
rcTrigger functionR75u(300); // 300ms required!
rcTrigger functionR75d(300); // 300ms required!
rcTrigger functionL100l(100);
rcTrigger functionL100r(100);
rcTrigger functionL75l(300); // 300ms required!
rcTrigger functionL75r(300); // 300ms required!

// Latching 2 position
rcTrigger mode1Trigger(100);
rcTrigger mode2Trigger(100);

// momentary buttons
rcTrigger momentary1Trigger(100);

// Flags
rcTrigger hazardsTrigger(100);
rcTrigger indicatorLTrigger(100);
rcTrigger indicatorRTrigger(100);

// Dashboard
Dashboard dashboard;

// Neopixel
#define RGB_LEDS_COUNT 1
CRGB rgbLEDs[RGB_LEDS_COUNT];

// Global variables **********************************************************************

// PWM processing variables
#define RMT_TICK_PER_US 1
// determines how many clock cycles one "tick" is
// [1..255], source is generally 80MHz APB clk
#define RMT_RX_CLK_DIV (80000000/RMT_TICK_PER_US/1000000)
// time before receiver goes idle (longer pulses will be ignored)
#define RMT_RX_MAX_US 3500
uint32_t maxPwmRpmPercentage = 380; // Limit required to prevent controller from crashing @ high engine RPM

// PPM signal processing variables
#define NUM_OF_PPM_CHL 8 // The number of channels inside our PPM signal (8 is the maximum!)
#define NUM_OF_PPM_AVG 1 // Number of averaging passes (usually one, more will be slow)
volatile int ppmInp[NUM_OF_PPM_CHL + 1] = {0}; // Input values
volatile int ppmBuf[16] = {0}; // Buffered values TODO, was [NUM_OF_PPM_CHL + 1]
volatile byte counter = NUM_OF_PPM_CHL;
volatile byte average  = NUM_OF_PPM_AVG;
volatile boolean ready = false;
volatile unsigned long timelast;
unsigned long timelastloop;
uint32_t maxPpmRpmPercentage = 390; // Limit required to prevent controller from crashing @ high engine RPM

// SBUS signal processing variables
SBUS sBus(Serial2); // SBUS object on Serial 2 port
// channel, fail safe, and lost frames data
uint16_t SBUSchannels[16];
bool SBUSfailSafe;
bool SBUSlostFrame;
bool sbusInit;
uint32_t maxSbusRpmPercentage = 390; // Limit required to prevent controller from crashing @ high engine RPM

// SUMD signal processing variables
HardwareSerial serial(2);
SUMD sumd(serial); // SUMD object on Serial 2 port
// channel, fail safe, and lost frames data
uint16_t SUMDchannels[16]; // only 12 are usable!
bool SUMD_failsafe;
bool SUMD_frame_lost;
bool SUMD_init;
uint32_t maxSumdRpmPercentage = 390; // Limit required to prevent controller from crashing @ high engine RPM

// IBUS signal processing variables
IBusBM iBus;    // IBUS object
bool ibusInit;
uint32_t maxIbusRpmPercentage = 340; // Limit required to prevent controller from crashing @ high engine RPM (was 350, but sometimes crashing)

// Interrupt latches
volatile boolean couplerSwitchInteruptLatch; // this is enabled, if the coupler switch pin change interrupt is detected

// Control input signals
#define PULSE_ARRAY_SIZE 14                              // 13 channels (+ the unused CH0)
uint16_t pulseWidthRaw[PULSE_ARRAY_SIZE];                // Current RC signal RAW pulse width [X] = channel number
uint16_t pulseWidthRaw2[PULSE_ARRAY_SIZE];               // Current RC signal RAW pulse width with linearity compensation [X] = channel number
uint16_t pulseWidth[PULSE_ARRAY_SIZE];                   // Current RC signal pulse width [X] = channel number
int16_t pulseOffset[PULSE_ARRAY_SIZE];                   // Offset for auto zero adjustment

uint16_t pulseMaxNeutral[PULSE_ARRAY_SIZE];              // PWM input signal configuration storage variables
uint16_t pulseMinNeutral[PULSE_ARRAY_SIZE];
uint16_t pulseMax[PULSE_ARRAY_SIZE];
uint16_t pulseMin[PULSE_ARRAY_SIZE];
uint16_t pulseMaxLimit[PULSE_ARRAY_SIZE];
uint16_t pulseMinLimit[PULSE_ARRAY_SIZE];

uint16_t pulseZero[PULSE_ARRAY_SIZE];                    // Usually 1500 (The mid point of 1000 - 2000 Microseconds)
uint16_t pulseLimit = 1100;                              // pulseZero +/- this value (1100)
uint16_t pulseMinValid = 700;                            // The minimum valid pulsewidth (was 950)
uint16_t pulseMaxValid = 2300;                           // The maximum valid pulsewidth (was 2050)
bool autoZeroDone;                                       // Auto zero offset calibration done
#define NONE 16                                          // The non existing "Dummy" channel number (usually 16) TODO

volatile boolean failSafe = false;                       // Triggered in emergency situations like: throttle signal lost etc.

boolean mode1;                                           // Signal state variables
boolean mode2;
boolean momentary1;
boolean hazard;
boolean left;
boolean right;
boolean unlock5thWheel;
boolean winchPull;
boolean winchRelease;

boolean winchEnabled;

// Sound
volatile boolean engineOn = false;                       // Signal for engine on / off
volatile boolean engineStart = false;                    // Active, if engine is starting up
volatile boolean engineRunning = false;                  // Active, if engine is running
volatile boolean engineStop = false;                     // Active, if engine is shutting down
volatile boolean jakeBrakeRequest = false;               // Active, if engine jake braking is requested
volatile boolean engineJakeBraking = false;              // Active, if engine is jake braking
volatile boolean wastegateTrigger = false;               // Trigger wastegate (blowoff) after rapid throttle drop
volatile boolean blowoffTrigger = false;                 // Trigger jake brake sound (blowoff) after rapid throttle drop
volatile boolean dieselKnockTrigger = false;             // Trigger Diesel ignition "knock"
volatile boolean dieselKnockTriggerFirst = false;        // The first  Diesel ignition "knock" per sequence
volatile boolean airBrakeTrigger = false;                // Trigger for air brake noise
volatile boolean parkingBrakeTrigger = false;            // Trigger for air parking brake noise
volatile boolean shiftingTrigger = false;                // Trigger for shifting noise
volatile boolean hornTrigger = false;                    // Trigger for horn on / off
volatile boolean sirenTrigger = false;                   // Trigger for siren  on / off
volatile boolean sound1trigger = false;                  // Trigger for sound1  on / off
volatile boolean couplingTrigger = false;                // Trigger for trailer coupling  sound
volatile boolean uncouplingTrigger = false;              // Trigger for trailer uncoupling  sound
volatile boolean bucketRattleTrigger = false;            // Trigger for bucket rattling  sound
volatile boolean indicatorSoundOn = false;               // active, if indicator bulb is on

// Sound latches
volatile boolean hornLatch = false;                      // Horn latch bit
volatile boolean sirenLatch = false;                     // Siren latch bit

// Sound volumes
volatile uint16_t throttleDependentVolume = 0;           // engine volume according to throttle position
volatile uint16_t throttleDependentRevVolume = 0;        // engine rev volume according to throttle position
volatile uint16_t rpmDependentJakeBrakeVolume = 0;       // Engine rpm dependent jake brake volume
volatile uint16_t throttleDependentKnockVolume = 0;      // engine Diesel knock volume according to throttle position
volatile uint16_t rpmDependentKnockVolume = 0;           // engine Diesel knock volume according to engine RPM
volatile uint16_t throttleDependentTurboVolume = 0;      // turbo volume according to rpm
volatile uint16_t throttleDependentFanVolume = 0;        // cooling fan volume according to rpm
volatile uint16_t throttleDependentChargerVolume = 0;    // cooling fan volume according to rpm
volatile uint16_t rpmDependentWastegateVolume = 0;       // wastegate volume according to rpm
// for excavator mode:
volatile uint16_t hydraulicPumpVolume = 0;               // hydraulic pump volume
volatile uint16_t hydraulicFlowVolume = 0;               // hydraulic flow volume
volatile uint16_t trackRattleVolume = 0;                 // track rattling volume
volatile uint16_t hydraulicDependentKnockVolume = 100;   // engine Diesel knock volume according to hydraulic load
volatile uint16_t hydraulicLoad = 0;                     // Hydraulic load dependent RPM drop

volatile int16_t masterVolume = 100;                     // Master volume percentage
volatile uint8_t dacOffset = 0;  // 128, but needs to be ramped up slowly to prevent popping noise, if switched on

// Throttle
int16_t currentThrottle = 0;                             // 0 - 500 (Throttle trigger input)
int16_t currentThrottleFaded = 0;                        // faded throttle for volume calculations etc.

// Engine
const int16_t maxRpm = 500;                              // always 500
const int16_t minRpm = 0;                                // always 0
int32_t currentRpm = 0;                                  // 0 - 500 (signed required!)
volatile uint8_t engineState = 0;                        // Engine state
enum EngineState                                         // Engine state enum
{
  OFF,
  STARTING,
  RUNNING,
  STOPPING,
  PARKING_BRAKE
};
int16_t engineLoad = 0;                                  // 0 - 500
volatile uint16_t engineSampleRate = 0;                  // Engine sample rate
int32_t speedLimit = maxRpm;                             // The speed limit, depending on selected virtual gear

// Clutch
boolean clutchDisengaged = true;                         // Active while clutch is disengaged

// Transmission
uint8_t selectedGear = 1;                                // The currently used gear of our shifting gearbox
uint8_t selectedAutomaticGear = 1;                       // The currently used gear of our automatic gearbox
boolean gearUpShiftingInProgress;                        // Active while shifting upwards
boolean gearDownShiftingInProgress;                      // Active while shifting downwards
boolean gearUpShiftingPulse;                             // Active, if shifting upwards begins
boolean gearDownShiftingPulse;                           // Active, if shifting downwards begins
volatile boolean neutralGear = false;                    // Transmission in neutral

// ESC
volatile boolean escIsBraking = false;                   // ESC is in a braking state
volatile boolean escIsDriving = false;                   // ESC is in a driving state
volatile boolean escInReverse = false;                   // ESC is driving or braking backwards
int8_t driveState = 0;                                   // for ESC state machine
int16_t escPulseMax;                                     // ESC calibration variables
int16_t escPulseMin;
int16_t escPulseMaxNeutral;
int16_t escPulseMinNeutral;
uint16_t currentSpeed = 0;                               // 0 - 500 (current ESC power)

// Lights
int8_t lightsState = 0;                                  // for lights state machine
volatile boolean lightsOn = false;                       // Lights on
volatile boolean headLightsFlasherOn = false;            // Headlights flasher impulse (Lichthupe)
volatile boolean headLightsHighBeamOn = false;           // Headlights high beam (Fernlicht)
volatile boolean blueLightTrigger = false;               // Bluelight on (Blaulicht)
boolean indicatorLon = false;                            // Left indicator (Blinker links)
boolean indicatorRon = false;                            // Right indicator (Blinker rechts)
boolean fogLightOn = false;                              // Fog light is on
boolean cannonFlash = false;                             // Flashing cannon fire

// Our main tasks
TaskHandle_t Task1;

// Loop time (for debug)
uint16_t loopTime;

// Sampling intervals for interrupt timer (adjusted according to your sound file sampling rate)
uint32_t maxSampleInterval = 4000000 / sampleRate;
uint32_t minSampleInterval = 4000000 / sampleRate * 100 / MAX_RPM_PERCENTAGE;

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
// INTERRUPT FOR VARIABLE SPEED PLAYBACK (Engine sound, turbo sound)
// =======================================================================================================
//

void IRAM_ATTR variablePlaybackTimer() {

  static uint32_t attenuatorMillis = 0;
  static uint32_t curEngineSample = 0;              // Index of currently loaded engine sample
  static uint32_t curRevSample = 0;                 // Index of currently loaded engine rev sample
  static uint32_t curTurboSample = 0;               // Index of currently loaded turbo sample
  static uint32_t curFanSample = 0;                 // Index of currently loaded fan sample
  static uint32_t curChargerSample = 0;             // Index of currently loaded charger sample
  static uint32_t curStartSample = 0;               // Index of currently loaded start sample
  static uint32_t curJakeBrakeSample = 0;           // Index of currently loaded jake brake sample
  static uint32_t curHydraulicPumpSample = 0;       // Index of currently loaded hydraulic pump sample
  static uint32_t lastDieselKnockSample = 0;        // Index of last Diesel knock sample
  static uint16_t attenuator = 0;                   // Used for volume adjustment during engine switch off
  static uint16_t speedPercentage = 0;              // slows the engine down during shutdown
  static int32_t a, a1, a2, a3, b, c, d, e = 0;     // Input signals for mixer: a = engine, b = additional sound, c = turbo sound, d = fan sound, e = supercharger sound
  static int32_t f = 0;                             // Input signals for mixer: f = hydraulic pump
  uint8_t a1Multi = 0;                              // Volume multipliers

  //portENTER_CRITICAL_ISR(&variableTimerMux);

  switch (engineState) {

    case OFF: // Engine off -----------------------------------------------------------------------
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      a = 0; // volume = zero
      if (engineOn) {
        engineState = STARTING;
        engineStart = true;
      }
      break;

    case STARTING: // Engine start --------------------------------------------------------------------
      variableTimerTicks = 4000000 / startSampleRate; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curStartSample < startSampleCount - 1) {
        a = (startSamples[curStartSample] * throttleDependentVolume / 100 * startVolumePercentage / 100);
        curStartSample ++;
      }
      else {
        curStartSample = 0;
        engineState = RUNNING;
        engineStart = false;
        engineRunning = true;
        airBrakeTrigger = true;
      }
      break;

    case RUNNING: // Engine running ------------------------------------------------------------------

      // Engine idle & revving sounds (mixed together according to engine rpm, new in v5.0)
      variableTimerTicks = engineSampleRate;  // our variable idle sampling rate!
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (!engineJakeBraking && !blowoffTrigger) {
        if (curEngineSample < sampleCount - 1) {
          a1 = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100); // Idle sound
          a3 = 0;
          curEngineSample ++;

          // Optional rev sound, recorded at medium rpm. Note, that it needs to represent the same number of ignition cycles as the
          // idle sound. For example 4 or 8 for a V8 engine. It also needs to have about the same length. In order to adjust the length
          // or "revSampleCount", change the "Rate" setting in Audacity until it is about the same.
#ifdef REV_SOUND
          a2 = (revSamples[curRevSample] * throttleDependentRevVolume / 100 * revVolumePercentage / 100); // Rev sound
          if (curRevSample < revSampleCount) curRevSample ++;
#endif

          // Trigger throttle dependent Diesel ignition "knock" sound (played in the fixed sample rate interrupt)
          if (curEngineSample - lastDieselKnockSample > (sampleCount / dieselKnockInterval)) {
            dieselKnockTrigger = true;
            dieselKnockTriggerFirst = false;
            lastDieselKnockSample = curEngineSample;
          }
        }
        else {
          curEngineSample = 0;
          if (jakeBrakeRequest) engineJakeBraking = true;
#ifdef REV_SOUND
          curRevSample = 0;
#endif
          lastDieselKnockSample = 0;
          dieselKnockTrigger = true;
          dieselKnockTriggerFirst = true;
        }
        curJakeBrakeSample = 0;
      }
      else { // Jake brake sound ----
#ifdef JAKE_BRAKE_SOUND
        a3 = (jakeBrakeSamples[curJakeBrakeSample] * rpmDependentJakeBrakeVolume / 100 * jakeBrakeVolumePercentage / 100); // Jake brake sound
        a2 = 0;
        a1 = 0;
        if (curJakeBrakeSample < jakeBrakeSampleCount - 1) curJakeBrakeSample ++;
        else {
          curJakeBrakeSample = 0;
          if (!jakeBrakeRequest) engineJakeBraking = false;
        }

        curEngineSample = 0;
        curRevSample = 0;
#endif
      }

      // Engine sound mixer ----
#ifdef REV_SOUND
      // Mixing the idle and rev sounds together, according to engine rpm
      // Below the "revSwitchPoint" target, the idle volume precentage is 90%, then falling to 0% @ max. rpm.
      // The total of idle and rev volume percentage is always 100%

      if (currentRpm > revSwitchPoint) a1Multi = map(currentRpm, idleEndPoint, revSwitchPoint, 0, idleVolumeProportionPercentage);
      else a1Multi = idleVolumeProportionPercentage; // 90 - 100% proportion
      if (currentRpm > idleEndPoint) a1Multi = 0;

      a1 = a1 * a1Multi / 100; // Idle volume
      a2 = a2 * (100 - a1Multi) / 100; // Rev volume

      a = a1 + a2 + a3; // Idle and rev sounds mixed together
#else
      a = a1 + a3; // Idle sound only
#endif

      // Turbo sound ----------------------------------
      if (curTurboSample < turboSampleCount - 1) {
        c = (turboSamples[curTurboSample] * throttleDependentTurboVolume / 100 * turboVolumePercentage / 100);
        curTurboSample ++;
      }
      else {
        curTurboSample = 0;
      }

      // Fan sound -----------------------------------
      if (curFanSample < fanSampleCount - 1) {
        d = (fanSamples[curFanSample] * throttleDependentFanVolume / 100 * fanVolumePercentage / 100);
        curFanSample ++;
      }
      else {
        curFanSample = 0;
      }
#if defined GEARBOX_WHINING
      if (neutralGear) d = 0; // used for gearbox whining simulation, so not active in gearbox neutral
#endif

      // Supercharger sound --------------------------
      if (curChargerSample < chargerSampleCount - 1) {
        e = (chargerSamples[curChargerSample] * throttleDependentChargerVolume / 100 * chargerVolumePercentage / 100);
        curChargerSample ++;
      }
      else {
        curChargerSample = 0;
      }

      // Hydraulic pump sound -----------------------
#if defined EXCAVATOR_MODE
      if (curHydraulicPumpSample < hydraulicPumpSampleCount - 1) {
        f = (hydraulicPumpSamples[curHydraulicPumpSample] * hydraulicPumpVolumePercentage / 100 * hydraulicPumpVolume / 100);
        curHydraulicPumpSample ++;
      }
      else {
        curHydraulicPumpSample = 0;
      }
#endif

      if (!engineOn) {
        speedPercentage = 100;
        attenuator = 1;
        engineState = STOPPING;
        engineStop = true;
        engineRunning = false;
      }
      break;

    case STOPPING: // Engine stop --------------------------------------------------------------------
      variableTimerTicks = 4000000 / sampleRate * speedPercentage / 100; // our fixed sampling rate
      timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

      if (curEngineSample < sampleCount - 1) {
        a = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100 / attenuator);
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
        a = 0;
        speedPercentage = 100;
        parkingBrakeTrigger = true;
        engineState = PARKING_BRAKE;
        engineStop = false;
      }
      break;

    case PARKING_BRAKE: // parking brake bleeding air sound after engine is off ----------------------------

      if (!parkingBrakeTrigger) {
        engineState = OFF;
      }
      break;

  } // end of switch case

  // DAC output (groups a, b, c mixed together) ************************************************************************

  dacWrite(DAC1, constrain(((a * 8 / 10) + (b / 2) + (c / 5) + (d / 5) + (e / 5)) + f * masterVolume / 100 + dacOffset, 0, 255)); // Mix signals, add 128 offset, write result to DAC
  //dacWrite(DAC1, constrain(a * masterVolume / 100 + dacOffset, 0, 255));
  //dacWrite(DAC1, constrain(a + 128, 0, 255));

  //portEXIT_CRITICAL_ISR(&variableTimerMux);
}

//
// =======================================================================================================
// INTERRUPT FOR FIXED SPEED PLAYBACK (Horn etc., played in parallel with engine sound)
// =======================================================================================================
//

void IRAM_ATTR fixedPlaybackTimer() {

  static uint32_t curHornSample = 0;                           // Index of currently loaded horn sample
  static uint32_t curSirenSample = 0;                          // Index of currently loaded siren sample
  static uint32_t curSound1Sample = 0;                         // Index of currently loaded sound1 sample
  static uint32_t curReversingSample = 0;                      // Index of currently loaded reversing beep sample
  static uint32_t curIndicatorSample = 0;                      // Index of currently loaded indicator tick sample
  static uint32_t curWastegateSample = 0;                      // Index of currently loaded wastegate sample
  static uint32_t curBrakeSample = 0;                          // Index of currently loaded brake sound sample
  static uint32_t curParkingBrakeSample = 0;                   // Index of currently loaded brake sound sample
  static uint32_t curShiftingSample = 0;                       // Index of currently loaded shifting sample
  static uint32_t curDieselKnockSample = 0;                    // Index of currently loaded Diesel knock sample
  static uint32_t curCouplingSample = 0;                       // Index of currently loaded trailer coupling sample
  static uint32_t curUncouplingSample = 0;                     // Index of currently loaded trailer uncoupling sample
  static uint32_t curHydraulicFlowSample = 0;                  // Index of currently loaded hydraulic flow sample
  static uint32_t curTrackRattleSample = 0;                    // Index of currently loaded track rattle sample
  static uint32_t curBucketRattleSample = 0;                   // Index of currently loaded bucket rattle sample
  static int32_t a, a1, a2 = 0;                                // Input signals "a" for mixer
  static int32_t b, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 = 0;// Input signals "b" for mixer
  static int32_t c, c1, c2, c3 = 0;                            // Input signals "c" for mixer
  static boolean knockSilent = 0;                              // This knock will be more silent
  static boolean knockMedium = 0;                              // This knock will be medium
  static uint8_t curKnockCylinder = 0;                         // Index of currently ignited zylinder

  //portENTER_CRITICAL_ISR(&fixedTimerMux);

  // Group "a" (horn & siren) ******************************************************************

  if (hornTrigger || hornLatch) {
    fixedTimerTicks = 4000000 / hornSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curHornSample < hornSampleCount - 1) {
      a1 =  (hornSamples[curHornSample] * hornVolumePercentage / 100);
      curHornSample ++;
#ifdef HORN_LOOP // Optional "endless loop" (points to be defined manually in horn file)
      if (hornTrigger && curHornSample == hornLoopEnd) curHornSample = hornLoopBegin; // Loop, if trigger still present
#endif
    }
    else { // End of sample
      curHornSample = 0;
      a1 = 0;
      hornLatch = false;
    }
  }

  if (sirenTrigger || sirenLatch) {
    fixedTimerTicks = 4000000 / sirenSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curSirenSample < sirenSampleCount - 1) {
      a2 = (sirenSamples[curSirenSample] * sirenVolumePercentage / 100);
      curSirenSample ++;
#ifdef SIREN_LOOP // Optional "endless loop" (points to be defined manually in siren file)
      if (sirenTrigger && curSirenSample == sirenLoopEnd) curSirenSample = sirenLoopBegin; // Loop, if trigger still present
#endif
    }
    else { // End of sample
      curSirenSample = 0;
      a2 = 0;
      sirenLatch = false;
    }
  }
  if (curSirenSample > 10 && curSirenSample < 500) cannonFlash = true; // Tank cannon flash triggering in TRACKED_MODE
  else cannonFlash = false;

  // Group "b" (other sounds) **********************************************************************

  // Sound 1 "b0" ----
  if (sound1trigger) {
    fixedTimerTicks = 4000000 / sound1SampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curSound1Sample < sound1SampleCount - 1) {
      b0 = (sound1Samples[curSound1Sample] * sound1VolumePercentage / 100);
      curSound1Sample ++;
    }
    else {
      sound1trigger = false;
    }
  }
  else {
    curSound1Sample = 0; // ensure, next sound will start @ first sample
    b0 = 0;
  }

  // Reversing beep sound "b1" ----
  if (engineRunning && escInReverse) {
    fixedTimerTicks = 4000000 / reversingSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curReversingSample < reversingSampleCount - 1) {
      b1 = (reversingSamples[curReversingSample] * reversingVolumePercentage / 100);
      curReversingSample ++;
    }
    else {
      curReversingSample = 0;
    }
  }
  else {
    curReversingSample = 0; // ensure, next sound will start @ first sample
    b1 = 0;
  }

  // Indicator tick sound "b2" ----------------------------------------------------------------------
  if (indicatorSoundOn) {
    fixedTimerTicks = 4000000 / indicatorSampleRate; // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curIndicatorSample < indicatorSampleCount - 1) {
      b2 = (indicatorSamples[curIndicatorSample] * indicatorVolumePercentage / 100);
      curIndicatorSample ++;
    }
    else {
      indicatorSoundOn = false;
    }
  }
  else {
    curIndicatorSample = 0; // ensure, next sound will start @ first sample
    b2 = 0;
  }

  // Wastegate (blowoff) sound, triggered after rapid throttle drop -----------------------------------
  if (wastegateTrigger) {
    if (curWastegateSample < wastegateSampleCount - 1) {
      b3 = (wastegateSamples[curWastegateSample] * rpmDependentWastegateVolume / 100 * wastegateVolumePercentage / 100);
      curWastegateSample ++;
    }
    else {
      wastegateTrigger = false;
    }
  }
  else {
    b3 = 0;
    curWastegateSample = 0; // ensure, next sound will start @ first sample
  }

  // Air brake release sound, triggered after stop -----------------------------------------------
  if (airBrakeTrigger) {
    if (curBrakeSample < brakeSampleCount - 1) {
      b4 = (brakeSamples[curBrakeSample] * brakeVolumePercentage / 100);
      curBrakeSample ++;
    }
    else {
      airBrakeTrigger = false;
    }
  }
  else {
    b4 = 0;
    curBrakeSample = 0; // ensure, next sound will start @ first sample
  }

  // Air parking brake attaching sound, triggered after engine off --------------------------------
  if (parkingBrakeTrigger) {
    if (curParkingBrakeSample < parkingBrakeSampleCount - 1) {
      b5 = (parkingBrakeSamples[curParkingBrakeSample] * parkingBrakeVolumePercentage / 100);
      curParkingBrakeSample ++;
    }
    else {
      parkingBrakeTrigger = false;
    }
  }
  else {
    b5 = 0;
    curParkingBrakeSample = 0; // ensure, next sound will start @ first sample
  }

  // Pneumatic gear shifting sound, triggered while shifting the TAMIYA 3 speed transmission ------
  if (shiftingTrigger && engineRunning  && !automatic && !doubleClutch) {
    if (curShiftingSample < shiftingSampleCount - 1) {
      b6 = (shiftingSamples[curShiftingSample] * shiftingVolumePercentage / 100);
      curShiftingSample ++;
    }
    else {
      shiftingTrigger = false;
    }
  }
  else {
    b6 = 0;
    curShiftingSample = 0; // ensure, next sound will start @ first sample
  }

  // Diesel ignition "knock" is played in fixed sample rate section, because we don't want changing pitch! ------
  if (dieselKnockTriggerFirst) {
    dieselKnockTriggerFirst = false;
    curKnockCylinder = 0;
  }

  if (dieselKnockTrigger) {
    dieselKnockTrigger = false;
    curKnockCylinder ++; // Count ignition sequence
    curDieselKnockSample = 0;
  }

#ifdef V8 // (former ADAPTIVE_KNOCK_VOLUME, rename it in your config file!)
  // Ford or Scania V8 ignition sequence: 1 - 5 - 4 - 2* - 6 - 3 - 7 - 8* (* = louder knock pulses, because 2nd exhaust in same manifold after 90°)
  if (curKnockCylinder == 4 || curKnockCylinder == 8) knockSilent = false;
  else knockSilent = true;
#endif

#ifdef V8_MEDIUM // (former ADAPTIVE_KNOCK_VOLUME, rename it in your config file!)
  // This is EXPERIMENTAL!! TODO
  if (curKnockCylinder == 5 || curKnockCylinder == 1) knockMedium = false;
  else knockMedium = true;
#endif

#ifdef V8_468 // (Chevy 468, containing 16 ignition pulses)
  // 1th, 5th, 9th and 13th are the loudest
  // Ignition sequence: 1 - 8 - 4* - 3 - 6 - 5 - 7* - 2
  if (curKnockCylinder == 1 || curKnockCylinder == 5 || curKnockCylinder == 9 || curKnockCylinder == 13) knockSilent = false;
  else knockSilent = true;
#endif

#ifdef V2
  // V2 engine: 1st and 2nd knock pulses (of 4) will be louder
  if (curKnockCylinder == 1 || curKnockCylinder == 2) knockSilent = false;
  else knockSilent = true;
#endif

#ifdef R6
  // R6 inline 6 engine: 6th knock pulse (of 6) will be louder
  if (curKnockCylinder == 6) knockSilent = false;
  else knockSilent = true;
#endif

  if (curDieselKnockSample < knockSampleCount) {
#if defined RPM_DEPENDENT_KNOCK // knock volume also depending on engine rpm
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100 * rpmDependentKnockVolume / 100);
#elif defined EXCAVATOR_MODE // knock volume also depending on hydraulic load
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100 * hydraulicDependentKnockVolume / 100);
#else // Just depending on throttle
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100);
#endif
    curDieselKnockSample ++;
    if (knockSilent && ! knockMedium) b7 = b7 * dieselKnockAdaptiveVolumePercentage / 100; // changing knock volume according to engine type and cylinder!
    if (knockMedium) b7 = b7 * dieselKnockAdaptiveVolumePercentage / 75;
  }

#if not defined EXCAVATOR_MODE
  // Trailer coupling sound, triggered by switch -----------------------------------------------
#ifdef COUPLING_SOUND
  if (couplingTrigger) {
    if (curCouplingSample < couplingSampleCount - 1) {
      b8 = (couplingSamples[curCouplingSample] * couplingVolumePercentage / 100);
      curCouplingSample ++;
    }
    else {
      couplingTrigger = false;
    }
  }
  else {
    b8 = 0;
    curCouplingSample = 0; // ensure, next sound will start @ first sample
  }

  // Trailer uncoupling sound, triggered by switch -----------------------------------------------
  if (uncouplingTrigger) {
    if (curUncouplingSample < uncouplingSampleCount - 1) {
      b9 = (uncouplingSamples[curUncouplingSample] * couplingVolumePercentage / 100);
      curUncouplingSample ++;
    }
    else {
      uncouplingTrigger = false;
    }
  }
  else {
    b9 = 0;
    curUncouplingSample = 0; // ensure, next sound will start @ first sample
  }
#endif
#endif

  // Group "c" (excavator sounds) **********************************************************************

#if defined EXCAVATOR_MODE

  // Hydraulic fluid flow sound -----------------------
  if (curHydraulicFlowSample < hydraulicFlowSampleCount - 1) {
    c1 = (hydraulicFlowSamples[curHydraulicFlowSample] * hydraulicFlowVolumePercentage / 100 * hydraulicFlowVolume / 100);
    curHydraulicFlowSample ++;
  }
  else {
    curHydraulicFlowSample = 0;
  }

  // Track rattle sound -----------------------
  if (curTrackRattleSample < trackRattleSampleCount - 1) {
    c2 = (trackRattleSamples[curTrackRattleSample] * trackRattleVolumePercentage / 100 * trackRattleVolume / 100);
    curTrackRattleSample ++;
  }
  else {
    curTrackRattleSample = 0;
  }

  // Track rattle sound -----------------------
  if (bucketRattleTrigger) {
    if (curBucketRattleSample < bucketRattleSampleCount - 1) {
      c3 = (bucketRattleSamples[curBucketRattleSample] * bucketRattleVolumePercentage / 100);
      curBucketRattleSample ++;
    }
    else {
      bucketRattleTrigger = false;
    }
  }
  else {
    c3 = 0;
    curBucketRattleSample = 0; // ensure, next sound will start @ first sample
  }
#endif

  // Mixing sounds together ----
  a = a1 + a2; // Horn & siren
  b = b0 * 5 + b1 + b2 / 2 + b3 + b4 + b5 + b6 + b7 + b8 + b9; // Other sounds
  c = c1 + c2 + c3; // Excavator sounds

  // DAC output (groups a + b mixed together) ****************************************************************************

  dacWrite(DAC2, constrain(((a * 8 / 10) + (b * 2 / 10) + c) * masterVolume / 100 + dacOffset, 0, 255)); // Mix signals, add 128 offset, write result to DAC
  //dacWrite(DAC2, constrain( c * masterVolume / 100 + dacOffset, 0, 255)); // Mix signals, add 128 offset, write result to DAC
  //dacWrite(DAC2, 0);

  //portEXIT_CRITICAL_ISR(&fixedTimerMux);
}

//
// =======================================================================================================
// PWM SIGNAL READ INTERRUPT
// =======================================================================================================
//

// Reference https://esp-idf.readthedocs.io/en/v1.0/api/rmt.html
static void IRAM_ATTR rmt_isr_handler(void* arg) {

  uint32_t intr_st = RMT.int_st.val;

  uint8_t i;
  for (i = 0; i < PWM_CHANNELS_NUM; i++) {
    uint8_t channel = PWM_CHANNELS[i];
    uint32_t channel_mask = BIT(channel * 3 + 1);

    if (!(intr_st & channel_mask)) continue;

    RMT.conf_ch[channel].conf1.rx_en = 0;
    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
    volatile rmt_item32_t* item = RMTMEM.chan[channel].data32;
    if (item) {
      pulseWidthRaw[i + 1] = item->duration0;
    }

    RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_RX;
    RMT.conf_ch[channel].conf1.rx_en = 1;

    //clear RMT interrupt status.
    RMT.int_clr.val = channel_mask;
  }
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

  if (timediff > 2500) {  // Synch gap detected:
    ppmInp[NUM_OF_PPM_CHL] = ppmInp[NUM_OF_PPM_CHL] + timediff; // add time
    counter = 0;
    if (average == NUM_OF_PPM_AVG) {
      for (int i = 0; i < NUM_OF_PPM_CHL + 1; i++) {
        ppmBuf[i] = ppmInp[i] / average;
        ppmInp[i] = 0;
      }
      average = 0;
      ready = true;
    }
    average++;
  }
  else {
    if (counter < NUM_OF_PPM_CHL) {
      ppmInp[counter] = ppmInp[counter] + timediff;
      counter++;
    }
  }
}

//
// =======================================================================================================
// TRAILER PRESENCE SWITCH INTERRUPT
// =======================================================================================================
//
#if not defined THIRD_BRAKLELIGHT
void trailerPresenceSwitchInterrupt() {
  couplerSwitchInteruptLatch = true;
}
#endif
//
// =======================================================================================================
// mcpwm SETUP (1x during startup)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void setupMcpwm() {
  // 1. set our servo output pins
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, STEERING_PIN);    //Set steering as PWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, SHIFTING_PIN);    //Set shifting as PWM0B
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, COUPLER_PIN);    //Set coupling as PWM1A
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, WINCH_PIN);    //Set winch as PWM1B

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
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

  // Watchdog timers need to be disabled, if task 1 is running without delay(1)
  disableCore0WDT();
  disableCore1WDT();

  // Set pin modes
  for (uint8_t i = 0; i < PWM_CHANNELS_NUM; i++) {
    pinMode(PWM_PINS[i], INPUT_PULLDOWN);
  }

  pinMode(PPM_PIN, INPUT_PULLDOWN);

  pinMode(COMMAND_RX, INPUT_PULLDOWN);


#if not defined THIRD_BRAKLELIGHT // If a third brakelight is not defined, pin 32 for the trailer presence switch
  pinMode(COUPLER_SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COUPLER_SWITCH_PIN), trailerPresenceSwitchInterrupt, CHANGE);
#endif

  // LED & shaker motor setup (note, that we only have timers from 0 - 15)
  headLight.begin(HEADLIGHT_PIN, 1, 20000); // Timer 1, 20kHz
  tailLight.begin(TAILLIGHT_PIN, 2, 20000); // Timer 2, 20kHz
  indicatorL.begin(INDICATOR_LEFT_PIN, 3, 20000); // Timer 3, 20kHz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 4, 20000); // Timer 4, 20kHz
  fogLight.begin(FOGLIGHT_PIN, 5, 20000); // Timer 5, 20kHz
  reversingLight.begin(REVERSING_LIGHT_PIN, 6, 20000); // Timer 6, 20kHz
  roofLight.begin(ROOFLIGHT_PIN, 7, 20000); // Timer 7, 20kHz

#if not defined SPI_DASHBOARD
  sideLight.begin(SIDELIGHT_PIN, 8, 20000); // Timer 8, 20kHz
  beaconLight1.begin(BEACON_LIGHT1_PIN, 9, 20000); // Timer 9, 20kHz
  beaconLight2.begin(BEACON_LIGHT2_PIN, 10, 20000); // Timer 10, 20kHz
#endif

#if defined THIRD_BRAKLELIGHT
  brakeLight.begin(BRAKELIGHT_PIN, 11, 20000); // Timer 11, 20kHz
#endif
  cabLight.begin(CABLIGHT_PIN, 12, 20000); // Timer 12, 20kHz

#if not defined SPI_DASHBOARD
  shakerMotor.begin(SHAKER_MOTOR_PIN, 13, 20000); // Timer 13, 20kHz
#endif

  escOut.begin(ESC_OUT_PIN, 15, 50, 16); // Timer 15, 50Hz, 16bit <-- ESC running @ 50Hz

#if defined SPI_DASHBOARD
  // Dashboard setup
  dashboard.init();
#endif

  // Neopixel setup
  FastLED.addLeds<NEOPIXEL, RGB_LEDS_PIN>(rgbLEDs, RGB_LEDS_COUNT);

  // Serial setup
  Serial.begin(115200); // USB serial (for DEBUG)

  // Communication setup --------------------------------------------
  indicatorL.on();
  indicatorR.on();

#if defined SBUS_COMMUNICATION // SBUS ----
  if (MAX_RPM_PERCENTAGE > maxSbusRpmPercentage) MAX_RPM_PERCENTAGE = maxSbusRpmPercentage; // Limit RPM range
  sBus.begin(COMMAND_RX, COMMAND_TX, sbusInverted); // begin SBUS communication with compatible receivers
  setupMcpwm(); // mcpwm servo output setup

#elif defined IBUS_COMMUNICATION // IBUS ----
  if (MAX_RPM_PERCENTAGE > maxIbusRpmPercentage) MAX_RPM_PERCENTAGE = maxIbusRpmPercentage; // Limit RPM range
  iBus.begin(Serial2, IBUSBM_NOTIMER, COMMAND_RX, COMMAND_TX); // begin IBUS communication with compatible receivers
  setupMcpwm(); // mcpwm servo output setup

#elif defined PPM_COMMUNICATION // PPM ----
  if (MAX_RPM_PERCENTAGE > maxPpmRpmPercentage) MAX_RPM_PERCENTAGE = maxPpmRpmPercentage; // Limit RPM range
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), readPpm, RISING); // begin PPM communication with compatible receivers
  setupMcpwm(); // mcpwm servo output setup

#elif defined SUMD_COMMUNICATION // Graupner SUMD ----
  // SUMD communication
  if (MAX_RPM_PERCENTAGE > maxSumdRpmPercentage) MAX_RPM_PERCENTAGE = maxSumdRpmPercentage; // Limit RPM range
  sumd.begin(COMMAND_RX); // begin SUMD communication with compatible receivers
  setupMcpwm(); // mcpwm servo output setup

#else
  // PWM ----
  if (MAX_RPM_PERCENTAGE > maxPwmRpmPercentage) MAX_RPM_PERCENTAGE = maxPwmRpmPercentage; // Limit RPM range
  // New: PWM read setup, using rmt. Thanks to croky-b
  uint8_t i;
  rmt_config_t rmt_channels[PWM_CHANNELS_NUM] = {};

  for (i = 0; i < PWM_CHANNELS_NUM; i++) {
    rmt_channels[i].channel = (rmt_channel_t) PWM_CHANNELS[i];
    rmt_channels[i].gpio_num = (gpio_num_t) PWM_PINS[i];
    rmt_channels[i].clk_div = RMT_RX_CLK_DIV;
    rmt_channels[i].mem_block_num = 1;
    rmt_channels[i].rmt_mode = RMT_MODE_RX;
    rmt_channels[i].rx_config.filter_en = true;
    rmt_channels[i].rx_config.filter_ticks_thresh = 100; // Pulses shorter than this will be filtered out
    rmt_channels[i].rx_config.idle_threshold = RMT_RX_MAX_US * RMT_TICK_PER_US;

    rmt_config(&rmt_channels[i]);
    rmt_set_rx_intr_en(rmt_channels[i].channel, true);
    rmt_rx_start(rmt_channels[i].channel, 1);
  }

  rmt_isr_register(rmt_isr_handler, NULL, 0, NULL); // This is our interrupt

#endif // -----------------------------------------------------------

  // Refresh sample intervals (important, because MAX_RPM_PERCENTAGE was probably changed above)
  maxSampleInterval = 4000000 / sampleRate;
  minSampleInterval = 4000000 / sampleRate * 100 / MAX_RPM_PERCENTAGE;

  // Time
  timelast = micros();
  timelastloop = timelast;

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
#if defined SBUS_COMMUNICATION
  sbusInit = false;
  Serial.println("Initializing SBUS, check transmitter & receiver (sbusInverted true / false OK?)...");
  while (!sbusInit) {
    readSbusCommands(); // SBUS communication (pin 36)
  }
  Serial.println("SBUS initialization done!");

#elif defined IBUS_COMMUNICATION
  ibusInit = false;
  Serial.println("Initializing IBUS, check transmitter & receiver...");
  while (!ibusInit) {
    readIbusCommands(); // IBUS communication (pin 36)
  }
  Serial.println("IBUS initialization done!");

#elif defined SUMD_COMMUNICATION
  SUMD_init = false;
  Serial.println("Initializing SUMD, check transmitter & receiver...");
  while (!SUMD_init)
  {
    readSumdCommands();
  }
  Serial.println("SUMD initialization done!");

#elif defined PPM_COMMUNICATION
  readPpmCommands();
#else
  // measure PWM RC signals mark space ratio
  readPwmSignals();
#endif

  // Calculate RC input signal ranges for all channels
  for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++) {
    pulseZero[i] = 1500; // Always 1500. This is the center position. Auto centering is now done in "processRawChannels()"

    // Input signals
    pulseMaxNeutral[i] = pulseZero[i] + pulseNeutral;
    pulseMinNeutral[i] = pulseZero[i] - pulseNeutral;
    pulseMax[i] = pulseZero[i] + pulseSpan;
    pulseMin[i] = pulseZero[i] - pulseSpan;
    pulseMaxLimit[i] = pulseZero[i] + pulseLimit;
    pulseMinLimit[i] = pulseZero[i] - pulseLimit;
  }

  // ESC output calibration
  escPulseMaxNeutral = pulseZero[3] + escTakeoffPunch; //Additional takeoff punch around zero
  escPulseMinNeutral = pulseZero[3] - escTakeoffPunch;

  escPulseMax = pulseZero[3] + escPulseSpan;
  escPulseMin = pulseZero[3] - escPulseSpan + escReversePlus; //Additional power for ESC with slow reverse
}

//
// =======================================================================================================
// DAC OFFSET FADER
// =======================================================================================================
//

static unsigned long dacOffsetMicros;
boolean dacInit;

void dacOffsetFade() {
  if (!dacInit) {
    if (micros() - dacOffsetMicros > 100) { // Every 0.1ms
      dacOffsetMicros = micros();
      dacOffset ++; // fade DAC offset slowly to prevent it from popping, if ESP32 powered up after amplifier
      if (dacOffset == 128) dacInit = true;
    }
  }
}

//
// =======================================================================================================
// READ PWM RC SIGNALS (plug in your channels according to order in "channelSetup.h")
// =======================================================================================================
//

void readPwmSignals() {

  // measure RC signal pulsewidth:
  // nothing is done here, the PWM signals are now read, using the
  // "static void IRAM_ATTR rmt_isr_handler(void* arg)" interrupt function

  // NOTE: There is no channel mapping in this mode! Just plug in the wires in the order as defined in "2_adjustmentsRemote.h"
  // for example: sound controller channel 2 (GEARBOX) connects to receiver channel 6

  // Normalize, auto zero and reverse channels
  processRawChannels();

  // Failsafe for RC signals
  failSafe = (pulseWidthRaw[3] < 500 || pulseWidthRaw[3] > 2500);
  failsafeRcSignals();
}

//
// =======================================================================================================
// READ PPM MULTI CHANNEL COMMMANDS (change the channel order in "2_adjustmentsRemote.h", if needed)
// =======================================================================================================
//

void readPpmCommands() {

  // NOTE: 8 channels is the maximum of this protocol!

  pulseWidthRaw[1] = ppmBuf[STEERING - 1]; // CH1 steering
  pulseWidthRaw[2] = ppmBuf[GEARBOX - 1]; // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = ppmBuf[THROTTLE - 1]; // CH3 throttle & brake
  pulseWidthRaw[4] = ppmBuf[HORN - 1]; // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = ppmBuf[FUNCTION_R - 1]; // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = ppmBuf[FUNCTION_L - 1]; // CH6 indicators, hazards
  pulseWidthRaw[7] = ppmBuf[POT2 - 1]; // CH7 pot 2
  pulseWidthRaw[8] = ppmBuf[MODE1 - 1]; // CH8 mode 1 switch

  // Normalize, auto zero and reverse channels
  processRawChannels();

  // Failsafe for RC signals
  failSafe = (pulseWidthRaw[3] < 500 || pulseWidthRaw[3] > 2500);
  failsafeRcSignals();
}

//
// =======================================================================================================
// READ SBUS SIGNALS (change the channel order in "2_adjustmentsRemote.h", if needed)
// =======================================================================================================
//

void readSbusCommands() {
  // Signals are coming in via SBUS protocol

  static unsigned long lastSbusFailsafe;

  // look for a good SBUS packet from the receiver
  if (sBus.read(&SBUSchannels[0], &SBUSfailSafe, &SBUSlostFrame)) {
    sbusInit = true;
    lastSbusFailsafe = millis();
  }

  //SBUSchannels[NONE - 1] = 991; // The NONE channel needs to be on neutral (991 = 1500ms) TODO

  // Proportional channels (in Microseconds)
  pulseWidthRaw[1] = map(SBUSchannels[STEERING - 1], 172, 1811, 1000, 2000); // CH1 steering
  pulseWidthRaw[2] = map(SBUSchannels[GEARBOX - 1], 172, 1811, 1000, 2000); // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = map(SBUSchannels[THROTTLE - 1], 172, 1811, 1000, 2000); // CH3 throttle & brake
  pulseWidthRaw[4] = map(SBUSchannels[HORN - 1], 172, 1811, 1000, 2000); // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = map(SBUSchannels[FUNCTION_R - 1], 172, 1811, 1000, 2000); // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = map(SBUSchannels[FUNCTION_L - 1], 172, 1811, 1000, 2000); // CH6 indicators, hazards
  pulseWidthRaw[7] = map(SBUSchannels[POT2 - 1], 172, 1811, 1000, 2000); // CH7 pot 2
  pulseWidthRaw[8] = map(SBUSchannels[MODE1 - 1], 172, 1811, 1000, 2000); // CH8 mode 1 switch
  pulseWidthRaw[9] = map(SBUSchannels[MODE2 - 1], 172, 1811, 1000, 2000); // CH9 mode 2 switch
  pulseWidthRaw[10] = map(SBUSchannels[MOMENTARY1 - 1], 172, 1811, 1000, 2000); // CH10
  pulseWidthRaw[11] = map(SBUSchannels[HAZARDS - 1], 172, 1811, 1000, 2000); // CH11
  pulseWidthRaw[12] = map(SBUSchannels[INDICATOR_LEFT - 1], 172, 1811, 1000, 2000); // CH12
  pulseWidthRaw[13] = map(SBUSchannels[INDICATOR_RIGHT - 1], 172, 1811, 1000, 2000); // CH13

  // Failsafe triggering (works, if SBUS wire is unplugged, but SBUSfailSafe signal from the receiver is untested!)
  if (millis() - lastSbusFailsafe > 50 && !SBUSfailSafe  && !SBUSlostFrame) {
    failSafe = true; // if timeout (signal loss)
    //Serial.println("SBUS failsafe");
  }
  else failSafe = false;

  if (sbusInit) { // TODO, experimental!
    // Normalize, auto zero and reverse channels
    processRawChannels();

    // Failsafe for RC signals
    failsafeRcSignals();
  }
}

//
// =======================================================================================================
// READ IBUS SIGNALS (change the channel order in "2_adjustmentsRemote.h", if needed).
// =======================================================================================================
//

// NOTE: "MAX_RPM_PERCENTAGE" > 350 will crash the ESP32, if used in iBus mode!
// Caution, this protocoll does NOT offer failsafe!! Bad contact on iBUS wire = crash!
// Better use SBUS instead, if available!

// IBUS Loop sub function ----
void loopIbus() {
  // Loop iBus (read signals)
  static unsigned long lastIbusRead;
  static uint16_t iBusReadCycles;
  if (millis() - lastIbusRead > 10) { // Every 10ms
    lastIbusRead = millis();
    iBus.loop();
    if (iBusReadCycles < 100) iBusReadCycles ++; else ibusInit = true; // We need to process the entire serial package, before we read the channels for the first time! 100 OK? TODO
  }
}

// Read IBUS signals ----
void readIbusCommands() {

  // Loop iBus (fill buffer)
  loopIbus();

  // NOTE: The channel mapping is in the order as defined in "channelsetup.h"
  // for example: sound controller channel 2 (GEARBOX) connects to receiver channel 6

  // Proportional channels (in Microseconds)
  pulseWidthRaw[1] = iBus.readChannel(STEERING - 1); // CH1 steering
  pulseWidthRaw[2] = iBus.readChannel(GEARBOX - 1); // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = iBus.readChannel(THROTTLE - 1); // CH3 throttle & brake
  pulseWidthRaw[4] = iBus.readChannel(HORN - 1); // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = iBus.readChannel(FUNCTION_R - 1); // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = iBus.readChannel(FUNCTION_L - 1); // CH6 indicators, hazards
  pulseWidthRaw[7] = iBus.readChannel(POT2 - 1); // CH7 pot 2
  pulseWidthRaw[8] = iBus.readChannel(MODE1 - 1); // CH8 mode 1 switch
  pulseWidthRaw[9] = iBus.readChannel(MODE2 - 1); // CH9 mode 2 switch
  pulseWidthRaw[10] = iBus.readChannel(MOMENTARY1 - 1); // CH10
  pulseWidthRaw[11] = iBus.readChannel(HAZARDS - 1); // CH11
  pulseWidthRaw[12] = iBus.readChannel(INDICATOR_LEFT - 1); // CH12
  pulseWidthRaw[13] = iBus.readChannel(INDICATOR_RIGHT - 1); // CH13

  if (ibusInit) {
    // Normalize, auto zero and reverse channels
    processRawChannels();
  }
}

//
// =======================================================================================================
// READ SUMD SIGNALS (change the channel order in "2_adjustmentsRemote.h", if needed).
// =======================================================================================================
//
void readSumdCommands() {
#if defined SUMD_COMMUNICATION
  // Signals are coming in via Graupner SUMD protocol

  // look for a good SUMD packet from the receiver
  if (sumd.read(SUMDchannels, &SUMD_failsafe, &SUMD_frame_lost) == 0)
  {
    SUMD_init = true;
  }

  // Proportional channels (in Microseconds)
  pulseWidthRaw[1] = map(SUMDchannels[STEERING - 1], 1100, 1900, 1000, 2000);    // CH1 steering
  pulseWidthRaw[2] = map(SUMDchannels[GEARBOX - 1], 1100, 1900, 1000, 2000);     // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = map(SUMDchannels[THROTTLE - 1], 1100, 1900, 1000, 2000);    // CH3 throttle & brake
  pulseWidthRaw[4] = map(SUMDchannels[HORN - 1], 1100, 1900, 1000, 2000);        // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = map(SUMDchannels[FUNCTION_R - 1], 1100, 1900, 1000, 2000);  // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = map(SUMDchannels[FUNCTION_L - 1], 1100, 1900, 1000, 2000);  // CH6 indicators, hazards
  pulseWidthRaw[7] = map(SUMDchannels[POT2 - 1], 1100, 1900, 1000, 2000);        // CH7 pot 2
  pulseWidthRaw[8] = map(SUMDchannels[MODE1 - 1], 1100, 1900, 1000, 2000);       // CH8 mode 1 switch
  pulseWidthRaw[9] = map(SUMDchannels[MODE2 - 1], 1100, 1900, 1000, 2000);       // CH9 mode 2 switch
  pulseWidthRaw[10] = map(SUMDchannels[MOMENTARY1 - 1], 1100, 1900, 1000, 2000); // CH10
  pulseWidthRaw[11] = map(SUMDchannels[HAZARDS - 1], 1100, 1900, 1000, 2000);    // CH11

  // Failsafe triggering
  if (SUMD_failsafe)
  {
    failSafe = true; // in most cases the rx buffer is not processed fast enough so old data is overwritten
  }
  else
    failSafe = false;

  if (SUMD_init)
  { // TODO, experimental!
    // Normalize, auto zero and reverse channels
    processRawChannels();

    // Failsafe for RC signals
    failsafeRcSignals();
  }
#endif
}

//
// =======================================================================================================
// PROZESS CHANNELS (Normalize, auto zero and reverse)
// =======================================================================================================
//

void processRawChannels() {

  static unsigned long lastOutOfRangeMillis;
  static int channel;

#ifdef TRACKED_MODE // If tracked mode: enable CH2 auto zero adjustment as well, if it is enabled for CH3
  if (channelAutoZero[3]) channelAutoZero[2] = true;
#endif

  if (millis() - lastOutOfRangeMillis > 500) {
    for (int i = 1; i < PULSE_ARRAY_SIZE; i++) { // For each channel:

      // Position valid for auto calibration? Must be between 1400 and 1600 microseconds
      if (channelAutoZero[i] && !autoZeroDone && (pulseWidthRaw[i] > 1600 || pulseWidthRaw[i] < 1400)) {
        channel = i;
        Serial.print("CH ");
        Serial.print(channel);
        Serial.println(" signal out of auto calibration range, check transmitter & receiver");
        channelZero();
        lastOutOfRangeMillis = millis();
        i--;
        return;
      }

      // Exponential throttle compensation ------------------
#ifdef EXPONENTIAL_THROTTLE
      if (i == 3) { // Throttle CH only
        pulseWidthRaw2[i] = reMap(curveExponentialThrottle, pulseWidthRaw[i]);
      }
      else {
        pulseWidthRaw2[i] = pulseWidthRaw[i];
      }
#else
      pulseWidthRaw2[i] = pulseWidthRaw[i];
#endif // --------------------------------------------------

      // Exponential steering compensation ------------------
#ifdef EXPONENTIAL_STEERING
      if (i == 1) { // Throttle CH only
        pulseWidthRaw2[i] = reMap(curveExponentialThrottle, pulseWidthRaw[i]);
      }
      else {
        pulseWidthRaw2[i] = pulseWidthRaw[i];
      }
#else
      pulseWidthRaw2[i] = pulseWidthRaw[i];
#endif // --------------------------------------------------

      // Take channel raw data, reverse them, if required and store them
      if (channelReversed[i]) pulseWidth[i] = map(pulseWidthRaw2[i], 0, 3000, 3000, 0); // Reversed
      else pulseWidth[i] = pulseWidthRaw2[i]; // Not reversed

      // Calculate zero offset (only within certain absolute range)
      if (channelAutoZero[i] && !autoZeroDone && pulseWidth[i] > pulseMinValid && pulseWidth[i] < pulseMaxValid) pulseOffset[i] = 1500 - pulseWidth[i];

      // Center channel, if out of range! TODO, experimental
      if (pulseWidth[i] > pulseMaxValid || pulseWidth[i] < pulseMinValid) pulseWidth[i] = pulseZero[i];

      // Limit channel, if out of range! TODO, experimental (required for RGT  MT-350 @ max. throttle dual rate)
      if (pulseWidth[i] > 2000) pulseWidth[i] = 2000;
      if (pulseWidth[i] < 1000) pulseWidth[i] = 1000;

      // Compensate pulsewidth with auto zero offset
      pulseWidth[i] += pulseOffset[i];
      if (!autoZeroDone) { // Print offsets, if switching on the controller
        Serial.print("Offset CH ");
        Serial.print(i);
        Serial.print(" [microseconds]: ");
        Serial.println(pulseOffset[i]);
      }

      // Set auto zero done flag
      if (i == PULSE_ARRAY_SIZE - 1) autoZeroDone = true;
    }
  }

  if (!autoZeroDone) { // Indicators are showing the number of channels, which are out of auto calibration range
    indicatorL.flash(140, 150, 500, channel);
    indicatorR.flash(140, 150, 500, channel);
  }

  // Print input signal debug infos
#ifdef CHANNEL_DEBUG // can slow down the playback loop!
  static unsigned long printChannelMillis;
  if (millis() - printChannelMillis > 1000 && autoZeroDone) { // Every 1000ms
    printChannelMillis = millis();

    Serial.println("Channels [microseconds]:");
    Serial.println(pulseWidth[1]);
    Serial.println(pulseWidth[2]);
    Serial.println(pulseWidth[3]);
    Serial.println(pulseWidth[4]);
    Serial.println(pulseWidth[5]);
    Serial.println(pulseWidth[6]);
    Serial.println(pulseWidth[7]);
    Serial.println(pulseWidth[8]);
    Serial.println(pulseWidth[9]);
    Serial.println(pulseWidth[10]);
    Serial.println(pulseWidth[11]);
    Serial.println(pulseWidth[12]);
    Serial.println(pulseWidth[13]);
    Serial.println("Throttle:");
    Serial.println(currentThrottle);
    Serial.println("States:");
    Serial.println(mode1);
    Serial.println(mode2);
    Serial.println(momentary1);
    Serial.println(hazard);
    Serial.println(left);
    Serial.println(right);
    Serial.println("Failsafe:");
    Serial.println(SBUSfailSafe);
    Serial.println(SBUSlostFrame);
    Serial.println(failSafe);
    Serial.println("Misc:");
    Serial.println(MAX_RPM_PERCENTAGE);
    Serial.println("");
  }
#endif
}

// Sub function for channel centering ----
void channelZero() {
  for (int i = 1; i < PULSE_ARRAY_SIZE; i++) {
    pulseWidth[i] = 1500;
  }
}

//
// =======================================================================================================
// RC SIGNAL FAILSAFE POSITIONS (if serial signal lost)
// =======================================================================================================
//

void failsafeRcSignals() {

  // Failsafe actions --------
  if (failSafe) pulseWidth[3] = pulseZero[3]; // Throttle to zero position!
}

//
// =======================================================================================================
// ROTATING BEACON CONTROL (BUS communication mode only)
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

  // Steering CH1 **********************
  uint16_t steeringServoMicros;
  static uint16_t steeringServoMicrosDelayed = CH1C;
  static unsigned long steeringDelayMicros;
  if (micros() - steeringDelayMicros > STEERING_RAMP_TIME) { // Adjustable steering max. ramp speed
    steeringDelayMicros = micros();
    if (pulseWidth[1] < 1500) steeringServoMicros = map(pulseWidth[1], 1000, 1500, CH1L, CH1C);
    else if (pulseWidth[1] > 1500) steeringServoMicros = map(pulseWidth[1], 1500, 2000, CH1C, CH1R);
    else steeringServoMicros = CH1C;
    if (steeringServoMicrosDelayed < steeringServoMicros) steeringServoMicrosDelayed++;
    if (steeringServoMicrosDelayed > steeringServoMicros) steeringServoMicrosDelayed--;
    steeringServoMicrosDelayed = constrain(steeringServoMicrosDelayed, min(CH1L, CH1R), max(CH1L, CH1R));
    //Serial.println(steeringServoMicros);
    //Serial.println(steeringServoMicrosDelayed);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, steeringServoMicrosDelayed);
  }

  // Shifting CH2 **********************
  static uint16_t shiftingServoMicros;
#if not defined MODE1_SHIFTING
  if (selectedGear == 1) shiftingServoMicros = CH2L;
  if (selectedGear == 2) shiftingServoMicros = CH2C;
  if (selectedGear == 3) shiftingServoMicros = CH2R;
#else
  if (currentSpeed > 100 && currentSpeed < 150) { // Only shift WPL gearbox, if vehicle is moving slowly, so it's engaging properly
    if (!mode1) shiftingServoMicros = CH2L;
    else shiftingServoMicros = CH2C;
  }
#endif
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, shiftingServoMicros);

  // Winch CH3 **********************
#if defined MODE2_WINCH
  static uint16_t winchServoMicrosDesired = CH3C;
  static uint16_t winchServoMicros = CH3C;
  static unsigned long winchDelayMicros;
  if (micros() - winchDelayMicros > 12000) {
    winchDelayMicros = micros();
    if (winchPull) winchServoMicrosDesired = CH3L;
    else if (winchRelease) winchServoMicrosDesired = CH3R;
    else winchServoMicrosDesired = CH3C;
    if (winchServoMicros < winchServoMicrosDesired) winchServoMicros ++;
    if (winchServoMicros > winchServoMicrosDesired) winchServoMicros --;
  }
  mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_B, winchServoMicros);
#endif

  // Beacon CH3 **********************
#if defined CH3_BEACON

  // Init (5 pulses are required to shut beacons off after power on)
  static bool blueLightInit;
  if (!blueLightInit) {
    if (beaconControl(5)) blueLightInit = true;
  }

  // Switching modes
  static uint16_t beaconServoMicros;
  static bool lockRotating, lockOff;
  if (blueLightInit) {
    if (blueLightTrigger && !lockRotating) { // Rotating mode on (1 pulse)
      if (beaconControl(1)) {
        lockRotating = true;
        lockOff = false;
      }
    }
    if (!blueLightTrigger && !lockOff && lockRotating) { // Off (4 pulses)
      if (beaconControl(4)) {
        lockOff = true;
        lockRotating = false;
      }
    }
  }
#endif

  // Trailer coupler (5th wheel) CH4 **********************
  static uint16_t couplerServoMicros;
  if (unlock5thWheel) couplerServoMicros = CH4R;
  else couplerServoMicros = CH4L;
  mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM_OPR_A, couplerServoMicros);

  // Print servo signal debug infos **********************
  static unsigned long printServoMillis;
#ifdef SERVO_DEBUG // can slow down the playback loop!
  if (millis() - printServoMillis > 1000) { // Every 1000ms
    printServoMillis = millis();

    Serial.println("Servo Positions: [microseconds], [degrees]");

    Serial.print("CH1 (steering):  ");
    Serial.print(steeringServoMicrosDelayed);
    Serial.print(", ");
    Serial.println((steeringServoMicrosDelayed - 500) / 11.111 - 90);

    Serial.print("CH2 (shifting):  ");
    Serial.print(shiftingServoMicros);
    Serial.print(", ");
    Serial.println((shiftingServoMicros - 500) / 11.111 - 90);

#if defined MODE2_WINCH
    Serial.print("CH3 (winch): ");
    Serial.print(winchServoMicros);
    Serial.print(", ");
    Serial.println((winchServoMicros - 500) / 11.111 - 90);
#endif

#if defined CH3_BEACON
    Serial.print("CH3 (beacon): ");
    Serial.print(beaconServoMicros);
    Serial.print(", ");
    Serial.println((beaconServoMicros - 500) / 11.111 - 90);
#endif

    Serial.print("CH4 (5th wheel): ");
    Serial.print(couplerServoMicros);
    Serial.print(", ");
    Serial.println((couplerServoMicros - 500) / 11.111 - 90);

    Serial.println("");
  }
#endif
}



//
// =======================================================================================================
// MAP PULSEWIDTH TO THROTTLE CH3
// =======================================================================================================
//

void mapThrottle() {

  // Input is around 1000 - 2000us, output 0-500 for forward and backwards

#if defined TRACKED_MODE // Dual throttle input for caterpillar vehicles ------------------
  int16_t currentThrottleLR[4]; // 2 & 3 is used, so required array size = 4!

  // check if pulsewidths 2 + 3 look like servo pulses
  for (int i = 2; i < 4; i++) {
    if (pulseWidth[i] > pulseMinLimit[i] && pulseWidth[i] < pulseMaxLimit[i]) {
      if (pulseWidth[i] < pulseMin[i]) pulseWidth[i] = pulseMin[i]; // Constrain the value
      if (pulseWidth[i] > pulseMax[i]) pulseWidth[i] = pulseMax[i];

      // calculate a throttle value from the pulsewidth signal
      if (pulseWidth[i] > pulseMaxNeutral[i]) {
        currentThrottleLR[i] = map(pulseWidth[i], pulseMaxNeutral[i], pulseMax[i], 0, 500);
      }
      else if (pulseWidth[i] < pulseMinNeutral[i]) {
        currentThrottleLR[i] = map(pulseWidth[i], pulseMinNeutral[i], pulseMin[i], 0, 500);
      }
      else {
        currentThrottleLR[i] = 0;
      }
    }
  }

  // Mixing both sides together (take the bigger value)
  currentThrottle = max(currentThrottleLR[2], currentThrottleLR[3]);

  // Print debug infos
  static unsigned long printTrackedMillis;
#ifdef TRACKED_DEBUG // can slow down the playback loop!
  if (millis() - printTrackedMillis > 1000) { // Every 1000ms
    printTrackedMillis = millis();

    Serial.println("TRACKED DEBUG:");
    Serial.println(currentThrottleLR[2]);
    Serial.println(currentThrottleLR[3]);
    Serial.println(currentThrottle);
  }
#endif

#elif defined EXCAVATOR_MODE // Excavator mode ---------------------------------------------- 

  static bool engineInit = false; // Only allow to start engine after switch was in up position

  // calculate a throttle value from the pulsewidth signal (forward only)
  if (pulseWidth[3] > pulseMaxNeutral[3]) {
    currentThrottle = map(pulseWidth[3], pulseMaxNeutral[3], pulseMax[3], 0, 500);
  }
  else {
    currentThrottle = 0;
  }

  // Engine on / off via 3 position switch
  if (pulseWidth[3] < 1200 && currentRpm < 50) {
    engineInit = true;
    engineOn = false;
  }
  else {
    if (engineInit) engineOn = true;
  }


#else // Normal mode --------------------------------------------------------------------------- 

  // check if the pulsewidth looks like a servo pulse
  if (pulseWidth[3] > pulseMinLimit[3] && pulseWidth[3] < pulseMaxLimit[3]) {
    if (pulseWidth[3] < pulseMin[3]) pulseWidth[3] = pulseMin[3]; // Constrain the value
    if (pulseWidth[3] > pulseMax[3]) pulseWidth[3] = pulseMax[3];

    // calculate a throttle value from the pulsewidth signal
    if (pulseWidth[3] > pulseMaxNeutral[3]) {
      currentThrottle = map(pulseWidth[3], pulseMaxNeutral[3], pulseMax[3], 0, 500);
    }
    else if (pulseWidth[3] < pulseMinNeutral[3]) {
      currentThrottle = map(pulseWidth[3], pulseMinNeutral[3], pulseMin[3], 0, 500);
    }
    else {
      currentThrottle = 0;
    }
  }
#endif

  // Auto throttle --------------------------------------------------------------------------
#if not defined EXCAVATOR_MODE
  // Auto throttle while gear shifting (synchronizing the Tamiya 3 speed gearbox)
  if (!escIsBraking && escIsDriving && shiftingAutoThrottle && !automatic && !doubleClutch) {
    if (gearUpShiftingInProgress) currentThrottle = 0; // No throttle
    if (gearDownShiftingInProgress) currentThrottle = 500; // Full throttle
    currentThrottle = constrain (currentThrottle, 0, 500); // Limit throttle range
  }
#endif

  // Volume calculations --------------------------------------------------------------------------

  // As a base for some calculations below, fade the current throttle to make it more natural
  static unsigned long throttleFaderMicros;
  static boolean blowoffLock;
  if (micros() - throttleFaderMicros > 500) { // Every 0.5ms
    throttleFaderMicros = micros();

    if (currentThrottleFaded < currentThrottle && currentThrottleFaded < 499) currentThrottleFaded += 2;
    if (currentThrottleFaded > currentThrottle && currentThrottleFaded > 2) currentThrottleFaded -= 2;
    //Serial.println(currentThrottleFaded);
  }

  // Calculate throttle dependent engine idle volume
  if (!escIsBraking && engineRunning) throttleDependentVolume = map(currentThrottleFaded, 0, 500, engineIdleVolumePercentage, fullThrottleVolumePercentage);
  else throttleDependentVolume = engineIdleVolumePercentage;

  // Calculate throttle dependent engine rev volume
  if (!escIsBraking && engineRunning) throttleDependentRevVolume = map(currentThrottleFaded, 0, 500, engineRevVolumePercentage, fullThrottleVolumePercentage);
  else throttleDependentRevVolume = engineRevVolumePercentage;

  // Calculate engine rpm dependent jake brake volume
  if (engineRunning) rpmDependentJakeBrakeVolume = map(currentRpm, 0, 500, jakeBrakeIdleVolumePercentage, 100);
  else rpmDependentJakeBrakeVolume = jakeBrakeIdleVolumePercentage;

  // Calculate throttle dependent Diesel knock volume
  if (!escIsBraking && engineRunning && (currentThrottleFaded > dieselKnockStartPoint)) throttleDependentKnockVolume = map(currentThrottleFaded, dieselKnockStartPoint, 500, dieselKnockIdleVolumePercentage, 100);
  else throttleDependentKnockVolume = dieselKnockIdleVolumePercentage;

  // Calculate RPM dependent Diesel knock volume
  if (currentRpm > 400) rpmDependentKnockVolume = map(currentRpm, 400, 500, 5, 100);
  else rpmDependentKnockVolume = 5;

  // Calculate engine rpm dependent turbo volume
  if (engineRunning) throttleDependentTurboVolume = map(currentRpm, 0, 500, turboIdleVolumePercentage, 100);
  else throttleDependentTurboVolume = turboIdleVolumePercentage;

  // Calculate engine rpm dependent cooling fan volume
  if (engineRunning && (currentRpm > fanStartPoint)) throttleDependentFanVolume = map(currentRpm, fanStartPoint, 500, fanIdleVolumePercentage, 100);
  else throttleDependentFanVolume = fanIdleVolumePercentage;

  // Calculate throttle dependent supercharger volume
  if (!escIsBraking && engineRunning && (currentRpm > chargerStartPoint)) throttleDependentChargerVolume = map(currentThrottleFaded, chargerStartPoint, 500, chargerIdleVolumePercentage, 100);
  else throttleDependentChargerVolume = chargerIdleVolumePercentage;

  // Calculate engine rpm dependent wastegate volume
  if (engineRunning) rpmDependentWastegateVolume = map(currentRpm, 0, 500, wastegateIdleVolumePercentage, 100);
  else rpmDependentWastegateVolume = wastegateIdleVolumePercentage;


  // Calculate engine load (used for torque converter slip simulation)
  engineLoad = currentThrottle - currentRpm;


  if (engineLoad < 0 || escIsBraking) engineLoad = 0; // Range is 0 - 180
  if (engineLoad > 180) engineLoad = 180;
}

//
// =======================================================================================================
// ENGINE MASS SIMULATION
// =======================================================================================================
//

void engineMassSimulation() {

  static int32_t  targetRpm = 0;        // The engine RPM target
  static int32_t  lastThrottle;
  uint16_t converterSlip;
  static unsigned long throtMillis;
  //static unsigned long printMillis; // TODO
  static unsigned long wastegateMillis;
  static unsigned long blowoffMillis;
  uint8_t timeBase;

#ifdef SUPER_SLOW
  timeBase = 6; // super slow running, heavy engines, for example locomotive diesels
#else
  timeBase = 2;
#endif

  if (millis() - throtMillis > timeBase) { // Every 2 or 6ms
    throtMillis = millis();

    if (currentThrottle > 500) currentThrottle = 500;

    // Virtual clutch **********************************************************************************
#if defined EXCAVATOR_MODE // Excavator mode ---
    clutchDisengaged = true;

    targetRpm = currentThrottle - hydraulicLoad;
    targetRpm = constrain(targetRpm, 0, 500);


#else   // Normal mode ---
    if ((currentSpeed < clutchEngagingPoint && currentRpm < maxClutchSlippingRpm) || gearUpShiftingInProgress || gearDownShiftingInProgress || neutralGear || currentRpm < 200) {
      clutchDisengaged = true;
    }
    else {
      clutchDisengaged = false;
    }


    // Transmissions ***********************************************************************************

    // automatic transmission ----
    if (automatic) {
      // Torque converter slip calculation
      if (selectedAutomaticGear < 2) converterSlip = engineLoad * torqueconverterSlipPercentage / 100 * 2; // more slip in first and reverse gear
      else converterSlip = engineLoad * torqueconverterSlipPercentage / 100;

      if (!neutralGear) targetRpm = currentSpeed * gearRatio[selectedAutomaticGear] / 10 + converterSlip; // Compute engine RPM
      else targetRpm = reMap(curveLinear, currentThrottle);
    }
    else if (doubleClutch) {
      // double clutch transmission
      if (!neutralGear) targetRpm = currentSpeed * gearRatio[selectedAutomaticGear] / 10; // Compute engine RPM
      else targetRpm = reMap(curveLinear, currentThrottle);

    }
    else {
      // Manual transmission ----
      if (clutchDisengaged) { // Clutch disengaged: Engine revving allowed
#if defined VIRTUAL_16_SPEED_SEQUENTIAL
        targetRpm = currentThrottle;
#else
        targetRpm = reMap(curveLinear, currentThrottle);
#endif
      }
      else { // Clutch engaged: Engine rpm synchronized with ESC power (speed)



#if defined VIRTUAL_3_SPEED || defined VIRTUAL_16_SPEED_SEQUENTIAL // Virtual 3 speed or sequential 16 speed transmission
        targetRpm = currentSpeed * virtualManualGearRatio[selectedGear] / 10; // Add virtual gear ratios
        if (targetRpm > 500) targetRpm = 500;

#else // Real 3 speed transmission           
        targetRpm = reMap(curveLinear, currentSpeed);
#endif
      }
    }
#endif

    // Engine RPM **************************************************************************************

    if (escIsBraking && currentSpeed < clutchEngagingPoint) targetRpm = 0; // keep engine @idle rpm, if braking at very low speed
    if (targetRpm > 500) targetRpm = 500;


    // Accelerate engine
    if (targetRpm > (currentRpm + acc) && (currentRpm + acc) < maxRpm && engineState == RUNNING && engineRunning) {
      if (!airBrakeTrigger) { // No acceleration, if brake release noise still playing
        if (!gearDownShiftingInProgress) currentRpm += acc;
        else currentRpm += acc / 2; // less aggressive rpm rise while downshifting
        if (currentRpm > maxRpm) currentRpm = maxRpm;
      }
    }

    // Decelerate engine
    if (targetRpm < currentRpm) {
      currentRpm -= dec;
      if (currentRpm < minRpm) currentRpm = minRpm;
    }

#if defined VIRTUAL_3_SPEED || defined VIRTUAL_16_SPEED_SEQUENTIAL
    // Limit top speed, depending on manual gear ratio. Ensures, that the engine will not blow up!
    if (!automatic && !doubleClutch) speedLimit = maxRpm * 10 / virtualManualGearRatio[selectedGear];
#endif

    // Speed (sample rate) output
    engineSampleRate = map(currentRpm, minRpm, maxRpm, maxSampleInterval, minSampleInterval); // Idle
  }

  // Prevent Wastegate from being triggered while downshifting
  if (gearDownShiftingInProgress) wastegateMillis = millis();

  // Trigger Wastegate, if throttle rapidly dropped
  if (lastThrottle - currentThrottle > 70 && !escIsBraking && millis() - wastegateMillis > 1000) {
    wastegateMillis = millis();
    wastegateTrigger = true;
  }

#if defined JAKEBRAKE_ENGINE_SLOWDOWN && defined JAKE_BRAKE_SOUND
  // Use jake brake to slow down engine while releasing throttle in neutral or during upshifting while applying throttle
  // for some vehicles like Volvo FH open pipe. See example: https://www.youtube.com/watch?v=MU1iwzl33Zw&list=LL&index=4
  if (!wastegateTrigger) blowoffMillis = millis();
  blowoffTrigger = ((gearUpShiftingInProgress || neutralGear) && millis() - blowoffMillis > 20 && millis() - blowoffMillis < 250 );
#endif

  lastThrottle = currentThrottle;
}

//
// =======================================================================================================
// SWITCH ENGINE ON OR OFF (for automatic mode)
// =======================================================================================================
//

void engineOnOff() {

  // static unsigned long pulseDelayMillis; // TODO
  static unsigned long idleDelayMillis;

  // Engine automatically switched on or off depending on throttle position and 15s delay timne
  if (currentThrottle > 80 || driveState != 0) idleDelayMillis = millis(); // reset delay timer, if throttle not in neutral


#ifdef AUTO_ENGINE_ON_OFF
  if (millis() - idleDelayMillis > 15000) {
    engineOn = false; // after delay, switch engine off
  }
#endif

#ifdef AUTO_LIGHTS
  if (millis() - idleDelayMillis > 10000) {
    lightsOn = false; // after delay, switch light off
  }
#endif

  // Engine start detection
  if (currentThrottle > 100 && !airBrakeTrigger) {
    engineOn = true;

#ifdef AUTO_LIGHTS
    lightsOn = true;
#endif
  }
}

//
// =======================================================================================================
// LED
// =======================================================================================================
//

uint8_t crankingDim;
uint8_t dipDim;
uint8_t xenonIgnitionFlash;
static unsigned long xenonMillis;
uint32_t indicatorFade = 300; // 300 is the fading time, simulating an incandescent bulb

// Brake light sub function ---------------------------------
void brakeLightsSub(int8_t brightness) {
  if (escIsBraking) {
    tailLight.pwm(255 - crankingDim);  // Taillights (full brightness)
    brakeLight.pwm(255 - crankingDim); // Brakelight on
  }
  else {
    tailLight.pwm(constrain(brightness - (crankingDim / 2), (brightness / 2), 255)); // Taillights (reduced brightness)
    brakeLight.pwm(constrain(brightness - (crankingDim / 2), (brightness / 2), 255)); // Brakelight (reduced brightness)
  }
}

// Headlights sub function ---------------------------------
void headLightsSub(bool head, bool fog, bool roof, bool park) {

  fogLightOn = fog;

#ifdef XENON_LIGHTS // Optional xenon ignition flash
  if (millis() - xenonMillis > 50) xenonIgnitionFlash = 0; else xenonIgnitionFlash = 170; // bulb is brighter for 50ms
#endif

#ifdef SEPARATE_FULL_BEAM // separate full beam bulb, wired to "rooflight" pin ----
  // Headlights (low beam bulb)
  if (!head && !park) {
    headLight.off();
    xenonMillis = millis();
    if (!headLightsFlasherOn) headLightsHighBeamOn = false;
  }
  else if (park) { // Parking lights
    headLight.pwm(constrain(headlightParkingBrightness - crankingDim, (headlightParkingBrightness / 2), 255));
    xenonMillis = millis();
    if (!headLightsFlasherOn) headLightsHighBeamOn = false;
  }
  else { //ON
    headLight.pwm(constrain(255 - crankingDim - 170 + xenonIgnitionFlash, 0, 255));
  }
  // Headlights (high beam bulb)
  if (headLightsFlasherOn || (headLightsHighBeamOn && head)) roofLight.pwm(200 - crankingDim); else roofLight.off();

#else // Bulbs wired as labeled on the board ----
  // Headlights
  if (!head && !park) { // OFF or flasher
    if (!headLightsFlasherOn) headLight.off(); else headLight.on();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }
  else if (park) { // Parking lights
    if (!headLightsFlasherOn) headLight.pwm(constrain(headlightParkingBrightness - crankingDim, (headlightParkingBrightness / 2), 255)); else headLight.on();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }
  else { //ON
    headLight.pwm(constrain(255 - crankingDim - dipDim + xenonIgnitionFlash, 0, 255));
  }

  // Roof lights
  if (!roof) roofLight.off(); else roofLight.pwm(130 - crankingDim);
#endif // ----

  // Fog lights
  if (!fog) fogLight.off(); else fogLight.pwm(200 - crankingDim);
}

// Main LED function --------------------------------------------------------------------------------------
void led() {

#if defined LED_INDICATORS
  indicatorFade = 0; // No soft indicator on / off, if LED
#endif

  // Lights brightness ----
  if (engineStart) crankingDim = 50; else crankingDim = 0; // lights are dimmer while engine cranking
  if (headLightsFlasherOn || headLightsHighBeamOn) dipDim = 10; else dipDim = 170; // High / low beam and headlight flasher (SBUS CH5)

  // Reversing light ----
  if ((engineRunning || engineStart) && escInReverse) reversingLight.pwm(reversingLightBrightness - crankingDim);
  else reversingLight.off();

#if not defined SPI_DASHBOARD
  // Beacons (blue light) ----
#if not defined TRACKED_MODE  // Normal beacons mode 
  if (blueLightTrigger) {
    if (doubleFlashBlueLight) {
      beaconLight1.flash(30, 80, 400, 2); // Simulate double flash lights
      beaconLight2.flash(30, 80, 400, 2, 330); // Simulate double flash lights (with delay for first pass)
    }
    else {
      beaconLight1.flash(30, 500, 0, 0); // Simulate rotating beacon lights with short flashes
      beaconLight2.flash(30, 500, 0, 0, 100); // Simulate rotating beacon lights with short flashes
    }
  }
  else {
    beaconLight2.off();
    beaconLight1.off();
  }
#else // Beacons used for tank cannon fire simulation flash in TRACKED_MODE
  if (cannonFlash) beaconLight1.on();
  else beaconLight1.off();
#endif
#endif


  // Indicators (turn signals, blinkers) ----
  uint8_t indicatorOffBrightness;
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
  if (lightsState > 1) indicatorOffBrightness = rearlightDimmedBrightness - crankingDim / 2;
  else indicatorOffBrightness = 0;
#else
  indicatorOffBrightness = 0;
#endif

  if (!hazard) {
    if (indicatorLon) {
      if (indicatorL.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness)) indicatorSoundOn = true; // Left indicator
    }
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
    else {
      if (lightsState > 1) indicatorL.pwm(rearlightDimmedBrightness - crankingDim / 2);
      else indicatorL.off(indicatorFade);
    }
#else
    else indicatorL.off(indicatorFade);
#endif

    if (indicatorRon) {
      if (indicatorR.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness)) indicatorSoundOn = true; // Left indicator
    }
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
    else {
      if (lightsState > 1) indicatorR.pwm(rearlightDimmedBrightness - crankingDim / 2);
      else indicatorR.off(indicatorFade);
    }
#else
    else indicatorR.off(indicatorFade);
#endif
  }
  else { // Hazard lights on, if no connection to transmitter (serial & SBUS control mode only)
    if (indicatorL.flash(375, 375, 0, 0, 0, indicatorFade)) indicatorSoundOn = true;
    indicatorR.flash(375, 375, 0, 0, 0, indicatorFade);
  }

  // Headlights, tail lights ----
#ifdef AUTO_LIGHTS // automatic lights mode (deprecated, not maintained anymore!) ************************

#ifdef XENON_LIGHTS // Optional xenon ignition flash
  if (millis() - xenonMillis > 50) xenonIgnitionFlash = 0; else xenonIgnitionFlash = 170; // bulb is brighter for 50ms
#endif
  if (lightsOn && (engineRunning || engineStart)) {
    headLight.pwm(constrain(255 - crankingDim - dipDim + xenonIgnitionFlash, 0, 255));
    brakeLightsSub(rearlightDimmedBrightness);
  }

  else {
    headLight.off();
    tailLight.off();
    brakeLight.off();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }

  // Foglights ----
  if (lightsOn && engineRunning) {
    fogLight.pwm(200 - crankingDim);
    fogLightOn = true;
  }
  else {
    fogLight.off();
    fogLightOn = false;
  }

  // Roof lights ----
  if (lightsOn) roofLight.pwm(130 - crankingDim);
  else roofLight.off();

  // Sidelights ----
  if (engineOn) sideLight.pwm(200 - crankingDim);
  else sideLight.off();

  // Cabin lights ----
  if (!lightsOn) cabLight.pwm(255 - crankingDim);
  else cabLight.off();

#else // manual lights mode ************************
  // Lights state machine
  switch (lightsState) {

    case 0: // lights off ---------------------------------------------------------------------
      cabLight.off();
      sideLight.off();
      lightsOn = false;
      headLightsSub(false, false, false, false);
      brakeLightsSub(0); // 0 brightness, if not braking
      break;

    case 1: // cab lights ---------------------------------------------------------------------
#ifdef NO_CABLIGHTS
      lightsState = 2; // Skip cablights
#else
      cabLight.pwm(cabLightsBrightness - crankingDim);
#endif
      sideLight.off();
      headLightsSub(false, false, false, false);
      brakeLightsSub(0); // 0 brightness, if not braking
      break;

    case 2: // cab & roof & side lights ---------------------------------------------------------------------
#ifndef NO_CABLIGHTS
      cabLight.pwm(cabLightsBrightness - crankingDim);
#endif
      sideLight.pwm(constrain(sideLightsBrightness - crankingDim, (sideLightsBrightness / 2), 255));
      headLightsSub(false, false, true, true);
      fogLight.off();
      brakeLightsSub(rearlightParkingBrightness); // () = brightness, if not braking
      break;

    case 3: // roof & side & head lights ---------------------------------------------------------------------
      cabLight.off();
      sideLight.pwm(constrain(sideLightsBrightness - crankingDim, (sideLightsBrightness / 2), 255));
      lightsOn = true;
      headLightsSub(true, false, true, false);
      brakeLightsSub(rearlightDimmedBrightness); // 50 brightness, if not braking
      break;

    case 4: // roof & side & head & fog lights ---------------------------------------------------------------------
#ifdef NO_FOGLIGHTS
      lightsState = 5; // Skip foglights
#endif
      cabLight.off();
      sideLight.pwm(constrain(sideLightsBrightness - crankingDim, (sideLightsBrightness / 2), 255));
      headLightsSub(true, true, true, false);
      brakeLightsSub(rearlightDimmedBrightness); // 50 brightness, if not braking
      break;

    case 5: // cab & roof & side & head & fog lights ---------------------------------------------------------------------
#ifdef NO_CABLIGHTS
      lightsState = 0; // Skip cablights
#endif
      cabLight.pwm(cabLightsBrightness - crankingDim);
      sideLight.pwm(constrain(sideLightsBrightness - crankingDim, (sideLightsBrightness / 2), 255));
      headLightsSub(true, true, true, false);
      brakeLightsSub(rearlightDimmedBrightness); // 50 brightness, if not braking
      break;

  } // End of state machine
#endif // End of manual lights mode ************************
}

//
// =======================================================================================================
// SHAKER (simulates engine vibrations)
// =======================================================================================================
//

void shaker() {
  int32_t shakerRpm = 0;

  // Set desired shaker rpm
  if (engineRunning) shakerRpm = map(currentRpm, minRpm, maxRpm, shakerIdle, shakerFullThrottle);
  if (engineStart) shakerRpm = shakerStart;
  if (engineStop) shakerRpm = shakerStop;

  // Shaker on / off
  if (engineRunning || engineStart || engineStop) shakerMotor.pwm(shakerRpm);
  else shakerMotor.off();
}

//
// =======================================================================================================
// MANUAL TAMIYA 3 SPEED GEARBOX DETECTION
// =======================================================================================================
//

void gearboxDetection() {

  static uint8_t previousGear = 1;
  static boolean previousReverse;
  static boolean sequentialLock;
  static unsigned long upShiftingMillis;
  static unsigned long downShiftingMillis;

#if defined TRACKED_MODE // CH2 is used for left throttle in TRACKED_MODE --------------------------------
  selectedGear = 2;

#else // only active, if not in TRACKED_MODE -------------------------------------------------------------
  // if automatic transmission, always 2nd gear
  if (automatic || doubleClutch) selectedGear = 2;

#ifndef VIRTUAL_16_SPEED_SEQUENTIAL // 3 gears, directly selected by 3 position switch ----
  // Gear detection
  if (pulseWidth[2] > 1700) selectedGear = 3;
  else if (pulseWidth[2] < 1300) selectedGear = 1;
  else selectedGear = 2;

#else // 16 gears, selected by up / down impulses ----
  if (pulseWidth[2] > 1700 && selectedGear < 16 && !sequentialLock) {
    sequentialLock = true;
    selectedGear ++;
  }
  else if (pulseWidth[2] < 1300 && selectedGear > 1 && !sequentialLock) {
    sequentialLock = true;
    selectedGear --;
  }
  if (pulseWidth[2] > 1400 && pulseWidth[2] < 1600) sequentialLock = false;
#endif // End of VIRTUAL_16_SPEED_SEQUENTIAL ----

  // Gear upshifting detection
  if (selectedGear > previousGear) {
    gearUpShiftingInProgress = true;
    gearUpShiftingPulse = true;
    shiftingTrigger = true;
    previousGear = selectedGear;
  }

  // Gear upshifting duration
  if (!gearUpShiftingInProgress) upShiftingMillis = millis();
  if (millis() - upShiftingMillis > 700) gearUpShiftingInProgress = false;

  // Gear downshifting detection
  if (selectedGear < previousGear) {
    gearDownShiftingInProgress = true;
    gearDownShiftingPulse = true;
    shiftingTrigger = true;
    previousGear = selectedGear;
  }

  // Gear downshifting duration
  if (!gearDownShiftingInProgress) downShiftingMillis = millis();
  if (millis() - downShiftingMillis > 300) gearDownShiftingInProgress = false;


  // Reverse gear engaging / disengaging detection
  if (escInReverse != previousReverse) {
    previousReverse = escInReverse;
    shiftingTrigger = true; // Play shifting sound
  }


#ifdef MANUAL_TRANS_DEBUG
  static unsigned long manualTransDebugMillis;
  if (millis() - manualTransDebugMillis > 100) {
    manualTransDebugMillis = millis();
    Serial.println(currentThrottle);
    Serial.println(selectedGear);
    Serial.println(sequentialLock);
    Serial.println(currentRpm);
    Serial.println(currentSpeed);
    Serial.println("");
  }
#endif

#endif // End of not TRACKED_MODE -----------------------------------------------------------------------
}

//
// =======================================================================================================
// SIMULATED AUTOMATIC TRANSMISSION GEAR SELECTOR
// =======================================================================================================
//

void automaticGearSelector() {

  static unsigned long gearSelectorMillis;
  static unsigned long lastUpShiftingMillis;
  static unsigned long lastDownShiftingMillis;
  uint16_t downShiftPoint = 200;
  uint16_t upShiftPoint = 490;

  if (millis() - gearSelectorMillis > 100) { // Waiting for 100ms is very important. Otherwise gears are skipped!
    gearSelectorMillis = millis();

    // compute load dependent shift points (less throttle = less rpm before shifting up, kick down will shift back!)
    upShiftPoint = map(engineLoad, 0, 180, 390, 490); // 390, 490
    downShiftPoint = map(engineLoad, 0, 180, 150, 250); // 150, 250

    if (escInReverse) { // Reverse (only one gear)
      selectedAutomaticGear = 0;
    }
    else { // Forward (multiple gears)

      // Adaptive shift points
      if (millis() - lastDownShiftingMillis > 500 && currentRpm >= upShiftPoint && engineLoad < 5) { // 500ms locking timer!
        selectedAutomaticGear ++; // Upshifting (load maximum is important to prevent gears from oscillating!)
        lastUpShiftingMillis = millis();
      }
      if (millis() - lastUpShiftingMillis > 600 && selectedAutomaticGear > 1 && (currentRpm <= downShiftPoint || engineLoad > 100)) { // 600ms locking timer! TODO was 1000
        selectedAutomaticGear --; // Downshifting incl. kickdown
        lastDownShiftingMillis = millis();
      }

      selectedAutomaticGear = constrain(selectedAutomaticGear, 1, NumberOfAutomaticGears);
    }

#ifdef AUTO_TRANS_DEBUG
    Serial.println(currentThrottle);
    Serial.println(selectedAutomaticGear);
    Serial.println(engineLoad);
    Serial.println(upShiftPoint);
    Serial.println(currentRpm);
    Serial.println(downShiftPoint);
    Serial.println("");
#endif
  }
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

#if not defined TRACKED_MODE // No ESC control in TRACKED_MODE

  static int32_t escPulseWidth = 1500;
  static int32_t escPulseWidthOut = 1500;
  static uint32_t escSignal;
  static unsigned long escMillis;
  static unsigned long lastStateTime;
  static int8_t pulse; // -1 = reverse, 0 = neutral, 1 = forward
  static int8_t escPulse; // -1 = reverse, 0 = neutral, 1 = forward
  static int8_t driveRampRate;
  static int8_t driveRampGain;
  static int8_t brakeRampRate;
  uint8_t escRampTime;

  // Gear dependent ramp speed for acceleration & deceleration
#if defined VIRTUAL_3_SPEED
  escRampTime = escRampTimeThirdGear * 10 / virtualManualGearRatio[selectedGear];

#elif defined VIRTUAL_16_SPEED_SEQUENTIAL
  //escRampTime = escRampTimeThirdGear;// * 10 / map(virtualManualGearRatio[selectedGear], 155, 10, 23, 10);
  escRampTime = escRampTimeThirdGear * virtualManualGearRatio[selectedGear] / 5;

#else // TAMIYA 3 speed shifting transmission
  if (selectedGear == 1) escRampTime = escRampTimeFirstGear; // about 20
  if (selectedGear == 2) escRampTime = escRampTimeSecondGear; // about 50
  if (selectedGear == 3) escRampTime = escRampTimeThirdGear; // about 75
#endif

  if (automatic || doubleClutch) {
    escRampTime = escRampTimeSecondGear; // always use 2nd gear acceleration for automatic transmissions
    if (escInReverse) escRampTime = escRampTime * 100 / automaticReverseAccelerationPercentage; // faster acceleration in automatic reverse, EXPERIMENTAL, TODO!
  }

  if (millis() - escMillis > escRampTime) { // About very 20 - 75ms
    escMillis = millis();

    // calulate throttle dependent brake & acceleration steps
    brakeRampRate = map (currentThrottle, 0, 500, 1, escBrakeSteps);
    driveRampRate = map (currentThrottle, 0, 500, 1, escAccelerationSteps);

    // Emergency ramp rates for falisafe
    if (failSafe) {
      brakeRampRate = escBrakeSteps;
      driveRampRate = escBrakeSteps;
    }

    // Comparators
    if (pulseWidth[3] > pulseMaxNeutral[3] && pulseWidth[3] < pulseMaxLimit[3]) pulse = 1; // 1 = Forward
    else if (pulseWidth[3] < pulseMinNeutral[3] && pulseWidth[3] > pulseMinLimit[3]) pulse = -1; // -1 = Backwards
    else pulse = 0; // 0 = Neutral

    if (escPulseWidth > pulseMaxNeutral[3] && escPulseWidth < pulseMaxLimit[3]) escPulse = 1; // 1 = Forward
    else if (escPulseWidth < pulseMinNeutral[3] && escPulseWidth > pulseMinLimit[3]) escPulse = -1; // -1 = Backwards
    else escPulse = 0; // 0 = Neutral

#ifdef ESC_DEBUG
    if (millis() - lastStateTime > 300) { // Print the data every 300ms
      lastStateTime = millis();
      Serial.println(driveState);
      Serial.println(pulse);
      Serial.println(escPulse);
      Serial.println(escPulseMin);
      Serial.println(escPulseMax);
      Serial.println(brakeRampRate);
      Serial.println(currentRpm);
      Serial.println(escPulseWidth);
      Serial.println(escPulseWidthOut);
      Serial.println(currentSpeed);
      Serial.println(speedLimit);
      Serial.println("");
    }
#endif

    // Drive state state machine **********************************************************************************
    switch (driveState) {

      case 0: // Standing still ---------------------------------------------------------------------
        escIsBraking = false;
        escInReverse = false;
        escIsDriving = false;
        escPulseWidth = pulseZero[3];  // ESC to neutral position
#ifdef VIRTUAL_16_SPEED_SEQUENTIAL
        selectedGear = 1;
#endif

        if (pulse == 1 && engineRunning && !neutralGear) driveState = 1; // Driving forward
        if (pulse == -1 && engineRunning && !neutralGear) driveState = 3; // Driving backwards
        break;

      case 1: // Driving forward ---------------------------------------------------------------------
        escIsBraking = false;
        escInReverse = false;
        escIsDriving = true;
        //if (!gearUpShiftingPulse && !gearDownShiftingPulse) { // TODO!!
        if (escPulseWidth < pulseWidth[3] && currentSpeed < speedLimit) {
          if (escPulseWidth >= escPulseMaxNeutral) escPulseWidth += (driveRampRate * driveRampGain); //Faster
          else escPulseWidth = escPulseMaxNeutral; // Initial boost
        }
        if (escPulseWidth > pulseWidth[3] && escPulseWidth > pulseZero[3]) escPulseWidth -= (driveRampRate * driveRampGain); // Slower
        //}

        if (gearUpShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch) { // lowering RPM, if shifting up transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed trasnsmission        
          escPulseWidth -= currentSpeed / 4; // Synchronize engine speed
          //escPulseWidth -= currentSpeed * 40 / 100; // Synchronize engine speed TODO
#endif
          gearUpShiftingPulse = false;
          escPulseWidth = constrain(escPulseWidth, pulseZero[3], pulseMax[3]);
        }
        if (gearDownShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch) { // increasing RPM, if shifting down transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed trasnsmission      
          escPulseWidth += 50; // Synchronize engine speed
          //escPulseWidth += currentSpeed;// * 40 / 100; // Synchronize engine speed TODO
#endif
          gearDownShiftingPulse = false;
          escPulseWidth = constrain(escPulseWidth, pulseZero[3], pulseMax[3]);
        }

        if (pulse == -1 && escPulse == 1) driveState = 2; // Braking forward
        if (pulse == 0 && escPulse == 0) driveState = 0; // standing still
        break;

      case 2: // Braking forward ---------------------------------------------------------------------
        escIsBraking = true;
        escInReverse = false;
        escIsDriving = false;
        if (escPulseWidth > pulseZero[3]) escPulseWidth -= brakeRampRate; // brake with variable deceleration
        if (escPulseWidth < pulseZero[3]) escPulseWidth = pulseZero[3]; // Overflow bug prevention!

        if (pulse == 0 && escPulse == 1 && !neutralGear) {
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
        escIsDriving = true;
        if (escPulseWidth > pulseWidth[3] && currentSpeed < speedLimit) {
          if (escPulseWidth <= escPulseMinNeutral) escPulseWidth -= (driveRampRate * driveRampGain); //Faster
          else escPulseWidth = escPulseMinNeutral; // Initial boost
        }
        if (escPulseWidth < pulseWidth[3] && escPulseWidth < pulseZero[3]) escPulseWidth += (driveRampRate * driveRampGain); // Slower

        if (gearUpShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch) { // lowering RPM, if shifting up transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed trasnsmission        
          escPulseWidth += currentSpeed / 4; // Synchronize engine speed
#endif
          gearUpShiftingPulse = false;
          escPulseWidth = constrain(escPulseWidth, pulseMin[3], pulseZero[3]);
        }
        if (gearDownShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch) { // increasing RPM, if shifting down transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed trasnsmission      
          escPulseWidth -= 50; // Synchronize engine speed
#endif
          gearDownShiftingPulse = false;
          escPulseWidth = constrain(escPulseWidth, pulseMin[3], pulseZero[3]);
        }

        if (pulse == 1 && escPulse == -1) driveState = 4; // Braking backwards
        if (pulse == 0 && escPulse == 0) driveState = 0; // standing still
        break;

      case 4: // Braking backwards ---------------------------------------------------------------------
        escIsBraking = true;
        escInReverse = true;
        escIsDriving = false;
        if (escPulseWidth < pulseZero[3]) escPulseWidth += brakeRampRate; // brake with variable deceleration
        if (escPulseWidth > pulseZero[3]) escPulseWidth = pulseZero[3]; // Overflow bug prevention!

        if (pulse == 0 && escPulse == -1 && !neutralGear) {
          driveState = 3; // Driving backwards
          airBrakeTrigger = true;
        }
        if (pulse == 0 && escPulse == 0) {
          driveState = 0; // standing still
          airBrakeTrigger = true;
        }
        break;

    } // End of state machine **********************************************************************************


    // Gain for drive ramp rate, depending on clutchEngagingPoint
    if (currentSpeed < clutchEngagingPoint) {
      if (!automatic  && !doubleClutch) driveRampGain = 2; // prevent clutch from slipping too much (2)
      else driveRampGain = 4; // Automatic transmission needs to catch immediately (4)
    }
    else driveRampGain = 1;

    // ESC linearity compensation ---------------------
#ifdef QUICRUN_FUSION
    escPulseWidthOut = reMap(curveQuicrunFusion, escPulseWidth);
#else
    escPulseWidthOut = escPulseWidth;
#endif // -----------------------------------------


    // ESC control
#ifndef ESC_DIR
    escSignal = map(escPulseWidthOut, escPulseMin, escPulseMax, 3278, 6553); // 1 - 2ms (5 - 10% pulsewidth of 65534, not reversed)
#else
    escSignal = map(escPulseWidthOut, escPulseMax, escPulseMin, 3278, 6553); // 1 - 2ms (5 - 10% pulsewidth of 65534, reversed)
#endif
    escOut.pwm(escSignal);


    // Calculate a speed value from the pulsewidth signal (used as base for engine sound RPM while clutch is engaged)
    if (escPulseWidth > pulseMaxNeutral[3]) {
      currentSpeed = map(escPulseWidth, pulseMaxNeutral[3], pulseMax[3], 0, 500);
    }
    else if (escPulseWidth < pulseMinNeutral[3]) {
      currentSpeed = map(escPulseWidth, pulseMinNeutral[3], pulseMin[3], 0, 500);
    }
    else currentSpeed = 0;

  }
#endif
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
// HORN, BLUELIGHT & SIREN TRIGGERING BY CH4 (POT), WINCH CONTROL
// =======================================================================================================
//

void triggerHorn() {

  //#if not defined EXCAVATOR_MODE // Only used, if our vehicle is not an excavator!

  if (!winchEnabled) { // Horn & siren control mode *************************
    winchPull = false;
    winchRelease = false;

    // detect horn trigger ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4]) {
      hornTrigger = true;
      hornLatch = true;
    }
    else {
      hornTrigger = false;
    }

#if not defined EXCAVATOR_MODE
#ifndef NO_SIREN
    // detect siren trigger ( impulse length < 1100us) ----------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4]) {
      sirenTrigger = true;
      sirenLatch = true;
    }
    else {
      sirenTrigger = false;
    }
#endif
#endif

    // detect bluelight trigger ( impulse length < 1300us) ----------
    if ((pulseWidth[4] < 1300 && pulseWidth[4] > pulseMinLimit[4]) || sirenLatch) {
      blueLightTrigger = true;
    }
    else {
      blueLightTrigger = false;
    }

  }
  else { // Winch control mode **********************************************
    // pull winch ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4]) winchPull = true;
    else winchPull = false;

    // release winch ( impulse length < 1100us) -------------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4]) winchRelease = true;
    else winchRelease = false;

  }

  //#endif
}

//
// =======================================================================================================
// INDICATOR (TURN SIGNAL) TRIGGERING
// =======================================================================================================
//

void triggerIndicators() {

#if not defined EXCAVATOR_MODE // Only used, if our vehicle is not an excavator!

  static boolean L;
  static boolean R;

#ifdef AUTO_INDICATORS // Automatic, steering triggered indicators ********
  // detect left indicator trigger -------------
  if (pulseWidth[1] > (1500 + indicatorOn)) {
    L = true;
    R = false;
  }
  if (pulseWidth[1] < (1500 + indicatorOn / 3)) L = false;

  // detect right indicator trigger -------------
  if (pulseWidth[1] < (1500 - indicatorOn)) {
    R = true;
    L = false;
  }
  if (pulseWidth[1] > (1500 - indicatorOn / 3)) R = false;

#else // Manually triggered indicators ********
  // detect left indicator trigger -------------
  if (pulseWidth[6] > 1900) {
    L = true;
    R = false;
  }
  if (pulseWidth[6] < (1500 - indicatorOn / 3)) L = false;

  // detect right indicator trigger -------------
  if (pulseWidth[6] < 1100) {
    R = true;
    L = false;
  }
  if (pulseWidth[6] > (1500 + indicatorOn / 3)) R = false;

  // Reset by steering -------------
  static int steeringOld;

  if (pulseWidth[1] < steeringOld - 50) {
    L = false;
    steeringOld = pulseWidth[1];
  }

  if (pulseWidth[1] > steeringOld + 50) {
    R = false;
    steeringOld = pulseWidth[1];
  }

#endif // End of manually triggered indicators

  // Indicator direction
  if (!INDICATOR_DIR) {
    indicatorLon = L;
    indicatorRon = R;
  }
  else {
    indicatorLon = R;
    indicatorRon = L;
  }

  if (indicatorLon || indicatorRon) hazard = false;

#endif
}

//
// =======================================================================================================
// RC TRIGGER SECTION (rcTrigger LIBRARY REQUIRED)
// =======================================================================================================
//

void rcTrigger() {

#if not defined EXCAVATOR_MODE // Only used, if our vehicle is not an excavator!

  // Channel assignment see "remoteSetup.xlsx"

  // Potentiometers or 3 position switches ******************************************************************

  // CH5 (FUNCTION_R) ----------------------------------------------------------------------
  // Cycling light state machine, if dual rate @75% and long in position -----
  static bool lightsStateLock;
  if (functionR75u.toggleLong(pulseWidth[5], 1150) != lightsStateLock) {
    if (lightsState >= 5) lightsState = 0;
    else lightsState ++;
    lightsStateLock = !lightsStateLock;
  }

  // Toggling high / low beam, if dual rate @100% and short in position
  static bool beamStateLock;
  if (functionR100u.toggleLong(pulseWidth[5], 1000) != beamStateLock) {
    headLightsHighBeamOn = !headLightsHighBeamOn; // This lock is required, because high / low beam needs to be able to be changed in other program sections!
    beamStateLock = !beamStateLock;
  }

  // Headlight flasher as long as in position, if dual rate @100% -----
  headLightsFlasherOn = functionR100u.momentary(pulseWidth[5], 1000);

  // Jake brake as long as in position, if dual rate @100% -----
#ifdef JAKE_BRAKE_SOUND
  jakeBrakeRequest = functionR100d.momentary(pulseWidth[5], 2000) && currentRpm > jakeBrakeMinRpm;
#endif

  // Volume adjustment, if vehicle standing still and dual rate @100%
  static bool volumeStateLock;
  static uint8_t volumeIndex = 0;
  if (driveState == 0) {
    //if (functionR100d.toggleLong(pulseWidth[5], 2000)) masterVolume = masterVolumePercentage[1]; else masterVolume = masterVolumePercentage[0]; // Change volume between indoor and outdoor mode
    if (functionR100d.toggleLong(pulseWidth[5], 2000) != volumeStateLock) {
      if (volumeIndex < numberOfVolumeSteps  - 1) volumeIndex ++; // Switch volume steps
      else volumeIndex = 0;
      volumeStateLock = ! volumeStateLock;
    }
    masterVolume = masterVolumePercentage[volumeIndex]; // Write volume
  }

  // Engine on / off, if dual rate @75% and long in position -----
#ifndef AUTO_ENGINE_ON_OFF
  static bool engineStateLock;
  if (driveState == 0 && (engineState == OFF || engineState == RUNNING)) { // Only, if vehicle stopped and engine idling or off!
    if (functionR75d.toggleLong(pulseWidth[5], 1850) != engineStateLock) {
      engineOn = !engineOn; // This lock is required, because engine on / off needs to be able to be changed in other program sections!
      engineStateLock = !engineStateLock;
    }
  }
#endif

  // CH6 (FUNCTION_L) ----------------------------------------------------------------------

  // Indicators are triggered in triggerIndicators()

  //Hazards on / off, if dual rate @75% and long in position -----
#ifndef AUTO_INDICATORS
  static bool hazardStateLock;
  if (functionL75l.toggleLong(pulseWidth[6], 1150) != hazardStateLock) {
    hazard = ! hazard;
    hazardStateLock = ! hazardStateLock;
  }
#endif

  // Couple / uncouple 5th wheel, if dual rate @75% and long in position -----
  static bool fifthWheelStateLock;
  if (driveState == 0) { // Only allow change, if vehicle stopped!
    if (functionL75r.toggleLong(pulseWidth[6], 1850) != fifthWheelStateLock)  {
      unlock5thWheel = !unlock5thWheel;
      fifthWheelStateLock = !fifthWheelStateLock;
    }
  }

  // Latching 2 position switches ******************************************************************
  mode1 = mode1Trigger.onOff(pulseWidth[8], 1800, 1200); // CH8 (MODE1)
#ifdef TRANSMISSION_NEUTRAL
  neutralGear = mode1; // Transmission neutral
#endif
  mode2 = mode2Trigger.onOff(pulseWidth[9], 1800, 1200); // CH9 (MODE2)
#ifndef MODE2_WINCH // Sound 1 mode
  if (mode2) sound1trigger = true; //Trigger sound 1 (It is reset after playback is done
#else // Winch mode
  if (mode2) winchEnabled = true;
  else winchEnabled = false;
#endif

  // Momentary buttons ******************************************************************
  // Engine on / off momentary button CH10 -----
#ifndef AUTO_ENGINE_ON_OFF
  static bool engineStateLock2;
  if (driveState == 0 && (engineState == OFF || engineState == RUNNING)) { // Only, if vehicle stopped and engine idling or off!
    if (momentary1Trigger.toggleLong(pulseWidth[10], 2000) != engineStateLock2) {
      engineOn = !engineOn; // This lock is required, because engine on / off needs to be able to be changed in other program sections!
      engineStateLock2 = !engineStateLock2;
    }
  }
#endif

  // Flags ******************************************************************
#ifndef AUTO_INDICATORS
  // left = indicatorLTrigger.onOff(pulseWidth[12], 1800, 1200); // CH12 INDICATOR_LEFT not used
  // right = indicatorRTrigger.onOff(pulseWidth[13], 1800, 1200); // CH13 INDICATOR_RIGHT not used
#else
  hazard = hazardsTrigger.onOff(pulseWidth[11], 1800, 1200); // CH11 HAZARDS
#endif

#endif
}


//
// =======================================================================================================
// TRAILER PRESENCE SWITCH
// =======================================================================================================
//

void trailerPresenceSwitchRead() {
#if not defined THIRD_BRAKLELIGHT
  static unsigned long switchMillis;
  static boolean couplerSwitchStateLatch;

  if (couplerSwitchInteruptLatch) {
    switchMillis = millis();
    couplerSwitchInteruptLatch = false;
    couplerSwitchStateLatch = true;
  }

  if (couplerSwitchStateLatch && millis() - switchMillis > 1) { // Debouncing delay
    if (digitalRead(COUPLER_SWITCH_PIN)) {
      couplingTrigger = true;
      couplerSwitchStateLatch = false;
    }
    else {
      uncouplingTrigger = true;
      couplerSwitchStateLatch = false;
    }
  }
#endif
}

//
// =======================================================================================================
// LCD DASHBOARD BY Gamadril: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
// =======================================================================================================
//

/* Bugs TODO:

    Start animation too fast after engine off
    Speed needle jump if shifting real 3 speed transmission. Better calibration needed
*/


bool engineStartAnimation() {
  static bool dirUp = true;
  static uint16_t lastFrameTime = millis();
  static uint16_t rpm = 0;
  static uint16_t speed = 0;
  static uint16_t fuel = 0;
  static uint16_t adblue = 0;
  static int16_t currentStep = 0; // 50 in total

  if (millis() - lastFrameTime > 14) {
    dashboard.setSpeed(speed);
    dashboard.setRPM(rpm);
    dashboard.setFuelLevel(fuel);
    dashboard.setAdBlueLevel(adblue);

    speed += (dirUp ? 1 : -1) * (SPEED_MAX - SPEED_MIN) / 50;
    rpm += (dirUp ? 1 : -1) * (RPM_MAX - RPM_MIN) / 50;
    fuel += (dirUp ? 1 : -1) * (FUEL_MAX - FUEL_MIN) / 50;
    adblue += (dirUp ? 1 : -1) * (ADBLUE_MAX - ADBLUE_MIN) / 50;
    currentStep = currentStep + (dirUp ? 1 : -1);

    lastFrameTime = millis();

    if (dirUp && currentStep >= 50) {
      speed = SPEED_MAX;
      rpm = RPM_MAX;
      fuel = FUEL_MAX;
      adblue = ADBLUE_MAX;
      currentStep = 50;
      dirUp = false;
    }
    else if (!dirUp && currentStep <= 0) { // Animation finished
      speed = 0;
      rpm = 0;
      fuel = 0;
      adblue = 0;
      currentStep = 0;
      dirUp = true;
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------
void updateDashboard() {
  static uint16_t lastFrameTime = millis();
  static uint16_t rpm = 0;
  static uint16_t fuel = 0;
  static uint16_t adblue = 0;
  static bool startAnimationFinished = false;

  static uint16_t rpmNeedle = 0;
  static uint16_t speedNeedle = 0;
  static uint16_t fuelNeedle = 0;
  static uint16_t adblueNeedle = 0;

  // Start animation triggering
  if ((engineState == STARTING || engineState == RUNNING) && !startAnimationFinished) {
    startAnimationFinished = engineStartAnimation();
    return;
  }
  else if (engineState == OFF) {
    startAnimationFinished = false;
  }

  // Calculations
  // RPM
  if (engineState == STARTING || engineState == RUNNING) {
    rpm = currentRpm * 450 / 500 + 50; // Idle rpm offset!
    fuel = 90;
    adblue = 80;
  }
  else {
    rpm = currentRpm;
    fuel = 0;
    adblue = 0;
  }

  // Speed
  uint16_t speed;

#if defined VIRTUAL_3_SPEED or defined VIRTUAL_16_SPEED_SEQUENTIAL
  speed = currentSpeed; // for all transmissions
#else
  if (!automatic) speed = currentSpeed * 100 / manualGearRatios[selectedGear - 1]; // Manual transmission
  else speed = currentSpeed; // Automatic transmission
#endif

  speed = map(speed, 0, RPM_MAX, 0, MAX_REAL_SPEED);

  if (!gearUpShiftingInProgress && !gearDownShiftingInProgress) { // avoid jumps between gear switches
    dashboard.setSpeed(speed);
  }

  // Central display
  if (neutralGear) {
    dashboard.setGear(0);
  }
  else if (escInReverse) {
    dashboard.setGear(-1);
  }
  else {
    if (!automatic) dashboard.setGear(selectedGear); // Manual transmission
    else dashboard.setGear(selectedAutomaticGear); // Automatic transmission
  }

  // Indicator lamps
  dashboard.setLeftIndicator(indicatorSoundOn && (indicatorLon || hazard));
  dashboard.setRightIndicator(indicatorSoundOn && (indicatorRon || hazard));
  dashboard.setLowBeamIndicator(lightsOn);
  dashboard.setHighBeamIndicator(headLightsHighBeamOn || headLightsFlasherOn);
  dashboard.setFogLightIndicator(fogLightOn);

  // Needles
  if (millis() - lastFrameTime > 14) {

    if (engineState == RUNNING) {
      rpmNeedle = rpm; // No delay, if running!
    }
    else {
      if (rpm > rpmNeedle) rpmNeedle++;
      if (rpm < rpmNeedle) rpmNeedle--;
    }

    if (fuel > fuelNeedle) fuelNeedle++;
    if (fuel < fuelNeedle) fuelNeedle--;

    if (adblue > adblueNeedle) adblueNeedle++;
    if (adblue < adblueNeedle) adblueNeedle--;

    dashboard.setRPM(rpmNeedle);
    dashboard.setFuelLevel(fuelNeedle);
    dashboard.setAdBlueLevel(adblueNeedle);
    lastFrameTime = millis();
  }
}

//
// =======================================================================================================
// NEOPIXEL WS2812 LED MB STAR BY Gamadril: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
// =======================================================================================================
//

void updateRGBLEDs() {
  uint8_t hue = map(pulseWidth[1], 1000, 2000, 0, 255);

  rgbLEDs[0] = CHSV(hue, hue < 255 ? 255 : 0, hue > 0 ? 255 : 0);
  FastLED.show();
}

//
// =======================================================================================================
// EXCAVATOR CONTROL
// =======================================================================================================
//

void excavatorControl() {

  static uint32_t lastFrameTime = millis();
  static uint16_t hydraulicPumpVolumeInternal[9];
  static uint16_t hydraulicPumpVolumeInternalUndelayed;
  static uint16_t hydraulicFlowVolumeInternalUndelayed;
  static uint16_t trackRattleVolumeInternal[9];
  static uint16_t trackRattleVolumeInternalUndelayed;
  static uint16_t lastBucketPulseWidth = pulseWidth[1];
  static uint16_t lastDipperPulseWidth = pulseWidth[2];

  if (millis() - lastFrameTime > 4) { // 3
    lastFrameTime = millis();

    // Calculate zylinder speed and engine RPM dependent hydraulic pump volume ----
    // Bucket ---
    if (pulseWidth[1] > pulseMaxNeutral[1]) hydraulicPumpVolumeInternal[1] = map(pulseWidth[1], pulseMaxNeutral[1], pulseMax[1], 0, 100);
    else if (pulseWidth[1] < pulseMinNeutral[1]) hydraulicPumpVolumeInternal[1] = map(pulseWidth[1], pulseMinNeutral[1], pulseMin[1], 0, 100);
    else hydraulicPumpVolumeInternal[1] = 0;

    // Dipper ---
    if (pulseWidth[2] > pulseMaxNeutral[2]) hydraulicPumpVolumeInternal[2] = map(pulseWidth[2], pulseMaxNeutral[2], pulseMax[2], 0, 100);
    else if (pulseWidth[2] < pulseMinNeutral[2]) hydraulicPumpVolumeInternal[2] = map(pulseWidth[2], pulseMinNeutral[2], pulseMin[2], 0, 100);
    else hydraulicPumpVolumeInternal[2] = 0;

    // Boom (upwards only) ---
    if (pulseWidth[5] < pulseMinNeutral[5]) hydraulicPumpVolumeInternal[5] = map(pulseWidth[5], pulseMinNeutral[5], (pulseMin[5] + 200), 0, 100);
    else hydraulicPumpVolumeInternal[5] = 0;

    // Swing ---
    if (pulseWidth[8] > pulseMaxNeutral[8]) hydraulicPumpVolumeInternal[8] = map(pulseWidth[8], pulseMaxNeutral[8], pulseMax[8], 0, 100);
    else if (pulseWidth[8] < pulseMinNeutral[8]) hydraulicPumpVolumeInternal[8] = map(pulseWidth[8], pulseMinNeutral[8], pulseMin[8], 0, 100);
    else hydraulicPumpVolumeInternal[8] = 0;

    hydraulicPumpVolumeInternalUndelayed = constrain(hydraulicPumpVolumeInternal[1] + hydraulicPumpVolumeInternal[2] + hydraulicPumpVolumeInternal[5]
                                           + hydraulicPumpVolumeInternal[8], 0, 100) * map(currentRpm, 0, 500, 30, 100) / 100;

    if (hydraulicPumpVolumeInternalUndelayed < hydraulicPumpVolume) hydraulicPumpVolume--;
    if (hydraulicPumpVolumeInternalUndelayed > hydraulicPumpVolume) hydraulicPumpVolume++;


    // Calculate zylinder speed dependent hydraulic flow volume ----
    // Boom (downwards) ---
    if (pulseWidth[5] > pulseMaxNeutral[5]) hydraulicFlowVolumeInternalUndelayed = map(pulseWidth[5], pulseMaxNeutral[5], (pulseMax[5] - 200), 0, 100);
    else hydraulicFlowVolumeInternalUndelayed = 0;

    if (hydraulicFlowVolumeInternalUndelayed < hydraulicFlowVolume) hydraulicFlowVolume--;
    if (hydraulicFlowVolumeInternalUndelayed > hydraulicFlowVolume) hydraulicFlowVolume++;


    // Calculate speed dependent track rattle volume ----
    // Left ---
    if (pulseWidth[6] > pulseMaxNeutral[6]) trackRattleVolumeInternal[6] = map(pulseWidth[6], pulseMaxNeutral[6], pulseMax[6], 0, 100);
    else if (pulseWidth[6] < pulseMinNeutral[6]) trackRattleVolumeInternal[6] = map(pulseWidth[6], pulseMinNeutral[6], pulseMin[6], 0, 100);
    else trackRattleVolumeInternal[6] = 0;

    // Right
    if (pulseWidth[7] > pulseMaxNeutral[7]) trackRattleVolumeInternal[7] = map(pulseWidth[7], pulseMaxNeutral[7], pulseMax[7], 0, 100);
    else if (pulseWidth[7] < pulseMinNeutral[7]) trackRattleVolumeInternal[7] = map(pulseWidth[7], pulseMinNeutral[7], pulseMin[7], 0, 100);
    else trackRattleVolumeInternal[7] = 0;

    if (engineRunning) trackRattleVolumeInternalUndelayed = constrain(trackRattleVolumeInternal[6] + trackRattleVolumeInternal[7], 0, 100) * map(currentRpm, 0, 500, 100, 150) / 100;
    else trackRattleVolumeInternalUndelayed = 0;

    if (trackRattleVolumeInternalUndelayed < trackRattleVolume) trackRattleVolume--;
    if (trackRattleVolumeInternalUndelayed > trackRattleVolume) trackRattleVolume++;

    // Calculate hydraulic load dependent Diesel knock (& engine) volume
    hydraulicDependentKnockVolume = map(hydraulicPumpVolume, 0, 100, 50, 100);

    // Calculate hydraulic load dependent engine RMP drop
    hydraulicLoad = map(hydraulicPumpVolume, 0, 100, 0, 40);

    // Bucket rattle sound triggering
    if (engineRunning && currentRpm > 400) {
      // If bucket stick is moved fast
      if (abs(pulseWidth[1] - lastBucketPulseWidth > 100)) {
        bucketRattleTrigger = true;
      }
      lastBucketPulseWidth = pulseWidth[1];

      // If dipper stick is moved fast
      if (abs(pulseWidth[2] - lastDipperPulseWidth > 100)) {
        bucketRattleTrigger = true;
      }
      lastDipperPulseWidth = pulseWidth[2];
    }
  }
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop() {

#if defined SBUS_COMMUNICATION
  readSbusCommands(); // SBUS communication (pin 36)
  mcpwmOutput(); // PWM servo signal output

#elif defined IBUS_COMMUNICATION
  readIbusCommands(); // IBUS communication (pin 36)
  mcpwmOutput(); // PWM servo signal output

#elif defined SUMD_COMMUNICATION
  readSumdCommands(); // SUMD communication (pin 36)
  mcpwmOutput(); // PWM servo signal output

#elif defined PPM_COMMUNICATION
  readPpmCommands(); // PPM communication (pin 34)
  mcpwmOutput(); // PWM servo signal output

#else
  // measure RC signals mark space ratio
  readPwmSignals();
#endif

  // Map pulsewidth to throttle
  mapThrottle();

  // Horn triggering
  triggerHorn();

  // Indicator (turn signal) triggering
  triggerIndicators();

  // rcTrigger
  rcTrigger();

  // Excavator specific controls
#if defined EXCAVATOR_MODE
  excavatorControl();
#endif

  // Read trailer switch state
#if not defined THIRD_BRAKLELIGHT
  trailerPresenceSwitchRead();
#endif

  // Dashboard control
#if defined SPI_DASHBOARD
  updateDashboard();
#endif

#if defined NEOPIXEL_LED
  // RGB LED control
  updateRGBLEDs();
#endif
}

//
// =======================================================================================================
// 1st MAIN TASK, RUNNING ON CORE 0 (Interrupts are running on this core as well)
// =======================================================================================================
//

void Task1code(void *pvParameters) {
  for (;;) {

    // DAC offset fader
    dacOffsetFade();

    // Simulate engine mass, generate RPM signal
    engineMassSimulation();

    // Call gear selector
    if (automatic || doubleClutch) automaticGearSelector();

    // Switch engine on or off
    engineOnOff();

    // LED control
    if (autoZeroDone) led();

#if not defined SPI_DASHBOARD
    // Shaker control
    shaker();
#endif

    // Gearbox detection
    gearboxDetection();

    // ESC control (Crawler ESC with direct brake on pin 33)
    esc();

    // measure loop time
    //loopTime = loopDuration(); // for debug only
  }
}
