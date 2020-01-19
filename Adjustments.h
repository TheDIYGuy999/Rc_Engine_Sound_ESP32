// ******* RECOMMENDED Audacity Settings: 22'050 Hz, 8 bit PCM **********

// Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
// converter code by bitluni (send him a high five, if you like the code)

// **********************************************************************************************************************
// Choose the receiver communication mode (never uncomment both! If both commented out, classic RC signal communication)--
// Serial communication --------
#define SERIAL_COMMUNICATION // control signals are coming in via the serial interface (comment it out for classic RC signals)
// Only for my "Micro RC" receiver! See: https://github.com/TheDIYGuy999/Micro_RC_Receiver

// PPM communication --------
//#define PPM_COMMUNICATION // control signals are coming in via the PPM interface (comment it out for classic RC signals)
#define NUM_OF_CHL 8          // The number of channels inside our PPM signal (usually max. 8)
#define NUM_OF_AVG 1          // Number of averaging passes (usually one, more will be slow)

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want) --------
volatile int startVolumePercentage = 100; // Adjust the start volume (usually = 100%, never more!)
//#include "UnionPacific2002Start.h" // Union Pacific 2002 SD70M Locomotive Start
//#include "ScaniaV8Start.h" // Scania V8 Start
//#include "ScaniaR500V8Start.h" // Scania R500 V8 Start
//#include "UralV8Start.h" // Ural 4320 V8 Start (use it for Diesel trucks, if no other is available)
//#include "HgP408Start.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "DefenderV8Start.h" // Land Rover Defender V8 Start
//#include "JaguarXjsHeStart.h" // Jaguar XJS HE Starts
#include "KenworthW900Start.h" // Kenworth W900 Truck Start
//#include "MackStart.h" // Mack Truck Start
//#include "DetroitDieselStart.h" // Detroit Diesel generic Truck Start
//#include "Cat3404BStart.h" // Caterpillar 3406B start
//#include "ActrosV8Start.h" // MB Actros V8 Truck Start
//#include "VWBeetleStart.h" // VW Beetle or Bug

// Choose the motor sound (uncomment the one you want) --------
volatile int idleVolumePercentage = 100; // Adjust the idle volume (usually = 100%, never more!)
volatile int engineIdleVolumePercentage = 60; // the engine volume will be throttle dependent (usually = 60%, never more than 100%!)
//#include "UnionPacific2002Idle.h" // Union Pacific 2002 SD70M Locomotive with 16 cylinder Diesel (set volume to 60%)
//#include "ScaniaV8Idle.h" // Scania V8
//#include "ScaniaR500V8Idle.h" // Scania R500 V8
//#include "ScaniaR620V8Idle.h" // Scania R620 V8 (a bit thin, add about 80% turbo volume)
//#include "UralV8Idle.h" // Ural 4320 V8
//#include "HumveeDieselIdle.h" // "Humvvee" (Hummer H1) V8 Diesel
//#include "HgP408Idle.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "DetroitDieselIdle.h" // Detroit Diesel generic Truck
//#include "DetroitDieselStraightPipeIdle.h" // Detroit Diesel Truck with straight pipes (use multiplier = 2, acc = 2, dec = 2)
//#include "DetroitDieselPeterbiltCabover.h" // Detroit Diesel Peterbilt cabover truck
//#include "DetroitDieselKenworth.h" // Detroit Diesel Kenworth truck (use Ural V8 Start & 100% turbo)
//#include "DetroitDieselJohnDeereTractor.h" // Detroit Diesel John Deere tractor
//#include "Cat3406BIdle.h" // Caterpillar 3406B idle
//#include "ActrosV8Idle.h" // MB Actros V8 Truck Idle (not very good)
#include "KenworthW900Idle.h" // Kenworth W900 Truck Idle
//#include "MackIdle.h" // Mack Truck Idle
//#include "DefenderV8Idle.h" // Land Rover Defender V8
//#include "Mustang68Idle.h" // Ford Mustang 1968
//#include "DodgeChallenger70Idle.h" // 1970 Dodge Challenger
//#include "MgBGtV8Idle.h" // MG B GT V8
//#include "LaFerrariIdle.h" // Ferrari "LaFerrari"
//#include "chevyNovaV8Idle.h" // Chevy Nova Coupe 1975
//#include "V8TrophyTruckIdle.h" // V8 Trophy Truck
//#include "JaguarXjsHeIdle.h" // Jaguar XJS HE (bad quality)
//#include "VWBeetleIdle.h" // VW Beetle or Bug
//#include "20Hz.h" // 20Hz test tone
//#include "100Hz.h" // 100Hz test tone
//#include "1000Hz.h" // 1000Hz test tone

// Adjust the additional turbo sound (set "turboVolumePercentage" to "0", if you don't want it) --------
volatile int turboVolumePercentage = 100; // Adjust the turbo volume (usually = 100%, never more!)
volatile int turboIdleVolumePercentage = 10; // the turbo volume will be throttle dependent (usually = 10%, never more than 100%!)
#include "TurboWhistle.h" // Turbo sound, playing in parallel with engine sound!

// Choose the horn sound (uncomment the one you want) --------
volatile int hornVolumePercentage = 100; // Adjust the horn volume (usually = 100%, never more!)
//#include "TrainHorn.h" // American train horn
#include "ManTgeHorn.h" // MAN TGE truck horn
//#include "westinghouseHorn.h" // American truck horn
//#include "FireTruckAirHorn.h" // US fire truck air horn
//#include "CarHorn.h" // A boring car horn
//#include "TruckHorn.h" // A generic truck horn
//#include "PeterbiltHorn.h" // A Peterbilt truck horn

// Choose the siren / additional horn sound (uncomment the one you want) --------
volatile int sirenVolumePercentage = 100; // Adjust the siren volume (usually = 100%, never more!)
//#include "UsPoliceSiren.h" // US Police siren
#include "FireTruckAirSiren.h" // US fire truck air siren
//#include "FeuerwehrMartinshorn.h" // European Feuerwehr Martinshorn
//#include "IrishFireEngineHorn.h" // Irish fire truck horn
//#include "PostAutoHorn.h" // The typical Swiss post bus horn

// Choose the air brake release sound (uncomment the one you want) --------
volatile int brakeVolumePercentage = 100; // Adjust the brake volume (usually = 100%, never more!)
//#include "AirBrakeDummy.h" // If you don't want air brake sound
//#include "TruckAirBrakes.h" // Short truck air brake sound
#include "TruckAirBrakesLong.h" // Long truck air brake sound

// Choose the additional "sound1" (uncomment the one you want) --------
volatile int sound1VolumePercentage = 100; // Adjust the sound1 volume (usually = 100%, never more!)
#include "EMDLocomotiveBell.h" // American EMD locomotive bell
//#include "007JamesBond.h" // James Bond melody
//#include "M2Fire.h" // M2 salve

// Choose the reversing beep sound --------
volatile uint8_t reverseSoundMode = 2; // 0 = off, 1 = forward, 2 = reverse (direction)
volatile int reversingVolumePercentage = 40; // Adjust the reversing sound volume (usually = 40%, never more than 100%!)
#include "TruckReversingBeep.h" // 1000Hz peep sound

// Choose the indicator options --------
volatile int indicatorVolumePercentage = 70; // Adjust the indicator sound volume (usually = 70%, never more than 100%!)
const boolean INDICATOR_DIR = true; // adjust indicator direction with true or false
#include "Indicator.h" // "Tick -Tack" sound

// Choose the blue light opions -----------------------------------------------------------------------------------------
const boolean doubleFlashBlueLight = true; // double flash blue lights if "true", "rotating" beacons if "false"
const boolean indicators = true; // "true", if you want to trigger indicator lights (turn signals)

// PWM input signal range calibration ------------------------------------------------------------------------------------
const uint16_t pulseNeutral = 20; // pulseZero +/- this value (20) is the neutral range
const uint16_t pulseSpan = 450; // pulseZero +/- this value (150 for JMT 10A ESC, otherwise around 450)

// PWM ESC output signal range calibration -------------------------------------------------------------------------------
const uint16_t escPulseSpan = 1000; // pulseZero +/- this value (> 500 = limited top speed, about 800 for King Hauler)
const uint8_t escRampTime = 6; // determines, how fast the acceleration and deceleration happens (about 2 - 6, 6 for King Hauler)
const uint8_t escBrakeSteps = 5; // determines, how fast the ESC is able to brake down (2 - 5, 3 for King Hauler)

// Horn trigger signal type (true / false)-------------------------------------------------------------------------------
const boolean pwmSoundTrigger = true; // horn triggered by RC PWM signal instead of constant high level signal, if "true"
// Do NOT enable this boolean, if no PWM signal is connected or you will experience huge engine RPM resopnse delays

// Brake parameters -----------------------------------------------------------------------------------------------------
const boolean directBrake = false; // true = ESC is braking immediately, if throttle stick is released

// Gearbox parameters ---------------------------------------------------------------------------------------------------
const boolean shifted = false; // false = linear rpm curve, true = shifting points

// Shaker parameters (simulating engine vibrations) ---------------------------------------------------------------------
const uint8_t shakerStart = 100; // Shaker power while engine start (max. 255, about 100)
const uint8_t shakerIdle = 49; // Shaker power while idling (max. 255, about 49)
const uint8_t shakerFullThrottle = 40; // Shaker power while full throttle (max. 255, about 40)
const uint8_t shakerStop = 60; // Shaker power while engine stop (max. 255, about 60)

// Engine parameters ----------------------------------------------------------------------------------------------------
//Activate for "engine on off" functionality in combination with "Micro RC" Receiver from TheDIYGuy999. No Pulse Zero auto calibration in this case!!
const boolean engineManualOnOff = false;

// Engine RPM range (2 for big Diesels, 4 for fast running motors)
const uint32_t TOP_SPEED_MULTIPLIER = 2; // RPM multiplier: the bigger the number, the larger the rev range, 2 - 4 is a good place to start. ESP32 will crash, if > 5 @ 22'050Hz!

// Engine mass simulation
const int8_t acc = 3; // Acceleration step (3) 1 = slow for locomotive engine, 9 = fast for trophy truck
const int8_t dec = 1; // Deceleration step (2) 1 = slow for locomotive engine, 5 = fast for trophy truck
