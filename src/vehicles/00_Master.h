#include <Arduino.h>

// Vehicle specific settings for sound, lights, ESC, transmission.
// This master file is containing all available sounds, which are not already used in pre made configurations

// Sound files (22'050 Hz, 8 bit PCM recommended) -----------------------------------------------------------------------
// Choose the start sound (uncomment the one you want) --------
volatile int startVolumePercentage = 150; // Adjust the start volume (usually = 100%)
//#include "sounds/UnionPacific2002start.h" // Union Pacific 2002 SD70M Locomotive Start
//#include "sounds/HgP408Start.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "sounds/KenworthW900Start.h" // Kenworth W900 Truck Start
//#include "sounds/MackStart.h" // Mack Truck Start
//#include "sounds/DetroitDieselStart.h" // Detroit Diesel generic Truck Start
//#include "sounds/InternationalDT-466Start.h" // International DT-466 Start
//#include "sounds/Cat3406BStart.h" // Caterpillar 3406B start
//#include "sounds/ActrosV8Start.h" // MB Actros V8 Truck Start
//#include "sounds/VWBeetleStart.h" // VW Beetle or Bug
//#include "sounds/HarleyDavidsonFXSBStart.h" // Harley Davidson FXSB start
//#include "sounds/JeepGrandCherokeeTrackhawkStart.h" // Jeep Grand Cherokee Trackhawk start
//#include "sounds/carCranking.h" // Generic car cranking
#include "sounds/Cat3408Start.h" // CAT 3408 V8 Diesel start (Kenworth W900A)
//#include "sounds/1981KenworthW900A_DetroitStart.h" // Detroit Diesel 8V71 start sound (1981 Kenworth W900A)
//#include "sounds/KenworthCummins335Start.h" // Cummins 335 R6 Diesel start sound (1952 Kenworth)

// Choose the motor idle sound (uncomment the one you want) --------
volatile int idleVolumePercentage = 90; // Adjust the idle volume (usually = 100%, more also working, depending on sound, 50 - 60% if additional diesel knock sound is used)
volatile int engineIdleVolumePercentage = 60; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!)
volatile int fullThrottleVolumePercentage = 130; // Volume Percentage while full throttle (for rev sound as well)
//#include "sounds/20Hz.h" // 20Hz test tone
//#include "sounds/100Hz.h" // 100Hz test tone
//#include "sounds/1000Hz.h" // 1000Hz test tone
//#include "sounds/UnionPacific2002idle.h" // Union Pacific 2002 SD70M Locomotive with 16 cylinder Diesel (set volume to 60%)
//#include "sounds/UralV8Idle2.h" // Ural 4320 V8
//#include "sounds/HumveeDieselIdle.h" // "Humvvee" (Hummer H1) V8 Diesel
//#include "sounds/HgP408Idle.h" // HG P408 Humvee Diesel (only for small speakers)
//#include "sounds/DetroitDieselIdle.h" // Detroit Diesel generic Truck (Volume 80%, 30%, Turbo 60%, 10%, Wastegate 50%, 1%, CEP 100, TSM 3)
//#include "sounds/DetroitDieselStraightPipeIdle.h" // Detroit Diesel Truck with straight pipes (use multiplier = 2, acc = 2, dec = 1)
//#include "sounds/DetroitDieselBassBoost15db.h" // Detroit Diesel Truck with straight pipes. Bass 100-200Hz + 15dB
//#include "sounds/DetroitDieselBassBoost7db.h" // Detroit Diesel Truck with straight pipes. Bass 100-200Hz + 7dB (use it for King Hauler, Volume 60%, 40%, Turbo 60%, 10, Wastegate 100%, 1%, CEP 100, TSM 3, Knock volume 400, 10%, interval 2, 50)
//#include "sounds/DetroitDieselPeterbiltCabover.h" // Detroit Diesel Peterbilt cabover truck
//#include "sounds/DetroitDieselKenworth.h" // Detroit Diesel Kenworth truck (use Ural V8 Start & 100% turbo)
//#include "sounds/DetroitDieselJohnDeereTractor.h" // Detroit Diesel John Deere tractor
//#include "sounds/InternationalDT-466Idle.h" // International DT-466 idle (Volume 100%, 35%, Turbo 60%, 10%, Wastegate 30%, 1%, CEP 100, TSM 3)
//#include "sounds/Cat3406BIdle.h" // Caterpillar 3406B idle (Volume 100%, 35%, Turbo 70%, 10%, Wastegate 30%, 1%, CEP 100, TSM 3)
//#include "sounds/ActrosV8Idle.h" // MB Actros V8 Truck Idle (very noisy)
//#include "sounds/ActrosLoggingTruckIdling.h" // Actros logging truck (Volume 80%, 40%, Turbo 60%, 10, Wastegate 100%, 1%, CEP 100, TSM 2, Knock volume 200, 10%, interval 4, 50)
//#include "sounds/ActrosLoggingTruckIdling2.h" // Actros logging truck (Volume 50%, 100%, Turbo 10%, 10, Wastegate 100%, 1%, CEP 100, TSM 3, Knock volume 500, 10%, interval 2, 50, ActrosLoggingTruckDieselKnock3.h)
//#include "sounds/KenworthW900Idle.h" // Kenworth W900 Truck Idle (Volume 80%, 30%, Turbo 60%, 10%, Wastegate 50%, 1%, CEP 100, TSM 2)
//#include "sounds/MackIdle.h" // Mack Truck Idle (very thin)
//#include "sounds/DodgeChallenger70Idle.h" // 1970 Dodge Challenger
//#include "sounds/LaFerrariIdle.h" // Ferrari "LaFerrari"
//#include "sounds/V8TrophyTruckIdle.h" // V8 Trophy Truck
//#include "sounds/DAFXFIdle.h" // DAF XF truck
//#include "sounds/3408CatIdle.h" // CAT 3408 V8 Diesel idle (Kenworth W900A)
//#include "sounds/3408CatIdleLowpass.h" // CAT 3408 V8 Diesel idle (Kenworth W900A, 1.5kHz lowpass filtered)
//#include "sounds/8V71DetroitDieselStraightStacksIdle.h" // Detroit 8V71 two stroke V8 Diesel
//#include "sounds/8V71DetroitDieselStraightStacksIdle2.h" // Detroit 8V71 two stroke V8 Diesel
//#include "sounds/1981KenworthW900A_DetroitIdle.h" // Detroit Diesel 8V71 idle sound (1981 Kenworth W900A)
//#include "sounds/KenworthCummins335Idle.h" // Cummins 335 R6 Diesel idle sound (1952 Kenworth)
#include "sounds/MackSuperLinerIdle.h" // MACK Super Liner idle

// Choose the motor revving sound (uncomment the one you want) --------
#define REV_SOUND // uncomment this, if you want to use the separate, optional rev sound
volatile int revVolumePercentage = 100; // Adjust the idle volume (usually = 100%, more also working, depending on sound)
volatile int engineRevVolumePercentage = 60; // the engine volume will be throttle dependent (usually = 40%, never more than 100%!)
volatile const uint16_t revSwitchPoint = 10; // The rev sound is played instead of the idle sound above this point
volatile const uint16_t idleEndPoint = 500; // above this point, we have 100% rev and 0% idle sound volume (usually 500, min. 50 more than revSwitchPoint)
volatile const uint16_t idleVolumeProportionPercentage = 90; // The idle sound volume proportion (rest is rev proportion) below "revSwitchPoint" (about 90 - 100%, never more than 100)
#ifdef REV_SOUND
//#include "sounds/3408CatRev.h" // CAT 3408 V8 Diesel idle (Kenworth W900A)
//#include "sounds/3408CaterpillarPeterbiltRev.h" // CAT 3408 V8 Diesel rev (Peterbilt)
//#include "sounds/BadAssRev.h" // Badass rev
//#include "sounds/CaboverCATrev.h" // Cabover wit CAT engine rev
//#include "sounds/1981KenworthW900A_DetroitRev.h" // Detroit Diesel 8V71 rev sound (1981 Kenworth W900A)
//#include "sounds/KenworthCummins335Rev2.h" // Cummins 335 R6 Diesel rev sound (1952 Kenworth)
//#include "sounds/truckSpotting44rev.h"
//#include "sounds/CaboverCATrev.h" // Cabover wit CAT engine rev
//#include "sounds/CaboverCATrevSlow.h" // Cabover wit CAT engine rev
//#include "sounds/straightPipedCompilationRev.h" // Straight piped V8 rev
#include "sounds/MackSuperLinerRev.h" // MACK Super Liner rev
#endif

// Choose the jake brake sound (uncomment the one you want) --------
#define JAKE_BRAKE_SOUND // uncomment this, if you want to use the jake brake sound
#define JAKEBRAKE_ENGINE_SLOWDOWN // Use jake brake to slow down engine while releasing throttle in neutral or during upshifting while applying throttle
volatile int jakeBrakeVolumePercentage = 140; // Adjust the max. volume (usually = 150%)
volatile int jakeBrakeIdleVolumePercentage = 0; // Adjust the min. volume (usually = 80%)
volatile int jakeBrakeMinRpm = 200; // Adjust the min. RPM for the jake brake (around 100)
#ifdef JAKE_BRAKE_SOUND
#include "sounds/JakeBrake.h" // CAT V8 Jake Brake sound
//#include "sounds/1981KenworthW900A_DetroitJakeBrake.h" // Detroit Diesel 8V71 Jake Brake sound (1981 Kenworth W900A)
//#include "sounds/KenworthCummins335JakeBrake.h" // Cummins 335 R6 Diesel Jake Brake sound (1952 Kenworth)
#endif

// Choose the Diesel (or whatever) ignition "knock" sound (played in the fixed sampling rate interrupt, uncomment the one you want,
// play around here, the results are amazing, if you hit the right combination with the idle sound!) --------
volatile int dieselKnockVolumePercentage = 200; // Adjust the Diesel knock volume (usually = 200 - 600%) 700
volatile int dieselKnockIdleVolumePercentage = 0; // Diesel knock volume while no throttle is applied (usually = 20%)
volatile int dieselKnockStartPoint = 10; // Volume will raise above this throttle input( usually 0, for "open pipe" exhaust about 250)
volatile int dieselKnockInterval = 8; // Idle sample length divided by this number (1 - 20, depending on sound files)
//#define R6 // 6th Knock will be louder
#define V8 // V8 engine (Ford, Scania): pulses 4 and 8 will bel louder, because only 90° gap between exhaust cycles in same manifold
//#define V8_468 // V8 468 engine
//#define V2 // V2 engine (Harley): first 2 of 4 pulses will be louder (set dieselKnockInterval to 4)
//#define R6 // 6th Knock will be louder
//#define R6_2 // 6th and 3rd Knock will be louder
volatile int dieselKnockAdaptiveVolumePercentage = 18; // Adjust the Diesel knock volume for the non-first knocks per engine cycle (usually = 50%) 18
//#define RPM_DEPENDENT_KNOCK // Knock volume also depending on engine RPM *****************
#ifdef RPM_DEPENDENT_KNOCK // These parameters are for RPM dependent knock mode only:
uint8_t minKnockVolumePercentage = 80; // percentage, if at knock start RPM (about 5 - 80)
uint16_t knockStartRpm = 50; // starting @ this RPM (about 50 - 400)
#endif // ********************************************************************************
//#include "sounds/DieselKnockDummy.h" // If you don't want Diesel knock sound
//#include "sounds/UralV8Knock2.h" // Ural 4320 V8
//#include "sounds/LanzBulldogDieselKnock.h" // Lanz Bulldog tractor (Interval = 2)
//#include "sounds/ActrosLoggingTruckDieselKnock.h" // Actros logging truck
//#include "sounds/V8TrophyTruckKnock.h" // V8 Trophy Truck
//#include "sounds/DAFXFKnock.h" // DAF XF truck
//#include "sounds/UnimogU1000TurboKnock.h" // Unimog U1000 Turbo
//#include "sounds/3408CatKnock.h" // CAT 3408 V8 Diesel knock (Kenworth W900A)
//#include "sounds/3408CaterpillarPeterbiltKnock.h" // CAT 3408 V8 Diesel rev (Peterbilt)
//#include "sounds/DetroitDieselKnock.h" // Detroit R6 Diesel
//#include "sounds/DetroitDieselKnock2.h" // Detroit R6 Diesel
//#include "sounds/8V71DetroitDieselStraightStacksKnock.h" // Detroit 8V71 two stroke V8 Diesel
//#include "sounds/BadAssKnock.h" // Badass knock
//#include "sounds/CaboverCATknock.h" // Cabover wit CAT engine knock
//#include "sounds/1981KenworthW900A_DetroitKnock.h" // Detroit Diesel 8V71 knock sound (1981 Kenworth W900A)
//#include "sounds/1981KenworthW900A_DetroitKnock2.h" // Detroit Diesel 8V71 knock sound (1981 Kenworth W900A)
//#include "sounds/KenworthCummins335Knock2.h" // Cummins 335 R6 Diesel knock sound (1952 Kenworth)
//#include "sounds/truckSpotting44knock.h"
//#include "sounds/CaboverCATknock.h" // Cabover wit CAT engine knock
//#include "sounds/CaboverCATknockSlow.h" // Cabover wit CAT engine knock
//#include "sounds/straightPipedCompilationKnock.h" // Straight piped V8 knock
#include "sounds/MackSuperLinerKnock.h" // MACK Super Liner knock


// Adjust the additional turbo sound (set "turboVolumePercentage" to "0", if you don't want it) --------
volatile int turboVolumePercentage = 25; // Adjust the turbo volume (usually = 70%)
volatile int turboIdleVolumePercentage = 0; // the turbo volume will be engine rpm dependent (usually = 10%)
#include "sounds/TurboWhistle.h" // Turbo sound, playing in parallel with engine sound!
//#include "sounds/UnionPacific2002turbo.h" // Union Pacific 2002 SD70M Locomotive with 16 cylinder Diesel

// Adjust the additional supercharger sound (set "chargerVolumePercentage" to "0", if you don't want it) --------
volatile int chargerVolumePercentage = 0; // Adjust the supercharger volume (usually = 70%)
volatile int chargerIdleVolumePercentage = 10; // the supercharger volume will be engine rpm dependent (usually = 10%)
volatile int chargerStartPoint = 10; // Volume will raise above this point ( usually 10)
//#include "sounds/supercharger.h" // supercharger sound, playing in parallel with engine sound!
#include "sounds/SuperchargerDummy.h"

// Adjust the additional turbo wastegate  / blowoff valve  sound (set "wastegateVolumePercentage" to "0", if you don't want it)--------
volatile int wastegateVolumePercentage = 90; // Adjust the wastegate volume (usually = 70%, up to 250%)
volatile int wastegateIdleVolumePercentage = 1; // Wastegate sound is played, after rapid throttle drop with engaged clutch
//#include "sounds/WastegateDummy.h"
#include "sounds/UnimogU1000TurboWastegate.h"
//#include "sounds/POWERSTROKEwastegate.h" // Ford Powerstroke 7.3l V8 Diesel

// Adjust the additional cooling fan sound (set "fanVolumePercentage" to "0", if you don't want it) --------
volatile int fanVolumePercentage = 0; // Adjust the fan volume (250% for Tatra 813, else 0%)
volatile int fanIdleVolumePercentage = 0; // the fan volume will be engine rpm dependent (usually = 10%)
volatile int fanStartPoint = 0; // Volume will raise above this point (250 for Tatra 813)
//#define GEARBOX_WHINING // Disables sound, if gearbox in neutral. Fan noise is used to simulate gearbox whining (set fanStartPoint above clutchEngagingPoint)
//#include "sounds/Tatra813Fan.h" // Tatra 813 8x8 V12 Diesel Cooling Fan
//#include "sounds/Tatra813FanNew.h" // Tatra 813 8x8 V12 Diesel Cooling Fan (use this)
//#include "sounds/Tatra813FanNewSlow.h" // Tatra 813 8x8 V12 Diesel Cooling Fan
#include "sounds/GenericFan.h" // Generic engine cooling fan
//#include "sounds/FanDummy.h"

// Choose the horn sound (uncomment the one you want) --------
volatile int hornVolumePercentage = 100; // Adjust the horn volume (usually = 100%)
//#include "sounds/TrainHorn.h" // American train horn (no loop)
//#include "sounds/HornblastersOUTLAWTrainHornShort.h" // Hornblasters outlaw train horn short (incl. loop)
//#include "sounds/ManTgeHorn.h" // MAN TGE truck horn (incl. loop)
#include "sounds/westinghouseHorn.h" // American truck horn (incl. loop)
//#include "sounds/FireTruckAirHorn.h" // US fire truck air horn (incl. loop)
//#include "sounds/CarHorn.h" // A boring car horn (incl. loop)
//#include "sounds/TruckHorn.h" // A generic truck horn (incl. loop)
//#include "sounds/PeterbiltHorn.h" // A Peterbilt truck horn (incl. loop, not the best)
//#include "sounds/2ToneTruckHorn.h" // A 2 tone truck horn
//#include "sounds/CaboverCAThorn.h" // Cabover with CAT engine horn (incl. loop)
//#include "sounds/MackSuperLinerHorn.h" // MACK Super Liner horn (incl. loop)
//#include "sounds/ScaniaV8trainHorn.h" // Scania with train horn (incl. loop)
//#include "sounds/DixieHorn.h" // Dixie horn
//#include "sounds/FireTruckAirHorn.h" // US fire truck (incl. loop)
//#include "sounds/StadtLuzernHorn.h" // Steam Ship Stadt Luzern Horn

// Choose the siren / additional horn sound (uncomment the one you want) --------
volatile int sirenVolumePercentage = 100; // Adjust the siren volume (usually = 100%)
//#include "sounds/sirenDummy.h" // If you don't want siren sound
//#include "sounds/UsPoliceSiren.h" // US Police siren
//#include "sounds/FireTruckAirSiren.h" // US fire truck air siren (King Hauler)
//#include "sounds/FireTruckAirSiren2.h" // US fire truck (incl. loop)
//#include "sounds/FeuerwehrMartinshorn.h" // European Feuerwehr Martinshorn
//#include "sounds/IrishFireEngineHorn.h" // Irish fire truck horn
//#include "sounds/siren01norskere22.h" // Norwegian siren
//#include "sounds/PostAutoHorn.h" // The typical Swiss post bus horn
//#include "sounds/brasil.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/in_the_summertime.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/JEEP_HORN_2.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/la_carica.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/la_cucaracha.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/marseillaise.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/river_kwai.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/startrek.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/susannah.h" // sound from nenno @ rc-modellbau-portal.de
#include "sounds/Tequila.h" // sound from nenno @ rc-modellbau-portal.de
//#include "sounds/DixieSiren.h" // Dixie siren
//#include "sounds/SwissPsalm.h" // Swiss national anthem
//#include "sounds/Alphorn.h" // Alphorn
//#include "sounds/kittScanner.h" // Knight Rider KITT scanner. Use it in combination with Neopixel animation.

// Choose the air brake release sound (uncomment the one you want) --------
volatile int brakeVolumePercentage = 150; // Adjust the brake volume (usually = 200%)
//#include "sounds/AirBrakeDummy.h" // If you don't want air brake sound
//#include "sounds/TruckAirBrakes.h" // Short truck air brake sound
//#include "sounds/TruckAirBrakesLong.h" // Long truck air brake sound
#include "sounds/TruckAirBrakes2.h" // Another truck air brake sound
//#include "sounds/AirBrakeSqueak.h" // Squeaky air brake sound
//#include "sounds/AirBrakeSqueak2.h" // Squeaky air brake sound
//#include "sounds/UralBrakeSqueak.h" // URAL 4320 air brake sound
//#include "sounds/Saurer2DMbrake.h" // // Saurer 2DM air brake sound

// Choose the parking brake engaging sound (uncomment the one you want) --------
volatile int parkingBrakeVolumePercentage = 150; // Adjust the brake volume (usually = 200%)
//#include "sounds/ParkingBrakeDummy.h" // If you don't want parking brake sound
#include "sounds/ParkingBrake.h" // Parking brake sound
//#include "sounds/Saurer2DMparkingBrake.h" // // Saurer 2DM air parking brake sound

// Choose the gear shifting sound (uncomment the one you want) --------
volatile int shiftingVolumePercentage = 200; // Adjust the shifting volume (usually = 200%)
//#include "sounds/AirShiftingDummy.h" // If you don't want pneumatic shifting sound
//#include "sounds/AirShifting.h" // Pneumatic shifting sound
#include "sounds/ClunkingGearShifting.h" // Manual clunking shifting sound

// Choose the additional "sound1" (uncomment the one you want) --------
volatile int sound1VolumePercentage = 100; // Adjust the sound1 volume (usually = 100%)
//#include "sounds/EMDLocomotiveBell.h" // American EMD locomotive bell
//#include "sounds/007JamesBond.h" // James Bond melody
#include "sounds/M2Fire.h" // M2 salve
//#include "sounds/GlenCanyon.h" // Glen Canyon country song for truckers ;-)
//#include "sounds/door.h" // opening and closing the door
//#include "sounds/Pigs.h" // pig transporter for Onkel_Tom ;-)
//#include "sounds/chirp.h" // central locking sound

// Choose the reversing beep sound --------
volatile int reversingVolumePercentage = 70; // Adjust the reversing sound volume (usually = 70%)
#include "sounds/TruckReversingBeep.h" // 1000Hz peep sound

// Choose the indicator / turn signal options --------
volatile int indicatorVolumePercentage = 100; // Adjust the indicator sound volume (usually = 100%)
const uint16_t indicatorOn = 300; // The indicator will be switched on above +/- this value, if wheels are turned
const boolean INDICATOR_DIR = true; // adjust indicator direction with true or false
#include "sounds/Indicator.h" // "Tick-Tack" sound

// Choose the trailer couplig & uncoupling sounds (uncomment the ones you want) --------
#define COUPLING_SOUND // uncomment this, if you want to use the trailer coupling & uncoupling sounds
volatile int couplingVolumePercentage = 100; // Adjust the volume (usually = 100%)
#ifdef COUPLING_SOUND
#include "sounds/coupling.h" // coupling
#include "sounds/uncoupling.h" // uncoupling
#endif

// Choose the tire squealing sound (uncomment the ones you want) --------
//#define TIRE_SQUEAL
volatile int tireSquealVolumePercentage = 160; // Adjust the tire squeal volume
#include "sounds/TireSqueal2.h"

// Choose the light options --------
#define XENON_LIGHTS // Headlights will show a xenon bulb ignition flash, if defined
//#define SEPARATE_FULL_BEAM // The full beam is a separate bulb, connected to roof lights pin. Connect roof lights to side lights in this case
//#define INDICATOR_SIDE_MARKERS // The indicators are used as side markers as well

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

// Clutch parameters (about 90 for manual transmission, 10 for automatic) -----------------------------------------------
uint16_t clutchEngagingPoint = 90; // CEP. The "clutch" is engaging above this point = engine rpm sound in synch with ESC power

// Engine parameters ----------------------------------------------------------------------------------------------------
// Engine max. RPM in % of idle RPM. About 200% for big Diesels, 400% for fast running motors.
uint32_t MAX_RPM_PERCENTAGE = 330; // NOTE! Was called TOP_SPEED_MULTIPLIER (TSM) in earlier versions and was a multiplier instead of a percentage!

// Engine mass simulation
//#define SUPER_SLOW // super slow running, heavy engine for locomotives etc. Makes rpm response much slower
const int8_t acc = 2; // Acceleration step (2) 1 = slow for locomotive engine, 9 = fast for trophy truck. Always 2 for automatic transmission!
const int8_t dec = 1; // Deceleration step (1) 1 = slow for locomotive engine, 5 = fast for trophy truck

// Vehicle type ----------------------------------------------------------------------------------------------------
// #define TRACKED_MODE // For Tanks, diggers, excavators etc. which use dual throttle input on CH2 and CH3
