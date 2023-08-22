/* RC engine sound & light controller for Arduino ESP32. Written by TheDIYGuy999
    Based on the code for ATmega 328: https://github.com/TheDIYGuy999/Rc_Engine_Sound

    ***** ESP32 CPU frequency must be set to 240MHz! *****

   Sound files converted with: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/tools/Audio2Header.html
   Original converter code by bitluni (send him a high five, if you like the code)

   Parts of automatic transmision code from Wombii's fork: https://github.com/Wombii/Rc_Engine_Sound_ESP32

   Dashboard, Neopixel and SUMD support by Gamadril: https://github.com/Gamadril/Rc_Engine_Sound_ESP32

   Contributors:
   - Christian Fiebig https://github.com/fiechr

   Visual Studio Code IDE support added (you need to install PlatformIO plugin and git https://git-scm.com)
   Arduino IDE is supported as well, but I recommend to use VS Code, because libraries and boards are managed automatically.
*/

char codeVersion[] = "9.12.0"; // Software revision.

//
// =======================================================================================================
// ERROR CODES (INDICATOR LIGHTS, BEEPS)
// =======================================================================================================
//
/*
   Indicators:
   - Constantly on = no SBUS signal (check "sbusInverted" true / false in "2_Remote.h")
   - Number of blinks = this channel signal is not between 1400 and 1600 microseconds and can't be auto calibrated (check channel trim settings)
   - 2 fast blinks = battery error during powering up (see below)
   - 3 fast blinks = no valid receiver bus signal detected / wrong remote configuration

   Beeps (only, if "#define BATTERY_PROTECTION" in "3_ESC.h"):
   - Number of beeps = number of detected battery cells in series
   - 10 fast beeps = battery error, disconnect it!
*/

//
// =======================================================================================================
// ! ! I M P O R T A N T ! !   ALL USER SETTINGS ARE DONE IN THE FOLLOWING TABS, WHICH ARE DISPLAYED ABOVE
// (ADJUST THEM BEFORE CODE UPLOAD), DO NOT CHANGE ANYTHING IN THIS TAB EXCEPT THE DEBUG OPTIONS
// =======================================================================================================
//

#include <Arduino.h>

// All the required user settings are done in the following .h files:
#include "0_generalSettings.h" // <<------- general settings
#include "1_Vehicle.h"         // <<------- Select the vehicle you want to simulate
#include "2_Remote.h"          // <<------- Remote control system related adjustments
#include "3_ESC.h"             // <<------- ESC related adjustments
#include "4_Transmission.h"    // <<------- Transmission related adjustments
#include "5_Shaker.h"          // <<------- Shaker related adjustments
#include "6_Lights.h"          // <<------- Lights related adjustments
#include "7_Servos.h"          // <<------- Servo output related adjustments
#include "8_Sound.h"           // <<------- Sound related adjustments
#include "9_Dashboard.h"       // <<------- Dashboard related adjustments
#include "10_Trailer.h"        // <<------- Trailer related adjustments

// TODO = Things to clean up!

//
// =======================================================================================================
// LIRBARIES & HEADER FILES, REQUIRED ESP32 BOARD DEFINITION
// =======================================================================================================
//

// Libraries (you have to install all of them in the "Arduino sketchbook"/libraries folder)
// !! Do NOT install the libraries in the sketch folder.
// No manual library download is required in Visual Studio Code IDE (see platformio.ini)
#include <statusLED.h>        // https://github.com/TheDIYGuy999/statusLED <<------- required for LED control
#if not defined EMBEDDED_SBUS // SBUS library only required, if not embedded SBUS code is used -----------
#include <SBUS.h>             // https://github.com/TheDIYGuy999/SBUS      <<------- you need to install my fork of this library!
#endif                        // ------------------------------------------------------------------------------------------------
#include <rcTrigger.h>        // https://github.com/TheDIYGuy999/rcTrigger <<------- required for RC signal processing
#include <IBusBM.h>           // https://github.com/bmellink/IBusBM        <<------- required for IBUS interface
#include <TFT_eSPI.h>         // https://github.com/Bodmer/TFT_eSPI        <<------- required for LCD dashboard. Use v2.3.70
#if defined NEOPIXEL_ENABLED
#include <FastLED.h> // https://github.com/FastLED/FastLED        <<------- required for Neopixel support. Use V3.3.3
#endif
#include <ESP32AnalogRead.h> // https://github.com/madhephaestus/ESP32AnalogRead <<------- required for battery voltage measurement
#include <Tone32.h>          // https://github.com/lbernstone/Tone32      <<------- required for battery cell detection beeps // Not for platform = espressif32@4.3.0

// Additional headers (included)
#include "src/curves.h"    // Nonlinear throttle curve arrays
#include "src/helper.h"    // Various stuff
#include "src/dashboard.h" // For LCD dashboard. See: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
#include "src/SUMD.h"      // For Graupner SUMD interface. See: https://github.com/Gamadril/Rc_Engine_Sound_ESP32
#if defined EMBEDDED_SBUS
#include "src/sbus.h" // For SBUS interface
#endif

// No need to install these, they come with the ESP32 board definition
#include "driver/uart.h"  // for UART macro UART_PIN_NO_CHANGE
#include "driver/rmt.h"   // for PWM signal detection
#include "driver/mcpwm.h" // for servo PWM output
#include "rom/rtc.h"      // for displaying reset reason
#include "soc/rtc_wdt.h"  // for watchdog timer
#include <esp_now.h>      // for wireless trailer
#include <WiFi.h>
#include <esp_wifi.h>
#include <Esp.h>    // for displaying memory information
#include <EEPROM.h> // for non volatile variable storage

// This stuff is required for Visual Studio Code IDE, if .ino is renamed into .cpp!
void Task1code(void *parameters);
void readSbusCommands();
void readIbusCommands();
void readSumdCommands();
void readPpmCommands();
void readPwmSignals();
void processRawChannels();
void failsafeRcSignals();
void channelZero();
float batteryVolts();
void eepromDebugRead();
void eepromRead();
void eepromInit();

// The following tasks only required for Arduino IDE! ----
// Install ESP32 board according to: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
// Warning: Use Espressif ESP32 board definition v1.05 or 10.6! v2.x is not working
// Adjust board settings according to: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/pictures/settings.png

// Visual Studio Code IDE instructions: ----
// - General settings are done in platformio.ini
// - !!IMPORTANT!! Rename src.ini to scr.cpp -> This will prevent potential compiler errors

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

// Serial DEBUG pins -----
#define DEBUG_RX UART_PIN_NO_CHANGE // 99 is just a dummy, because the "RX0" pin (GPIO3) is used for the headlights and causing issues, if rx enabled! -1 (3 headlights)
#define DEBUG_TX 1                  // The "RX0" is on pin 1

// Serial command pins for SBUS, IBUS, PPM, SUMD -----
#define COMMAND_RX 36                 // pin 36, labelled with "VP", connect it to "Micro RC Receiver" pin "TXO"
#define COMMAND_TX UART_PIN_NO_CHANGE // 98 is just a dummy -1 (17 reversing)

#define BATTERY_DETECT_PIN 39 // Voltage divider resistors connected to pin "VN & GND"

// PWM RC signal input pins (active, if no other communications profile is enabled) -----
// Channel numbers may be different on your recveiver!
// CH1: (steering)
// CH2: (gearbox) (left throttle in TRACKED_MODE)
// CH3: (throttle) (right throttle in TRACKED_MODE)
// CH4: (horn and bluelight / siren)
// CH5: (high / low beam, transmission neutral, jake brake etc.)
// CH6: (indicators, hazards)
#define PWM_CHANNELS_NUM 6                                           // Number of PWM signal input pins 6
const uint8_t PWM_CHANNELS[PWM_CHANNELS_NUM] = {1, 2, 3, 4, 5, 6};   // Channel numbers
const uint8_t PWM_PINS[PWM_CHANNELS_NUM] = {13, 12, 14, 27, 35, 34}; // Input pin numbers (pin 34 & 35 only usable as inputs!)

// Output pins -----
#define ESC_OUT_PIN 33 // connect crawler type ESC here. Not supported in TRACKED_MODE -----

#define RZ7886_PIN1 33 // RZ7886 motor driver pin 1 (same as ESC_OUT_PIN)
#define RZ7886_PIN2 32 // RZ7886 motor driver pin 2 (same as BRAKELIGHT_PIN)

#define STEERING_PIN 13 // CH1 output for steering servo (bus communication only)
#define SHIFTING_PIN 12 // CH2 output for shifting servo (bus communication only)
#define WINCH_PIN 14    // CH3 output for winch servo (bus communication only)
#define COUPLER_PIN 27  // CH4 output for coupler (5th. wheel) servo (bus communication only)

#ifdef WEMOS_D1_MINI_ESP32 // switching headlight pin depending on the board variant
#define HEADLIGHT_PIN 22   // Headlights connected to GPIO 22
#define CABLIGHT_PIN -1    // No Cabin lights
#define NO_CABLIGHTS
#else
#define HEADLIGHT_PIN 3 // Headlights connected to GPIO 3
#define CABLIGHT_PIN 22 // Cabin lights connected to GPIO 22
#endif

#define TAILLIGHT_PIN 15       // Red tail- & brake-lights (combined)
#define INDICATOR_LEFT_PIN 2   // Orange left indicator (turn signal) light
#define INDICATOR_RIGHT_PIN 4  // Orange right indicator (turn signal) light
#define FOGLIGHT_PIN 16        // (16 = RX2) Fog lights
#define REVERSING_LIGHT_PIN 17 // (TX2) White reversing light
#define ROOFLIGHT_PIN 5        // Roof lights (high beam, if "define SEPARATE_FULL_BEAM")
#define SIDELIGHT_PIN 18       // Side lights (connect roof ligthts here, if "define SEPARATE_FULL_BEAM")
#define BEACON_LIGHT2_PIN 19   // Blue beacons light
#define BEACON_LIGHT1_PIN 21   // Blue beacons light

#define RGB_LEDS_PIN 0 // Pin is used for WS2812 LED control

#if defined THIRD_BRAKELIGHT
#define BRAKELIGHT_PIN 32 // Upper brake lights
#else
#define COUPLER_SWITCH_PIN 32 // switch for trailer coupler sound
#endif

#define SHAKER_MOTOR_PIN 23 // Shaker motor (shaking truck while idling and engine start / stop)

#define DAC1 25 // connect pin25 (do not change the pin) to a 10kOhm resistor
#define DAC2 26 // connect pin26 (do not change the pin) to a 10kOhm resistor
// both outputs of the resistors above are connected together and then to the outer leg of a
// 20kOhm potentiometer. The other outer leg connects to GND. The middle leg connects to both inputs
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

// Other objects -----
// Dashboard
Dashboard dashboard;

#if defined NEOPIXEL_ENABLED
// Neopixel
CRGB rgbLEDs[NEOPIXEL_COUNT];
#endif

// Battery voltage
ESP32AnalogRead battery;

// Webserver on port 80
WiFiServer server(80);

// Global variables **********************************************************************

// WiFi variables
String ssid;
String password;

// MAC address for communication with tractor TODO
uint8_t customMACAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// HTTP request memory
String header;

// For HTTP GET value
String valueString = "";
int pos1 = 0;
int pos2 = 0;

// PWM processing variables
#define RMT_TICK_PER_US 1
// determines how many clock cycles one "tick" is
// [1..255], source is generally 80MHz APB clk
#define RMT_RX_CLK_DIV (80000000 / RMT_TICK_PER_US / 1000000)
// time before receiver goes idle (longer pulses will be ignored)
#define RMT_RX_MAX_US 3500
volatile uint16_t pwmBuf[PWM_CHANNELS_NUM + 2] = {0};
uint32_t maxPwmRpmPercentage = 390; // Limit required to prevent controller from crashing @ high engine RPM

// PPM signal processing variables
#define NUM_OF_PPM_CHL 8                       // The number of channels inside our PPM signal (8 is the maximum!)
#define NUM_OF_PPM_AVG 1                       // Number of averaging passes (usually one, more will be slow)
volatile int ppmInp[NUM_OF_PPM_CHL + 1] = {0}; // Input values
volatile int ppmBuf[16] = {0};
volatile byte counter = NUM_OF_PPM_CHL;
volatile byte average = NUM_OF_PPM_AVG;
volatile boolean ready = false;
volatile unsigned long timelast;
unsigned long timelastloop;
uint32_t maxPpmRpmPercentage = 390; // Limit required to prevent controller from crashing @ high engine RPM

// SBUS signal processing variables
#if not defined EMBEDDED_SBUS // ------------------------
SBUS sBus(Serial2);           // SBUS object on Serial 2 port
// channel, fail safe, and lost frames data
uint16_t SBUSchannels[16];
bool SBUSfailSafe;
bool SBUSlostFrame;
#else  // ------------------------------------------------
bfs::SbusRx sBus(&Serial2);
std::array<int16_t, bfs::SbusRx::NUM_CH()> SBUSchannels;
#endif // -----------------------------------------------
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
IBusBM iBus; // IBUS object
bool ibusInit;
uint32_t maxIbusRpmPercentage = 320; // Limit required to prevent controller from crashing @ high engine RPM (was 350, but sometimes crashing)

// Interrupt latches
volatile boolean couplerSwitchInteruptLatch; // this is enabled, if the coupler switch pin change interrupt is detected

// Control input signals
#define PULSE_ARRAY_SIZE 14                // 13 channels (+ the unused CH0)
uint16_t pulseWidthRaw[PULSE_ARRAY_SIZE];  // Current RC signal RAW pulse width [X] = channel number
uint16_t pulseWidthRaw2[PULSE_ARRAY_SIZE]; // Current RC signal RAW pulse width with linearity compensation [X] = channel number
uint16_t pulseWidthRaw3[PULSE_ARRAY_SIZE]; // Current RC signal RAW pulse width before averaging [X] = channel number
uint16_t pulseWidth[PULSE_ARRAY_SIZE];     // Current RC signal pulse width [X] = channel number
int16_t pulseOffset[PULSE_ARRAY_SIZE];     // Offset for auto zero adjustment

uint16_t pulseMaxNeutral[PULSE_ARRAY_SIZE]; // PWM input signal configuration storage variables
uint16_t pulseMinNeutral[PULSE_ARRAY_SIZE];
uint16_t pulseMax[PULSE_ARRAY_SIZE];
uint16_t pulseMin[PULSE_ARRAY_SIZE];
uint16_t pulseMaxLimit[PULSE_ARRAY_SIZE];
uint16_t pulseMinLimit[PULSE_ARRAY_SIZE];

uint16_t pulseZero[PULSE_ARRAY_SIZE]; // Usually 1500 (The mid point of 1000 - 2000 Microseconds)
uint16_t pulseLimit = 1100;           // pulseZero +/- this value (1100)
uint16_t pulseMinValid = 700;         // The minimum valid pulsewidth (was 950)
uint16_t pulseMaxValid = 2300;        // The maximum valid pulsewidth (was 2050)
bool autoZeroDone;                    // Auto zero offset calibration done
#define NONE 16                       // The non existing "Dummy" channel number (usually 16) TODO

volatile boolean failSafe = false; // Triggered in emergency situations like: throttle signal lost etc.

boolean mode1; // Signal state variables
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
volatile boolean engineOn = false;                // Signal for engine on / off
volatile boolean engineStart = false;             // Active, if engine is starting up
volatile boolean engineRunning = false;           // Active, if engine is running
volatile boolean engineStop = false;              // Active, if engine is shutting down
volatile boolean jakeBrakeRequest = false;        // Active, if engine jake braking is requested
volatile boolean engineJakeBraking = false;       // Active, if engine is jake braking
volatile boolean wastegateTrigger = false;        // Trigger wastegate (blowoff) after rapid throttle drop
volatile boolean blowoffTrigger = false;          // Trigger jake brake sound (blowoff) after rapid throttle drop
volatile boolean dieselKnockTrigger = false;      // Trigger Diesel ignition "knock"
volatile boolean dieselKnockTriggerFirst = false; // The first Diesel ignition "knock" per sequence
volatile boolean airBrakeTrigger = false;         // Trigger for air brake noise
volatile boolean parkingBrakeTrigger = false;     // Trigger for air parking brake noise
volatile boolean shiftingTrigger = false;         // Trigger for shifting noise
volatile boolean hornTrigger = false;             // Trigger for horn on / off
volatile boolean sirenTrigger = false;            // Trigger for siren  on / off
volatile boolean sound1trigger = false;           // Trigger for sound1  on / off
volatile boolean couplingTrigger = false;         // Trigger for trailer coupling  sound
volatile boolean uncouplingTrigger = false;       // Trigger for trailer uncoupling  sound
volatile boolean bucketRattleTrigger = false;     // Trigger for bucket rattling  sound
volatile boolean indicatorSoundOn = false;        // active, if indicator bulb is on
volatile boolean outOfFuelMessageTrigger = false; // Trigger for out of fuel message

// Sound latches
volatile boolean hornLatch = false;  // Horn latch bit
volatile boolean sirenLatch = false; // Siren latch bit

// Sound volumes
volatile uint16_t throttleDependentVolume = 0;        // engine volume according to throttle position
volatile uint16_t throttleDependentRevVolume = 0;     // engine rev volume according to throttle position
volatile uint16_t rpmDependentJakeBrakeVolume = 0;    // Engine rpm dependent jake brake volume
volatile uint16_t throttleDependentKnockVolume = 0;   // engine Diesel knock volume according to throttle position
volatile uint16_t rpmDependentKnockVolume = 0;        // engine Diesel knock volume according to engine RPM
volatile uint16_t throttleDependentTurboVolume = 0;   // turbo volume according to rpm
volatile uint16_t throttleDependentFanVolume = 0;     // cooling fan volume according to rpm
volatile uint16_t throttleDependentChargerVolume = 0; // cooling fan volume according to rpm
volatile uint16_t rpmDependentWastegateVolume = 0;    // wastegate volume according to rpm
volatile uint16_t tireSquealVolume = 0;               // Tire squeal volume according to speed and cornering radius
// for excavator mode:
volatile uint16_t hydraulicPumpVolume = 0;             // hydraulic pump volume
volatile uint16_t hydraulicFlowVolume = 0;             // hydraulic flow volume
volatile uint16_t trackRattleVolume = 0;               // track rattling volume
volatile uint16_t hydraulicDependentKnockVolume = 100; // engine Diesel knock volume according to hydraulic load
volatile uint16_t hydraulicLoad = 0;                   // Hydraulic load dependent RPM drop

volatile uint64_t dacDebug = 0; // DAC debug variable TODO

volatile int16_t masterVolume = 100; // Master volume percentage
volatile uint8_t dacOffset = 0;      // 128, but needs to be ramped up slowly to prevent popping noise, if switched on

// Throttle
int16_t currentThrottle = 0;      // 0 - 500 (Throttle trigger input)
int16_t currentThrottleFaded = 0; // faded throttle for volume calculations etc.

// Engine
const int16_t maxRpm = 500;       // always 500
const int16_t minRpm = 0;         // always 0
int32_t currentRpm = 0;           // 0 - 500 (signed required!)
volatile uint8_t engineState = 0; // Engine state
enum EngineState                  // Engine state enum
{
  OFF,      // Engine is off
  STARTING, // Engine is starting
  RUNNING,  // Engine is running
  STOPPING, // Engine is stopping
  PARKING_BRAKE
};
int16_t engineLoad = 0;                 // 0 - 500
volatile uint16_t engineSampleRate = 0; // Engine sample rate
int32_t speedLimit = maxRpm;            // The speed limit, depending on selected virtual gear

// Clutch
boolean clutchDisengaged = true; // Active while clutch is disengaged

// Transmission
uint8_t selectedGear = 1;             // The currently used gear of our shifting gearbox
uint8_t selectedAutomaticGear = 1;    // The currently used gear of our automatic gearbox
boolean gearUpShiftingInProgress;     // Active while shifting upwards
boolean doubleClutchInProgress;       // Double-clutch (Zwischengas)
boolean gearDownShiftingInProgress;   // Active while shifting downwards
boolean gearUpShiftingPulse;          // Active, if shifting upwards begins
boolean gearDownShiftingPulse;        // Active, if shifting downwards begins
volatile boolean neutralGear = false; // Transmission in neutral
boolean lowRange = false;             // Transmission range (off road reducer)

// ESC
volatile boolean escIsBraking = false; // ESC is in a braking state
volatile boolean escIsDriving = false; // ESC is in a driving state
volatile boolean escInReverse = false; // ESC is driving or braking backwards
volatile boolean brakeDetect = false;  // Additional brake detect signal, enabled immediately, if brake applied
int8_t driveState = 0;                 // for ESC state machine
uint16_t escPulseMax = 2000;           // ESC calibration variables (values will be changed later)
uint16_t escPulseMin = 1000;
uint16_t escPulseMaxNeutral = 1500;
uint16_t escPulseMinNeutral = 1500;
uint16_t currentSpeed = 0;         // 0 - 500 (current ESC power)
volatile bool crawlerMode = false; // Crawler mode intended for crawling competitons (withouth sound and virtual inertia)

// Lights
int8_t lightsState = 0;                        // for lights state machine
volatile boolean lightsOn = false;             // Lights on
volatile boolean headLightsFlasherOn = false;  // Headlights flasher impulse (Lichthupe)
volatile boolean headLightsHighBeamOn = false; // Headlights high beam (Fernlicht)
volatile boolean blueLightTrigger = false;     // Bluelight on (Blaulicht)
boolean indicatorLon = false;                  // Left indicator (Blinker links)
boolean indicatorRon = false;                  // Right indicator (Blinker rechts)
boolean fogLightOn = false;                    // Fog light is on
boolean cannonFlash = false;                   // Flashing cannon fire

// Trailer
bool legsUp;
bool legsDown;
bool rampsUp;
bool rampsDown;
bool trailerDetected;

// Battery
float batteryCutoffvoltage;
float batteryVoltage;
uint8_t numberOfCells;
bool batteryProtection = false;

// ESP NOW variables for wireless trailer communication ----------------------------
#if defined ENABLE_WIRELESS

volatile uint16_t pollRate = 20;

esp_now_peer_info_t peerInfo; // This MUST be global!! Transmission is not working otherwise!

typedef struct struct_message
{ // This is the data packet
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
#endif // --------------------------------------------------------------------------

// The following variables are buffered in the eeprom an can be modified, using the web interface -----
// 5th wheel switch enable / disable
bool fifthWhweelDetectionActive = true;

bool useTrailer1;
bool useTrailer2;
bool useTrailer3;

uint8_t broadcastAddress1[6];
uint8_t broadcastAddress2[6];
uint8_t broadcastAddress3[6];

// Eeprom size and storage addresses ----------------------------------------------
#define EEPROM_SIZE 256 // 256 Bytes (512 is maximum)

#define adr_eprom_init 0 // Eeprom initialized or not?

#define adr_eprom_useTrailer1 4 // Trailer 1
#define adr_eprom_Trailer1Mac0 8
#define adr_eprom_Trailer1Mac1 12
#define adr_eprom_Trailer1Mac2 16
#define adr_eprom_Trailer1Mac3 20
#define adr_eprom_Trailer1Mac4 24
#define adr_eprom_Trailer1Mac5 28

#define adr_eprom_useTrailer2 32 // Trailer 2
#define adr_eprom_Trailer2Mac0 36
#define adr_eprom_Trailer2Mac1 40
#define adr_eprom_Trailer2Mac2 44
#define adr_eprom_Trailer2Mac3 48
#define adr_eprom_Trailer2Mac4 52
#define adr_eprom_Trailer2Mac5 56

#define adr_eprom_useTrailer3 60 // Trailer 3
#define adr_eprom_Trailer3Mac0 64
#define adr_eprom_Trailer3Mac1 68
#define adr_eprom_Trailer3Mac2 72
#define adr_eprom_Trailer3Mac3 76
#define adr_eprom_Trailer3Mac4 80
#define adr_eprom_Trailer3Mac5 84

#define adr_eprom_fifthWhweelDetectionActive 88
#define adr_eprom_tailLightBrightness 92
#define adr_eprom_sideLightBrightness 96
#define adr_eprom_reversingLightBrightness 100
#define adr_eprom_indicatorLightBrightness 104
#define adr_eprom_ssid 192     // 192
#define adr_eprom_password 224 // 224

// DEBUG stuff
volatile uint8_t coreId = 99;

// Our main tasks
TaskHandle_t Task1;

// Loop time (for debug)
uint16_t loopTime;

// Sampling intervals for interrupt timer (adjusted according to your sound file sampling rate)
uint32_t maxSampleInterval = 4000000 / sampleRate;
uint32_t minSampleInterval = 4000000 / sampleRate * 100 / MAX_RPM_PERCENTAGE;

// Interrupt timer for variable sample rate playback (engine sound)
hw_timer_t *variableTimer = NULL;
portMUX_TYPE variableTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t variableTimerTicks = maxSampleInterval;

// Interrupt timer for fixed sample rate playback (horn etc., playing in parallel with engine sound)
hw_timer_t *fixedTimer = NULL;
portMUX_TYPE fixedTimerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t fixedTimerTicks = maxSampleInterval;

// Declare a mutex Semaphore Handles.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xPwmSemaphore;
SemaphoreHandle_t xRpmSemaphore;

// These are used to print the reset reason on startup
const char *RESET_REASONS[] = {"POWERON_RESET", "NO_REASON", "SW_RESET", "OWDT_RESET", "DEEPSLEEP_RESET", "SDIO_RESET", "TG0WDT_SYS_RESET", "TG1WDT_SYS_RESET", "RTCWDT_SYS_RESET", "INTRUSION_RESET", "TGWDT_CPU_RESET", "SW_CPU_RESET", "RTCWDT_CPU_RESET", "EXT_CPU_RESET", "RTCWDT_BROWN_OUT_RESET", "RTCWDT_RTC_RESET"};

// Convert µs to degrees (°)
float us2degree(uint16_t value)
{
  return (value - 500) / 11.111 - 90.0;
}

//
// =======================================================================================================
// INTERRUPT FOR VARIABLE SPEED PLAYBACK (Engine sound, turbo sound)
// =======================================================================================================
//

void IRAM_ATTR variablePlaybackTimer()
{

  // coreId = xPortGetCoreID(); // Running on core 1

  static uint32_t attenuatorMillis = 0;
  static uint32_t curEngineSample = 0;          // Index of currently loaded engine sample
  static uint32_t curRevSample = 0;             // Index of currently loaded engine rev sample
  static uint32_t curTurboSample = 0;           // Index of currently loaded turbo sample
  static uint32_t curFanSample = 0;             // Index of currently loaded fan sample
  static uint32_t curChargerSample = 0;         // Index of currently loaded charger sample
  static uint32_t curStartSample = 0;           // Index of currently loaded start sample
  static uint32_t curJakeBrakeSample = 0;       // Index of currently loaded jake brake sample
  static uint32_t curHydraulicPumpSample = 0;   // Index of currently loaded hydraulic pump sample
  static uint32_t curTrackRattleSample = 0;     // Index of currently loaded train track rattle sample
  static uint32_t lastDieselKnockSample = 0;    // Index of last Diesel knock sample
  static uint16_t attenuator = 0;               // Used for volume adjustment during engine switch off
  static uint16_t speedPercentage = 0;          // slows the engine down during shutdown
  static int32_t a, a1, a2, a3, b, c, d, e = 0; // Input signals for mixer: a = engine, b = additional sound, c = turbo sound, d = fan sound, e = supercharger sound
  static int32_t f = 0;                         // Input signals for mixer: f = hydraulic pump
  static int32_t g = 0;                         // Input signals for mixer: g = train track rattle
  uint8_t a1Multi = 0;                          // Volume multipliers

  // portENTER_CRITICAL_ISR(&variableTimerMux); // disables C callable interrupts (on the current core) and locks the mutex by the current core.

  switch (engineState)
  {

  case OFF:                                                   // Engine off -----------------------------------------------------------------------
    variableTimerTicks = 4000000 / startSampleRate;           // our fixed sampling rate
    timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

    a = 0; // volume = zero
    if (engineOn)
    {
      engineState = STARTING;
      engineStart = true;
    }
    break;

  case STARTING:                                              // Engine start --------------------------------------------------------------------
    variableTimerTicks = 4000000 / startSampleRate;           // our fixed sampling rate
    timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

    if (curStartSample < startSampleCount - 1)
    {
#if defined STEAM_LOCOMOTIVE_MODE
      a = (startSamples[curStartSample] * startVolumePercentage / 100);
#else
      a = (startSamples[curStartSample] * throttleDependentVolume / 100 * startVolumePercentage / 100);
#endif
      curStartSample++;
    }
    else
    {
      curStartSample = 0;
      engineState = RUNNING;
      engineStart = false;
      engineRunning = true;
      airBrakeTrigger = true;
    }
    break;

  case RUNNING: // Engine running ------------------------------------------------------------------

    // Engine idle & revving sounds (mixed together according to engine rpm, new in v5.0)
    variableTimerTicks = engineSampleRate;                    // our variable idle sampling rate!
    timerAlarmWrite(variableTimer, variableTimerTicks, true); // // change timer ticks, autoreload true

    if (!engineJakeBraking && !blowoffTrigger)
    {
      if (curEngineSample < sampleCount - 1)
      {
        a1 = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100); // Idle sound
        a3 = 0;
        curEngineSample++;

        // Optional rev sound, recorded at medium rpm. Note, that it needs to represent the same number of ignition cycles as the
        // idle sound. For example 4 or 8 for a V8 engine. It also needs to have about the same length. In order to adjust the length
        // or "revSampleCount", change the "Rate" setting in Audacity until it is about the same.
#ifdef REV_SOUND
        a2 = (revSamples[curRevSample] * throttleDependentRevVolume / 100 * revVolumePercentage / 100); // Rev sound
        if (curRevSample < revSampleCount)
          curRevSample++;
#endif

        // Trigger throttle dependent Diesel ignition "knock" sound (played in the fixed sample rate interrupt)
        if (curEngineSample - lastDieselKnockSample > (sampleCount / dieselKnockInterval))
        {
          dieselKnockTrigger = true;
          dieselKnockTriggerFirst = false;
          lastDieselKnockSample = curEngineSample;
        }
      }
      else
      {
        curEngineSample = 0;
        if (jakeBrakeRequest)
          engineJakeBraking = true;
#ifdef REV_SOUND
        curRevSample = 0;
#endif
        lastDieselKnockSample = 0;
        dieselKnockTrigger = true;
        dieselKnockTriggerFirst = true;
      }
      curJakeBrakeSample = 0;
    }
    else
    { // Jake brake sound ----
#ifdef JAKE_BRAKE_SOUND
      a3 = (jakeBrakeSamples[curJakeBrakeSample] * rpmDependentJakeBrakeVolume / 100 * jakeBrakeVolumePercentage / 100); // Jake brake sound
      a2 = 0;
      a1 = 0;
      if (curJakeBrakeSample < jakeBrakeSampleCount - 1)
        curJakeBrakeSample++;
      else
      {
        curJakeBrakeSample = 0;
        if (!jakeBrakeRequest)
          engineJakeBraking = false;
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

    if (currentRpm > revSwitchPoint)
      a1Multi = map(currentRpm, idleEndPoint, revSwitchPoint, 0, idleVolumeProportionPercentage);
    else
      a1Multi = idleVolumeProportionPercentage; // 90 - 100% proportion
    if (currentRpm > idleEndPoint)
      a1Multi = 0;

    a1 = a1 * a1Multi / 100;         // Idle volume
    a2 = a2 * (100 - a1Multi) / 100; // Rev volume

    a = a1 + a2 + a3; // Idle and rev sounds mixed together
#else
    a = a1 + a3; // Idle sound only
#endif

    // Turbo sound ----------------------------------
    if (curTurboSample < turboSampleCount - 1)
    {
      c = (turboSamples[curTurboSample] * throttleDependentTurboVolume / 100 * turboVolumePercentage / 100);
      curTurboSample++;
    }
    else
    {
      curTurboSample = 0;
    }

    // Fan sound -----------------------------------
    if (curFanSample < fanSampleCount - 1)
    {
      d = (fanSamples[curFanSample] * throttleDependentFanVolume / 100 * fanVolumePercentage / 100);
      curFanSample++;
    }
    else
    {
      curFanSample = 0;
    }
#if defined GEARBOX_WHINING
    if (neutralGear)
      d = 0; // used for gearbox whining simulation, so not active in gearbox neutral
#endif

    // Supercharger sound --------------------------
    if (curChargerSample < chargerSampleCount - 1)
    {
      e = (chargerSamples[curChargerSample] * throttleDependentChargerVolume / 100 * chargerVolumePercentage / 100);
      curChargerSample++;
    }
    else
    {
      curChargerSample = 0;
    }

    // Hydraulic pump sound -----------------------
#if defined EXCAVATOR_MODE
    if (curHydraulicPumpSample < hydraulicPumpSampleCount - 1)
    {
      f = (hydraulicPumpSamples[curHydraulicPumpSample] * hydraulicPumpVolumePercentage / 100 * hydraulicPumpVolume / 100);
      curHydraulicPumpSample++;
    }
    else
    {
      curHydraulicPumpSample = 0;
    }
#endif

#if defined STEAM_LOCOMOTIVE_MODE
    // Track rattle sound -----------------------
    if (curTrackRattleSample < trackRattleSampleCount - 1)
    {
      g = (trackRattleSamples[curTrackRattleSample] * trackRattleVolumePercentage / 100 * trackRattleVolume / 100);
      curTrackRattleSample++;
    }
    else
    {
      curTrackRattleSample = 0;
    }
#endif

    if (!engineOn)
    {
      speedPercentage = 100;
      attenuator = 1;
      engineState = STOPPING;
      engineStop = true;
      engineRunning = false;
    }
    break;

  case STOPPING:                                                       // Engine stop --------------------------------------------------------------------
    variableTimerTicks = 4000000 / sampleRate * speedPercentage / 100; // our fixed sampling rate
    timerAlarmWrite(variableTimer, variableTimerTicks, true);          // // change timer ticks, autoreload true

    if (curEngineSample < sampleCount - 1)
    {
      a = (samples[curEngineSample] * throttleDependentVolume / 100 * idleVolumePercentage / 100 / attenuator);
      curEngineSample++;
    }
    else
    {
      curEngineSample = 0;
    }

    // fade engine sound out
    if (millis() - attenuatorMillis > 100)
    { // Every 50ms
      attenuatorMillis = millis();
      attenuator++;          // attenuate volume
      speedPercentage += 20; // make it slower (10)
    }

    if (attenuator >= 50 || speedPercentage >= 500)
    { // 50 & 500
      a = 0;
      speedPercentage = 100;
      parkingBrakeTrigger = true;
      engineState = PARKING_BRAKE;
      engineStop = false;
    }
    break;

  case PARKING_BRAKE: // parking brake bleeding air sound after engine is off ----------------------------

    if (!parkingBrakeTrigger)
    {
      engineState = OFF;
    }
    break;

  } // end of switch case

  // DAC output (groups a, b, c mixed together) ************************************************************************

  uint8_t value = constrain(((a * 8 / 10) + (b / 2) + (c / 5) + (d / 5) + (e / 5) + f + g) * masterVolume / 100 + dacOffset, 0, 255);
  SET_PERI_REG_BITS(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_DAC, value, RTC_IO_PDAC1_DAC_S);

  // portEXIT_CRITICAL_ISR(&variableTimerMux);
}

//
// =======================================================================================================
// INTERRUPT FOR FIXED SPEED PLAYBACK (Horn etc., played in parallel with engine sound)
// =======================================================================================================
//

void IRAM_ATTR fixedPlaybackTimer()
{

  // coreId = xPortGetCoreID(); // Running on core 1

  static uint32_t curHornSample = 0;                            // Index of currently loaded horn sample
  static uint32_t curSirenSample = 0;                           // Index of currently loaded siren sample
  static uint32_t curSound1Sample = 0;                          // Index of currently loaded sound1 sample
  static uint32_t curReversingSample = 0;                       // Index of currently loaded reversing beep sample
  static uint32_t curIndicatorSample = 0;                       // Index of currently loaded indicator tick sample
  static uint32_t curWastegateSample = 0;                       // Index of currently loaded wastegate sample
  static uint32_t curBrakeSample = 0;                           // Index of currently loaded brake sound sample
  static uint32_t curParkingBrakeSample = 0;                    // Index of currently loaded brake sound sample
  static uint32_t curShiftingSample = 0;                        // Index of currently loaded shifting sample
  static uint32_t curDieselKnockSample = 0;                     // Index of currently loaded Diesel knock sample
  static uint32_t curCouplingSample = 0;                        // Index of currently loaded trailer coupling sample
  static uint32_t curUncouplingSample = 0;                      // Index of currently loaded trailer uncoupling sample
  static uint32_t curHydraulicFlowSample = 0;                   // Index of currently loaded hydraulic flow sample
  static uint32_t curTrackRattleSample = 0;                     // Index of currently loaded track rattle sample
  static uint32_t curBucketRattleSample = 0;                    // Index of currently loaded bucket rattle sample
  static uint32_t curTireSquealSample = 0;                      // Index of currently loaded tire squeal sample
  static uint32_t curOutOfFuelSample = 0;                       // Index of currently loaded out of fuel sample
  static int32_t a, a1, a2 = 0;                                 // Input signals "a" for mixer
  static int32_t b, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 = 0; // Input signals "b" for mixer
  static int32_t c, c1, c2, c3 = 0;                             // Input signals "c" for mixer
  static int32_t d, d1, d2 = 0;                                 // Input signals "d" for mixer
  static boolean knockSilent = 0;                               // This knock will be more silent
  static boolean knockMedium = 0;                               // This knock will be medium
  static uint8_t curKnockCylinder = 0;                          // Index of currently ignited zylinder

  // portENTER_CRITICAL_ISR(&fixedTimerMux);

  // Group "a" (horn & siren) ******************************************************************

  if (hornTrigger || hornLatch)
  {
    fixedTimerTicks = 4000000 / hornSampleRate;         // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curHornSample < hornSampleCount - 1)
    {
      a1 = (hornSamples[curHornSample] * hornVolumePercentage / 100);
      curHornSample++;
#ifdef HORN_LOOP // Optional "endless loop" (points to be defined manually in horn file)
      if (hornTrigger && curHornSample == hornLoopEnd)
        curHornSample = hornLoopBegin; // Loop, if trigger still present
#endif
    }
    else
    { // End of sample
      curHornSample = 0;
      a1 = 0;
      hornLatch = false;
    }
  }

  if (sirenTrigger || sirenLatch)
  {
    fixedTimerTicks = 4000000 / sirenSampleRate;        // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

#if defined SIREN_STOP
    if (!sirenTrigger)
    {
      sirenLatch = false;
      curSirenSample = 0;
      a2 = 0;
    }
#endif

    if (curSirenSample < sirenSampleCount - 1)
    {
      a2 = (sirenSamples[curSirenSample] * sirenVolumePercentage / 100);
      curSirenSample++;
#ifdef SIREN_LOOP // Optional "endless loop" (points to be defined manually in siren file)
      if (sirenTrigger && curSirenSample == sirenLoopEnd)
        curSirenSample = sirenLoopBegin; // Loop, if trigger still present
#endif
    }
    else
    { // End of sample
      curSirenSample = 0;
      a2 = 0;
      sirenLatch = false;
    }
  }
  if (curSirenSample > 10 && curSirenSample < 500)
    cannonFlash = true; // Tank cannon flash triggering in TRACKED_MODE
  else
    cannonFlash = false;

  // Group "b" (other sounds) **********************************************************************

  // Sound 1 "b0" ----
  if (sound1trigger)
  {
    fixedTimerTicks = 4000000 / sound1SampleRate;       // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curSound1Sample < sound1SampleCount - 1)
    {
      b0 = (sound1Samples[curSound1Sample] * sound1VolumePercentage / 100);
      curSound1Sample++;
    }
    else
    {
      sound1trigger = false;
    }
  }
  else
  {
    curSound1Sample = 0; // ensure, next sound will start @ first sample
    b0 = 0;
  }

  // Reversing beep sound "b1" ----
  if (engineRunning && escInReverse)
  {
    fixedTimerTicks = 4000000 / reversingSampleRate;    // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curReversingSample < reversingSampleCount - 1)
    {
      b1 = (reversingSamples[curReversingSample] * reversingVolumePercentage / 100);
      curReversingSample++;
    }
    else
    {
      curReversingSample = 0;
    }
  }
  else
  {
    curReversingSample = 0; // ensure, next sound will start @ first sample
    b1 = 0;
  }

  // Indicator tick sound "b2" ----------------------------------------------------------------------
#if not defined NO_INDICATOR_SOUND
  if (indicatorSoundOn)
  {
    fixedTimerTicks = 4000000 / indicatorSampleRate;    // our fixed sampling rate
    timerAlarmWrite(fixedTimer, fixedTimerTicks, true); // // change timer ticks, autoreload true

    if (curIndicatorSample < indicatorSampleCount - 1)
    {
      b2 = (indicatorSamples[curIndicatorSample] * indicatorVolumePercentage / 100);
      curIndicatorSample++;
    }
    else
    {
      indicatorSoundOn = false;
    }
  }
  else
  {
    curIndicatorSample = 0; // ensure, next sound will start @ first sample
    b2 = 0;
  }
#endif

  // Wastegate (blowoff) sound, triggered after rapid throttle drop -----------------------------------
  if (wastegateTrigger)
  {
    if (curWastegateSample < wastegateSampleCount - 1)
    {
      b3 = (wastegateSamples[curWastegateSample] * rpmDependentWastegateVolume / 100 * wastegateVolumePercentage / 100);
      curWastegateSample++;
    }
    else
    {
      wastegateTrigger = false;
    }
  }
  else
  {
    b3 = 0;
    curWastegateSample = 0; // ensure, next sound will start @ first sample
  }

  // Air brake release sound, triggered after stop -----------------------------------------------
  if (airBrakeTrigger)
  {
    if (curBrakeSample < brakeSampleCount - 1)
    {
      b4 = (brakeSamples[curBrakeSample] * brakeVolumePercentage / 100);
      curBrakeSample++;
    }
    else
    {
      airBrakeTrigger = false;
    }
  }
  else
  {
    b4 = 0;
    curBrakeSample = 0; // ensure, next sound will start @ first sample
  }

  // Air parking brake attaching sound, triggered after engine off --------------------------------
  if (parkingBrakeTrigger)
  {
    if (curParkingBrakeSample < parkingBrakeSampleCount - 1)
    {
      b5 = (parkingBrakeSamples[curParkingBrakeSample] * parkingBrakeVolumePercentage / 100);
      curParkingBrakeSample++;
    }
    else
    {
      parkingBrakeTrigger = false;
    }
  }
  else
  {
    b5 = 0;
    curParkingBrakeSample = 0; // ensure, next sound will start @ first sample
  }

  // Pneumatic gear shifting sound, triggered while shifting the TAMIYA 3 speed transmission ------
  if (shiftingTrigger && engineRunning && !automatic && !doubleClutch)
  {
    if (curShiftingSample < shiftingSampleCount - 1)
    {
      b6 = (shiftingSamples[curShiftingSample] * shiftingVolumePercentage / 100);
      curShiftingSample++;
    }
    else
    {
      shiftingTrigger = false;
    }
  }
  else
  {
    b6 = 0;
    curShiftingSample = 0; // ensure, next sound will start @ first sample
  }

  // Diesel ignition "knock" is played in fixed sample rate section, because we don't want changing pitch! ------
  if (dieselKnockTriggerFirst)
  {
    dieselKnockTriggerFirst = false;
    curKnockCylinder = 0;
  }

  if (dieselKnockTrigger)
  {
    dieselKnockTrigger = false;
    curKnockCylinder++; // Count ignition sequence
    curDieselKnockSample = 0;
  }

#ifdef V8 // (former ADAPTIVE_KNOCK_VOLUME, rename it in your config file!)
  // Ford or Scania V8 ignition sequence: 1 - 5 - 4 - 2* - 6 - 3 - 7 - 8* (* = louder knock pulses, because 2nd exhaust in same manifold after 90°)
  if (curKnockCylinder == 4 || curKnockCylinder == 8)
    knockSilent = false;
  else
    knockSilent = true;
#endif

#ifdef V8_MEDIUM // (former ADAPTIVE_KNOCK_VOLUME, rename it in your config file!)
  // This is EXPERIMENTAL!! TODO
  if (curKnockCylinder == 5 || curKnockCylinder == 1)
    knockMedium = false;
  else
    knockMedium = true;
#endif

#ifdef V8_468 // (Chevy 468, containing 16 ignition pulses)
  // 1th, 5th, 9th and 13th are the loudest
  // Ignition sequence: 1 - 8 - 4* - 3 - 6 - 5 - 7* - 2
  if (curKnockCylinder == 1 || curKnockCylinder == 5 || curKnockCylinder == 9 || curKnockCylinder == 13)
    knockSilent = false;
  else
    knockSilent = true;
#endif

#ifdef V2
  // V2 engine: 1st and 2nd knock pulses (of 4) will be louder
  if (curKnockCylinder == 1 || curKnockCylinder == 2)
    knockSilent = false;
  else
    knockSilent = true;
#endif

#ifdef R6
  // R6 inline 6 engine: 6th knock pulse (of 6) will be louder
  if (curKnockCylinder == 6)
    knockSilent = false;
  else
    knockSilent = true;
#endif

#ifdef R6_2
  // R6 inline 6 engine: 6th and 3rd knock pulse (of 6) will be louder
  if (curKnockCylinder == 6 || curKnockCylinder == 3)
    knockSilent = false;
  else
    knockSilent = true;
#endif

  if (curDieselKnockSample < knockSampleCount)
  {
#if defined RPM_DEPENDENT_KNOCK // knock volume also depending on engine rpm
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100 * rpmDependentKnockVolume / 100);
#elif defined EXCAVATOR_MODE // knock volume also depending on hydraulic load
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100 * hydraulicDependentKnockVolume / 100);
#else                        // Just depending on throttle
    b7 = (knockSamples[curDieselKnockSample] * dieselKnockVolumePercentage / 100 * throttleDependentKnockVolume / 100);
#endif
    curDieselKnockSample++;
    if (knockSilent && !knockMedium)
      b7 = b7 * dieselKnockAdaptiveVolumePercentage / 100; // changing knock volume according to engine type and cylinder!
    if (knockMedium)
      b7 = b7 * dieselKnockAdaptiveVolumePercentage / 75;
  }

#if not defined EXCAVATOR_MODE
  // Trailer coupling sound, triggered by switch -----------------------------------------------
#ifdef COUPLING_SOUND
  if (couplingTrigger)
  {
    if (curCouplingSample < couplingSampleCount - 1)
    {
      b8 = (couplingSamples[curCouplingSample] * couplingVolumePercentage / 100);
      curCouplingSample++;
    }
    else
    {
      couplingTrigger = false;
    }
  }
  else
  {
    b8 = 0;
    curCouplingSample = 0; // ensure, next sound will start @ first sample
  }

  // Trailer uncoupling sound, triggered by switch -----------------------------------------------
  if (uncouplingTrigger)
  {
    if (curUncouplingSample < uncouplingSampleCount - 1)
    {
      b9 = (uncouplingSamples[curUncouplingSample] * couplingVolumePercentage / 100);
      curUncouplingSample++;
    }
    else
    {
      uncouplingTrigger = false;
    }
  }
  else
  {
    b9 = 0;
    curUncouplingSample = 0; // ensure, next sound will start @ first sample
  }
#endif
#endif

  // Group "c" (excavator sounds) **********************************************************************

#if defined EXCAVATOR_MODE

  // Hydraulic fluid flow sound -----------------------
  if (curHydraulicFlowSample < hydraulicFlowSampleCount - 1)
  {
    c1 = (hydraulicFlowSamples[curHydraulicFlowSample] * hydraulicFlowVolumePercentage / 100 * hydraulicFlowVolume / 100);
    curHydraulicFlowSample++;
  }
  else
  {
    curHydraulicFlowSample = 0;
  }

  // Track rattle sound -----------------------
  if (curTrackRattleSample < trackRattleSampleCount - 1)
  {
    c2 = (trackRattleSamples[curTrackRattleSample] * trackRattleVolumePercentage / 100 * trackRattleVolume / 100);
    curTrackRattleSample++;
  }
  else
  {
    curTrackRattleSample = 0;
  }

  // Bucket rattle sound -----------------------
  if (bucketRattleTrigger)
  {
    if (curBucketRattleSample < bucketRattleSampleCount - 1)
    {
      c3 = (bucketRattleSamples[curBucketRattleSample] * bucketRattleVolumePercentage / 100);
      curBucketRattleSample++;
    }
    else
    {
      bucketRattleTrigger = false;
    }
  }
  else
  {
    c3 = 0;
    curBucketRattleSample = 0; // ensure, next sound will start @ first sample
  }
#endif

  // Group "d" (additional sounds) **********************************************************************

#if defined TIRE_SQUEAL
  // Tire squeal sound -----------------------
  if (curTireSquealSample < tireSquealSampleCount - 1)
  {
    d1 = (tireSquealSamples[curTireSquealSample] * tireSquealVolumePercentage / 100 * tireSquealVolume / 100);
    curTireSquealSample++;
  }
  else
  {
    d1 = 0;
    curTireSquealSample = 0;
  }
#endif

#if defined BATTERY_PROTECTION
  // Out of fuel sound, triggered by battery voltage -----------------------------------------------
  if (outOfFuelMessageTrigger)
  {
    if (curOutOfFuelSample < outOfFuelSampleCount - 1)
    {
      d2 = (outOfFuelSamples[curOutOfFuelSample] * outOfFuelVolumePercentage / 100);
      curOutOfFuelSample++;
    }
    else
    {
      outOfFuelMessageTrigger = false;
    }
  }
  else
  {
    d2 = 0;
    curOutOfFuelSample = 0; // ensure, next sound will start @ first sample
  }
#endif

  // Mixing sounds together **********************************************************************
  a = a1 + a2; // Horn & siren
  // if (a < 2 && a > -2) a = 0; // Remove noise floor TODO, experimental
  b = b0 * 5 + b1 + b2 / 2 + b3 + b4 + b5 + b6 + b7 + b8 + b9; // Other sounds
  c = c1 + c2 + c3;                                            // Excavator sounds
  d = d1 + d2;                                                 // Additional sounds

  // DAC output (groups mixed together) ****************************************************************************

  uint8_t value = constrain(((a * 8 / 10) + (b * 2 / 10) + c + d) * masterVolume / 100 + dacOffset, 0, 255);
  SET_PERI_REG_BITS(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_DAC, value, RTC_IO_PDAC2_DAC_S);

  // portEXIT_CRITICAL_ISR(&fixedTimerMux);
}

//
// =======================================================================================================
// PWM SIGNAL READ INTERRUPT
// =======================================================================================================
//

// Reference https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html?highlight=rmt
static void IRAM_ATTR rmt_isr_handler(void *arg)
{

  uint32_t intr_st = RMT.int_st.val;

  static uint32_t lastFrameTime = millis();

  if (millis() - lastFrameTime > 20)
  { // Only do it every 20ms (very important for system stability)

    // See if we can obtain or "Take" the Semaphore.
    // If the semaphore is not available, wait 1 ticks of the Scheduler to see if it becomes free.
    if (xSemaphoreTake(xPwmSemaphore, portMAX_DELAY))
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the pwmBuf variable for us alone,
      // so we don't want it getting stolen during the middle of a conversion.

      uint8_t i;
      for (i = 0; i < PWM_CHANNELS_NUM; i++)
      {
        uint8_t channel = PWM_CHANNELS[i];
        uint32_t channel_mask = BIT(channel * 3 + 1);

        if (!(intr_st & channel_mask))
          continue;

        RMT.conf_ch[channel].conf1.rx_en = 0;
        RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
        volatile rmt_item32_t *item = RMTMEM.chan[channel].data32;

        if (item)
        {
          pwmBuf[i + 1] = item->duration0; // pointer -> variable
        }

        RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
        RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_RX;
        RMT.conf_ch[channel].conf1.rx_en = 1;

        // clear RMT interrupt status.
        RMT.int_clr.val = channel_mask;
      }

      xSemaphoreGive(xPwmSemaphore); // Now free or "Give" the semaphore for others.
    }
    lastFrameTime = millis();
  }
  else
  {
    xSemaphoreGive(xPwmSemaphore); // Free or "Give" the semaphore for others, if not required!
  }
}

//
// =======================================================================================================
// PPM SIGNAL READ INTERRUPT
// =======================================================================================================
//

void IRAM_ATTR readPpm()
{
  unsigned long timenew = micros();
  unsigned long timediff = timenew - timelast;
  timelast = timenew;

  if (timediff > 2500)
  {                                                             // Synch gap detected:
    ppmInp[NUM_OF_PPM_CHL] = ppmInp[NUM_OF_PPM_CHL] + timediff; // add time
    counter = 0;
    if (average == NUM_OF_PPM_AVG)
    {
      for (int i = 0; i < NUM_OF_PPM_CHL + 1; i++)
      {
        ppmBuf[i] = ppmInp[i] / average;
        ppmInp[i] = 0;
      }
      average = 0;
      ready = true;
    }
    average++;
  }
  else
  {
    if (counter < NUM_OF_PPM_CHL)
    {
      ppmInp[counter] = ppmInp[counter] + timediff;
      counter++;
    }
  }
}

//
// =======================================================================================================
// TRAILER PRESENCE SWITCH INTERRUPT (not usable with third brake light or RZ7886 motor driver)
// =======================================================================================================
//
#if not defined THIRD_BRAKELIGHT and not defined RZ7886_DRIVER_MODE
void IRAM_ATTR trailerPresenceSwitchInterrupt()
{
  couplerSwitchInteruptLatch = true;
}
#endif

//
// =======================================================================================================
// ESP NOW TRAILER DATA SENT CALLBACK
// =======================================================================================================
//

// callback when data is sent
void IRAM_ATTR onTrailerDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{

#ifdef ESPNOW_DEBUG

  // This will confirm, if data was received by peer and which one
  Serial.printf("ESP-NOW data received by peer (trailer) %02x:%02x:%02x:%02x:%02x:%02x : %s\n",
                (unsigned char)mac_addr[0],
                (unsigned char)mac_addr[1],
                (unsigned char)mac_addr[2],
                (unsigned char)mac_addr[3],
                (unsigned char)mac_addr[4],
                (unsigned char)mac_addr[5],
                ESP_NOW_SEND_SUCCESS == status ? "OK" : "FAILED");

  // pollRate = ESP_NOW_SEND_SUCCESS ? 20 : 100; // TODO

#endif
}

//
// =======================================================================================================
// mcpwm unit 0 SETUP for servos (1x during startup)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void setupMcpwm()
{
  // 1. set our servo output pins
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, STEERING_PIN); // Set steering as PWM0A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, SHIFTING_PIN); // Set shifting as PWM0B
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, COUPLER_PIN);  // Set coupling as PWM1A
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, WINCH_PIN);    // Set winch  or beacon as PWM1B

  // 2. configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = SERVO_FREQUENCY; // frequency usually = 50Hz, some servos may run smoother @ 100Hz
  pwm_config.cmpr_a = 0;                  // duty cycle of PWMxa = 0
  pwm_config.cmpr_b = 0;                  // duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // 0 = not inverted, 1 = inverted

  // 3. configure channels with settings above
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); // Configure PWM0A & PWM0B
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config); // Configure PWM1A & PWM1B
}

//
// =======================================================================================================
// mcpwm unit 1 SETUP for ESC (1x during startup)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void setupMcpwmESC()
{
#if not defined RZ7886_DRIVER_MODE // Setup for classic crawler style RC ESC ----
  Serial.printf("Standard ESC mode configured. Connect crawler ESC to ESC header. RZ7886 motor driver not usable!\n");

  // 1. set our ESC output pin
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, ESC_OUT_PIN); // Set ESC as PWM0A

  // 2. configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 50; // frequency always 50Hz
  pwm_config.cmpr_a = 0;     // duty cycle of PWMxa = 0
  pwm_config.cmpr_b = 0;     // duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // 0 = not inverted, 1 = inverted

  // 3. configure channels with settings above
  mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config); // Configure PWM0A & PWM0B

#else // Setup for RZ7886 motor driver ----
  Serial.printf("RZ7886 motor driver mode configured. Don't connect ESC to ESC header!\n");

  // 1. set our ESC output pin
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, RZ7886_PIN1); // Set RZ7886 pin 1 as PWM0A
  mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, RZ7886_PIN2); // Set RZ7886 pin 2 as PWM0B

  // 2. configure MCPWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = RZ7886_FREQUENCY; // frequency
  pwm_config.cmpr_a = 0;                   // duty cycle of PWMxa = 0
  pwm_config.cmpr_b = 0;                   // duty cycle of PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0; // 0 = not inverted, 1 = inverted

  // 3. configure channels with settings above
  mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config); // Configure PWM0A & PWM0B
#endif
  Serial.printf("-------------------------------------\n");
}

//
// =======================================================================================================
// ESP NOW SETUP FOR WIRELESS TRAILER CONTROL
// =======================================================================================================
//

void setupEspNow()
{

#if defined ENABLE_WIRELESS
  Serial.printf("ENABLE_WIRELESS option enabled\n");
  // Serial.printf("Sound controller MAC address: %s\n", WiFi.macAddress().c_str());
  //  Set device as a Wi-Fi Station for ESP-NOW
  WiFi.mode(WIFI_STA); // WIFI_STA = Station

  // Set IP address
  IPAddress IP = WiFi.softAPIP();

  Serial.printf("\nInformations for web configuration via your cell phone or computer *******************************\n");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("IP address: ");
  Serial.println(IP);

  // shut down wifi
  WiFi.disconnect();

  // Start access point
  WiFi.softAP(ssid.c_str(), password.c_str());

  Serial.printf("\nWiFi Tx Power Level: %u", WiFi.getTxPower());
  WiFi.setTxPower(cpType); // WiFi and ESP-Now power according to "0_generalSettings.h"
  Serial.printf("\nWiFi Tx Power Level changed to: %u\n\n", WiFi.getTxPower());

  server.begin(); // Start Webserver

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.printf("Error initializing ESP-NOW!\n");
    return;
  }

  // Register callback function to call when data was sent (used only for debug for now)
  esp_now_register_send_cb(onTrailerDataSent); // TODO, optional

  // Register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  Serial.printf("The following trailers are currently enabled:\n");
  // Add peer 1 (1st trailer)
  if (useTrailer1)
  {
    memcpy(peerInfo.peer_addr, broadcastAddress1, 6); // TODO!
    Serial.printf("Trailer 1 MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", broadcastAddress1[0], broadcastAddress1[1], broadcastAddress1[2], broadcastAddress1[3], broadcastAddress1[4], broadcastAddress1[5]);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.printf("Failed to add peer #1 (1st trailer)\n");
      return;
    }
  }

  // Add peer 2 (2nd trailer)
  if (useTrailer2)
  {
    memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
    Serial.printf("Trailer 2 MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", broadcastAddress2[0], broadcastAddress2[1], broadcastAddress2[2], broadcastAddress2[3], broadcastAddress2[4], broadcastAddress2[5]);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.printf("Failed to add peer #2 (2nd trailer)\n");
      return;
    }
  }

  // Add peer 3 (3rd trailer)
  if (useTrailer3)
  {
    memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
    Serial.printf("Trailer 3 MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", broadcastAddress3[0], broadcastAddress3[1], broadcastAddress3[2], broadcastAddress3[3], broadcastAddress3[4], broadcastAddress3[5]);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.printf("Failed to add peer #3 (3rd trailer)\n");
      return;
    }
  }
#else
  Serial.printf("ENABLE_WIRELESS option disabled, no WiFi configuration or ESP-Now Trailer!\n");
#endif // ENABLE_WIRELESS
  Serial.printf("-------------------------------------\n");
}

//
// =======================================================================================================
// BATTERY SETUP
// =======================================================================================================
//

void setupBattery()
{
#if defined BATTERY_PROTECTION

  Serial.printf("Battery voltage: %.2f V\n", batteryVolts());
  Serial.printf("Cutoff voltage per cell: %.2f V\n", CUTOFF_VOLTAGE);
  Serial.printf("Fully charged voltage per cell: %.2f V\n", FULLY_CHARGED_VOLTAGE);

#define CELL_SETPOINT (CUTOFF_VOLTAGE - ((FULLY_CHARGED_VOLTAGE - CUTOFF_VOLTAGE) / 2))

  if (batteryVolts() <= CELL_SETPOINT * 2)
    numberOfCells = 1;
  if (batteryVolts() > CELL_SETPOINT * 2)
    numberOfCells = 2;
  if (batteryVolts() > CELL_SETPOINT * 3)
    numberOfCells = 3;
  if (batteryVolts() > FULLY_CHARGED_VOLTAGE * 3)
    numberOfCells = 4;
  batteryCutoffvoltage = CUTOFF_VOLTAGE * numberOfCells; // Calculate cutoff voltage for battery protection
  if (numberOfCells > 1 && numberOfCells < 4)
  { // Only 2S & 3S batteries are supported!
    Serial.printf("Number of cells: %i (%iS battery detected) Based on setpoint: %.2f V\n", numberOfCells, numberOfCells, (CELL_SETPOINT * numberOfCells));
    Serial.printf("Battery cutoff voltage: %.2f V (%i * %.2f V) \n", batteryCutoffvoltage, numberOfCells, CUTOFF_VOLTAGE);
    for (uint8_t beeps = 0; beeps < numberOfCells; beeps++)
    { // Number of beeps = number of cells in series
      tone(26, 3000, 4, 0);
      // tone(26, 3000, 4); // For platform = espressif32@4.3.0
      delay(200);
    }
  }
  else
  {
    Serial.printf("Error, no valid battery detected! Only 2S & 3S batteries are supported!\n");
    Serial.printf("REMOVE BATTERY, CONTROLLER IS LOCKED = 2 FAST FLASHES!\n");
    bool locked = true;
    for (uint8_t beeps = 0; beeps < 10; beeps++)
    { // Number of beeps = number of cells in series
      tone(26, 3000, 4, 0);
      // tone(26, 3000, 4); // For platform = espressif32@4.3.0
      delay(30);
    }
    while (locked)
    {
      // wait here forever!
      indicatorL.flash(70, 75, 500, 2); // Show 2 fast flashes on indicators!
      indicatorR.flash(70, 75, 500, 2);
      rtc_wdt_feed(); // Feed watchdog timer
    }
  }
#else
  Serial.printf("Warning, BATTERY_PROTECTION disabled! ESC with low discharge protection required!\n");
#endif
  Serial.printf("-------------------------------------\n");
}

//
// =======================================================================================================
// EEPROM SETUP
// =======================================================================================================
//

void setupEeprom()
{
  EEPROM.begin(EEPROM_SIZE);
#if defined ERASE_EEPROM_ON_BOOT
  eepromErase(); // uncomment this option, if you want to erase all stored settings!
#endif
  eepromInit(); // Init new board with default values
  eepromRead(); // Read settings from Eeprom
  Serial.print("current eeprom_id: ");
  Serial.println(EEPROM.read(adr_eprom_init));
  Serial.println("change it for default value upload!\n");
  eepromDebugRead(); // Shows content of entire eeprom, except of empty areas
}

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup()
{
  // Watchdog timers need to be disabled, if task 1 is running without delay(1)
  disableCore0WDT();
  // disableCore1WDT(); // Core 1 WDT can stay enabled TODO

  // Setup RTC (Real Time Clock) watchdog
  rtc_wdt_protect_off(); // Disable RTC WDT write protection
  rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
  rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_SYSTEM);
  rtc_wdt_set_time(RTC_WDT_STAGE0, 10000); // set 10s timeout
  rtc_wdt_enable();                        // Start the RTC WDT timer
  // rtc_wdt_disable();            // Disable the RTC WDT timer
  rtc_wdt_protect_on(); // Enable RTC WDT write protection

  // Serial setup
  Serial.begin(115200, SERIAL_8N1, DEBUG_RX, DEBUG_TX); // USB serial (for DEBUG) Mode, Rx pin (99 = not used), Tx pin

  // ADC setup
  battery.attach(BATTERY_DETECT_PIN);

  // Print some system and software info to serial monitor
  delay(1000); // Give serial port/connection some time to get ready
  Serial.printf("\n**************************************************************************************************\n");
  Serial.printf("TheDIYGuy999 RC engine sound & light controller for ESP32 software version %s\n", codeVersion);
  Serial.printf("https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32\n");
  Serial.printf("Please read carefully: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/README.md\n");
  Serial.printf("XTAL Frequency: %i MHz, CPU Clock: %i MHz, APB Bus Clock: %i Hz\n", getXtalFrequencyMhz(), getCpuFrequencyMhz(), getApbFrequency());
  Serial.printf("Internal RAM size: %i Byte, Free: %i Byte\n", ESP.getHeapSize(), ESP.getFreeHeap());
  Serial.printf("WiFi MAC address: %s\n", WiFi.macAddress().c_str());
  for (uint8_t coreNum = 0; coreNum < 2; coreNum++)
  {
    uint8_t resetReason = rtc_get_reset_reason(coreNum);
    if (resetReason <= (sizeof(RESET_REASONS) / sizeof(RESET_REASONS[0])))
    {
      Serial.printf("Core %i reset reason: %i: %s\n", coreNum, rtc_get_reset_reason(coreNum), RESET_REASONS[resetReason - 1]);
    }
  }
#if defined BATTERY_PROTECTION
  Serial.printf("Battery protection calibration data:\n");
  Serial.printf("RESISTOR_TO_BATTTERY_PLUS: %i Ω\n", RESISTOR_TO_BATTTERY_PLUS);
  Serial.printf("RESISTOR_TO_GND: %i Ω\n", RESISTOR_TO_GND);
  Serial.printf("DIODE_DROP: %.2f V\n", DIODE_DROP);
#endif

  Serial.printf("**************************************************************************************************\n\n");

  // Eeprom Setup
  setupEeprom();

  // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
  // because it is sharing a resource, such as the PWM variable.
  // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
  if (xPwmSemaphore == NULL) // Check to confirm that the PWM Semaphore has not already been created.
  {
    xPwmSemaphore = xSemaphoreCreateMutex(); // Create a mutex semaphore we will use to manage variable access
    if ((xPwmSemaphore) != NULL)
      xSemaphoreGive((xPwmSemaphore)); // Make the PWM variable available for use, by "Giving" the Semaphore.
  }

  if (xRpmSemaphore == NULL) // Check to confirm that the RPM Semaphore has not already been created.
  {
    xRpmSemaphore = xSemaphoreCreateMutex(); // Create a mutex semaphore we will use to manage variable access
    if ((xRpmSemaphore) != NULL)
      xSemaphoreGive((xRpmSemaphore)); // Make the RPM variable available for use, by "Giving" the Semaphore.
  }

  // Set pin modes
  pinMode(COMMAND_RX, INPUT_PULLDOWN);

#if not defined THIRD_BRAKELIGHT and not defined RZ7886_DRIVER_MODE // If a third brakelight is not defined and RZ7886 motor driver is not defined, pin 32 for the trailer presence switch
  pinMode(COUPLER_SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COUPLER_SWITCH_PIN), trailerPresenceSwitchInterrupt, CHANGE);
#endif

  // LED & shaker motor setup (note, that we only have timers from 0 - 15, but 0 - 1 are used for interrupts!)
  headLight.begin(HEADLIGHT_PIN, 15, 20000);           // Timer 15, 20kHz
  tailLight.begin(TAILLIGHT_PIN, 2, 20000);            // Timer 2, 20kHz
  indicatorL.begin(INDICATOR_LEFT_PIN, 3, 20000);      // Timer 3, 20kHz
  indicatorR.begin(INDICATOR_RIGHT_PIN, 4, 20000);     // Timer 4, 20kHz
  fogLight.begin(FOGLIGHT_PIN, 5, 20000);              // Timer 5, 20kHz
  reversingLight.begin(REVERSING_LIGHT_PIN, 6, 20000); // Timer 6, 20kHz
  roofLight.begin(ROOFLIGHT_PIN, 7, 20000);            // Timer 7, 20kHz

#if not defined SPI_DASHBOARD
  sideLight.begin(SIDELIGHT_PIN, 8, 20000);         // Timer 8, 20kHz
  beaconLight1.begin(BEACON_LIGHT1_PIN, 9, 20000);  // Timer 9, 20kHz
  beaconLight2.begin(BEACON_LIGHT2_PIN, 10, 20000); // Timer 10, 20kHz
#endif

#if defined THIRD_BRAKELIGHT and not defined RZ7886_DRIVER_MODE
  brakeLight.begin(BRAKELIGHT_PIN, 11, 20000); // Timer 11, 20kHz
#endif
  cabLight.begin(CABLIGHT_PIN, 12, 20000); // Timer 12, 20kHz

#if not defined SPI_DASHBOARD
  shakerMotor.begin(SHAKER_MOTOR_PIN, 13, 20000); // Timer 13, 20kHz
#endif

  // Battery setup
  setupBattery();

  // ESP NOW setup
  setupEspNow();

#if defined SPI_DASHBOARD
  // Dashboard setup
  Serial.printf("SPI_DASHBOARD enabled. Pins 18 (sidelights), 19 (beacon2), 21 (beacon1), 23 (shaker) not usable!\n");
  Serial.printf("-------------------------------------\n");
  dashboard.init(dashRotation);
#endif

  // Neopixel setup
#ifdef NEOPIXEL_ENABLED
  FastLED.addLeds<NEOPIXEL, RGB_LEDS_PIN>(rgbLEDs, NEOPIXEL_COUNT);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(NEOPIXEL_BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
#endif

  // Communication setup --------------------------------------------
  indicatorL.on();
  indicatorR.on();

#if defined SBUS_COMMUNICATION // SBUS ----
  if (MAX_RPM_PERCENTAGE > maxSbusRpmPercentage)
    MAX_RPM_PERCENTAGE = maxSbusRpmPercentage;                // Limit RPM range
  sBus.begin(COMMAND_RX, COMMAND_TX, sbusInverted, sbusBaud); // begin SBUS communication with compatible receivers
  setupMcpwm();                                               // mcpwm servo output setup

#elif defined IBUS_COMMUNICATION // IBUS ----
  if (MAX_RPM_PERCENTAGE > maxIbusRpmPercentage)
    MAX_RPM_PERCENTAGE = maxIbusRpmPercentage;                 // Limit RPM range
  iBus.begin(Serial2, IBUSBM_NOTIMER, COMMAND_RX, COMMAND_TX); // begin IBUS communication with compatible receivers
  setupMcpwm();                                                // mcpwm servo output setup

#elif defined PPM_COMMUNICATION // PPM ----
  if (MAX_RPM_PERCENTAGE > maxPpmRpmPercentage)
    MAX_RPM_PERCENTAGE = maxPpmRpmPercentage;                          // Limit RPM range
  attachInterrupt(digitalPinToInterrupt(COMMAND_RX), readPpm, RISING); // begin PPM communication with compatible receivers
  setupMcpwm();                                                        // mcpwm servo output setup

#elif defined SUMD_COMMUNICATION // Graupner SUMD ----
  // SUMD communication
  if (MAX_RPM_PERCENTAGE > maxSumdRpmPercentage)
    MAX_RPM_PERCENTAGE = maxSumdRpmPercentage; // Limit RPM range
  sumd.begin(COMMAND_RX);                      // begin SUMD communication with compatible receivers
  setupMcpwm();                                // mcpwm servo output setup

#else
  // PWM ----
#define PWM_COMMUNICATION
  if (MAX_RPM_PERCENTAGE > maxPwmRpmPercentage)
    MAX_RPM_PERCENTAGE = maxPwmRpmPercentage; // Limit RPM range
  for (uint8_t i = 0; i < PWM_CHANNELS_NUM; i++)
  {
    pinMode(PWM_PINS[i], INPUT_PULLDOWN);
  }
  // New: PWM read setup, using rmt. Thanks to croky-b
  uint8_t i;
  rmt_config_t rmt_channels[PWM_CHANNELS_NUM] = {};

  for (i = 0; i < PWM_CHANNELS_NUM; i++)
  {
    rmt_channels[i].channel = (rmt_channel_t)PWM_CHANNELS[i];
    rmt_channels[i].gpio_num = (gpio_num_t)PWM_PINS[i];
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
  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      Task1code, // Task function
      "Task1",   // name of task
      8192,      // Stack size of task (8192)
      NULL,      // parameter of the task
      1,         // priority of the task (1 = low, 3 = medium, 5 = highest)
      &Task1,    // Task handle to keep track of created task
      0);        // pin task to core 0

  // once write with the "normal" way.
  // all further writes are done directly in the register since
  // it's much faster
  dacWrite(DAC1, 0);
  dacWrite(DAC2, 0);

  // Interrupt timer for variable sample rate playback
  variableTimer = timerBegin(0, 20, true);                           // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 20 -> 250 ns = 0.25 us, countUp
  timerAttachInterrupt(variableTimer, &variablePlaybackTimer, true); // edge (not level) triggered
  timerAlarmWrite(variableTimer, variableTimerTicks, true);          // autoreload true
  timerAlarmEnable(variableTimer);                                   // enable

  // Interrupt timer for fixed sample rate playback
  fixedTimer = timerBegin(1, 20, true);                        // timer 1, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 20 -> 250 ns = 0.25 us, countUp
  timerAttachInterrupt(fixedTimer, &fixedPlaybackTimer, true); // edge (not level) triggered
  timerAlarmWrite(fixedTimer, fixedTimerTicks, true);          // autoreload true
  timerAlarmEnable(fixedTimer);                                // enable

  // wait for RC receiver to initialize
  while (millis() <= 1000)
    ;

    // Read RC signals for the first time (used for offset calculations)
#if defined SBUS_COMMUNICATION
  sbusInit = false;
  Serial.printf("Initializing SBUS (sbusInverted = %s, needs to be true for most standard radios) ...\n", sbusInverted ? "true" : "false");
  Serial.printf("(Make sure radio and receiver are connected, turned on, bound and configured for SBUS output.)\n");
  while (!sbusInit)
  {
    readSbusCommands();               // SBUS communication (pin 36)
    indicatorL.flash(70, 75, 500, 3); // Show 3 fast flashes on indicators!
    indicatorR.flash(70, 75, 500, 3);
    rtc_wdt_feed(); // Feed watchdog timer
  }
  Serial.printf("... SBUS initialization succesful!\n");

#elif defined IBUS_COMMUNICATION
  ibusInit = false;
  Serial.printf("Initializing IBUS ...\n");
  Serial.printf("(Make sure radio and receiver are connected, turned on, bound and configured for IBUS output.)\n");
  while (!ibusInit)
  {
    readIbusCommands();               // IBUS communication (pin 36)
    indicatorL.flash(70, 75, 500, 3); // Show 3 fast flashes on indicators!
    indicatorR.flash(70, 75, 500, 3);
    rtc_wdt_feed(); // Feed watchdog timer
  }
  Serial.printf("... IBUS initialization succesful!\n");

#elif defined SUMD_COMMUNICATION
  SUMD_init = false;
  Serial.printf("Initializing SUMD ...\n");
  Serial.printf("(Make sure radio and receiver are connected, turned on, bound and configured for SUMD output.)\n");
  while (!SUMD_init)
  {
    readSumdCommands();
    indicatorL.flash(70, 75, 500, 3); // Show 3 fast flashes on indicators!
    indicatorR.flash(70, 75, 500, 3);
    rtc_wdt_feed(); // Feed watchdog timer
  }
  Serial.printf("... SUMD initialization succesful!\n");

#elif defined PPM_COMMUNICATION
  readPpmCommands();
#else
  // measure PWM RC signals mark space ratio
  readPwmSignals();
  Serial.printf("... PWM communication mode active.\n");
#endif
  Serial.printf("-------------------------------------\n");

  // Calculate RC input signal ranges for all channels
  for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
  {
    pulseZero[i] = 1500; // Always 1500. This is the center position. Auto centering is now done in "processRawChannels()"

    // Input signals
    pulseMaxNeutral[i] = pulseZero[i] + pulseNeutral;
    pulseMinNeutral[i] = pulseZero[i] - pulseNeutral;
    pulseMax[i] = pulseZero[i] + pulseSpan;
    pulseMin[i] = pulseZero[i] - pulseSpan;
    pulseMaxLimit[i] = pulseZero[i] + pulseLimit;
    pulseMinLimit[i] = pulseZero[i] - pulseLimit;
  }

  // ESC output range calibration
  escPulseMaxNeutral = pulseZero[3] + escTakeoffPunch; // Additional takeoff punch around zero
  escPulseMinNeutral = pulseZero[3] - escTakeoffPunch;

  escPulseMax = pulseZero[3] + escPulseSpan;
  escPulseMin = pulseZero[3] - escPulseSpan + escReversePlus; // Additional power for ESC with slow reverse

  // ESC setup
  setupMcpwmESC(); // ESC now using mpcpwm
}

//
// =======================================================================================================
// DAC OFFSET FADER
// =======================================================================================================
//

static unsigned long dacOffsetMicros;
boolean dacInit;

void dacOffsetFade()
{
  if (!dacInit)
  {
    if (micros() - dacOffsetMicros > 100)
    { // Every 0.1ms
      dacOffsetMicros = micros();
      dacOffset++; // fade DAC offset slowly to prevent it from popping, if ESP32 powered up after amplifier
      if (dacOffset == 128)
        dacInit = true;
    }
  }
}

//
// =======================================================================================================
// READ PWM RC SIGNALS (plug in your channels according to order in "channelSetup.h")
// =======================================================================================================
//

void readPwmSignals()
{

  static uint32_t lastFrameTime = millis();

  if (millis() - lastFrameTime > 20)
  { // Only do it every 20ms
    // measure RC signal pulsewidth:
    // nothing is done here, the PWM signals are now read, using the
    // "static void IRAM_ATTR rmt_isr_handler(void* arg)" interrupt function

    // NOTE: There is no channel mapping in this mode! Just plug in the wires in the order as defined in "2_adjustmentsRemote.h"
    // for example: sound controller channel 2 (GEARBOX) connects to receiver channel 6

    // See if we can obtain or "Take" the Semaphore.
    // If the semaphore is not available, wait 1 ticks of the Scheduler to see if it becomes free.
    if (xSemaphoreTake(xPwmSemaphore, portMAX_DELAY))
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      // We want to have the pwmBuf variable for us alone,
      // so we don't want it getting stolen during the middle of a conversion.
      for (uint8_t i = 1; i < PWM_CHANNELS_NUM + 1; i++)
      {
        if (pwmBuf[i] > 500 && pwmBuf[i] < 2500)
          pulseWidthRaw[i] = pwmBuf[i]; // Only take valid signals!
      }

      xSemaphoreGive(xPwmSemaphore); // Now free or "Give" the semaphore for others.
    }

    // Normalize, auto zero and reverse channels
    processRawChannels();

    // Failsafe for RC signals
    failSafe = (pulseWidthRaw[3] < 500 || pulseWidthRaw[3] > 2500);
    failsafeRcSignals();

    lastFrameTime = millis();
  }
  else
  {
    xSemaphoreGive(xPwmSemaphore); // Free or "Give" the semaphore for others, if not required!
  }
}

//
// =======================================================================================================
// READ PPM MULTI CHANNEL COMMMANDS (change the channel order in "2_adjustmentsRemote.h", if needed)
// =======================================================================================================
//

void readPpmCommands()
{

  // NOTE: 8 channels is the maximum of this protocol!

  pulseWidthRaw[1] = ppmBuf[STEERING - 1];   // CH1 steering
  pulseWidthRaw[2] = ppmBuf[GEARBOX - 1];    // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = ppmBuf[THROTTLE - 1];   // CH3 throttle & brake
  pulseWidthRaw[4] = ppmBuf[HORN - 1];       // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = ppmBuf[FUNCTION_R - 1]; // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = ppmBuf[FUNCTION_L - 1]; // CH6 indicators, hazards
  pulseWidthRaw[7] = ppmBuf[POT2 - 1];       // CH7 pot 2
  pulseWidthRaw[8] = ppmBuf[MODE1 - 1];      // CH8 mode 1 switch

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

void readSbusCommands()
{
  // Signals are coming in via SBUS protocol

  static unsigned long lastSbusFailsafe;

  // look for a good SBUS packet from the receiver
#if not defined EMBEDDED_SBUS // ------------------------
  if (sBus.read(&SBUSchannels[0], &SBUSfailSafe, &SBUSlostFrame))
  {
#else  // ------------------------------------------------
  if (sBus.read() && !sBus.failsafe() && !sBus.lost_frame())
  {
#endif // -----------------------------------------------
    sbusInit = true;
    lastSbusFailsafe = millis();
  }

  // Failsafe triggering (works, if SBUS wire is unplugged, but SBUSfailSafe signal from the receiver is untested!)
#if not defined EMBEDDED_SBUS // ------------------------
  if (millis() - lastSbusFailsafe > sbusFailsafeTimeout && !SBUSfailSafe && !SBUSlostFrame)
  {
#else                // ------------------------------------------------
  if (millis() - lastSbusFailsafe > sbusFailsafeTimeout)
  {
#endif               // -----------------------------------------------
    failSafe = true; // if timeout (signal loss)
  }
  else
    failSafe = false;

  // SBUSchannels[NONE - 1] = 991; // The NONE channel needs to be on neutral (991 = 1500ms) TODO

  // Proportional channels (in Microseconds)
  if (!failSafe)
  {
#if defined EMBEDDED_SBUS // ----------------------------
    SBUSchannels = sBus.ch();
#endif                                                                                 // -----------------------------------------------
    pulseWidthRaw[1] = map(SBUSchannels[STEERING - 1], 172, 1811, 1000, 2000);         // CH1 steering
    pulseWidthRaw[2] = map(SBUSchannels[GEARBOX - 1], 172, 1811, 1000, 2000);          // CH2 3 position switch for gearbox (left throttle in tracked mode)
    pulseWidthRaw[3] = map(SBUSchannels[THROTTLE - 1], 172, 1811, 1000, 2000);         // CH3 throttle & brake
    pulseWidthRaw[4] = map(SBUSchannels[HORN - 1], 172, 1811, 1000, 2000);             // CH5 jake brake, high / low beam, headlight flasher, engine on / off
    pulseWidthRaw[5] = map(SBUSchannels[FUNCTION_R - 1], 172, 1811, 1000, 2000);       // CH5 jake brake, high / low beam, headlight flasher, engine on / off
    pulseWidthRaw[6] = map(SBUSchannels[FUNCTION_L - 1], 172, 1811, 1000, 2000);       // CH6 indicators, hazards
    pulseWidthRaw[7] = map(SBUSchannels[POT2 - 1], 172, 1811, 1000, 2000);             // CH7 pot 2
    pulseWidthRaw[8] = map(SBUSchannels[MODE1 - 1], 172, 1811, 1000, 2000);            // CH8 mode 1 switch
    pulseWidthRaw[9] = map(SBUSchannels[MODE2 - 1], 172, 1811, 1000, 2000);            // CH9 mode 2 switch
    pulseWidthRaw[10] = map(SBUSchannels[MOMENTARY1 - 1], 172, 1811, 1000, 2000);      // CH10
    pulseWidthRaw[11] = map(SBUSchannels[HAZARDS - 1], 172, 1811, 1000, 2000);         // CH11
    pulseWidthRaw[12] = map(SBUSchannels[INDICATOR_LEFT - 1], 172, 1811, 1000, 2000);  // CH12
    pulseWidthRaw[13] = map(SBUSchannels[INDICATOR_RIGHT - 1], 172, 1811, 1000, 2000); // CH13
  }

  if (sbusInit)
  {
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
void loopIbus()
{
  // Loop iBus (read signals)
  static unsigned long lastIbusRead;
  static uint16_t iBusReadCycles;
  if (millis() - lastIbusRead > 10)
  { // Every 10ms
    lastIbusRead = millis();
    iBus.loop();
    if (iBusReadCycles < 100)
      iBusReadCycles++;
    else
      ibusInit = true; // We need to process the entire serial package, before we read the channels for the first time! 100 OK? TODO
  }
}

// Read IBUS signals ----
void readIbusCommands()
{

  // Loop iBus (fill buffer)
  loopIbus();

  // NOTE: The channel mapping is in the order as defined in "channelsetup.h"
  // for example: sound controller channel 2 (GEARBOX) connects to receiver channel 6

  // Proportional channels (in Microseconds)
  pulseWidthRaw[1] = iBus.readChannel(STEERING - 1);         // CH1 steering
  pulseWidthRaw[2] = iBus.readChannel(GEARBOX - 1);          // CH2 3 position switch for gearbox (left throttle in tracked mode)
  pulseWidthRaw[3] = iBus.readChannel(THROTTLE - 1);         // CH3 throttle & brake
  pulseWidthRaw[4] = iBus.readChannel(HORN - 1);             // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[5] = iBus.readChannel(FUNCTION_R - 1);       // CH5 jake brake, high / low beam, headlight flasher, engine on / off
  pulseWidthRaw[6] = iBus.readChannel(FUNCTION_L - 1);       // CH6 indicators, hazards
  pulseWidthRaw[7] = iBus.readChannel(POT2 - 1);             // CH7 pot 2
  pulseWidthRaw[8] = iBus.readChannel(MODE1 - 1);            // CH8 mode 1 switch
  pulseWidthRaw[9] = iBus.readChannel(MODE2 - 1);            // CH9 mode 2 switch
  pulseWidthRaw[10] = iBus.readChannel(MOMENTARY1 - 1);      // CH10
  pulseWidthRaw[11] = iBus.readChannel(HAZARDS - 1);         // CH11
  pulseWidthRaw[12] = iBus.readChannel(INDICATOR_LEFT - 1);  // CH12
  pulseWidthRaw[13] = iBus.readChannel(INDICATOR_RIGHT - 1); // CH13

  if (ibusInit)
  {
    // Normalize, auto zero and reverse channels
    processRawChannels();
  }
}

//
// =======================================================================================================
// READ SUMD SIGNALS (change the channel order in "2_adjustmentsRemote.h", if needed).
// =======================================================================================================
//
void readSumdCommands()
{
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
// PROCESS CHANNELS (Normalize, auto zero and reverse)
// =======================================================================================================
//

void processRawChannels()
{

  static unsigned long lastOutOfRangeMillis;
  static int channel;
  static bool exThrottlePrint;
  static bool exSteeringPrint;

#ifdef TRACKED_MODE // If tracked mode: enable CH2 auto zero adjustment as well, if it is enabled for CH3
  if (channelAutoZero[3])
    channelAutoZero[2] = true;
#endif

#ifdef AIRPLANE_MODE // If airplane mode: always disable CH3 auto zero adjustment
  channelAutoZero[3] = false;
#endif

  if (millis() - lastOutOfRangeMillis > 500)
  {
    for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
    { // For each channel:

      // Position valid for auto calibration? Must be between 1400 and 1600 microseconds
      if (channelAutoZero[i] && !autoZeroDone && (pulseWidthRaw[i] > 1600 || pulseWidthRaw[i] < 1400))
      {
        channel = i;
        Serial.printf(" CH%i: signal out of auto calibration range, check transmitter & receiver!\n", channel);
        channelZero();
        lastOutOfRangeMillis = millis();
        i--;
        return;
      }

      // Exponential throttle compensation ------------------
#ifdef EXPONENTIAL_THROTTLE
      if (!exThrottlePrint)
      {
        Serial.printf("EXPONENTIAL_THROTTLE mode enabled\n");
        exThrottlePrint = true;
      }
      if (i == 3)
      { // Throttle CH only
        pulseWidthRaw2[i] = reMap(curveExponentialThrottle, pulseWidthRaw[i]);
      }
      else
      {
        pulseWidthRaw2[i] = pulseWidthRaw[i];
      }
#else
      pulseWidthRaw2[i] = pulseWidthRaw[i];
#endif // --------------------------------------------------

      // Exponential steering compensation ------------------
#ifdef EXPONENTIAL_STEERING
      if (!exSteeringPrint)
      {
        Serial.printf("EXPONENTIAL_STEERING mode enabled\n");
        exSteeringPrint = true;
      }
      if (i == 1)
      { // Throttle CH only
        pulseWidthRaw2[i] = reMap(curveExponentialThrottle, pulseWidthRaw[i]);
      }
      else
      {
        pulseWidthRaw2[i] = pulseWidthRaw[i];
      }
#else
      pulseWidthRaw2[i] = pulseWidthRaw[i];
#endif // --------------------------------------------------

      // Take channel raw data, reverse them, if required and store them
      if (channelReversed[i])
        pulseWidthRaw3[i] = map(pulseWidthRaw2[i], 0, 3000, 3000, 0); // Reversed
      else
        pulseWidthRaw3[i] = pulseWidthRaw2[i]; // Not reversed

      // Calculate zero offset (only within certain absolute range)
      if (channelAutoZero[i] && !autoZeroDone && pulseWidthRaw3[i] > pulseMinValid && pulseWidthRaw3[i] < pulseMaxValid)
        pulseOffset[i] = 1500 - pulseWidthRaw3[i];

      // Center channel, if out of range!
      if (pulseWidthRaw3[i] > pulseMaxValid || pulseWidthRaw3[i] < pulseMinValid)
        pulseWidthRaw3[i] = pulseZero[i];

      // Limit channel, if out of range (required for RGT  MT-350 @ max. throttle dual rate)
      if (pulseWidthRaw3[i] > 2000)
        pulseWidthRaw3[i] = 2000;
      if (pulseWidthRaw3[i] < 1000)
        pulseWidthRaw3[i] = 1000;

      // Compensate pulsewidth with auto zero offset
      pulseWidthRaw3[i] += pulseOffset[i];
      if (!autoZeroDone)
      { // Print offsets, if switching on the controller
        if (i == 1)
          Serial.printf("\nTransmitter channel offsets (calculated, if channelAutoZero[] = true):\n");
        if (channelAutoZero[i])
          Serial.printf(" CH%i: %i µs\n", i, pulseOffset[i]);
      }

      // Set auto zero done flag
      if (i == PULSE_ARRAY_SIZE - 1)
        autoZeroDone = true;
    }
  }

  if (!autoZeroDone)
  {                                           // Indicators are showing the number of channels, which are out of auto calibration range
    indicatorL.flash(140, 150, 500, channel); // ON, OFF, PAUSE, PULSES, (OPTIONAL DELAY FOR FIRST PASS)
    indicatorR.flash(140, 150, 500, channel);
  }

#if defined CHANNEL_AVERAGING // --------------------------------------------------------------------------------
  uint16_t n = 4;             // 2 - 4 !
  static bool initDone = false;
  static uint32_t smoothed[PULSE_ARRAY_SIZE];
  static unsigned long averagingMillis = millis();

  if (millis() - averagingMillis > 15)
  { // Every 15ms (SBUS packets are coming in every 14ms)
    averagingMillis = millis();
    for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
    { // With averaging -----
      if (initDone)
      {
        smoothed[i] = (smoothed[i] * (n - 1) + pulseWidthRaw3[i]) / n;
        pulseWidth[i] = smoothed[i];
      }
      else
      {
        smoothed[i] = pulseWidthRaw3[i];
        pulseWidth[i] = pulseWidthRaw3[i];
        if (i >= PULSE_ARRAY_SIZE - 1)
          initDone = true;
      }
    }
  }
#else // Without averaging -----
  for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
  {
    pulseWidth[i] = pulseWidthRaw3[i];
  }
#endif

  // Print input signal debug infos -----------------------------------------------------------------------------
#ifdef CHANNEL_DEBUG // can slow down the playback loop!
  static unsigned long printChannelMillis;
  if (millis() - printChannelMillis > 1000 && autoZeroDone)
  { // Every 1000ms
    printChannelMillis = millis();

    Serial.printf("CHANNEL_DEBUG:\n");
    for (uint8_t channelNum = 1; channelNum < PULSE_ARRAY_SIZE; channelNum++)
    {
      Serial.printf(" CH%i: %i µs\n", channelNum, pulseWidth[channelNum]);
    }
    Serial.printf("Throttle: %i/%i\n", currentThrottle, maxRpm);
    Serial.printf("States:\n");
    Serial.printf(" MODE1:            %s\n", mode1 ? "true" : "false");
    Serial.printf(" MODE2:            %s\n", mode2 ? "true" : "false");
    Serial.printf(" MOMENTARY1:       %s\n", momentary1 ? "true" : "false");
    Serial.printf(" HAZARDS:          %s\n", hazard ? "true" : "false");
    Serial.printf(" INDICATOR_LEFT:   %s\n", indicatorLon ? "true" : "false");
    Serial.printf(" INDICATOR_RIGHT:  %s\n", indicatorRon ? "true" : "false");
#if not defined EMBEDDED_SBUS // ------------------------
    Serial.printf(" SBUS Failsafe:    %s\n", SBUSfailSafe ? "true" : "false");
    Serial.printf(" SBUS Lost frames: %s\n", SBUSlostFrame ? "true" : "false");
#endif // -----------------------------------------------
    Serial.printf(" Failsafe state:   %s\n", failSafe ? "true" : "false");
    Serial.printf("Misc:\n");
    Serial.printf(" MAX_RPM_PERCENTAGE: %i\n", MAX_RPM_PERCENTAGE);
    Serial.printf(" loopTime:           %i\n", loopTime);
    Serial.printf("-----------------------------------\n");
  }
#endif // CHANNEL_DEBUG
}

// Sub function for channel centering ----
void channelZero()
{
  for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
  {
    pulseWidth[i] = 1500;
  }
}

//
// =======================================================================================================
// RC SIGNAL FAILSAFE POSITIONS (if serial signal lost)
// =======================================================================================================
//

void failsafeRcSignals()
{

  // Failsafe actions --------
  if (failSafe)
  {
    for (uint8_t i = 1; i < PULSE_ARRAY_SIZE; i++)
    {
      if (i != 1 && i != 2 && i != 8 && i != 9)
        pulseWidth[i] = pulseZero[i]; // Channels to zero position, but never for CH1 (Steering), CH8, CH9
    }
  }
}

//
// =======================================================================================================
// ROTATING BEACON CONTROL (BUS communication mode only)
// =======================================================================================================
//

bool beaconControl(uint8_t pulses)
{

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

  if (millis() - pulseMillis > 40)
  { // Every 40ms (this is the required minimum)
    pulseMillis = millis();
    if (pulseWidth == CH3L)
    {
      pulseWidth = CH3R;
    }
    else
    {
      pulseWidth = CH3L;
      i++;
    }
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, pulseWidth);
  }

  if (i >= pulses)
  {
    i = 0;
    return true;
  }
  else
    return false;
}

//
// =======================================================================================================
// MCPWM SERVO RC SIGNAL OUTPUT (BUS communication mode only)
// =======================================================================================================
//
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html#configure

void mcpwmOutput()
{
  if (autoZeroDone) // Only generate servo signals, if auto zero was successful!
  {

    // Steering CH1 **********************
    uint16_t steeringServoMicros;
    static uint16_t steeringServoMicrosDelayed = CH1C;
    static unsigned long steeringDelayMicros;
    int16_t steeringDeviation = 1;
    if (micros() - steeringDelayMicros > STEERING_RAMP_TIME)
    { // Adjustable steering max. ramp speed
      // It is required to calculate a variable deviation, according to how much "delay" the steering ramp time has
      // Reason: we have a high interrupt load at high engine RPM. The servo movements are getting too slow otherwise
      steeringDeviation = (micros() - steeringDelayMicros) - STEERING_RAMP_TIME;
      steeringDeviation = constrain(steeringDeviation, 1, 10);
      steeringDelayMicros = micros();

      if (pulseWidth[1] < 1500)
        steeringServoMicros = map(pulseWidth[1], 1000, 1500, CH1L, CH1C);
      else if (pulseWidth[1] > 1500)
        steeringServoMicros = map(pulseWidth[1], 1500, 2000, CH1C, CH1R);
      else
        steeringServoMicros = CH1C;
      if (steeringServoMicrosDelayed < steeringServoMicros)
        steeringServoMicrosDelayed += steeringDeviation;
      if (steeringServoMicrosDelayed > steeringServoMicros)
        steeringServoMicrosDelayed -= steeringDeviation;
      steeringServoMicrosDelayed = constrain(steeringServoMicrosDelayed, min(CH1L, CH1R), max(CH1L, CH1R));
      // Serial.printf("steeringServoMicros: %s\n", steeringServoMicros);
      // Serial.printf("steeringServoMicrosDelayed: %s\n", steeringServoMicrosDelayed);
      mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, steeringServoMicrosDelayed);
    }

    // Shifting CH2 **********************
    static uint16_t shiftingServoMicros;
#if not defined MODE1_SHIFTING
    if (selectedGear == 1)
      shiftingServoMicros = CH2L;
    if (selectedGear == 2)
      shiftingServoMicros = CH2C;
    if (selectedGear >= 3)
      shiftingServoMicros = CH2R;
#else
#undef TRANSMISSION_NEUTRAL // Not usable in this case!
    if (currentSpeed > 50 && currentSpeed < 150)
    { // Only shift WPL gearbox, if vehicle is moving slowly, so it's engaging properly
      if (!mode1)
      {
        shiftingServoMicros = CH2L;
        lowRange = true;
      }
      else
      {
        shiftingServoMicros = CH2C;
        lowRange = false;
      }
    }
#endif
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, shiftingServoMicros);

    // Winch CH3 **********************
#if defined NO_WINCH_DELAY
    uint16_t winchDelayTarget = 0; // Servo signal for winch is changed immediately
#else
    uint16_t winchDelayTarget = 12000; // Servo signal for winch is changed slowly
#endif

#if defined MODE2_WINCH
    static uint16_t winchServoMicrosTarget = CH3C;
    static uint16_t winchServoMicros = CH3C;
    static unsigned long winchDelayMicros;
    if (micros() - winchDelayMicros > winchDelayTarget)
    {
      winchDelayMicros = micros();
      if (winchPull)
        winchServoMicrosTarget = CH3L;
      else if (winchRelease)
        winchServoMicrosTarget = CH3R;
      else
        winchServoMicrosTarget = CH3C;
      if (winchServoMicros < winchServoMicrosTarget)
        winchServoMicros++;
      if (winchServoMicros > winchServoMicrosTarget)
        winchServoMicros--;
    }
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, winchServoMicros);
#endif

    // Beacon CH3 **********************
#if defined CH3_BEACON

    // Init (5 pulses are required to shut beacons off after power on)
    static bool blueLightInit;
    if (!blueLightInit)
    {
      if (beaconControl(5))
        blueLightInit = true;
    }

    // Switching modes
    static uint16_t beaconServoMicros;
    static bool lockRotating, lockOff;
    if (blueLightInit)
    {
      if (blueLightTrigger && !lockRotating)
      { // Rotating mode on (1 pulse)
        if (beaconControl(1))
        {
          lockRotating = true;
          lockOff = false;
        }
      }
      if (!blueLightTrigger && !lockOff && lockRotating)
      { // Off (4 pulses)
        if (beaconControl(4))
        {
          lockOff = true;
          lockRotating = false;
        }
      }
    }
#endif

    // Trailer coupler (5th wheel) CH4 **********************
    static uint16_t couplerServoMicros;
    if (unlock5thWheel)
      couplerServoMicros = CH4R;
    else
      couplerServoMicros = CH4L;
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, couplerServoMicros);
  }

  // Print servo signal debug infos **********************
  static unsigned long printServoMillis;
#ifdef SERVO_DEBUG // can slow down the playback loop!
  if (millis() - printServoMillis > 1000)
  { // Every 1000ms
    printServoMillis = millis();

    Serial.printf("SERVO_DEBUG:\n");
    Serial.printf("CH1 (steering) :  %i µS, %.2f°\n", steeringServoMicrosDelayed, us2degree(steeringServoMicrosDelayed));
    Serial.printf("CH2 (shifting) :  %i µS, %.2f°\n", shiftingServoMicros, us2degree(shiftingServoMicros));

#if defined MODE2_WINCH
    Serial.printf("CH3 (winch)    :  %i µS, %f°\n", winchServoMicros, us2degree(winchServoMicros));
#endif

#if defined CH3_BEACON
    Serial.printf("CH3 (beacon)    :  %i µS, %f°\n", beaconServoMicros, us2degree(beaconServoMicros));
#endif

    Serial.printf("CH4 (5th wheel):  %i µS, %.2f°\n", couplerServoMicros, us2degree(couplerServoMicros));
    Serial.printf("-------------------------------------\n");
  }
#endif // SERVO_DEBUG
}

//
// =======================================================================================================
// DISABLE INTERRUPTS
// =======================================================================================================
//

// it is required to disable interrupts prior to EEPROM access!
void disableAllInterrupts()
{

  timerDetachInterrupt(variableTimer);
  timerDetachInterrupt(fixedTimer);

  Serial.print("Interrupts disabled, reboot required!");
}

//
// =======================================================================================================
// EEPROM
// =======================================================================================================
//

// Write string to EEPROM ------
// https://roboticsbackend.com/arduino-write-string-in-eeprom/#Write_the_String

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

// Read string from EEPROM ------
int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

// Erase EEPROM ------
void eepromErase()
{
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM erased!");
  delay(500);
  eepromDebugRead();
}

// Init new board with the default values you want ------
void eepromInit()
{
  if (EEPROM.read(adr_eprom_init) != eeprom_id)
  { // Only do init, if ID has changed!
    EEPROM.write(adr_eprom_init, eeprom_id);

    EEPROM.write(adr_eprom_useTrailer1, defaultUseTrailer1);
    EEPROM.write(adr_eprom_Trailer1Mac0, defaultBroadcastAddress1[0]); // Should always be 0xFE!
    EEPROM.write(adr_eprom_Trailer1Mac1, defaultBroadcastAddress1[1]); // Country number
    EEPROM.write(adr_eprom_Trailer1Mac2, defaultBroadcastAddress1[2]); // Region number
    EEPROM.write(adr_eprom_Trailer1Mac3, defaultBroadcastAddress1[3]); // User Number 1
    EEPROM.write(adr_eprom_Trailer1Mac4, defaultBroadcastAddress1[4]); // User Number 2
    EEPROM.write(adr_eprom_Trailer1Mac5, defaultBroadcastAddress1[5]); // 0x01 = trailer #1

    EEPROM.write(adr_eprom_useTrailer2, defaultUseTrailer2);
    EEPROM.write(adr_eprom_Trailer2Mac0, defaultBroadcastAddress2[0]); // Should always be 0xFE!
    EEPROM.write(adr_eprom_Trailer2Mac1, defaultBroadcastAddress2[1]); // Country number
    EEPROM.write(adr_eprom_Trailer2Mac2, defaultBroadcastAddress2[2]); // Region number
    EEPROM.write(adr_eprom_Trailer2Mac3, defaultBroadcastAddress2[3]); // User Number 1
    EEPROM.write(adr_eprom_Trailer2Mac4, defaultBroadcastAddress2[4]); // User Number 2
    EEPROM.write(adr_eprom_Trailer2Mac5, defaultBroadcastAddress2[5]); // 0x02 = trailer #2

    EEPROM.write(adr_eprom_useTrailer3, defaultUseTrailer3);
    EEPROM.write(adr_eprom_Trailer3Mac0, defaultBroadcastAddress3[0]); // Should always be 0xFE!
    EEPROM.write(adr_eprom_Trailer3Mac1, defaultBroadcastAddress3[1]); // Country number
    EEPROM.write(adr_eprom_Trailer3Mac2, defaultBroadcastAddress3[2]); // Region number
    EEPROM.write(adr_eprom_Trailer3Mac3, defaultBroadcastAddress3[3]); // User Number 1
    EEPROM.write(adr_eprom_Trailer3Mac4, defaultBroadcastAddress3[4]); // User Number 2
    EEPROM.write(adr_eprom_Trailer3Mac5, defaultBroadcastAddress3[5]); // 0x02 = trailer #2

    // EEPROM.write(adr_eprom_fifthWhweelDetectionActive, defaulFifthWhweelDetectionActive);
    // EEPROM.write(adr_eprom_tailLightBrightness, defaultLightsBrightness);
    // EEPROM.write(adr_eprom_sideLightBrightness, defaultLightsBrightness);
    // EEPROM.write(adr_eprom_reversingLightBrightness, defaultLightsBrightness);
    // EEPROM.write(adr_eprom_indicatorLightBrightness, defaultLightsBrightness);
    writeStringToEEPROM(adr_eprom_ssid, default_ssid);
    writeStringToEEPROM(adr_eprom_password, default_password);
    EEPROM.commit();
    Serial.println("EEPROM initialized.");
  }
}

// Write new values to EEPROM ------
void eepromWrite()
{

  disableAllInterrupts();

  EEPROM.write(adr_eprom_useTrailer1, useTrailer1);
  EEPROM.write(adr_eprom_Trailer1Mac0, broadcastAddress1[0]); // Should always be 0xFE!
  EEPROM.write(adr_eprom_Trailer1Mac1, broadcastAddress1[1]); // Country number
  EEPROM.write(adr_eprom_Trailer1Mac2, broadcastAddress1[2]); // Region number
  EEPROM.write(adr_eprom_Trailer1Mac3, broadcastAddress1[3]); // User Number 1
  EEPROM.write(adr_eprom_Trailer1Mac4, broadcastAddress1[4]); // User Number 2
  EEPROM.write(adr_eprom_Trailer1Mac5, broadcastAddress1[5]); // 0x01 = trailer #1

  EEPROM.write(adr_eprom_useTrailer2, useTrailer2);
  EEPROM.write(adr_eprom_Trailer2Mac0, broadcastAddress2[0]); // Should always be 0xFE!
  EEPROM.write(adr_eprom_Trailer2Mac1, broadcastAddress2[1]); // Country number
  EEPROM.write(adr_eprom_Trailer2Mac2, broadcastAddress2[2]); // Region number
  EEPROM.write(adr_eprom_Trailer2Mac3, broadcastAddress2[3]); // User Number 1
  EEPROM.write(adr_eprom_Trailer2Mac4, broadcastAddress2[4]); // User Number 2
  EEPROM.write(adr_eprom_Trailer2Mac5, broadcastAddress2[5]); // 0x02 = trailer #2

  EEPROM.write(adr_eprom_useTrailer3, useTrailer3);
  EEPROM.write(adr_eprom_Trailer3Mac0, broadcastAddress3[0]); // Should always be 0xFE!
  EEPROM.write(adr_eprom_Trailer3Mac1, broadcastAddress3[1]); // Country number
  EEPROM.write(adr_eprom_Trailer3Mac2, broadcastAddress3[2]); // Region number
  EEPROM.write(adr_eprom_Trailer3Mac3, broadcastAddress3[3]); // User Number 1
  EEPROM.write(adr_eprom_Trailer3Mac4, broadcastAddress3[4]); // User Number 2
  EEPROM.write(adr_eprom_Trailer3Mac5, broadcastAddress3[5]); // 0x02 = trailer #2

  // EEPROM.write(adr_eprom_fifthWhweelDetectionActive, fifthWhweelDetectionActive);
  // EEPROM.write(adr_eprom_tailLightBrightness, tailLightBrightness);
  // EEPROM.write(adr_eprom_sideLightBrightness, sideLightBrightness);
  // EEPROM.write(adr_eprom_reversingLightBrightness, reversingLightBrightness);
  // EEPROM.write(adr_eprom_indicatorLightBrightness, indicatorLightBrightness);
  writeStringToEEPROM(adr_eprom_ssid, ssid);
  writeStringToEEPROM(adr_eprom_password, password);
  EEPROM.commit();
  Serial.println("EEPROM written.");
  eepromDebugRead();
}

// Read values from EEPROM ------
void eepromRead()
{
  useTrailer1 = EEPROM.read(adr_eprom_useTrailer1);
  broadcastAddress1[0] = EEPROM.read(adr_eprom_Trailer1Mac0);
  broadcastAddress1[1] = EEPROM.read(adr_eprom_Trailer1Mac1);
  broadcastAddress1[2] = EEPROM.read(adr_eprom_Trailer1Mac2);
  broadcastAddress1[3] = EEPROM.read(adr_eprom_Trailer1Mac3);
  broadcastAddress1[4] = EEPROM.read(adr_eprom_Trailer1Mac4);
  broadcastAddress1[5] = EEPROM.read(adr_eprom_Trailer1Mac5);

  useTrailer2 = EEPROM.read(adr_eprom_useTrailer2);
  broadcastAddress2[0] = EEPROM.read(adr_eprom_Trailer2Mac0);
  broadcastAddress2[1] = EEPROM.read(adr_eprom_Trailer2Mac1);
  broadcastAddress2[2] = EEPROM.read(adr_eprom_Trailer2Mac2);
  broadcastAddress2[3] = EEPROM.read(adr_eprom_Trailer2Mac3);
  broadcastAddress2[4] = EEPROM.read(adr_eprom_Trailer2Mac4);
  broadcastAddress2[5] = EEPROM.read(adr_eprom_Trailer2Mac5);

  useTrailer3 = EEPROM.read(adr_eprom_useTrailer3);
  broadcastAddress3[0] = EEPROM.read(adr_eprom_Trailer3Mac0);
  broadcastAddress3[1] = EEPROM.read(adr_eprom_Trailer3Mac1);
  broadcastAddress3[2] = EEPROM.read(adr_eprom_Trailer3Mac2);
  broadcastAddress3[3] = EEPROM.read(adr_eprom_Trailer3Mac3);
  broadcastAddress3[4] = EEPROM.read(adr_eprom_Trailer3Mac4);
  broadcastAddress3[5] = EEPROM.read(adr_eprom_Trailer3Mac5);

  // fifthWhweelDetectionActive = EEPROM.read(adr_eprom_fifthWhweelDetectionActive);
  // tailLightBrightness = EEPROM.read(adr_eprom_tailLightBrightness);
  // sideLightBrightness = EEPROM.read(adr_eprom_sideLightBrightness);
  // reversingLightBrightness = EEPROM.read(adr_eprom_reversingLightBrightness);
  // indicatorLightBrightness = EEPROM.read(adr_eprom_indicatorLightBrightness);
  readStringFromEEPROM(adr_eprom_ssid, &ssid);
  readStringFromEEPROM(adr_eprom_password, &password);

  Serial.println("EEPROM read.");
}

void eepromDebugRead()
{
#if defined DEBUG
  String eepromDebug;
  Serial.println("EEPROM debug dump begin **********************************************");
  eepromDebug = "";
  for (int i = 0; i < EEPROM_SIZE; ++i)
  {
    eepromDebug += char(EEPROM.read(i));
  }
  Serial.println(eepromDebug);
  Serial.println("");
  Serial.println("EEPROM debug dump end ************************************************");
#endif
}

//
// =======================================================================================================
// WEB INTERFACE
// =======================================================================================================
//

#include "src/webInterface.h" // Configuration website

//
// =======================================================================================================
// MAP PULSEWIDTH TO THROTTLE CH3
// =======================================================================================================
//

void mapThrottle()
{

  // Input is around 1000 - 2000us, output 0-500 for forward and backwards

#if defined TRACKED_MODE        // Dual throttle input for caterpillar vehicles ------------------
  int16_t currentThrottleLR[4]; // 2 & 3 is used, so required array size = 4!

  // check if pulsewidths 2 + 3 look like servo pulses
  for (int i = 2; i < 4; i++)
  {
    if (pulseWidth[i] > pulseMinLimit[i] && pulseWidth[i] < pulseMaxLimit[i])
    {
      if (pulseWidth[i] < pulseMin[i])
        pulseWidth[i] = pulseMin[i]; // Constrain the value
      if (pulseWidth[i] > pulseMax[i])
        pulseWidth[i] = pulseMax[i];

      // calculate a throttle value from the pulsewidth signal
      if (pulseWidth[i] > pulseMaxNeutral[i])
      {
        currentThrottleLR[i] = map(pulseWidth[i], pulseMaxNeutral[i], pulseMax[i], 0, 500);
      }
      else if (pulseWidth[i] < pulseMinNeutral[i])
      {
        currentThrottleLR[i] = map(pulseWidth[i], pulseMinNeutral[i], pulseMin[i], 0, 500);
      }
      else
      {
        currentThrottleLR[i] = 0;
      }
    }
  }

  // Mixing both sides together (take the bigger value)
  currentThrottle = max(currentThrottleLR[2], currentThrottleLR[3]);

  // Print debug infos
  static unsigned long printTrackedMillis;
#ifdef TRACKED_DEBUG // can slow down the playback loop!
  if (millis() - printTrackedMillis > 1000)
  { // Every 1000ms
    printTrackedMillis = millis();

    Serial.printf("TRACKED DEBUG:\n");
    Serial.printf("currentThrottleLR[2]: %i\n", currentThrottleLR[2]);
    Serial.printf("currentThrottleLR[3]: %i\n", currentThrottleLR[3]);
    Serial.printf("currentThrottle: %i\n", currentThrottle);
  }
#endif // TRACKED_DEBUG

#elif defined EXCAVATOR_MODE // Excavator mode ----------------------------------------------

  static bool engineInit = false; // Only allow to start engine after switch was in up position
  static unsigned long rpmLoweringMillis;
  static uint8_t rpmLowering;

  // calculate a throttle value from the pulsewidth signal (forward only)
  if (pulseWidth[3] > pulseMaxNeutral[3])
  {
    currentThrottle = map(pulseWidth[3], pulseMaxNeutral[3], pulseMax[3], 0, (500 - rpmLowering));
  }
  else
  {
    currentThrottle = 0;
  }

  // Engine on / off via 3 position switch
  if (pulseWidth[3] < 1200 && currentRpm < 50)
  { // Off
    engineInit = true;
    engineOn = false;
    // rpmLoweringMillis = millis();
  }
  else
  { // On
    if (engineInit)
      engineOn = true;
  }

  // Engine RPM lowering, if hydraulic not used for 5s
  if (hydraulicLoad > 1 || pulseWidth[3] < pulseMaxNeutral[3])
    rpmLoweringMillis = millis();
  if (millis() - rpmLoweringMillis > 5000)
    rpmLowering = 250; // Medium RPM
  else
    rpmLowering = 0; // Full RPM

#elif defined AIRPLANE_MODE // Airplane mode ----------------------------------------------

  // Never engage clutch
  maxClutchSlippingRpm = 500;
  clutchEngagingPoint = 500;

  // calculate a throttle value from the pulsewidth signal (forward only, throttle zero @1000)
  if (pulseWidth[3] > 1100)
  {
    currentThrottle = map(pulseWidth[3], 1100, 2000, 0, 500);
  }
  else
  {
    currentThrottle = 0;
  }

#else // Normal mode ---------------------------------------------------------------------------

  // check if the pulsewidth looks like a servo pulse
  if (pulseWidth[3] > pulseMinLimit[3] && pulseWidth[3] < pulseMaxLimit[3])
  {
    if (pulseWidth[3] < pulseMin[3])
      pulseWidth[3] = pulseMin[3]; // Constrain the value
    if (pulseWidth[3] > pulseMax[3])
      pulseWidth[3] = pulseMax[3];

    // calculate a throttle value from the pulsewidth signal
    if (pulseWidth[3] > pulseMaxNeutral[3])
    {
      currentThrottle = map(pulseWidth[3], pulseMaxNeutral[3], pulseMax[3], 0, 500);
    }
    else if (pulseWidth[3] < pulseMinNeutral[3])
    {
      currentThrottle = map(pulseWidth[3], pulseMinNeutral[3], pulseMin[3], 0, 500);
    }
    else
    {
      currentThrottle = 0;
    }
  }
#endif

  // Auto throttle --------------------------------------------------------------------------
#if not defined EXCAVATOR_MODE
  // Auto throttle while gear shifting (synchronizing the Tamiya 3 speed gearbox)
  if (!escIsBraking && escIsDriving && shiftingAutoThrottle && !automatic && !doubleClutch)
  {
    if (gearUpShiftingInProgress && !doubleClutchInProgress)
      currentThrottle = 0; // No throttle
    if (gearDownShiftingInProgress || doubleClutchInProgress)
      currentThrottle = 500;                              // Full throttle
    currentThrottle = constrain(currentThrottle, 0, 500); // Limit throttle range
  }
#endif

  // Volume calculations --------------------------------------------------------------------------

  // As a base for some calculations below, fade the current throttle to make it more natural
  static unsigned long throttleFaderMicros;
  static boolean blowoffLock;
  if (micros() - throttleFaderMicros > 500)
  { // Every 0.5ms
    throttleFaderMicros = micros();

    if (currentThrottleFaded < currentThrottle && !escIsBraking && currentThrottleFaded < 499)
      currentThrottleFaded += 2;
    if ((currentThrottleFaded > currentThrottle || escIsBraking) && currentThrottleFaded > 2)
      currentThrottleFaded -= 2;

    // Calculate throttle dependent engine idle volume
    if (!escIsBraking && !brakeDetect && engineRunning)
      throttleDependentVolume = map(currentThrottleFaded, 0, 500, engineIdleVolumePercentage, fullThrottleVolumePercentage);
    // else throttleDependentVolume = engineIdleVolumePercentage; // TODO
    else
    {
      if (throttleDependentVolume > engineIdleVolumePercentage)
        throttleDependentVolume--;
      else
        throttleDependentVolume = engineIdleVolumePercentage;
    }

    // Calculate throttle dependent engine rev volume
    if (!escIsBraking && !brakeDetect && engineRunning)
      throttleDependentRevVolume = map(currentThrottleFaded, 0, 500, engineRevVolumePercentage, fullThrottleVolumePercentage);
    // else throttleDependentRevVolume = engineRevVolumePercentage; // TODO
    else
    {
      if (throttleDependentRevVolume > engineRevVolumePercentage)
        throttleDependentRevVolume--;
      else
        throttleDependentRevVolume = engineRevVolumePercentage;
    }

    // Calculate throttle dependent Diesel knock volume
    if (!escIsBraking && !brakeDetect && engineRunning && (currentThrottleFaded > dieselKnockStartPoint))
      throttleDependentKnockVolume = map(currentThrottleFaded, dieselKnockStartPoint, 500, dieselKnockIdleVolumePercentage, 100);
    // else throttleDependentKnockVolume = dieselKnockIdleVolumePercentage;
    else
    {
      if (throttleDependentKnockVolume > dieselKnockIdleVolumePercentage)
        throttleDependentKnockVolume--;
      else
        throttleDependentKnockVolume = dieselKnockIdleVolumePercentage;
    }

    // Calculate engine rpm dependent jake brake volume
    if (engineRunning)
      rpmDependentJakeBrakeVolume = map(currentRpm, 0, 500, jakeBrakeIdleVolumePercentage, 100);
    else
      rpmDependentJakeBrakeVolume = jakeBrakeIdleVolumePercentage;

#if defined RPM_DEPENDENT_KNOCK // knock volume also depending on engine rpm
    // Calculate RPM dependent Diesel knock volume
    if (currentRpm > 400)
      rpmDependentKnockVolume = map(currentRpm, knockStartRpm, 500, minKnockVolumePercentage, 100);
    else
      rpmDependentKnockVolume = minKnockVolumePercentage;
#endif

    // Calculate engine rpm dependent turbo volume
    if (engineRunning)
      throttleDependentTurboVolume = map(currentRpm, 0, 500, turboIdleVolumePercentage, 100);
    else
      throttleDependentTurboVolume = turboIdleVolumePercentage;

    // Calculate engine rpm dependent cooling fan volume
    if (engineRunning && (currentRpm > fanStartPoint))
      throttleDependentFanVolume = map(currentRpm, fanStartPoint, 500, fanIdleVolumePercentage, 100);
    else
      throttleDependentFanVolume = fanIdleVolumePercentage;

    // Calculate throttle dependent supercharger volume
    if (!escIsBraking && !brakeDetect && engineRunning && (currentRpm > chargerStartPoint))
      throttleDependentChargerVolume = map(currentThrottleFaded, chargerStartPoint, 500, chargerIdleVolumePercentage, 100);
    else
      throttleDependentChargerVolume = chargerIdleVolumePercentage;

    // Calculate engine rpm dependent wastegate volume
    if (engineRunning)
      rpmDependentWastegateVolume = map(currentRpm, 0, 500, wastegateIdleVolumePercentage, 100);
    else
      rpmDependentWastegateVolume = wastegateIdleVolumePercentage;
  }

  // Calculate engine load (used for torque converter slip simulation)
  engineLoad = currentThrottle - currentRpm;

  if (engineLoad < 0 || escIsBraking || brakeDetect)
    engineLoad = 0; // Range is 0 - 180
  if (engineLoad > 180)
    engineLoad = 180;

  // Additional sounds volumes -----------------------------

  // Tire squealing ----
  uint8_t steeringAngle = 0;
  uint8_t brakeSquealVolume = 0;

  // Cornering squealing
  if (pulseWidth[1] < 1500)
    steeringAngle = map(pulseWidth[1], 1000, 1500, 100, 0);
  else if (pulseWidth[1] > 1500)
    steeringAngle = map(pulseWidth[1], 1500, 2000, 0, 100);
  else
    steeringAngle = 0;

  tireSquealVolume = steeringAngle * currentSpeed * currentSpeed / 125000; // Volume = steering angle * speed * speed

  // Brake squealing
  if ((driveState == 2 || driveState == 4) && currentSpeed > 50 && currentThrottle > 250)
  {
    tireSquealVolume += map(currentThrottle, 250, 500, 0, 100);
  }

  tireSquealVolume = constrain(tireSquealVolume, 0, 100);
}

//
// =======================================================================================================
// ENGINE MASS SIMULATION (running on core 0)
// =======================================================================================================
//

void engineMassSimulation()
{

  static int32_t targetRpm = 0;   // The engine RPM target
  static int32_t _currentRpm = 0; // Private current RPM (to prevent conflict with core 1)
  static int32_t _currentThrottle = 0;
  static int32_t lastThrottle;
  uint16_t converterSlip;
  static unsigned long throtMillis;
  static unsigned long wastegateMillis;
  static unsigned long blowoffMillis;
  uint8_t timeBase;

#ifdef SUPER_SLOW
  timeBase = 6; // super slow running, heavy engines, for example locomotive diesels
#else
  timeBase = 2;
#endif

  _currentThrottle = currentThrottle;

  if (millis() - throtMillis > timeBase)
  { // Every 2 or 6ms
    throtMillis = millis();

    if (_currentThrottle > 500)
      _currentThrottle = 500;

      // Virtual clutch **********************************************************************************
#if defined EXCAVATOR_MODE // Excavator mode ---
    clutchDisengaged = true;

    targetRpm = _currentThrottle - hydraulicLoad;
    targetRpm = constrain(targetRpm, 0, 500);

#else // Normal mode ---
    // if ((currentSpeed < clutchEngagingPoint && _currentRpm < maxClutchSlippingRpm) || gearUpShiftingInProgress || gearDownShiftingInProgress || neutralGear || _currentRpm < 200) { // TODO Bug?
    if ((currentSpeed < clutchEngagingPoint && _currentRpm < maxClutchSlippingRpm) || gearUpShiftingInProgress || gearDownShiftingInProgress || neutralGear)
    {
      clutchDisengaged = true;
    }
    else
    {
      clutchDisengaged = false;
    }

    // Transmissions ***********************************************************************************

    // automatic transmission ----
    if (automatic)
    {
      // Torque converter slip calculation
      if (selectedAutomaticGear < 2)
        converterSlip = engineLoad * torqueconverterSlipPercentage / 100 * 2; // more slip in first and reverse gear
      else
        converterSlip = engineLoad * torqueconverterSlipPercentage / 100;

      if (!neutralGear)
        targetRpm = currentSpeed * gearRatio[selectedAutomaticGear] / 10 + converterSlip; // Compute engine RPM
      else
        targetRpm = reMap(curveLinear, _currentThrottle);
    }
    else if (doubleClutch)
    {
      // double clutch transmission
      if (!neutralGear)
        targetRpm = currentSpeed * gearRatio[selectedAutomaticGear] / 10; // Compute engine RPM
      else
        targetRpm = reMap(curveLinear, _currentThrottle);
    }
    else
    {
      // Manual transmission ----
      if (clutchDisengaged)
      { // Clutch disengaged: Engine revving allowed
#if defined VIRTUAL_16_SPEED_SEQUENTIAL
        targetRpm = _currentThrottle;
#else
        targetRpm = reMap(curveLinear, _currentThrottle);

#endif
      }
      else
      {                                                                                             // Clutch engaged: Engine rpm synchronized with ESC power (speed)

#if defined VIRTUAL_3_SPEED || defined VIRTUAL_16_SPEED_SEQUENTIAL // Virtual 3 speed or sequential 16 speed transmission
        targetRpm = reMap(curveLinear, (currentSpeed * virtualManualGearRatio[selectedGear] / 10)); // Add virtual gear ratios
        if (targetRpm > 500)
          targetRpm = 500;

#elif defined STEAM_LOCOMOTIVE_MODE
        targetRpm = currentSpeed;

#else // Real 3 speed transmission
        targetRpm = reMap(curveLinear, currentSpeed);
#endif
      }
    }
#endif

    // Engine RPM **************************************************************************************

    if (escIsBraking && currentSpeed < clutchEngagingPoint)
      targetRpm = 0; // keep engine @idle rpm, if braking at very low speed
    if (targetRpm > 500)
      targetRpm = 500;

    // Accelerate engine
    if (targetRpm > (_currentRpm + acc) && (_currentRpm + acc) < maxRpm && engineState == RUNNING && engineRunning)
    {
      if (!airBrakeTrigger)
      { // No acceleration, if brake release noise still playing
        if (!gearDownShiftingInProgress)
          _currentRpm += acc;
        else
          _currentRpm += acc / 2; // less aggressive rpm rise while downshifting
        if (_currentRpm > maxRpm)
          _currentRpm = maxRpm;
      }
    }

    // Decelerate engine
    if (targetRpm < _currentRpm)
    {
      _currentRpm -= dec;
      if (_currentRpm < minRpm)
        _currentRpm = minRpm;
    }

#if (defined VIRTUAL_3_SPEED || defined VIRTUAL_16_SPEED_SEQUENTIAL) and not defined STEAM_LOCOMOTIVE_MODE
    // Limit top speed, depending on manual gear ratio. Ensures, that the engine will not blow up!
    if (!automatic && !doubleClutch)
      speedLimit = maxRpm * 10 / virtualManualGearRatio[selectedGear];
#endif

    // Speed (sample rate) output
    engineSampleRate = map(_currentRpm, minRpm, maxRpm, maxSampleInterval, minSampleInterval); // Idle

    // if ( xSemaphoreTake( xRpmSemaphore, portMAX_DELAY ) )
    //{
    currentRpm = _currentRpm;
    // xSemaphoreGive( xRpmSemaphore ); // Now free or "Give" the semaphore for others.
    // }
  }

  // Prevent Wastegate from being triggered while downshifting
  if (gearDownShiftingInProgress)
    wastegateMillis = millis();

  // Trigger Wastegate, if throttle rapidly dropped
  if (lastThrottle - _currentThrottle > 70 && !escIsBraking && millis() - wastegateMillis > 1000)
  {
    wastegateMillis = millis();
    wastegateTrigger = true;
  }

#if defined JAKEBRAKE_ENGINE_SLOWDOWN && defined JAKE_BRAKE_SOUND
  // Use jake brake to slow down engine while releasing throttle in neutral or during upshifting while applying throttle
  // for some vehicles like Volvo FH open pipe. See example: https://www.youtube.com/watch?v=MU1iwzl33Zw&list=LL&index=4
  if (!wastegateTrigger)
    blowoffMillis = millis();
  blowoffTrigger = ((gearUpShiftingInProgress || neutralGear) && millis() - blowoffMillis > 20 && millis() - blowoffMillis < 250);
#endif

  lastThrottle = _currentThrottle;
}

//
// =======================================================================================================
// SWITCH ENGINE ON OR OFF (for automatic mode)
// =======================================================================================================
//

void engineOnOff()
{

  // static unsigned long pulseDelayMillis; // TODO
  static unsigned long idleDelayMillis;

  // Engine automatically switched on or off depending on throttle position and 15s delay timne
  if (currentThrottle > 80 || driveState != 0)
    idleDelayMillis = millis(); // reset delay timer, if throttle not in neutral

#ifdef AUTO_ENGINE_ON_OFF
  if (millis() - idleDelayMillis > 15000)
  {
    engineOn = false; // after delay, switch engine off
  }
#endif

#ifdef AUTO_LIGHTS
  if (millis() - idleDelayMillis > 10000)
  {
    lightsOn = false; // after delay, switch light off
  }
#endif

  // Engine start detection
  if (currentThrottle > 100 && !airBrakeTrigger)
  {
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
void brakeLightsSub(uint8_t brightness)
{
  if (escIsBraking)
  {
    tailLight.pwm(255 - crankingDim);  // Taillights (full brightness)
    brakeLight.pwm(255 - crankingDim); // Brakelight on
  }
  else
  {
    tailLight.pwm(constrain(brightness - (crankingDim / 2), (brightness / 2), 255));  // Taillights (reduced brightness)
    brakeLight.pwm(constrain(brightness - (crankingDim / 2), (brightness / 2), 255)); // Brakelight (reduced brightness)
  }
}

// Headlights sub function ---------------------------------
void headLightsSub(bool head, bool fog, bool roof, bool park)
{

  fogLightOn = fog;

#ifdef XENON_LIGHTS // Optional xenon ignition flash
  if (millis() - xenonMillis > 50)
    xenonIgnitionFlash = 0;
  else
    xenonIgnitionFlash = 170; // bulb is brighter for 50ms
#endif

#ifdef SEPARATE_FULL_BEAM // separate full beam bulb, wired to "rooflight" pin ----
  // Headlights (low beam bulb)
  if (!head && !park)
  {
    headLight.off();
    xenonMillis = millis();
    if (!headLightsFlasherOn)
      headLightsHighBeamOn = false;
  }
  else if (park)
  { // Parking lights
    headLight.pwm(constrain(headlightParkingBrightness - crankingDim, (headlightParkingBrightness / 2), 255));
    xenonMillis = millis();
    if (!headLightsFlasherOn)
      headLightsHighBeamOn = false;
  }
  else
  { // ON
    headLight.pwm(constrain(255 - crankingDim - 170 + xenonIgnitionFlash, 0, 255));
  }
  // Headlights (high beam bulb)
  if (headLightsFlasherOn || (headLightsHighBeamOn && head))
    roofLight.pwm(200 - crankingDim);
  else
    roofLight.off();

#else  // Bulbs wired as labeled on the board ----
  // Headlights
  if (!head && !park)
  { // OFF or flasher
    if (!headLightsFlasherOn)
      headLight.off();
    else
      headLight.on();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }
  else if (park)
  { // Parking lights
    if (!headLightsFlasherOn)
      headLight.pwm(constrain(headlightParkingBrightness - crankingDim, (headlightParkingBrightness / 2), 255));
    else
      headLight.on();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }
  else
  { // ON
    headLight.pwm(constrain(255 - crankingDim - dipDim + xenonIgnitionFlash, 0, 255));
  }

  // Roof lights
  if (!roof)
    roofLight.off();
  else
    roofLight.pwm(130 - crankingDim);
#endif // ----

  // Fog lights
  if (!fog)
    fogLight.off();
  else
    fogLight.pwm(200 - crankingDim);
}

// Main LED function --------------------------------------------------------------------------------------
void led()
{

#if defined LED_INDICATORS
  indicatorFade = 0; // No soft indicator on / off, if LED
#endif

  // Lights brightness ----
#if defined FLICKERING_WHILE_CRANKING
  static unsigned long flickerMillis;
  if (millis() - flickerMillis > 30)
  { // Every 30ms
    flickerMillis = millis();
    if (engineStart)
      crankingDim = random(25, 55);
    else
      crankingDim = 0; // lights are dimmer and flickering while engine cranking
  }
#else
  if (engineStart)
    crankingDim = 50;
  else
    crankingDim = 0; // lights are dimmer while engine cranking
#endif

  if (headLightsFlasherOn || headLightsHighBeamOn)
    dipDim = 10;
  else
    dipDim = 170; // High / low beam and headlight flasher (SBUS CH5)

  // Reversing light ----
  if ((engineRunning || engineStart) && escInReverse)
    reversingLight.pwm(reversingLightBrightness - crankingDim);
  else
    reversingLight.off();

#if not defined SPI_DASHBOARD
    // Beacons (blue light) ----
#if not defined TRACKED_MODE // Normal beacons mode
  if (blueLightTrigger)
  {
    if (doubleFlashBlueLight)
    {
      beaconLight1.flash(30, 80, 400, 2);      // Simulate double flash lights
      beaconLight2.flash(30, 80, 400, 2, 330); // Simulate double flash lights (with delay for first pass)
    }
    else
    {
      beaconLight1.flash(30, 500, 0, 0);      // Simulate rotating beacon lights with short flashes
      beaconLight2.flash(30, 500, 0, 0, 100); // Simulate rotating beacon lights with short flashes
    }
  }
  else
  {
    beaconLight2.off();
    beaconLight1.off();
  }
#else // Beacons used for tank cannon fire simulation flash in TRACKED_MODE
  if (cannonFlash)
    beaconLight1.on();
  else
    beaconLight1.off();
#endif
#endif

  // Indicators (turn signals, blinkers) ----
  uint8_t indicatorOffBrightness;
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
  if (lightsState > 1)
    indicatorOffBrightness = rearlightDimmedBrightness - crankingDim / 2;
  else
    indicatorOffBrightness = 0;
#else
  indicatorOffBrightness = 0;
#endif

#ifdef HAZARDS_WHILE_5TH_WHEEL_UNLOCKED
  if (!hazard && !unlock5thWheel && !batteryProtection)
  { // Hazards also active, if 5th wheel unlocked
#else
  if (!hazard && !batteryProtection)
  {
#endif
    if (indicatorLon)
    {
      if (indicatorL.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness))
        indicatorSoundOn = true; // Left indicator
    }
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
    else
    {
      if (lightsState > 1)
        indicatorL.pwm(rearlightDimmedBrightness - crankingDim / 2);
      else
        indicatorL.off(indicatorFade);
    }
#else
    else
      indicatorL.off(indicatorFade);
#endif

    if (indicatorRon)
    {
      if (indicatorR.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness))
        indicatorSoundOn = true; // Left indicator
    }
#if defined INDICATOR_SIDE_MARKERS // Indicators used as US style side markers as well
    else
    {
      if (lightsState > 1)
        indicatorR.pwm(rearlightDimmedBrightness - crankingDim / 2);
      else
        indicatorR.off(indicatorFade);
    }
#else
    else
      indicatorR.off(indicatorFade);
#endif
  }
  else
  { // Hazard lights on, if no connection to transmitter (serial & SBUS control mode only)
    if (indicatorL.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness))
      indicatorSoundOn = true;
    indicatorR.flash(375, 375, 0, 0, 0, indicatorFade, indicatorOffBrightness);
  }

  // Headlights, tail lights ----
#ifdef AUTO_LIGHTS // automatic lights mode (deprecated, not maintained anymore!) ************************

#ifdef XENON_LIGHTS // Optional xenon ignition flash
  if (millis() - xenonMillis > 50)
    xenonIgnitionFlash = 0;
  else
    xenonIgnitionFlash = 170; // bulb is brighter for 50ms
#endif
  if (lightsOn && (engineRunning || engineStart))
  {
    headLight.pwm(constrain(255 - crankingDim - dipDim + xenonIgnitionFlash, 0, 255));
    brakeLightsSub(rearlightDimmedBrightness);
  }

  else
  {
    headLight.off();
    tailLight.off();
    brakeLight.off();
    xenonMillis = millis();
    headLightsHighBeamOn = false;
  }

  // Foglights ----
  if (lightsOn && engineRunning)
  {
    fogLight.pwm(200 - crankingDim);
    fogLightOn = true;
  }
  else
  {
    fogLight.off();
    fogLightOn = false;
  }

  // Roof lights ----
  if (lightsOn)
    roofLight.pwm(130 - crankingDim);
  else
    roofLight.off();

  // Sidelights ----
  if (engineOn)
    sideLight.pwm(200 - crankingDim);
  else
    sideLight.off();

  // Cabin lights ----
  if (!lightsOn)
    cabLight.pwm(255 - crankingDim);
  else
    cabLight.off();

#else // manual lights mode ************************
  // Lights state machine
  switch (lightsState)
  {

  case 0: // lights off ---------------------------------------------------------------------
    cabLight.off();
    sideLight.off();
    lightsOn = false;
    headLightsSub(false, false, false, false);
    brakeLightsSub(0); // 0 brightness, if not braking
    break;

  case 1:            // cab lights ---------------------------------------------------------------------
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

  case 4:            // roof & side & head & fog lights ---------------------------------------------------------------------
#ifdef NO_FOGLIGHTS
    lightsState = 5; // Skip foglights
#endif
    cabLight.off();
    sideLight.pwm(constrain(sideLightsBrightness - crankingDim, (sideLightsBrightness / 2), 255));
    headLightsSub(true, true, true, false);
    brakeLightsSub(rearlightDimmedBrightness); // 50 brightness, if not braking
    break;

  case 5:            // cab & roof & side & head & fog lights ---------------------------------------------------------------------
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

void shaker()
{
  int32_t shakerRpm = 0;

  // Set desired shaker rpm
  if (engineRunning)
    shakerRpm = map(currentRpm, minRpm, maxRpm, shakerIdle, shakerFullThrottle);
  if (engineStart)
    shakerRpm = shakerStart;
  if (engineStop)
    shakerRpm = shakerStop;

  // Shaker on / off
  if (engineRunning || engineStart || engineStop)
    shakerMotor.pwm(shakerRpm);
  else
    shakerMotor.off();
}

//
// =======================================================================================================
// MANUAL GEARBOX DETECTION (Real 3 speed, virtual 3 speed, virtual 16 speed, semi automatic)
// =======================================================================================================
//

void gearboxDetection()
{

  static uint8_t previousGear = 1;
  static bool previousReverse;
  static bool sequentialLock;
  static bool overdrive = false;
  static unsigned long upShiftingMillis;
  static unsigned long downShiftingMillis;
  static unsigned long lastShiftingMillis; // This timer is used to prevent transmission from oscillating!

#if defined TRACKED_MODE or defined STEAM_LOCOMOTIVE_MODE // CH2 is used for left throttle in TRACKED_MODE --------------------------------
  selectedGear = 2;

#else // only active, if not in TRACKED_MODE -------------------------------------------------------------

#if defined OVERDRIVE && defined VIRTUAL_3_SPEED // Additional 4th gear mode for virtual 3 speed ********************************
  if (!crawlerMode)
  {
    // The 4th gear (overdrive) is engaged automatically, if driving @ full throttle in 3rd gear
    if (currentRpm > 490 && selectedGear == 3 && engineLoad < 5 && currentThrottle > 490 && millis() - lastShiftingMillis > 2000)
    {
      overdrive = true;
    }
    if (!escIsBraking)
    { // Lower downshift point, if not braking
      if (currentRpm < 200 && millis() - lastShiftingMillis > 2000)
      {
        overdrive = false;
      }
    }
    else
    { // Higher downshift point, if braking
      if ((currentRpm < 400 || engineLoad > 150) && millis() - lastShiftingMillis > 2000)
      {
        overdrive = false;
      }
    }
    if (selectedGear < 3)
      overdrive = false;
  }
#endif                                           // End of overdrive ******************************************************************************************************

#if not defined VIRTUAL_16_SPEED_SEQUENTIAL && not defined SEMI_AUTOMATIC // 3 gears, selected by 3 position switch **************
  // Gear detection
  if (pulseWidth[2] > 1700)
    selectedGear = 3;
  else if (pulseWidth[2] < 1300)
    selectedGear = 1;
  else
    selectedGear = 2;
  if (overdrive && selectedGear == 3)
    selectedGear = 4;
#endif                                                                    // End of manual 3 speed *************************************************************************************************

#if defined VIRTUAL_16_SPEED_SEQUENTIAL // 16 gears, selected by up / down impulses *********************************************
  if (pulseWidth[2] > 1700 && selectedGear < 16 && !sequentialLock)
  {
    sequentialLock = true;
    selectedGear++;
  }
  else if (pulseWidth[2] < 1300 && selectedGear > 1 && !sequentialLock)
  {
    sequentialLock = true;
    selectedGear--;
  }
  if (pulseWidth[2] > 1400 && pulseWidth[2] < 1600)
    sequentialLock = false;
#endif                                  // End of VIRTUAL_16_SPEED_SEQUENTIAL *************************************************************************************

#if defined SEMI_AUTOMATIC // gears not controlled by the 3 position switch but by RPM limits ************************************
  if (currentRpm > 490 && selectedGear < 3 && engineLoad < 5 && currentThrottle > 490 && millis() - lastShiftingMillis > 2000)
  {
    selectedGear++;
  }
  if (!escIsBraking)
  { // Lower downshift point, if not braking
    if (currentRpm < 200 && selectedGear > 1 && millis() - lastShiftingMillis > 2000)
    {
      selectedGear--; //
    }
  }
  else
  { // Higher downshift point, if braking
    if ((currentRpm < 400 || engineLoad > 150) && selectedGear > 1 && millis() - lastShiftingMillis > 2000)
    {
      selectedGear--; // Higher downshift point, if braking
    }
  }
  if (neutralGear || escInReverse)
    selectedGear = 1;
#endif                     // End of SEMI_AUTOMATIC **************************************************************************************************

  // Gear upshifting detection
  if (selectedGear > previousGear)
  {
    gearUpShiftingInProgress = true;
    gearUpShiftingPulse = true;
    shiftingTrigger = true;
    previousGear = selectedGear;
    lastShiftingMillis = millis();
  }

  // Gear upshifting duration
  static uint16_t upshiftingDuration = 700;
  if (!gearUpShiftingInProgress)
    upShiftingMillis = millis();
  if (millis() - upShiftingMillis > upshiftingDuration)
  {
    gearUpShiftingInProgress = false;
  }
  // Double-clutch (Zwischengas während dem Hochschalten)
#if defined DOUBLE_CLUTCH
  upshiftingDuration = 900;
  doubleClutchInProgress = (millis() - upShiftingMillis >= 500 && millis() - upShiftingMillis < 600); // Apply full throttle
#endif

  // Gear downshifting detection
  if (selectedGear < previousGear)
  {
    gearDownShiftingInProgress = true;
    gearDownShiftingPulse = true;
    shiftingTrigger = true;
    previousGear = selectedGear;
    lastShiftingMillis = millis();
  }

  // Gear downshifting duration
  if (!gearDownShiftingInProgress)
    downShiftingMillis = millis();
  if (millis() - downShiftingMillis > 300)
  {
    gearDownShiftingInProgress = false;
  }

  // Reverse gear engaging / disengaging detection
  if (escInReverse != previousReverse)
  {
    previousReverse = escInReverse;
    shiftingTrigger = true; // Play shifting sound
  }

#ifdef MANUAL_TRANS_DEBUG
  static unsigned long manualTransDebugMillis;
  if (millis() - manualTransDebugMillis > 100)
  {
    manualTransDebugMillis = millis();
    Serial.printf("MANUAL_TRANS_DEBUG:\n");
    Serial.printf("currentThrottle: %i\n", currentThrottle);
    Serial.printf("selectedGear: %i\n", selectedGear);
    Serial.printf("overdrive: %i\n", overdrive);
    Serial.printf("engineLoad: %i\n", engineLoad);
    Serial.printf("sequentialLock: %s\n", sequentialLock ? "true" : "false");
    Serial.printf("currentRpm: %i\n", currentRpm);
    Serial.printf("currentSpeed: %i\n", currentSpeed);
    Serial.printf("---------------------------------\n");
  }
#endif // MANUAL_TRANS_DEBUG

#endif // End of not TRACKED_MODE -----------------------------------------------------------------------
}

//
// =======================================================================================================
// SIMULATED AUTOMATIC TRANSMISSION GEAR SELECTOR (running on core 0)
// =======================================================================================================
//

void automaticGearSelector()
{

  static unsigned long gearSelectorMillis;
  static unsigned long lastUpShiftingMillis;
  static unsigned long lastDownShiftingMillis;
  uint16_t downShiftPoint = 200;
  uint16_t upShiftPoint = 490;
  static int32_t _currentRpm = 0; // Private current RPM (to prevent conflict with core 1)

  // if ( xSemaphoreTake( xRpmSemaphore, portMAX_DELAY ) )
  //{
  _currentRpm = currentRpm;
  // xSemaphoreGive( xRpmSemaphore ); // Now free or "Give" the semaphore for others.
  // }

  if (millis() - gearSelectorMillis > 100)
  { // Waiting for 100ms is very important. Otherwise gears are skipped!
    gearSelectorMillis = millis();

    // compute load dependent shift points (less throttle = less rpm before shifting up, kick down will shift back!)
    upShiftPoint = map(engineLoad, 0, 180, 390, 490);   // 390, 490
    downShiftPoint = map(engineLoad, 0, 180, 150, 250); // 150, 250

    if (escInReverse)
    { // Reverse (only one gear)
      selectedAutomaticGear = 0;
    }
    else
    { // Forward (multiple gears)

      // Adaptive shift points
      if (millis() - lastDownShiftingMillis > 500 && _currentRpm >= upShiftPoint && engineLoad < 5)
      {                          // 500ms locking timer!
        selectedAutomaticGear++; // Upshifting (load maximum is important to prevent gears from oscillating!)
        lastUpShiftingMillis = millis();
      }
      if (millis() - lastUpShiftingMillis > 600 && selectedAutomaticGear > 1 && (_currentRpm <= downShiftPoint || engineLoad > 100))
      {                          // 600ms locking timer! TODO was 1000
        selectedAutomaticGear--; // Downshifting incl. kickdown
        lastDownShiftingMillis = millis();
      }

      selectedAutomaticGear = constrain(selectedAutomaticGear, 1, NumberOfAutomaticGears);
    }

#ifdef AUTO_TRANS_DEBUG
    Serial.printf("AUTO_TRANS_DEBUG:\n");
    Serial.printf("currentThrottle: %i\n", currentThrottle);
    Serial.printf("selectedAutomaticGear: %i\n", selectedAutomaticGear);
    Serial.printf("engineLoad: %i\n", engineLoad);
    Serial.printf("upShiftPoint: %i\n", upShiftPoint);
    Serial.printf("_currentRpm: %i\n", _currentRpm);
    Serial.printf("downShiftPoint: %i\n", downShiftPoint);
    Serial.printf("-----------------------------------\n");
#endif
  }
}

//
// =======================================================================================================
// ESC CONTROL (including optional battery protection)
// =======================================================================================================
//

static uint16_t escPulseWidth = 1500;
static uint16_t escPulseWidthOut = 1500;
static uint16_t escSignal = 1500;
static uint8_t motorDriverDuty = 0;
static unsigned long escMillis;
static unsigned long lastStateTime;
// static int8_t pulse; // -1 = reverse, 0 = neutral, 1 = forward
// static int8_t escPulse; // -1 = reverse, 0 = neutral, 1 = forward
static int8_t driveRampRate;
static int8_t driveRampGain;
static int8_t brakeRampRate;
uint16_t escRampTime;

// ESC sub functions =============================================
// We always need the data up to date, so these comparators are programmed as sub functions!
int8_t pulse()
{ // Throttle direction
  int8_t pulse;
  if (pulseWidth[3] > pulseMaxNeutral[3] && pulseWidth[3] < pulseMaxLimit[3])
    pulse = 1; // 1 = Forward
  else if (pulseWidth[3] < pulseMinNeutral[3] && pulseWidth[3] > pulseMinLimit[3])
    pulse = -1; // -1 = Backwards
  else
    pulse = 0; // 0 = Neutral
  return pulse;
}
int8_t escPulse()
{ // ESC direction
  int8_t escPulse;
  if (escPulseWidth > pulseMaxNeutral[3] && escPulseWidth < pulseMaxLimit[3])
    escPulse = 1; // 1 = Forward
  else if (escPulseWidth < pulseMinNeutral[3] && escPulseWidth > pulseMinLimit[3])
    escPulse = -1; // -1 = Backwards
  else
    escPulse = 0; // 0 = Neutral
  return escPulse;
}

// If you connect your ESC to pin 33, the vehicle inertia is simulated. Direct brake (crawler) ESC required
// *** WARNING!! Do it at your own risk!! There is a falisafe function in case, the signal input from the
// receiver is lost, but if the ESP32 crashes, the vehicle could get out of control!! ***

void esc()
{ // ESC main function ================================

  // Battery protection --------------------------------
#if defined BATTERY_PROTECTION
  static unsigned long lastBatteryTime;
  static bool outOfFuelMessageLock;
  if (millis() - lastBatteryTime > 300)
  { // Check battery voltage every 300ms
    lastBatteryTime = millis();
    batteryVoltage = batteryVolts(); // Store voltage in global variable (also used in dashboard)
    if (batteryVoltage < batteryCutoffvoltage)
    {
      Serial.printf("Battery protection triggered, slowing down! Battery: %.2f V Threshold: %.2f V \n", batteryVoltage, batteryCutoffvoltage);
      Serial.printf("Disconnect battery to prevent it from overdischarging!\n", batteryVoltage, batteryCutoffvoltage);
      batteryProtection = true;
    }
    if (batteryVoltage > batteryCutoffvoltage + (FULLY_CHARGED_VOLTAGE * numberOfCells))
    { // Recovery hysteresis
      batteryProtection = false;
    }
    // Out of fuel message triggering
    if (batteryProtection && !outOfFuelMessageLock)
    {
      outOfFuelMessageTrigger = true;
      outOfFuelMessageLock = true; // only trigger message once
    }
  }
#endif // --------------------------------------------

#if not defined TRACKED_MODE && not defined AIRPLANE_MODE // No ESC control in TRACKED_MODE or in AIRPLANE_MODE
  // Gear dependent ramp speed for acceleration & deceleration
#if defined VIRTUAL_3_SPEED
  escRampTime = escRampTimeThirdGear * 10 / virtualManualGearRatio[selectedGear];

#elif defined VIRTUAL_16_SPEED_SEQUENTIAL
  escRampTime = escRampTimeThirdGear * virtualManualGearRatio[selectedGear] / 5;

#elif defined STEAM_LOCOMOTIVE_MODE
  escRampTime = escRampTimeSecondGear;

#else // TAMIYA 3 speed shifting transmission
  if (selectedGear == 1)
    escRampTime = escRampTimeFirstGear; // about 20
  if (selectedGear == 2)
    escRampTime = escRampTimeSecondGear; // about 50
  if (selectedGear == 3)
    escRampTime = escRampTimeThirdGear; // about 75
#endif

  if (automatic || doubleClutch)
  {
    escRampTime = escRampTimeSecondGear; // always use 2nd gear acceleration for automatic transmissions
    if (escInReverse)
      escRampTime = escRampTime * 100 / automaticReverseAccelerationPercentage; // faster acceleration in automatic reverse, EXPERIMENTAL, TODO!
  }

  // Allows to scale vehicle file dependent acceleration
  escRampTime = escRampTime * 100 / globalAccelerationPercentage;

  // ESC ramp time compensation in low range
  if (lowRange)
    escRampTime = escRampTime * lowRangePercentage / 100;

  // Drive mode -------------------------------------------
  // Crawler mode for direct control -----
  crawlerMode = (masterVolume <= masterVolumeCrawlerThreshold); // Direct control, depending on master volume

  if (crawlerMode)
  { // almost no virtual inertia (just for drive train protection), for crawling competitions
    escRampTime = crawlerEscRampTime;
    brakeRampRate = map(currentThrottle, 0, 500, 1, 10);
    driveRampRate = 10;
  }
  else
  { // Virtual inertia mode -----
    // calulate throttle dependent brake & acceleration steps
    brakeRampRate = map(currentThrottle, 0, 500, 1, escBrakeSteps);
    driveRampRate = map(currentThrottle, 0, 500, 1, escAccelerationSteps);
  } // ----------------------------------------------------

  // Emergency ramp rates for falisafe
  if (failSafe)
  {
    brakeRampRate = escBrakeSteps;
    driveRampRate = escBrakeSteps;
  }

  // Additional brake detection signal, applied immediately. Used to prevent sound issues, if braking very quickly
  brakeDetect = ((pulse() == 1 && escPulse() == -1) || (pulse() == -1 && escPulse() == 1));

#ifdef ESC_DEBUG
  if (millis() - lastStateTime > 300)
  { // Print the data every 300ms
    lastStateTime = millis();
    Serial.printf("ESC_DEBUG:\n");
    Serial.printf("driveState:            %i\n", driveState);
    Serial.printf("pulse():               %i\n", pulse());
    Serial.printf("escPulse():            %i\n", escPulse());
    Serial.printf("brakeDetect:           %s\n", brakeDetect ? "true" : "false");
    Serial.printf("escPulseMin:           %i\n", escPulseMin);
    Serial.printf("escPulseMinNeutral:    %i\n", escPulseMinNeutral);
    Serial.printf("escPulseMaxNeutral:    %i\n", escPulseMaxNeutral);
    Serial.printf("escPulseMax:           %i\n", escPulseMax);
    Serial.printf("brakeRampRate:         %i\n", brakeRampRate);
    Serial.printf("lowRange:              %s\n", lowRange ? "true" : "false");
    Serial.printf("currentRpm:            %i\n", currentRpm);
    Serial.printf("escPulseWidth:         %i\n", escPulseWidth);
    Serial.printf("escPulseWidthOut:      %i\n", escPulseWidthOut);
    Serial.printf("escSignal:             %i\n", escSignal);
    Serial.printf("motorDriverDuty:       %i\n", motorDriverDuty);
    Serial.printf("currentSpeed:          %i\n", currentSpeed);
    Serial.printf("speedLimit:            %i\n", speedLimit);
    Serial.printf("batteryProtection:     %s\n", batteryProtection ? "true" : "false");
    Serial.printf("batteryVoltage:        %.2f\n", batteryVoltage);
    Serial.printf("--------------------------------------\n");
  }
#endif // ESC_DEBUG

  if (millis() - escMillis > escRampTime)
  { // About very 20 - 75ms
    escMillis = millis();

    // Drive state state machine **********************************************************************************
    switch (driveState)
    {

    case 0: // Standing still ---------------------------------------------------------------------
      escIsBraking = false;
      escInReverse = false;
      escIsDriving = false;
      escPulseWidth = pulseZero[3]; // ESC to neutral position
#ifdef VIRTUAL_16_SPEED_SEQUENTIAL
      selectedGear = 1;
#endif

      if (pulse() == 1 && engineRunning && !neutralGear)
        driveState = 1; // Driving forward
      if (pulse() == -1 && engineRunning && !neutralGear)
        driveState = 3; // Driving backwards
      break;

    case 1: // Driving forward ---------------------------------------------------------------------
      escIsBraking = false;
      escInReverse = false;
      escIsDriving = true;
      if (escPulseWidth < pulseWidth[3] && currentSpeed < speedLimit && !batteryProtection)
      {
        if (escPulseWidth >= escPulseMaxNeutral)
          escPulseWidth += (driveRampRate * driveRampGain); // Faster
        else
          escPulseWidth = escPulseMaxNeutral; // Initial boost
      }
      if ((escPulseWidth > pulseWidth[3] || batteryProtection) && escPulseWidth > pulseZero[3])
        escPulseWidth -= (driveRampRate * driveRampGain); // Slower

      if (gearUpShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch)
      {                                                                    // lowering RPM, if shifting up transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed transmission
        escPulseWidth -= currentSpeed / 4;                                 // Synchronize engine speed
                                                                           // escPulseWidth -= currentSpeed * 40 / 100; // Synchronize engine speed TODO
#endif
        gearUpShiftingPulse = false;
        escPulseWidth = constrain(escPulseWidth, pulseZero[3], pulseMax[3]);
      }
      if (gearDownShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch)
      {                                                                    // increasing RPM, if shifting down transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed transmission
        escPulseWidth += 50;                                               // Synchronize engine speed
                                                                           // escPulseWidth += currentSpeed;// * 40 / 100; // Synchronize engine speed TODO
#endif
        gearDownShiftingPulse = false;
        escPulseWidth = constrain(escPulseWidth, pulseZero[3], pulseMax[3]);
      }

      if (pulse() == -1 && escPulse() == 1)
        driveState = 2; // Braking forward
      if (pulse() == -1 && escPulse() == 0)
        driveState = 3; // Driving backwards, if ESC not yet moving. Prevents state machine from hanging! v9.7.0
      if (pulse() == 0 && escPulse() == 0)
        driveState = 0; // standing still
      break;

    case 2: // Braking forward ---------------------------------------------------------------------
      escIsBraking = true;
      escInReverse = false;
      escIsDriving = false;
      if (escPulseWidth > pulseZero[3])
        escPulseWidth -= brakeRampRate; // brake with variable deceleration
      if (escPulseWidth < pulseZero[3] + brakeMargin && pulse() == -1)
        escPulseWidth = pulseZero[3] + brakeMargin; // Don't go completely back to neutral, if brake applied
      if (escPulseWidth < pulseZero[3] && pulse() == 0)
        escPulseWidth = pulseZero[3]; // Overflow prevention!

      if (pulse() == 0 && escPulse() == 1 && !neutralGear)
      {
        driveState = 1; // Driving forward
        airBrakeTrigger = true;
      }
      if (pulse() == 0 && escPulse() == 0)
      {
        driveState = 0; // standing still
        airBrakeTrigger = true;
      }
      break;

    case 3: // Driving backwards ---------------------------------------------------------------------
      escIsBraking = false;
      escInReverse = true;
      escIsDriving = true;
      if (escPulseWidth > pulseWidth[3] && currentSpeed < speedLimit && !batteryProtection)
      {
        if (escPulseWidth <= escPulseMinNeutral)
          escPulseWidth -= (driveRampRate * driveRampGain); // Faster
        else
          escPulseWidth = escPulseMinNeutral; // Initial boost
      }
      if ((escPulseWidth < pulseWidth[3] || batteryProtection) && escPulseWidth < pulseZero[3])
        escPulseWidth += (driveRampRate * driveRampGain); // Slower

      if (gearUpShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch)
      {                                                                    // lowering RPM, if shifting up transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed transmission
        escPulseWidth += currentSpeed / 4;                                 // Synchronize engine speed
#endif
        gearUpShiftingPulse = false;
        escPulseWidth = constrain(escPulseWidth, pulseMin[3], pulseZero[3]);
      }
      if (gearDownShiftingPulse && shiftingAutoThrottle && !automatic && !doubleClutch)
      {                                                                    // increasing RPM, if shifting down transmission
#if not defined VIRTUAL_3_SPEED && not defined VIRTUAL_16_SPEED_SEQUENTIAL // Only, if we have a real 3 speed transmission
        escPulseWidth -= 50;                                               // Synchronize engine speed
#endif
        gearDownShiftingPulse = false;
        escPulseWidth = constrain(escPulseWidth, pulseMin[3], pulseZero[3]);
      }

      if (pulse() == 1 && escPulse() == -1)
        driveState = 4; // Braking backwards
      if (pulse() == 1 && escPulse() == 0)
        driveState = 1; // Driving forward, if ESC not yet moving. Prevents state machine from hanging! v9.7.0
      if (pulse() == 0 && escPulse() == 0)
        driveState = 0; // standing still
      break;

    case 4: // Braking backwards ---------------------------------------------------------------------
      escIsBraking = true;
      escInReverse = true;
      escIsDriving = false;
      if (escPulseWidth < pulseZero[3])
        escPulseWidth += brakeRampRate; // brake with variable deceleration
      if (escPulseWidth > pulseZero[3] - brakeMargin && pulse() == 1)
        escPulseWidth = pulseZero[3] - brakeMargin; // Don't go completely back to neutral, if brake applied
      if (escPulseWidth > pulseZero[3] && pulse() == 0)
        escPulseWidth = pulseZero[3]; // Overflow prevention!

      if (pulse() == 0 && escPulse() == -1 && !neutralGear)
      {
        driveState = 3; // Driving backwards
        airBrakeTrigger = true;
      }
      if (pulse() == 0 && escPulse() == 0)
      {
        driveState = 0; // standing still
        airBrakeTrigger = true;
      }
      break;

    } // End of state machine **********************************************************************************

    // Gain for drive ramp rate, depending on clutchEngagingPoint
    if (currentSpeed < clutchEngagingPoint)
    {
      if (!automatic && !doubleClutch)
        driveRampGain = 2; // prevent clutch from slipping too much (2)
      else
        driveRampGain = 4; // Automatic transmission needs to catch immediately (4)
    }
    else
      driveRampGain = 1;

      // ESC linearity compensation ---------------------
#ifdef QUICRUN_FUSION
    escPulseWidthOut = reMap(curveQuicrunFusion, escPulseWidth);
#elif defined QUICRUN_16BL30
    escPulseWidthOut = reMap(curveQuicrun16BL30, escPulseWidth);
#else
    escPulseWidthOut = escPulseWidth;
#endif // --------------------------------------------

    // ESC range & direction calibration -------------
#ifndef ESC_DIR
    // escSignal = escPulseWidthOut;
    escSignal = map(escPulseWidthOut, escPulseMin, escPulseMax, 1000, 2000);
#else
    escSignal = map(escPulseWidthOut, escPulseMax, escPulseMin, 1000, 2000); // direction inversed
#endif // --------------------------------------------

#if not defined RZ7886_DRIVER_MODE                                             // Classic crawler style RC ESC mode ----
    mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, escSignal); // ESC now using MCPWM

#else // RZ 7886 motor driver mode ----
    // Note: according to the datasheet, the driver outputs are open, if both inputs are low. In order to achieve a good linearity and proportional brake,
    // we need to make sure, that both inputs never are low @ the same time! If both inputs are high @ the same time, both outputs are low and connecting
    // both motor outputs together. This will brake the motor. This state is also enabling drag brake in neutral.

    if (escSignal > 1500)
    { // Forward
      motorDriverDuty = map(escSignal, 1500, 2000, 0, 100);
      mcpwm_set_signal_high(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A); // Pin A high!
      mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, motorDriverDuty);
      mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_1); // MCPWM_DUTY_MODE_1 = inverse PWM mode, high, if 0% PWM
    }
    else if (escSignal < 1500)
    { // Reverse
      motorDriverDuty = map(escSignal, 1500, 1000, 0, 100);
      mcpwm_set_signal_high(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B); // Pin B high!
      mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, motorDriverDuty);
      mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_1);
    }
    else
    {                      // Neutral
      motorDriverDuty = 0; // Just for cosmetic reasons in ESC_DEBUG

      // Both pins pwm @ the same time = variable drag brake
      mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, RZ7886_DRAGBRAKE_DUTY);
      mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, RZ7886_DRAGBRAKE_DUTY);
      mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
      mcpwm_set_duty_type(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    }
#endif

    // Calculate a speed value from the pulsewidth signal (used as base for engine sound RPM while clutch is engaged)
    if (escPulseWidth > pulseMaxNeutral[3])
    {
      currentSpeed = map(escPulseWidth, pulseMaxNeutral[3], pulseMax[3], 0, 500);
    }
    else if (escPulseWidth < pulseMinNeutral[3])
    {
      currentSpeed = map(escPulseWidth, pulseMinNeutral[3], pulseMin[3], 0, 500);
    }
    else
      currentSpeed = 0;
  }
#endif
}

//
// =======================================================================================================
// BATTERY MONITORING
// =======================================================================================================
//

float batteryVolts()
{
  static float raw[6];
  static bool initDone = false;
#define VOLTAGE_CALIBRATION (RESISTOR_TO_BATTTERY_PLUS + RESISTOR_TO_GND) / RESISTOR_TO_GND + DIODE_DROP

  if (!initDone)
  { // Init array, if first measurement (important for call in setup)!
    for (uint8_t i = 0; i <= 5; i++)
    {
      raw[i] = battery.readVoltage();
    }
    initDone = true;
  }

  raw[5] = raw[4]; // Move array content, then add latest measurement (averaging)
  raw[4] = raw[3];
  raw[3] = raw[2];
  raw[2] = raw[1];
  raw[1] = raw[0];

  raw[0] = battery.readVoltage(); // read analog input

  float voltage = (raw[0] + raw[1] + raw[2] + raw[3] + raw[4] + raw[5]) / 6 * VOLTAGE_CALIBRATION;
  return voltage;
}

//
// =======================================================================================================
// LOOP TIME MEASUREMENT
// =======================================================================================================
//

unsigned long loopDuration()
{
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

void triggerHorn()
{

  if (!winchEnabled && !unlock5thWheel && !hazard)
  { // Horn & siren control mode *************
    winchPull = false;
    winchRelease = false;
    legsUp = false;
    legsDown = false;
    rampsUp = false;
    rampsDown = false;

    // detect horn trigger ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4])
    {
      hornTrigger = true;
      hornLatch = true;
    }
    else
    {
      hornTrigger = false;
    }

#if not defined EXCAVATOR_MODE
#ifndef NO_SIREN
    // detect siren trigger ( impulse length < 1100us) ----------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4])
    {
      sirenTrigger = true;
      sirenLatch = true;
    }
    else
    {
      sirenTrigger = false;
    }
#endif
#endif

    // detect bluelight trigger ( impulse length < 1300us) ----------
    static uint32_t bluelightOffDelay = millis();
    if ((pulseWidth[4] < 1300 && pulseWidth[4] > pulseMinLimit[4]) || sirenLatch)
    {
      bluelightOffDelay = millis();
      blueLightTrigger = true;
    }
    if (millis() - bluelightOffDelay > 50)
    { // Switch off delay
      blueLightTrigger = false;
    }
  }

  else if (unlock5thWheel)
  { // Trailer leg control mode *************************************
    winchPull = false;
    winchRelease = false;
    rampsUp = false;
    rampsDown = false;

    // legs down ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4])
      legsDown = true;
    else
      legsDown = false;

    // legs up ( impulse length < 1100us) -------------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4])
      legsUp = true;
    else
      legsUp = false;
  }

  else if (hazard)
  { // Trailer ramps control mode ***************************************
    winchPull = false;
    winchRelease = false;
    legsUp = false;
    legsDown = false;

    // ramps down ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4])
      rampsDown = true;
    else
      rampsDown = false;

    // ramps up ( impulse length < 1100us) -------------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4])
      rampsUp = true;
    else
      rampsUp = false;
  }

  else
  { // Winch control mode *****************************************************************
    legsUp = false;
    legsDown = false;
    rampsUp = false;
    rampsDown = false;

    // pull winch ( impulse length > 1900us) -------------
    if (pulseWidth[4] > 1900 && pulseWidth[4] < pulseMaxLimit[4])
      winchPull = true;
    else
      winchPull = false;

    // release winch ( impulse length < 1100us) -------------
    if (pulseWidth[4] < 1100 && pulseWidth[4] > pulseMinLimit[4])
      winchRelease = true;
    else
      winchRelease = false;
  }
}

//
// =======================================================================================================
// INDICATOR (TURN SIGNAL) TRIGGERING
// =======================================================================================================
//

void triggerIndicators()
{

#if not defined EXCAVATOR_MODE // Only used, if our vehicle is not an excavator!

  static boolean L;
  static boolean R;

#ifdef AUTO_INDICATORS // Automatic, steering triggered indicators ********
  // detect left indicator trigger -------------
  if (pulseWidth[1] > (1500 + indicatorOn))
  {
    L = true;
    R = false;
  }
  if (pulseWidth[1] < (1500 + indicatorOn / 3))
    L = false;

  // detect right indicator trigger -------------
  if (pulseWidth[1] < (1500 - indicatorOn))
  {
    R = true;
    L = false;
  }
  if (pulseWidth[1] > (1500 - indicatorOn / 3))
    R = false;

#else // Manually triggered indicators ********
  // detect left indicator trigger -------------
  if (pulseWidth[6] > 1900)
  {
    L = true;
    R = false;
  }
  if (pulseWidth[6] < (1500 - indicatorOn / 3))
    L = false;

  // detect right indicator trigger -------------
  if (pulseWidth[6] < 1100)
  {
    R = true;
    L = false;
  }
  if (pulseWidth[6] > (1500 + indicatorOn / 3))
    R = false;

  // Reset by steering -------------
  static int steeringOld;

  if (pulseWidth[1] < steeringOld - 50)
  {
    L = false;
    steeringOld = pulseWidth[1];
  }

  if (pulseWidth[1] > steeringOld + 50)
  {
    R = false;
    steeringOld = pulseWidth[1];
  }

#endif // End of manually triggered indicators

  // Indicator direction
  if (!INDICATOR_DIR)
  {
    indicatorLon = L;
    indicatorRon = R;
  }
  else
  {
    indicatorLon = R;
    indicatorRon = L;
  }

  if (indicatorLon || indicatorRon)
    hazard = false;

#endif
}

//
// =======================================================================================================
// RC TRIGGER SECTION (rcTrigger LIBRARY REQUIRED)
// =======================================================================================================
//

void rcTriggerRead()
{

#if not defined EXCAVATOR_MODE // Only used, if our vehicle is not an excavator!

  // Channel assignment see "remoteSetup.xlsx"

  // Potentiometers or 3 position switches ******************************************************************

  // CH5 (FUNCTION_R) ----------------------------------------------------------------------
  // Cycling light state machine, if dual rate @75% and long in position -----
  static bool lightsStateLock;
  if (functionR75u.toggleLong(pulseWidth[5], 1150) != lightsStateLock)
  {
    if (lightsState >= 5)
      lightsState = 0;
    else
      lightsState++;
    lightsStateLock = !lightsStateLock;
  }

  // Toggling high / low beam, if dual rate @100% and short in position
  static bool beamStateLock;
  if (functionR100u.toggleLong(pulseWidth[5], 1000) != beamStateLock)
  {
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
  if (driveState == 0)
  {
    // if (functionR100d.toggleLong(pulseWidth[5], 2000)) masterVolume = masterVolumePercentage[1]; else masterVolume = masterVolumePercentage[0]; // Change volume between indoor and outdoor mode
    if (functionR100d.toggleLong(pulseWidth[5], 2000) != volumeStateLock)
    {
      if (volumeIndex < numberOfVolumeSteps - 1)
        volumeIndex++; // Switch volume steps
      else
        volumeIndex = 0;
      volumeStateLock = !volumeStateLock;
    }
    masterVolume = masterVolumePercentage[volumeIndex]; // Write volume
  }

  // Engine on / off, if dual rate @75% and long in position -----
#ifndef AUTO_ENGINE_ON_OFF
  static bool engineStateLock;
  if (driveState == 0 && (engineState == OFF || engineState == RUNNING))
  { // Only, if vehicle stopped and engine idling or off!
    if (functionR75d.toggleLong(pulseWidth[5], 1850) != engineStateLock)
    {
      engineOn = !engineOn; // This lock is required, because engine on / off needs to be able to be changed in other program sections!
      engineStateLock = !engineStateLock;
    }
  }
#endif

  // CH6 (FUNCTION_L) ----------------------------------------------------------------------

  // Indicators are triggered in triggerIndicators()

  // Hazards on / off, if dual rate @75% and long in position -----
#ifndef AUTO_INDICATORS
  static bool hazardStateLock;
  if (functionL75l.toggleLong(pulseWidth[6], 1150) != hazardStateLock)
  {
    hazard = !hazard;
    hazardStateLock = !hazardStateLock;
  }
#endif

  // Couple / uncouple 5th wheel, if dual rate @75% and long in position -----
  static bool fifthWheelStateLock;
  if (driveState == 0)
  { // Only allow change, if vehicle stopped!
    if (functionL75r.toggleLong(pulseWidth[6], 1850) != fifthWheelStateLock)
    {
      unlock5thWheel = !unlock5thWheel;
      fifthWheelStateLock = !fifthWheelStateLock;
    }
  }

  // Latching 2 position switches ******************************************************************

  // Mode 1 ----
  mode1 = mode1Trigger.onOff(pulseWidth[8], 1800, 1200); // CH8 (MODE1)
#ifdef TRANSMISSION_NEUTRAL
  neutralGear = mode1; // Transmission neutral
#endif

  // Mode 2 ----
  mode2 = mode2Trigger.onOff(pulseWidth[9], 1800, 1200); // CH9 (MODE2)

#if defined MODE2_WINCH // Winch control mode
  if (mode2)
    winchEnabled = true;
  else
    winchEnabled = false;

#elif defined MODE2_TRAILER_UNLOCKING // 5th wheel unlocking mode
  static bool fifthWheelStateLock2;
  if (driveState == 0)
  { // Only allow change, if vehicle stopped!
    if (mode2 != fifthWheelStateLock2)
    {
      unlock5thWheel = !unlock5thWheel;
      fifthWheelStateLock2 = !fifthWheelStateLock2;
    }
  }

#else // Sound 1 triggering mode
  if (mode2)
    sound1trigger = true; // Trigger sound 1 (It is reset after playback is done
#endif

    // Momentary buttons ******************************************************************
    // Engine on / off momentary button CH10 -----
#ifndef AUTO_ENGINE_ON_OFF
  static bool engineStateLock2;
  if (driveState == 0 && (engineState == OFF || engineState == RUNNING))
  { // Only, if vehicle stopped and engine idling or off!
    if (momentary1Trigger.toggleLong(pulseWidth[10], 2000) != engineStateLock2)
    {
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

void trailerPresenceSwitchRead()
{
#if not defined THIRD_BRAKELIGHT and not defined RZ7886_DRIVER_MODE
  static unsigned long switchMillis;
  static boolean couplerSwitchStateLatch;

  if (couplerSwitchInteruptLatch)
  {
    switchMillis = millis();
    couplerSwitchInteruptLatch = false;
    couplerSwitchStateLatch = true;
  }

  if (couplerSwitchStateLatch && millis() - switchMillis > 10)
  { // Debouncing delay
    if (digitalRead(COUPLER_SWITCH_PIN))
    {
      couplingTrigger = true;
      trailerDetected = true;
      couplerSwitchStateLatch = false;
    }
    else
    {
      uncouplingTrigger = true;
      trailerDetected = false;
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

    Speed needle jump if shifting real 3 speed transmission. Better calibration needed
*/

bool engineStartAnimation()
{
  static bool dirUp = true;
  static uint32_t lastFrameTime = millis();
  static uint16_t rpm = 0;
  static uint16_t speed = 0;
  static int16_t fuel = 0;
  static uint16_t adblue = 0;
  static int16_t currentStep = 0; // 50 in total

  if (millis() - lastFrameTime > 14)
  {
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

    if (dirUp && currentStep >= 50)
    {
      speed = SPEED_MAX;
      rpm = RPM_MAX;
      fuel = FUEL_MAX;
      adblue = ADBLUE_MAX;
      currentStep = 50;
      dirUp = false;
    }
    else if (!dirUp && currentStep <= 0)
    { // Animation finished
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
void updateDashboard()
{
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
  if ((engineState == STARTING || engineState == RUNNING) && !startAnimationFinished)
  {
    startAnimationFinished = engineStartAnimation();
    return;
  }
  else if (engineState == OFF)
  {
    startAnimationFinished = false;
  }

  // Calculations
  // RPM, fuel, adblue
  if (engineState == STARTING || engineState == RUNNING)
  {
    rpm = currentRpm * 450 / 500 + 50; // Idle rpm offset!

#if defined BATTERY_PROTECTION
    fuel = map_Generic<float, float, float, int16_t, int16_t>((batteryVoltage / numberOfCells), CUTOFF_VOLTAGE, FULLY_CHARGED_VOLTAGE, 0, 100);
    if (fuel > 100)
      fuel = 100;
    if (fuel < 0)
      fuel = 0;
    if ((batteryVoltage / numberOfCells) < CUTOFF_VOLTAGE)
      fuel = 0;
#else
    fuel = 90;
#endif
    adblue = 80;
  }
  else
  {
    rpm = currentRpm;
    fuel = 0;
    adblue = 0;
  }

  // Speed
  uint16_t speed;

#if defined VIRTUAL_3_SPEED or defined VIRTUAL_16_SPEED_SEQUENTIAL or defined STEAM_LOCOMOTIVE_MODE
  speed = currentSpeed; // for all transmissions
#else
  if (!automatic)
    speed = currentSpeed * 100 / manualGearRatios[selectedGear - 1]; // Manual transmission
  else
    speed = currentSpeed; // Automatic transmission
#endif

  speed = map(speed, 0, RPM_MAX, 0, MAX_REAL_SPEED);

  if (!gearUpShiftingInProgress && !gearDownShiftingInProgress)
  { // avoid jumps between gear switches
    dashboard.setSpeed(speed);
  }

  // Central display
#if defined BATTERY_PROTECTION
  dashboard.setVolt(batteryVoltage, CUTOFF_VOLTAGE * numberOfCells);
#endif

  if (neutralGear)
  {
    dashboard.setGear(0);
  }
  else if (escInReverse)
  {
    dashboard.setGear(-1);
  }
  else
  {
    if (!automatic)
      dashboard.setGear(selectedGear); // Manual transmission
    else
      dashboard.setGear(selectedAutomaticGear); // Automatic transmission
  }

  // Indicator lamps
#ifdef HAZARDS_WHILE_5TH_WHEEL_UNLOCKED
  dashboard.setLeftIndicator(indicatorSoundOn && (indicatorLon || hazard || unlock5thWheel || batteryProtection));
  dashboard.setRightIndicator(indicatorSoundOn && (indicatorRon || hazard || unlock5thWheel || batteryProtection));
#else
  dashboard.setLeftIndicator(indicatorSoundOn && (indicatorLon || hazard || batteryProtection));
  dashboard.setRightIndicator(indicatorSoundOn && (indicatorRon || hazard || batteryProtection));
#endif
  dashboard.setLowBeamIndicator(lightsOn);
  dashboard.setHighBeamIndicator(headLightsHighBeamOn || headLightsFlasherOn);
  dashboard.setFogLightIndicator(fogLightOn);

  // Needles
  if (millis() - lastFrameTime > 14)
  {

    if (engineState == RUNNING)
    {
      rpmNeedle = rpm; // No delay, if running!
    }
    else
    {
      if (rpm > rpmNeedle)
        rpmNeedle++;
      if (rpm < rpmNeedle)
        rpmNeedle--;
    }

    if (fuel > fuelNeedle)
      fuelNeedle++;
    if (fuel < fuelNeedle)
      fuelNeedle--;

    if (adblue > adblueNeedle)
      adblueNeedle++;
    if (adblue < adblueNeedle)
      adblueNeedle--;

    dashboard.setRPM(rpmNeedle);
    dashboard.setFuelLevel(fuelNeedle);
    dashboard.setAdBlueLevel(adblueNeedle);
    lastFrameTime = millis();
  }
}

//
// =======================================================================================================
// NEOPIXEL WS2812 LED
// =======================================================================================================
//

void updateRGBLEDs()
{
#if defined NEOPIXEL_ENABLED

  static uint32_t lastNeopixelTime = millis();
  static bool knightRiderLatch = false;
  static bool unionJackLatch = false;
  static bool neopixelShow = false;

#ifdef NEOPIXEL_DEMO // Demo -------------------------------------------------------------
  if (millis() - lastNeopixelTime > 20)
  { // Every 20 ms
    lastNeopixelTime = millis();

    uint8_t hue = map(pulseWidth[1], 1000, 2000, 0, 255);

    rgbLEDs[0] = CHSV(hue, hue < 255 ? 255 : 0, hue > 0 ? 255 : 0);
    rgbLEDs[1] = CRGB::Red;
    rgbLEDs[2] = CRGB::White;
    rgbLEDs[3] = CRGB::Yellow;
    rgbLEDs[4] = CRGB::Blue;
    rgbLEDs[5] = CRGB::Green;
    neopixelShow = true;
  }
#endif

#ifdef NEOPIXEL_KNIGHT_RIDER // Knight Rider scanner -------------------------------------
  static int16_t increment = 1;
  static int16_t counter = 0;

  if (millis() - lastNeopixelTime > 91)
  { // Every 91 ms (must match with sound)
    lastNeopixelTime = millis();

    if (sirenTrigger || knightRiderLatch)
    { // Only active, if siren signal!
      if (counter >= NEOPIXEL_COUNT - 1)
        increment = -1;
      if (counter <= 0)
        increment = 1;
      knightRiderLatch = (counter > 0);
      rgbLEDs[counter] = CRGB::Red;
      counter += increment;
    }
    else
    {
      counter = 0;
    }
    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
      rgbLEDs[i].nscale8(160); // 160
    }
    neopixelShow = true;
  }
#endif

#ifdef NEOPIXEL_BLUELIGHT // Bluelight ----------------------------------------------------
  static uint32_t lastNeopixelBluelightTime = millis();

  if (millis() - lastNeopixelTime > 20)
  { // Every 20 ms
    lastNeopixelTime = millis();
    if (blueLightTrigger)
    {
      if (millis() - lastNeopixelBluelightTime > 0)
      { // Step 1
        rgbLEDs[0] = CRGB::Red;
        rgbLEDs[1] = CRGB::Blue;
        rgbLEDs[3] = CRGB::Red;
        rgbLEDs[4] = CRGB::Blue;
        rgbLEDs[6] = CRGB::Red;
        rgbLEDs[7] = CRGB::Blue;
      }
      if (millis() - lastNeopixelBluelightTime > 160)
      { // Step 2
        fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black);
      }
      if (millis() - lastNeopixelBluelightTime > 300)
      { // Step 3
        rgbLEDs[0] = CRGB::Blue;
        rgbLEDs[1] = CRGB::Red;
        rgbLEDs[3] = CRGB::Blue;
        rgbLEDs[4] = CRGB::Red;
        rgbLEDs[6] = CRGB::Blue;
        rgbLEDs[7] = CRGB::Red;
      }
      if (millis() - lastNeopixelBluelightTime > 460)
      { // Step 4
        fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black);
      }
      if (millis() - lastNeopixelBluelightTime > 600)
      { // Step 5
        lastNeopixelBluelightTime = millis();
      }
    }
    else
      fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black); // Off
    neopixelShow = true;
  }
#endif

#ifdef NEOPIXEL_UNION_JACK // United Kingdom animation ---------------------------------------
  static uint32_t lastNeopixelUnionJackTime = millis();
  static uint8_t animationStep = 1;

  if (sirenTrigger || unionJackLatch)
  {
    unionJackLatch = true;
    if (millis() - lastNeopixelUnionJackTime > 789)
    { // Every 789 ms (must match with sound "BritishNationalAnthemSiren.h")
      lastNeopixelUnionJackTime = millis();
      if (animationStep == 1 || animationStep == 9)
      { // Step 1 or 9
        rgbLEDs[0] = CRGB::Red;
        rgbLEDs[1] = CRGB::Blue;
        rgbLEDs[2] = CRGB::Blue;
        rgbLEDs[3] = CRGB::Red;
        rgbLEDs[4] = CRGB::Red;
        rgbLEDs[5] = CRGB::Blue;
        rgbLEDs[6] = CRGB::Blue;
        rgbLEDs[7] = CRGB::Red;
      }
      if (animationStep == 2 || animationStep == 8)
      { // Step 2 or 8
        rgbLEDs[0] = CRGB::White;
        rgbLEDs[1] = CRGB::Red;
        rgbLEDs[2] = CRGB::Blue;
        rgbLEDs[3] = CRGB::Red;
        rgbLEDs[4] = CRGB::Red;
        rgbLEDs[5] = CRGB::Blue;
        rgbLEDs[6] = CRGB::Red;
        rgbLEDs[7] = CRGB::White;
      }
      if (animationStep == 3 || animationStep == 7)
      { // Step 3 or 7
        rgbLEDs[0] = CRGB::Blue;
        rgbLEDs[1] = CRGB::White;
        rgbLEDs[2] = CRGB::Red;
        rgbLEDs[3] = CRGB::Red;
        rgbLEDs[4] = CRGB::Red;
        rgbLEDs[5] = CRGB::Red;
        rgbLEDs[6] = CRGB::White;
        rgbLEDs[7] = CRGB::Blue;
      }
      if (animationStep == 4 || animationStep == 5 || animationStep == 6)
      { // Step 4
        rgbLEDs[0] = CRGB::Red;
        rgbLEDs[1] = CRGB::Red;
        rgbLEDs[2] = CRGB::Red;
        rgbLEDs[3] = CRGB::Red;
        rgbLEDs[4] = CRGB::Red;
        rgbLEDs[5] = CRGB::Red;
        rgbLEDs[6] = CRGB::Red;
        rgbLEDs[7] = CRGB::Red;
      }
      if (animationStep == 10)
      { // Step 10
        fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black);
        animationStep = 0;
        unionJackLatch = false;
      }
      animationStep++;
      neopixelShow = true;
    }
  }
  else
  {
    animationStep = 1;
    lastNeopixelUnionJackTime = millis();
  }
#endif

#ifdef NEOPIXEL_HIGHBEAM // Neopixel bar is used as high beam as well --------------------
  static uint32_t lastNeopixelHighbeamTime = millis();
  if (millis() - lastNeopixelHighbeamTime > 20)
  { // Every 20 ms
    lastNeopixelHighbeamTime = millis();

    if (!knightRiderLatch && !sirenTrigger && !blueLightTrigger && !unionJackLatch)
    {
      if (headLightsHighBeamOn || headLightsFlasherOn)
        fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::White);
      else
        fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black);
    }
    neopixelShow = true;
  }
#endif

#ifdef NEOPIXEL_B33LZ3BUB // B33lz3bub Austria cab light ---------------------------------
  if (millis() - lastNeopixelTime > 20)
  { // Every 20 ms
    lastNeopixelTime = millis();

    uint8_t hue = map(pulseWidth[7], 1000, 2000, 0, 255); // map pulseWidth[7] from poti VRA to hue
    if (hue <= 20)
    { // LEDs off
      fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::Black);
    }
    else if (hue > 20 && hue < 235)
    { // different colors depending on potentiometer VRA CH7
      for (int i = 0; i < NEOPIXEL_COUNT; i++)
      {
        rgbLEDs[i] = CHSV(hue, hue < 255 ? 255 : 0, hue > 0 ? 255 : 0);
      }
    }
    else if (hue >= 235 && hue < 250)
    { // colors red-white-red -> flag color of Austria ;-)
      rgbLEDs[0] = CRGB::Red;
      rgbLEDs[1] = CRGB::White;
      rgbLEDs[2] = CRGB::Red;
    }
    else
    {
      fill_solid(rgbLEDs, NEOPIXEL_COUNT, CRGB::White); // only white
    }
    neopixelShow = true;
  }
#endif

  // Neopixel refresh for all options above ------------------------------------------------
  static uint32_t lastNeopixelRefreshTime = millis();
  if (millis() - lastNeopixelRefreshTime > 20 && neopixelShow)
  { // Every 20 ms
    lastNeopixelRefreshTime = millis();
    FastLED.show();
    neopixelShow = false;
  }
#endif
}

//
// =======================================================================================================
// EXCAVATOR CONTROL
// =======================================================================================================
//

void excavatorControl()
{

  static uint32_t lastFrameTime = millis();
  static uint16_t hydraulicPumpVolumeInternal[9];
  static uint16_t hydraulicPumpVolumeInternalUndelayed;
  static uint16_t hydraulicFlowVolumeInternalUndelayed;
  static uint16_t trackRattleVolumeInternal[9];
  static uint16_t trackRattleVolumeInternalUndelayed;
  static uint16_t lastBucketPulseWidth = pulseWidth[1];
  static uint16_t lastDipperPulseWidth = pulseWidth[2];

  if (millis() - lastFrameTime > 4)
  { // 3
    lastFrameTime = millis();

    // Calculate zylinder speed and engine RPM dependent hydraulic pump volume ----
    // Bucket ---
    if (pulseWidth[1] > pulseMaxNeutral[1])
      hydraulicPumpVolumeInternal[1] = map(pulseWidth[1], pulseMaxNeutral[1], pulseMax[1], 0, 100);
    else if (pulseWidth[1] < pulseMinNeutral[1])
      hydraulicPumpVolumeInternal[1] = map(pulseWidth[1], pulseMinNeutral[1], pulseMin[1], 0, 100);
    else
      hydraulicPumpVolumeInternal[1] = 0;

    // Dipper ---
    if (pulseWidth[2] > pulseMaxNeutral[2])
      hydraulicPumpVolumeInternal[2] = map(pulseWidth[2], pulseMaxNeutral[2], pulseMax[2], 0, 100);
    else if (pulseWidth[2] < pulseMinNeutral[2])
      hydraulicPumpVolumeInternal[2] = map(pulseWidth[2], pulseMinNeutral[2], pulseMin[2], 0, 100);
    else
      hydraulicPumpVolumeInternal[2] = 0;

    // Boom (upwards only) ---
    if (pulseWidth[5] < pulseMinNeutral[5])
      hydraulicPumpVolumeInternal[5] = map(pulseWidth[5], pulseMinNeutral[5], (pulseMin[5] + 200), 0, 100);
    else
      hydraulicPumpVolumeInternal[5] = 0;

    // Swing ---
    if (pulseWidth[8] > pulseMaxNeutral[8])
      hydraulicPumpVolumeInternal[8] = map(pulseWidth[8], pulseMaxNeutral[8], (pulseMax[8] - 150), 0, 100);
    else if (pulseWidth[8] < pulseMinNeutral[8])
      hydraulicPumpVolumeInternal[8] = map(pulseWidth[8], pulseMinNeutral[8], (pulseMin[8] + 150), 0, 100);
    else
      hydraulicPumpVolumeInternal[8] = 0;

    hydraulicPumpVolumeInternalUndelayed = constrain(hydraulicPumpVolumeInternal[1] + hydraulicPumpVolumeInternal[2] + hydraulicPumpVolumeInternal[5] + hydraulicPumpVolumeInternal[8], 0, 100) * map(currentRpm, 0, 500, 30, 100) / 100;

    if (hydraulicPumpVolumeInternalUndelayed < hydraulicPumpVolume)
      hydraulicPumpVolume--;
    if (hydraulicPumpVolumeInternalUndelayed > hydraulicPumpVolume)
      hydraulicPumpVolume++;

    // Calculate zylinder speed dependent hydraulic flow volume ----
    // Boom (downwards) ---
    if (pulseWidth[5] > pulseMaxNeutral[5])
      hydraulicFlowVolumeInternalUndelayed = map(pulseWidth[5], pulseMaxNeutral[5], (pulseMax[5] - 200), 0, 100);
    else
      hydraulicFlowVolumeInternalUndelayed = 0;

    if (hydraulicFlowVolumeInternalUndelayed < hydraulicFlowVolume)
      hydraulicFlowVolume--;
    if (hydraulicFlowVolumeInternalUndelayed > hydraulicFlowVolume)
      hydraulicFlowVolume++;

    // Calculate speed dependent track rattle volume ----
    // Left ---
    if (pulseWidth[6] > pulseMaxNeutral[6])
      trackRattleVolumeInternal[6] = map(pulseWidth[6], pulseMaxNeutral[6], (pulseMax[6] - 150), 0, 100);
    else if (pulseWidth[6] < pulseMinNeutral[6])
      trackRattleVolumeInternal[6] = map(pulseWidth[6], pulseMinNeutral[6], (pulseMin[6] + 150), 0, 100);
    else
      trackRattleVolumeInternal[6] = 0;

    // Right
    if (pulseWidth[7] > pulseMaxNeutral[7])
      trackRattleVolumeInternal[7] = map(pulseWidth[7], pulseMaxNeutral[7], (pulseMax[7] - 100), 0, 100);
    else if (pulseWidth[7] < pulseMinNeutral[7])
      trackRattleVolumeInternal[7] = map(pulseWidth[7], pulseMinNeutral[7], (pulseMin[7] + 100), 0, 100);
    else
      trackRattleVolumeInternal[7] = 0;

    if (engineRunning)
      trackRattleVolumeInternalUndelayed = constrain(trackRattleVolumeInternal[6] + trackRattleVolumeInternal[7], 0, 100) * map(currentRpm, 0, 500, 100, 150) / 100;
    else
      trackRattleVolumeInternalUndelayed = 0;

    if (trackRattleVolumeInternalUndelayed < trackRattleVolume)
      trackRattleVolume--;
    if (trackRattleVolumeInternalUndelayed > trackRattleVolume)
      trackRattleVolume++;

    // Calculate hydraulic load dependent Diesel knock volume
    hydraulicDependentKnockVolume = map(hydraulicPumpVolume, 0, 100, 50, 100);

    // Calculate hydraulic load dependent engine RMP drop
    hydraulicLoad = map(hydraulicPumpVolume, 0, 100, 0, 40);

    // Bucket rattle sound triggering
    if (engineRunning && currentRpm > 400)
    {
      // If bucket stick is moved fast
      if (abs(pulseWidth[1] - lastBucketPulseWidth > 100))
      {
        bucketRattleTrigger = true;
      }
      lastBucketPulseWidth = pulseWidth[1];

      // If dipper stick is moved fast
      if (abs(pulseWidth[2] - lastDipperPulseWidth > 100))
      {
        bucketRattleTrigger = true;
      }
      lastDipperPulseWidth = pulseWidth[2];
    }
  }
}

//
// =======================================================================================================
// STEAM LOCOMOTIVE CONTROL
// =======================================================================================================
//

void steamLocomotiveControl()
{
#if defined STEAM_LOCOMOTIVE_MODE
  static uint32_t lastFrameTime = millis();
  if (millis() - lastFrameTime > 4)
  { // every 4ms
    lastFrameTime = millis();

    // Calculate speed dependent track rattle volume ----
    if (currentSpeed > 1)
      trackRattleVolume = map(currentSpeed, 1, 500, 10, trackRattleVolumePercentage);
    else
      trackRattleVolume = 0;
  }
#endif
}

//
// =======================================================================================================
// TRAILER CONTROL (ESP NOW)
// =======================================================================================================
//

void trailerControl()
{
#if defined ENABLE_WIRELESS

  static unsigned long espNowMillis;

  uint8_t taillightOld = 0;
  uint8_t sidelightOld = 0;
  uint8_t reversingLightOld = 0;
  uint8_t indicatorLOld = 0;
  uint8_t indicatorROld = 0;
  bool legsUpOld = false;
  bool legsdownOld = false;
  bool rampsUpOld = false;
  bool rampsdownOld = false;
  bool beaconsOnOld = false;

  if (millis() - espNowMillis > pollRate)
  { // Every 20ms
    espNowMillis = millis();

    // Store previous values for comparison
    taillightOld = trailerData.tailLight;
    sidelightOld = trailerData.sideLight;
    reversingLightOld = trailerData.reversingLight;
    indicatorLOld = trailerData.indicatorL;
    indicatorROld = trailerData.indicatorR;
    legsUpOld = trailerData.legsUp;
    legsdownOld = trailerData.legsDown;
    rampsUpOld = trailerData.rampsUp;
    rampsdownOld = trailerData.rampsDown;
    beaconsOnOld = trailerData.beaconsOn;

    // Set values to send
    // Lights (timer number in brackets, not pin number, see LED setup section)
#ifdef TRAILER_LIGHTS_TRAILER_PRESENCE_SWITCH_DEPENDENT // Tralier lights depending on truck mounted switch
    if (trailerDetected)
    {
      trailerData.tailLight = ledcRead(2);
      trailerData.sideLight = ledcRead(8);
      trailerData.reversingLight = ledcRead(6);
      trailerData.indicatorL = ledcRead(3);
      trailerData.indicatorR = ledcRead(4);
    }
    else
    {
      trailerData.tailLight = 0;
      trailerData.sideLight = 0;
      trailerData.reversingLight = 0;
      trailerData.indicatorL = 0;
      trailerData.indicatorR = 0;
    }
#else // Trailer lights always on
    trailerData.tailLight = ledcRead(2); // These are timer numbers, not pin numbers!
    trailerData.sideLight = ledcRead(8);
    trailerData.reversingLight = ledcRead(6);
    trailerData.indicatorL = ledcRead(3);
    trailerData.indicatorR = ledcRead(4);
#endif
    // Other signals
    trailerData.legsUp = legsUp;
    trailerData.legsDown = legsDown;
    trailerData.rampsUp = rampsUp;
    trailerData.rampsDown = rampsDown;
    trailerData.beaconsOn = blueLightTrigger;

    // Check, if one or more values have changed (saves battery live, prevents speaker noise and the ESP32 runs cooler)
    if (taillightOld != trailerData.tailLight ||
        sidelightOld != trailerData.sideLight ||
        reversingLightOld != trailerData.reversingLight ||
        indicatorLOld != trailerData.indicatorL ||
        indicatorROld != trailerData.indicatorR ||
        legsUpOld != trailerData.legsUp ||
        legsdownOld != trailerData.legsDown ||
        rampsUpOld != trailerData.rampsUp ||
        rampsdownOld != trailerData.rampsDown ||
        beaconsOnOld != trailerData.beaconsOn)
    {

      // If so, then send message via ESP-NOW
      esp_err_t result = esp_now_send(0, (uint8_t *)&trailerData, sizeof(trailerData));

#ifdef ESPNOW_DEBUG

      // This will confirm if a message was SENT successfully (callback will later check if RECEIVED and by whom).
      Serial.printf("ESP-NOW data sent: %s\n", result == ESP_NOW_SEND_SUCCESS ? "OK" : "FAILED");
#endif
    }
  }
#endif
}

//
// =======================================================================================================
// MAIN LOOP, RUNNING ON CORE 1
// =======================================================================================================
//

void loop()
{

#if defined SBUS_COMMUNICATION
  readSbusCommands(); // SBUS communication (pin 36)
  mcpwmOutput();      // PWM servo signal output

#elif defined IBUS_COMMUNICATION
  readIbusCommands(); // IBUS communication (pin 36)
  mcpwmOutput();      // PWM servo signal output

#elif defined SUMD_COMMUNICATION
  readSumdCommands(); // SUMD communication (pin 36)
  mcpwmOutput();      // PWM servo signal output

#elif defined PPM_COMMUNICATION
  readPpmCommands(); // PPM communication (pin 36)
  mcpwmOutput();     // PWM servo signal output

#else
  // measure RC signals mark space ratio
  readPwmSignals();
#endif

  // Horn triggering
  triggerHorn();

  // Indicator (turn signal) triggering
  triggerIndicators();

  if (xSemaphoreTake(xRpmSemaphore, portMAX_DELAY))
  {
    // Map pulsewidth to throttle
    mapThrottle();

    // rcTrigger
    rcTriggerRead();

    // Excavator specific controls
#if defined EXCAVATOR_MODE
    excavatorControl();
#endif

    // Steam locomotive specific controls
#if defined STEAM_LOCOMOTIVE_MODE
    steamLocomotiveControl();
#endif

    // Dashboard control
#if defined SPI_DASHBOARD
    updateDashboard();
#endif
    xSemaphoreGive(xRpmSemaphore); // Now free or "Give" the semaphore for others.
  }

  // Read trailer switch state
#if not defined THIRD_BRAKELIGHT and not defined RZ7886_DRIVER_MODE
  trailerPresenceSwitchRead();
#endif

  // RGB LED control
#if defined NEOPIXEL_ENABLED
  updateRGBLEDs();
#endif

  // Trailer control, using ESP NOW
  trailerControl();

  // Configuration website
  webInterface();

  // Core ID debug
#if defined CORE_DEBUG
  Serial.print("Running on core ");
  Serial.println(coreId);
#endif

  // Feeding the RTC watchtog timer is essential!
  rtc_wdt_feed();
}

//
// =======================================================================================================
// 1st MAIN TASK, RUNNING ON CORE 0 (Interrupts are running on this core as well)
// =======================================================================================================
//

void Task1code(void *pvParameters)
{
  for (;;)
  {

    // coreId = xPortGetCoreID(); // Running on core 0

    // DAC offset fader
    dacOffsetFade();

    if (xSemaphoreTake(xRpmSemaphore, portMAX_DELAY))
    {
      // Simulate engine mass, generate RPM signal
      engineMassSimulation();

      // Call gear selector
      if (automatic || doubleClutch)
        automaticGearSelector();
      xSemaphoreGive(xRpmSemaphore); // Now free or "Give" the semaphore for others.
    }

    // Switch engine on or off
    engineOnOff();

    // LED control
    if (autoZeroDone)
      led();

#if not defined SPI_DASHBOARD
    // Shaker control
    shaker();
#endif

    // Gearbox detection
    gearboxDetection();

    // ESC control & low discharge protection
    esc();

    // measure loop time
    loopTime = loopDuration(); // for debug only

    // Feeding the RTC watchtog timer is essential!
    rtc_wdt_feed(); // TODO, test only
  }
}
