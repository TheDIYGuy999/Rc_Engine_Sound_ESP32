#include <Arduino.h>

// Vehicle specific settings for sound, lights, ESC, transmission. This is a vehicle specific file.

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want) --------
volatile int startVolumePercentage = 144; // Adjust the start volume (usually = 80%)
#include "sounds/1981KenworthW900A_DetroitStart.h" // Detroit Diesel 8V71 start sound (1981 Kenworth W900A)

// Choose the motor idle sound (uncomment the one you want) --------
volatile int idleVolumePercentage = 56; // Adjust the idle volume (usually = 80%, more also working, depending on sound, 40 - 48% if additional diesel knock sound is used)
volatile int engineIdleVolumePercentage = 60; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!)
volatile int fullThrottleVolumePercentage = 140; // Volume Percentage while full throttle (for rev sound as well)
//#include "sounds/DetroitDieselIdle.h" // Detroit Diesel generic Truck (Volume 80%, 30%, Turbo 60%, 10%, Wastegate 50%, 1%, CEP 100, TSM 3)
//#include "sounds/DetroitDieselStraightPipeIdle.h" // Detroit Diesel Truck with straight pipes (use multiplier = 2, acc = 2, dec = 1)
//#include "sounds/DetroitDieselBassBoost15db.h" // Detroit Diesel Truck with straight pipes. Bass 100-200Hz + 15dB
//#include "sounds/DetroitDieselBassBoost7db.h" // Detroit Diesel Truck with straight pipes. Bass 100-200Hz + 7dB (use it for King Hauler, Volume 60%, 40%, Turbo 60%, 10, Wastegate 100%, 1%, CEP 100, TSM 3, Knock volume 400, 10%, interval 2, 50)
//#include "sounds/DetroitDieselPeterbiltCabover.h" // Detroit Diesel Peterbilt cabover truck
//#include "sounds/DetroitDieselKenworth.h" // Detroit Diesel Kenworth truck (use Ural V8 Start & 100% turbo)
//#include "sounds/DetroitDieselJohnDeereTractor.h" // Detroit Diesel John Deere tractor
//#include "sounds/8V71DetroitDieselStraightStacksIdle.h" // Detroit 8V71 two stroke V8 Diesel
//#include "sounds/8V71DetroitDieselStraightStacksIdle2.h" // Detroit 8V71 two stroke V8 Diesel
#include "sounds/1981KenworthW900A_DetroitIdle.h" // Detroit Diesel 8V71 idle sound (1981 Kenworth W900A)

// Choose the motor revving sound (uncomment the one you want) --------
#define REV_SOUND // uncomment this, if you want to use the separate, optional rev sound
volatile int revVolumePercentage = 120; // Adjust the idle volume (usually = 80%, more also working, depending on sound)
volatile int engineRevVolumePercentage = 60; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!)
volatile const uint16_t revSwitchPoint = 50; // The rev sound is played instead of the idle sound above this point
volatile const uint16_t idleEndPoint = 500; // above this point, we have 100% rev and 0% idle sound volume (usually 500, min. 50 more than revSwitchPoint)
volatile const uint16_t idleVolumeProportionPercentage = 90; // The idle sound volume proportion (rest is rev proportion) below "revSwitchPoint" (about 90 - 100%, never more than 100)
#ifdef REV_SOUND
//#include "sounds/ScaniaV8rev.h" // Scania V8
//#include "sounds/MesserschmittBf109rev.h" // Messerschmitt Bf109 Rev
//#include "sounds/3408CatRev.h" // CAT 3408 V8 Diesel idle (Kenworth W900A)
//#include "sounds/3408CaterpillarPeterbiltRev.h" // CAT 3408 V8 Diesel rev (Peterbilt)
//#include "sounds/BadAssRev.h" // Badass rev
//#include "sounds/CaboverCATrev.h" // Cabover wit CAT engine rev
#include "sounds/1981KenworthW900A_DetroitRev.h" // Detroit Diesel 8V71 rev sound (1981 Kenworth W900A)
#endif

// Choose the jake brake sound (uncomment the one you want) --------
#define JAKE_BRAKE_SOUND // uncomment this, if you want to use the jake brake sound
volatile int jakeBrakeVolumePercentage = 200; // Adjust the max. volume (usually = 150%)
volatile int jakeBrakeIdleVolumePercentage = 0; // Adjust the min. volume (usually = 80%)
volatile int jakeBrakeMinRpm = 200; // Adjust the min. RPM for the jake brake (around 100)
#ifdef JAKE_BRAKE_SOUND
//#include "sounds/JakeBrake.h" // CAT V8 Jake Brake sound
#include "sounds/1981KenworthW900A_DetroitJakeBrake.h" // Detroit Diesel 8V71 Jake Brake sound (1981 Kenworth W900A)
#endif

// Choose the Diesel (or whatever) ignition "knock" sound (played in the fixed sampling rate interrupt, uncomment the one you want,
// play around here, the results are amazing, if you hit the right combination with the idle sound!) --------
volatile int dieselKnockVolumePercentage = 80; // Adjust the Diesel knock volume (usually = 40 - 120%)
volatile int dieselKnockIdleVolumePercentage = 0; // Diesel knock volume while idling (usually = 20%)
volatile int dieselKnockInterval = 4; // Idle sample length divided by this number (1 - 20, depending on sound files)
volatile int dieselKnockStartPoint = 110; // Volume will raise above this point ( usually 0, for "open pipe" exhaust about 250)
#define V8 // V8 engine (Ford, Scania): pulses 4 and 8 will bel louder, because only 90° gap between exhaust cycles in same manifold
//#define V2 // V2 engine (Harley): first 2 of 4 pulses will be louder (set dieselKnockInterval to 4)
volatile int dieselKnockAdaptiveVolumePercentage = 50; // Adjust the Diesel knock volume for the non-first knocks per engine cycle (usually = 50%)
//#include "sounds/DieselKnockDummy.h" // If you don't want Diesel knock sound
//#include "sounds/1981KenworthW900A_DetroitKnock.h" // Detroit Diesel 8V71 knock sound (1981 Kenworth W900A)
#include "sounds/1981KenworthW900A_DetroitKnock2.h" // Detroit Diesel 8V71 knock sound (1981 Kenworth W900A)

// Adjust the additional turbo sound (set "turboVolumePercentage" to "0", if you don't want it) --------
volatile int turboVolumePercentage = 0; // Adjust the turbo volume (usually = 14%)
volatile int turboIdleVolumePercentage = 0; // the turbo volume will be engine rpm dependent (usually = 10%)
#include "sounds/TurboWhistle.h" // Turbo sound, playing in parallel with engine sound!

// Adjust the additional supercharger sound (set "chargerVolumePercentage" to "0", if you don't want it) --------
volatile int chargerVolumePercentage = 0; // Adjust the supercharger volume (usually = 14%)
volatile int chargerIdleVolumePercentage = 10; // the supercharger volume will be engine rpm dependent (usually = 10%)
volatile int chargerStartPoint = 10; // Volume will raise above this point ( usually 10)
#include "sounds/supercharger.h" // supercharger sound, playing in parallel with engine sound!

// Adjust the additional turbo wastegate  / blowoff valve  sound (set "wastegateVolumePercentage" to "0", if you don't want it)--------
volatile int wastegateVolumePercentage = 10; // Adjust the wastegate volume (usually = 14%, up to 50%)
volatile int wastegateIdleVolumePercentage = 1; // Wastegate sound is played, after rapid throttle drop with engaged clutch
#include "sounds/WastegateDummy.h"
//#include "sounds/UnimogU1000TurboWastegate.h"
//#include "sounds/ScaniaR730V8TurboWastegate.h"

// Adjust the additional cooling fan sound (set "fanVolumePercentage" to "0", if you don't want it) --------
volatile int fanVolumePercentage = 7; // Adjust the fan volume (50% for Tatra 813, else 0%)
volatile int fanIdleVolumePercentage = 0; // the fan volume will be engine rpm dependent (usually = 10%)
volatile int fanStartPoint = 110; // Volume will raise above this point (250 for Tatra 813)
#define GEARBOX_WHINING // Disables sound, if gearbox in neutral. Fan noise is used to simulate gearbox whining (set fanStartPoint above clutchEngagingPoint)
#include "sounds/Tatra813FanNew.h" // Tatra 813 8x8 V12 Diesel Cooling Fan (use this to simulate gearbox whining above clutch engaging point)
//#include "sounds/GenericFan.h" // Generic engine cooling fan

// Choose the horn sound (uncomment the one you want) --------
volatile int hornVolumePercentage = 160; // Adjust the horn volume (usually = 80%)
//#include "sounds/TrainHorn.h" // American train horn
//#include "sounds/HornblastersOUTLAWTrainHornShort.h" // Hornblasters outlaw train horn short
//#include "sounds/ManTgeHorn.h" // MAN TGE truck horn (King Hauler)
//#include "sounds/westinghouseHorn.h" // American truck horn (the best)
//#include "sounds/FireTruckAirHorn.h" // US fire truck air horn
//#include "sounds/CarHorn.h" // A boring car horn
//#include "sounds/TruckHorn.h" // A generic truck horn
//#include "sounds/PeterbiltHorn.h" // A Peterbilt truck horn
//#include "sounds/2ToneTruckHorn.h" // A 2 tone truck horn
#include "sounds/CaboverCAThorn.h" // Cabover wit CAT engine horn
//#include "sounds/ScaniaV8trainHorn.h" // Scania with train horn

// Choose the siren / additional horn sound (uncomment the one you want) --------
volatile int sirenVolumePercentage = 80; // Adjust the siren volume (usually = 80%)
#include "sounds/sirenDummy.h" // If you don't want siren sound
//#include "sounds/FireTruckAirSiren2.h" // US fire truck (incl. loop)

// Choose the air brake release sound (uncomment the one you want) --------
volatile int brakeVolumePercentage = 30; // Adjust the brake volume (usually = 40%)
//#include "sounds/AirBrakeDummy.h" // If you don't want air brake sound
//#include "sounds/TruckAirBrakes.h" // Short truck air brake sound
//#include "sounds/TruckAirBrakesLong.h" // Long truck air brake sound
#include "sounds/TruckAirBrakes2.h" // Another truck air brake sound
//#include "sounds/AirBrakeSqueak.h" // Squeaky air brake sound
//#include "sounds/UralBrakeSqueak.h" // URAL 4320 air brake sound

// Choose the parking brake engaging sound (uncomment the one you want) --------
volatile int parkingBrakeVolumePercentage = 30; // Adjust the brake volume (usually = 40%)
//#include "sounds/ParkingBrakeDummy.h" // If you don't want parking brake sound
#include "sounds/ParkingBrake.h" // Parking brake sound

// Choose the gear shifting sound (uncomment the one you want) --------
volatile int shiftingVolumePercentage = 40; // Adjust the shifting volume (usually = 40%)
//#include "sounds/AirShiftingDummy.h" // If you don't want pneumatic shifting sound
//#include "sounds/AirShifting.h" // Pneumatic shifting sound
#include "sounds/ClunkingGearShifting.h" // Manual clunking shifting sound

// Choose the additional "sound1" (uncomment the one you want) --------
volatile int sound1VolumePercentage = 100; // Adjust the sound1 volume (usually = 100%)
//#include "sounds/EMDLocomotiveBell.h" // American EMD locomotive bell
//#include "sounds/007JamesBond.h" // James Bond melody
//#include "sounds/M2Fire.h" // M2 salve
//#include "sounds/GlenCanyon.h" // Glen Canyon country song for truckers ;-)
#include "sounds/door.h" // opening and closing the door

// Choose the reversing beep sound --------
volatile int reversingVolumePercentage = 14; // Adjust the reversing sound volume (usually = 14%)
#include "sounds/TruckReversingBeep.h" // 1000Hz peep sound

// Choose the indicator / turn signal options --------
volatile int indicatorVolumePercentage = 10; // Adjust the indicator sound volume (usually = 10%)
const uint16_t indicatorOn = 300; // The indicator will be switched on above +/- this value, if wheels are turned
const boolean INDICATOR_DIR = true; // adjust indicator direction with true or false
#include "sounds/Indicator.h" // "Tick-Tack" sound

// Choose the trailer couplig & uncoupling sounds (uncomment the ones you want) --------
#define COUPLING_SOUND // uncomment this, if you want to use the trailer coupling & uncoupling sounds
volatile int couplingVolumePercentage = 20; // Adjust the volume (usually = 20%)
#ifdef COUPLING_SOUND
#include "sounds/coupling.h" // coupling
#include "sounds/uncoupling.h" // uncoupling
#endif

// Choose the light options --------
//#define XENON_LIGHTS // Headlights will show a xenon bulb ignition flash, if defined
#define INDICATOR_SIDE_MARKERS // The indicators are used as side markers as wellß

// Choose the blue light options -----------------------------------------------------------------------------------------
const boolean doubleFlashBlueLight = true; // double flash blue lights if "true", "rotating" beacons if "false"

// Acceleration & deceleration settings ----------------------------------------------------------------------------------
const uint8_t escRampTimeFirstGear = 20; // determines, how fast the acceleration and deceleration happens (about 15 - 25, 20 for King Hauler)
const uint8_t escRampTimeSecondGear = 50; // 50 for King Hauler (this value is always in use for automatic transmission, about 80)
const uint8_t escRampTimeThirdGear = 75; // 75 for King Hauler
const uint8_t escBrakeSteps = 30; // determines, how fast the ESC is able to brake down (20 - 30, 30 for King Hauler)
const uint8_t escAccelerationSteps = 3; // determines, how fast the ESC is able to accelerate (2 - 3, 3 for King Hauler)

// Gearbox parameters ---------------------------------------------------------------------------------------------------
const boolean automatic = false; // false = linear rpm curve, true = automatic transmission with torque converter is simulated (use it, if you don't have a real shifting transmission)
#define NumberOfAutomaticGears 3 // <<------- Select 3, 4 or 6 gears!
const boolean doubleClutch = false; // do not activate it at the same time as automatic!
const boolean shiftingAutoThrottle = true; // For Tamiya 3 speed tansmission, throttle is altered for synchronizing, if "true"

// Clutch parameters ---------------------------------------------------------------------------------------------------
uint16_t clutchEngagingPoint = 80; // CEP. The "clutch" is engaging above this point = engine rpm sound in synch with ESC power

// Engine parameters ----------------------------------------------------------------------------------------------------
// Engine max. RPM in % of idle RPM. About 200% for big Diesels, 400% for fast running motors.
uint32_t MAX_RPM_PERCENTAGE = 350; // NOTE! Was called TOP_SPEED_MULTIPLIER (TSM) in earlier versions and was a multiplier instead of a percentage!

// Engine mass simulation
const int8_t acc = 3; // Acceleration step (2) 1 = slow for locomotive engine, 9 = fast for trophy truck
const int8_t dec = 1; // Deceleration step (1) 1 = slow for locomotive engine, 5 = fast for trophy truck

// Vehicle type ----------------------------------------------------------------------------------------------------
// #define TRACKED_MODE // For Tanks, diggers, excavators etc. which use dual throttle input on CH2 and CH3
