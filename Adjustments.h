// ******* RECOMMENDED Audacity Settings: 22'050 Hz, 8 bit PCM **********

// Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
// converter code by bitluni (send him a high five if you like the code)

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want)
//#include "UnionPacific2002Start.h" // Union Pacific 2002 SD70M Locomotive Start
//#include "ScaniaV8Start.h" // Scania V8 Start
#include "UralV8Start.h" // Ural 4320 V8 Start
//#include "HgP408Start.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "DefenderV8Start.h" // Land Rover Defender V8 Start <------- Check this one out!
//#include "JaguarXjsHeStart.h" // Jaguar XJS HE Start

// Choose the motor sound (uncomment the one you want)
volatile int idleVolumePercentage = 100; // Adjust the idle volume (usually = 100%, never more!),
//#include "UnionPacific2002Idle.h" // Union Pacific 2002 SD70M Locomotive with 16 cylinder Diesel (set volume to 60%)
//#include "ScaniaV8Idle.h" // Scania V8
//#include "UralV8Idle.h" // Ural 4320 V8
#include "HumveeDieselIdle.h" // "Humvvee" (Hummer H1) V8 Diesel <------- nice turbo whining!
//#include "HgP408Idle.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "DetroitDieselIdle.h" // Detroit Diesel Truck (use multiplier = 2, acc = 2, dec = 2)
//#include "DefenderV8Idle.h" // Land Rover Defender V8 <------- Check this one out!
//#include "Mustang68Idle.h" // Ford Mustang 1968
//#include "DodgeChallenger70Idle.h" // 1970 Dodge Challenger
//#include "MgBGtV8Idle.h" // MG B GT V8
//#include "LaFerrariIdle.h" // Ferrari "LaFerrari"
//#include "chevyNovaV8Idle.h" // Chevy Nova Coupe 1975 <------- The best sounding!
//#include "V8TrophyTruckIdle.h" // V8 Trophy Truck
//#include "JaguarXjsHeIdle.h" // Jaguar XJS HE (bad quality)
//#include "20Hz.h" // 20Hz test tone
//#include "100Hz.h" // 100Hz test tone
//#include "1000Hz.h" // 1000Hz test tone

// Choose the horn sound (uncomment the one you want)
//#include "TrainHorn.h" // American train horn
//#include "UsPoliceSiren.h" // US Police siren
//#include "FeuerwehrMartinshorn.h" // European Feuerwehr Martinshorn
#include "ManTgeHorn.h" // MAN TGE truck horn <------- Bombastic!
//#include "PostAutoHorn.h" // The typical Swiss post bus horn
//#include "CarHorn.h" // A boring car horn

// Choose the air brake release sound (uncomment the one you want)
//#include "AirBrakeDummy.h" // If you don't want air brake sound
//#include "TruckAirBrakes.h" // Short truck air brake sound
#include "TruckAirBrakesLong.h" // Long truck air brake sound

// Choose the additional sound (uncomment the one you want)
//#include "EMDLocomotiveBell.h" // American EMD locomotive bell
//#include "007JamesBond.h" // James Bond melody
#include "M2Fire.h" // M2 salve

// Choose the reversing beep sound
volatile uint8_t reverseSoundMode = 2; // 0 = off, 1 = forward, 2 = revesre
volatile int reversingvolumePercentage = 30; // Adjust the reversing sound volume (usually = 30%, never more than 100%!),
#include "TruckReversingBeep.h" // 1000Hz peep sound

// PWM Throttle range calibration ---------------------------------------------------------------------------------------
int16_t pulseZero = 1500; // Usually 1500 (range 1000 - 2000us) Autocalibration active, if "engineManualOnOff" = "false"
int16_t pulseNeutral = 20; // pulseZero +/- this value (20)
int16_t pulseSpan = 450; // pulseZero +/- this value (150 for JMT 10A ESC, otherwise around 450)
int16_t pulseLimit = 700; // pulseZero +/- this value (700)

// Horn trigger signal type (true / false)-------------------------------------------------------------------------------
const boolean pwmHornTrigger = true; // horn triggered by RC PWM signal instead of constant high level signal, if "true"
// Do NOT enable this boolean, if no PWM signal is connected or you will experience huge engine RPM resopnse delays

// Gearbox parameters ---------------------------------------------------------------------------------------------------
const boolean shifted = false; // false = linear rpm curve, true = shifting points

// Engine parameters ----------------------------------------------------------------------------------------------------
//Activate for "engine on off" functionality in combination with "Micro RC" Receiver from TheDIYGuy999. No Pulse Zero auto calibration in this case!!
const boolean engineManualOnOff = false;

// Engine RPM range (2 for locomotive, 4 for fast running motors)
const uint32_t TOP_SPEED_MULTIPLIER = 2; // RPM multiplier: the bigger the number, the larger the rev range, 2 - 4 is a good place to start. ESP32 will crash, if > 5 @ 22'050Hz!

// Engine mass simulation
const int8_t acc = 2; // Acceleration step (3) 1 = slow for locomotive engine, 9 = fast for trophy truck
const int8_t dec = 1; // Deceleration step per (2) 1 = slow for locomotive engine, 5 = fast for trophy truck
