// ******* RECOMMENDED Audacity Settings: 22'050 Hz, 8 bit PCM **********

// Sound files converted with: https://bitluni.net/wp-content/uploads/2018/01/Audio2Header.html
// converter code by bitluni (send him a high five if you like the code)

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want)
//#include "ScaniaV8Start.h" // Scania V8 Start
//#include "UralV8Start.h" // Ural 4320 V8 Start
#include "DefenderV8Start.h" // Land Rover Defender V8 Start

// Choose the motor sound (uncomment the one you want)
//#include "ScaniaV8Idle.h" // Scania V8
//#include "UralV8Idle.h" // Ural 4320 V8
#include "DefenderV8Idle.h" // Land Rover Defender V8
//#include "Mustang68Idle.h" // Ford Mustang 1968
//#include "DodgeChallenger70Idle.h" // 1970 Dodge Challenger
//#include "MgBGtV8Idle.h" // MG B GT V8
//#include "LaFerrariIdle.h" // Ferrari "LaFerrari"
//#include "chevyNovaV8Idle.h" // Chevy Nova Coupe 1975 <------- The best sounding!
//#include "V8TrophyTruckIdle.h" // V8 Trophy Truck

// Choose the horn sound (uncomment the one you want)
//#include "TrainHorn.h" // American train horn
#include "ManTgeHorn.h" // MAN TGE truck horn

// PWM Throttle range calibration ---------------------------------------------------------------------------------------
int16_t pulseZero = 1500; // Usually 1500 (range 1000 - 2000us) Autocalibration active, if "engineManualOnOff" = "false"
int16_t pulseNeutral = 20; // pulseZero +/- this value
int16_t pulseSpan = 450; // pulseZero +/- this value (150 for JMT 10A ESC, otherwise around 450)
int16_t pulseLimit = 700; // pulseZero +/- this value (700)

// Engine parameters ----------------------------------------------------------------------------------------------------
//Activate for "engine on off" functionality in combination with "Micro RC" Receiver from TheDIYGuy999. No Pulse Zero auto calibration in this case!!
boolean engineManualOnOff = false; 

// Engine RPM range
#define TOP_SPEED_MULTIPLIER 3 // RPM multiplier: the bigger the number, the larger the rev range, 3 - 5 is a good place to start. ESP32 will crash, if > 5 @ 22'050Hz!

// Engine mass simulation
const int8_t acc = 18; // Acceleration step per 5ms (9)
const int8_t dec = 9; // Deceleration step per 5ms (6)
