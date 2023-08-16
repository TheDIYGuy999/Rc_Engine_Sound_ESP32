#include <Arduino.h>

// Vehicle specific settings for sound, lights, ESC, transmission. This is a vehicle specific file.

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want) --------
volatile int startVolumePercentage = 80; // Adjust the start volume (usually = 100%)
//#include "sounds/468ChevyBigBlockStart.h" // Chevy 486 V8 Start
#include "sounds/ChevyPickupV8SoundStart.h" // Chevy V8 Pickup Start

// Choose the motor idle sound (uncomment the one you want) --------
volatile int idleVolumePercentage = 80; // Adjust the idle volume (usually = 100%, more also working, depending on sound, 50 - 60% if additional diesel knock sound is used)
volatile int engineIdleVolumePercentage = 70; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!) 60
volatile int fullThrottleVolumePercentage = 170; // Volume Percentage while full throttle (for rev sound as well)
//#include "sounds/468ChevyBigBlockIdle.h" // Chevy 486 V8
#include "sounds/ChevyPickupV8SoundIdle.h" // Chevy V8 Pickup

// Choose the motor revving sound (uncomment the one you want) --------
#define REV_SOUND // uncomment this, if you want to use the separate, optional rev sound
volatile int revVolumePercentage = 100; // Adjust the idle volume (usually = 100%, more also working, depending on sound)
volatile int engineRevVolumePercentage = 80; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!) 60
volatile const uint16_t revSwitchPoint = 10; // The rev sound is played instead of the idle sound above this point 190
volatile const uint16_t idleEndPoint = 500; // above this point, we have 100% rev and 0% idle sound volume (usually 500, min. 50 more than revSwitchPoint) 300
volatile const uint16_t idleVolumeProportionPercentage = 90; // The idle sound volume proportion (rest is rev proportion) below "revSwitchPoint" (about 90 - 100%, never more than 100)
#ifdef REV_SOUND
//#include "sounds/468ChevyBigBlockRev.h" // Chevy 486 V8 Rev (not good)
//#include "sounds/468ChevyBigBlockMedium.h" // Chevy 486 V8 Rev
#include "sounds/ChevyPickupV8SoundRev.h" // Chevy V8 Pickup
//#include "sounds/ChevyPickupV8SoundRev5.h" // Chevy V8 Pickup (not good)
#endif

// Choose the jake brake sound (uncomment the one you want) --------
//#define JAKE_BRAKE_SOUND // uncomment this, if you want to use the jake brake sound
volatile int jakeBrakeVolumePercentage = 200; // Adjust the max. volume (usually = 150%)
volatile int jakeBrakeIdleVolumePercentage = 0; // Adjust the min. volume (usually = 80%)
volatile int jakeBrakeMinRpm = 200; // Adjust the min. RPM for the jake brake (around 100)
#ifdef JAKE_BRAKE_SOUND
#include "sounds/JakeBrake.h" // V8 Jake Brake sound
#endif

// Choose the Diesel (or whatever) ignition "knock" sound (played in the fixed sampling rate interrupt, uncomment the one you want,
// play around here, the results are amazing, if you hit the right combination with the idle sound!) --------
volatile int dieselKnockVolumePercentage = 600; // Adjust the Diesel knock volume (usually = 200 - 600%) 700
volatile int dieselKnockIdleVolumePercentage = 20; // Diesel knock volume while no throttle is applied (usually = 20%)
volatile int dieselKnockStartPoint = 10; // Volume will raise above this throttle input( usually 0, for "open pipe" exhaust about 250)
volatile int dieselKnockInterval = 16; // Idle sample length divided by this number (1 - 20, depending on sound files)
//#define V8 // V8 engine (Ford, Scania): pulses 4 and 8 will bel louder, because only 90° gap between exhaust cycles in same manifold
#define V8_468 // V8 468 engine
//#define V2 // V2 engine (Harley): first 2 of 4 pulses will be louder (set dieselKnockInterval to 4)
volatile int dieselKnockAdaptiveVolumePercentage = 50; // Adjust the Diesel knock volume for the non-first knocks per engine cycle (usually = 50%)
#define RPM_DEPENDENT_KNOCK // Knock volume also depending on engine RPM *****************
#ifdef RPM_DEPENDENT_KNOCK // These parameters are for RPM dependent knock mode only:
uint8_t minKnockVolumePercentage = 40; // percentage, if at knock start RPM (about 5 - 80)
uint16_t knockStartRpm = 50; // starting @ this RPM (about 50 - 400)
#endif // ********************************************************************************
//#include "sounds/DieselKnockDummy.h" // No knock
//#include "sounds/468ChevyBigBlockKnock.h" // Chevy 486 V8 Knock
//#include "sounds/468ChevyBigBlockKnock2.h" // Chevy 486 V8 Knock (not good)
#include "sounds/ChevyPickupV8SoundKnock.h" // Chevy V8 Pickup
//#include "sounds/ChevyPickupV8SoundKnock2.h" // Chevy V8 Pickup

// Adjust the additional turbo sound (set "turboVolumePercentage" to "0", if you don't want it) --------
volatile int turboVolumePercentage = 0; // Adjust the turbo volume (usually = 70%)
volatile int turboIdleVolumePercentage = 0; // the turbo volume will be engine rpm dependent (usually = 10%)
#include "sounds/TurboWhistle.h" // Turbo sound, playing in parallel with engine sound!

// Adjust the additional supercharger sound (set "chargerVolumePercentage" to "0", if you don't want it) --------
volatile int chargerVolumePercentage = 0; // Adjust the supercharger volume (usually = 70%)
volatile int chargerIdleVolumePercentage = 10; // the supercharger volume will be engine rpm dependent (usually = 10%)
volatile int chargerStartPoint = 10; // Volume will raise above this point ( usually 10)
//#include "sounds/supercharger.h" // supercharger sound, playing in parallel with engine sound!
#include "sounds/SuperchargerDummy.h"

// Adjust the additional turbo wastegate  / blowoff valve  sound (set "wastegateVolumePercentage" to "0", if you don't want it)--------
volatile int wastegateVolumePercentage = 0; // Adjust the wastegate volume (usually = 70%, up to 250%)
volatile int wastegateIdleVolumePercentage = 1; // Wastegate sound is played, after rapid throttle drop with engaged clutch
#include "sounds/WastegateDummy.h"
//#include "sounds/DefenderTd5wastegate.h" // Land Rover Defender Td5 5 cylinder Diesel

// Adjust the additional cooling fan sound (set "fanVolumePercentage" to "0", if you don't want it) --------
volatile int fanVolumePercentage = 0; // Adjust the fan volume (250% for Tatra 813, else 0%)
volatile int fanIdleVolumePercentage = 0; // the fan volume will be engine rpm dependent (usually = 10%)
volatile int fanStartPoint = 250; // Volume will raise above this point (250 for Tatra 813)
//#define GEARBOX_WHINING // Disables sound, if gearbox in neutral. Fan noise is used to simulate gearbox whining (set fanStartPoint above clutchEngagingPoint)
//#include "sounds/Tatra813Fan.h" // Tatra 813 8x8 V12 Diesel Cooling Fan
//#include "sounds/Tatra813FanNew.h" // Tatra 813 8x8 V12 Diesel Cooling Fan (use this)
//#include "sounds/Tatra813FanNewSlow.h" // Tatra 813 8x8 V12 Diesel Cooling Fan
#include "sounds/FanDummy.h"

// Choose the horn sound (uncomment the one you want) --------
volatile int hornVolumePercentage = 100; // Adjust the horn volume (usually = 100%)
//#include "sounds/TrainHorn.h" // American train horn
//#include "sounds/HornblastersOUTLAWTrainHornShort.h" // Hornblasters outlaw train horn short
//#include "sounds/ManTgeHorn.h" // MAN TGE truck horn (King Hauler)
//#include "sounds/westinghouseHorn.h" // American truck horn (the best)
//#include "sounds/FireTruckAirHorn.h" // US fire truck air horn
#include "sounds/CarHorn.h" // A boring car horn
//#include "sounds/TruckHorn.h" // A generic truck horn
//#include "sounds/PeterbiltHorn.h" // A Peterbilt truck horn
//#include "sounds/2ToneTruckHorn.h" // A 2 tone truck horn

// Choose the siren / additional horn sound (uncomment the one you want) --------
volatile int sirenVolumePercentage = 60; // Adjust the siren volume (usually = 100%)
//#include "sounds/sirenDummy.h" // If you don't want siren sound
//#include "sounds/UsPoliceSiren.h" // US Police siren
//#include "sounds/FireTruckAirSiren.h" // US fire truck air siren (King Hauler)
//#include "sounds/FeuerwehrMartinshorn.h" // European Feuerwehr Martinshorn
//#include "sounds/IrishFireEngineHorn.h" // Irish fire truck horn
//#include "sounds/siren01norskere22.h" // Norwegian siren
//#include "sounds/PostAutoHorn.h" // The typical Swiss post bus horn
//#include "sounds/brasil.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/in_the_summertime(1).h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/JEEP_HORN_2.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/la_carica.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/la_cucaracha.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/marseillaise.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/river_kwai.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/startrek.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/susannah.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/Tequila.h" // sound from nenno @ rc-modellbau-portal.de
#include "sounds/kittScanner.h" // Knight Rider KITT scanner. Use it in combination with Neopixel animation.

// Choose the air brake release sound (uncomment the one you want) --------
volatile int brakeVolumePercentage = 200; // Adjust the brake volume (usually = 200%)
#include "sounds/AirBrakeDummy.h" // If you don't want air brake sound
//#include "sounds/TruckAirBrakes.h" // Short truck air brake sound
//#include "sounds/TruckAirBrakesLong.h" // Long truck air brake sound
//#include "sounds/TruckAirBrakes2.h" // Another truck air brake sound
//#include "sounds/AirBrakeSqueak.h" // Squeaky air brake sound

// Choose the parking brake engaging sound (uncomment the one you want) --------
volatile int parkingBrakeVolumePercentage = 200; // Adjust the brake volume (usually = 200%)
#include "sounds/ParkingBrakeDummy.h" // If you don't want parking brake sound
//#include "sounds/ParkingBrake.h" // Parking brake sound

// Choose the gear shifting sound (uncomment the one you want) --------
volatile int shiftingVolumePercentage = 100; // Adjust the shifting volume (usually = 200%)
#include "sounds/AirShiftingDummy.h" // If you don't want pneumatic shifting sound
//#include "sounds/AirShifting.h" // Pneumatic shifting sound
//#include "sounds/ClunkingGearShifting.h" // Manual clunking shifting sound

// Choose the additional "sound1" (uncomment the one you want) --------
volatile int sound1VolumePercentage = 100; // Adjust the sound1 volume (usually = 100%)
//#include "sounds/007JamesBond.h" // James Bond melody
#include "sounds/door.h" // opening and closing the door

// Choose the reversing beep sound --------
volatile int reversingVolumePercentage = 0; // Adjust the reversing sound volume (usually = 70%)
#include "sounds/TruckReversingBeep.h" // 1000Hz peep sound

// Choose the indicator / turn signal options --------
volatile int indicatorVolumePercentage = 100; // Adjust the indicator sound volume (usually = 100%)
const uint16_t indicatorOn = 300; // The indicator will be switched on above +/- this value, if wheels are turned
const boolean INDICATOR_DIR = true; // adjust indicator direction with true or false
#include "sounds/Indicator.h" // "Tick-Tack" sound

// Choose the light options --------
//#define XENON_LIGHTS // Headlights will show a xenon bulb ignition flash, if defined

// Choose the blue light options -----------------------------------------------------------------------------------------
const boolean doubleFlashBlueLight = true; // double flash blue lights if "true", "rotating" beacons if "false"

// Acceleration & deceleration settings ----------------------------------------------------------------------------------
const uint8_t escRampTimeFirstGear = 20; // determines, how fast the acceleration and deceleration happens (about 15 - 25, 20 for King Hauler)
const uint8_t escRampTimeSecondGear = 80; // 50 for King Hauler (this value is always in use for automatic transmission, about 80)
const uint8_t escRampTimeThirdGear = 75; // 75 for King Hauler
const uint8_t escBrakeSteps = 30; // determines, how fast the ESC is able to brake down (20 - 30, 30 for King Hauler)
const uint8_t escAccelerationSteps = 3; // determines, how fast the ESC is able to accelerate (2 - 3, 3 for King Hauler)

// Gearbox parameters (select number of automatic gears in curves.h)-----------------------------------------------------
const boolean automatic = true; // false = linear rpm curve, true = automatic transmission with torque converter is simulated (use it, if you don't have a real shifting transmission)
#define NumberOfAutomaticGears 3 // <<------- Select 3, 4 or 6 gears!
const boolean doubleClutch = false; // do not activate it at the same time as automatic!
const boolean shiftingAutoThrottle = true; // For Tamiya 3 speed tansmission, throttle is altered for synchronizing, if "true"

// Clutch parameters ---------------------------------------------------------------------------------------------------
uint16_t clutchEngagingPoint = 10; // CEP. The "clutch" is engaging above this point = engine rpm sound in synch with ESC power

// Engine parameters ----------------------------------------------------------------------------------------------------
// Engine max. RPM in % of idle RPM. About 200% for big Diesels, 400% for fast running motors.
uint32_t MAX_RPM_PERCENTAGE = 330; // NOTE! Was called TOP_SPEED_MULTIPLIER (TSM) in earlier versions and was a multiplier instead of a percentage!

// Engine mass simulation
const int8_t acc = 2; // Acceleration step (2) 1 = slow for locomotive engine, 9 = fast for trophy truck
const int8_t dec = 1; // Deceleration step (1) 1 = slow for locomotive engine, 5 = fast for trophy truck

// Vehicle type ----------------------------------------------------------------------------------------------------
// #define TRACKED_MODE // For Tanks, diggers, excavators etc. which use dual throttle input on CH2 and CH3
