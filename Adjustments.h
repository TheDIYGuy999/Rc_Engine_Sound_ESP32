/* ******* RECOMMENDED Audacity Settings: 22'050 Hz, 8 bit PCM **********

 Sound files converted with: https://github.com/TheDIYGuy999/Rc_Engine_Sound_ESP32/blob/master/Audio2Header.html
 Original converter code by bitluni (send him a high five, if you like the code)

Recommended ESC & settings for TAMIYA trucks with 3 speed transmission:

ESC: HOBBYWING QUICRUN WP Crawler Brushed with the following settings:
1: 3
2: 1
3: 3
4: 3
5: 4
6: 2
7: 9
8: 1 
9: 8 (change it, important)
10: 4
11: 4
12: 5
13: 4
14: 1 (be careful here!!)
15: 1 (change it, important)

Motor: 540 size, 35 turns, stock pinion
*/

// BOARD SETTINGS ******************************************************************************************************
// Choose the board version
//#define PROTOTYPE_36 // 36 or 30 pin board (do not uncomment it or it will cause boot issues!)

// COMMUNICATION SETTINGS **********************************************************************************************
// Choose the receiver communication mode (never uncomment more than one!)
// NOTE: SBUS is strongly recommended, because it allows to have a bigger RPM range: MAX_RPM_PERCENTAGE can be 400 instead of 300!

// PWM servo signal communication (CH1 - 4 headers) --------
// PWM mode active, if SBUS, IBUS, SERIAL and PPM are disabled (// in front of #define)
//#define PWM_CHANNEL_5 // If you want to feed in a 5th PWM channel on pin 35 for additional functions (not recommended, TODO, not working!)

// SBUS communication (SBUS header)--------
#define SBUS_COMMUNICATION // control signals are coming in via the SBUS interface (comment it out for classic PWM RC signals)
boolean sbusInverted = true; // true = wired to non standard (inverted) SBUS signal (for example from "Micro RC" receiver)

// IBUS communication (RX header, TODO experimental, untested!) --------
//#define IBUS_COMMUNICATION // control signals are coming in via the IBUS interface (comment it out for classic PWM RC signals)

// Serial communication (RX header, deprecated, use SBUS) --------
//#define SERIAL_COMMUNICATION // control signals are coming in via the serial interface (comment it out for classic PWM RC signals)
// Only for my "Micro RC" receiver! See: https://github.com/TheDIYGuy999/Micro_RC_Receiver

// PPM communication (PPM header) --------
//#define PPM_COMMUNICATION // control signals are coming in via the PPM interface (comment it out for classic PWM RC signals)
#define NUM_OF_CHL 8          // The number of channels inside our PPM signal (usually max. 8)
#define NUM_OF_AVG 1          // Number of averaging passes (usually one, more will be slow)

// VEHICLE SETTINGS ****************************************************************************************************
// Select the vehicle you want (uncomment the one you want)

// Master --------
//#include "vehicles/00_Master.h" // This master preset file contains all available sound files, which are not used in existing vehicle presets

// US trucks --------
//#include "vehicles/CaboverCAT3408.h" // Cabover truck with Caterpillar 3408 V8 Diesel
//#include "vehicles/KenworthW900ADetroit8V71.h" // Kenworth W900A with Detroit 8V71 V8 2 stroke Diesel
//#include "vehicles/KenworthW900ACAT3408.h" // Kenworth W900A with Caterpillar 3408 V8 Diesel
//#include "vehicles/KenworthCummins335.h" // 1950ies Kenworth with Cummins 335 R6 Diesel
//#include "vehicles/MackSuperLiner.h" // MACK Super Liner (TODO, needs rework)
//#include "vehicles/M35.h"// AM General M35 *deuce and a half" military truck, turbocharged R6 universal fuel engine

// EU trucks --------
//#include "vehicles/Tatra813.h" // Tatra 813 8x8 V12 Diesel military truck (old version for comparison, don't use it)
//#include "vehicles/Tatra813new.h" // Tatra 813 8x8 V12 Diesel military truck
//#include "vehicles/UmimogU1000.h" // Umimog U 1000 with turbocharged R6 Diesel incl. Feuerwehr "Martinshorn" siren (TODO, rework idle)
//#include "vehicles/MercedesActros1836.h" // Mercedes Actros 1863 or 3363 truck with R6 Diesel
//#include "vehicles/ScaniaV8_50ton.h" // SCANIA V8 50 ton truck. Unknown model. Lots of bass, but a bit noisy
//#include "vehicles/ScaniaV8.h" // SCANIA V8 truck, unknown model
//#include "vehicles/1000HpScaniaV8.h" // 1000 HP SCANIA V8 truck with open pipes. Insane sound! Good bass speakers reqired
//#include "vehicles/Scania143.h" // SCANIA 143 V8 - the legend! The best sounding in my opinion
//#include "vehicles/ScaniaV8Firetruck.h" // SCANIA V8 firetruck, automatic Allison 6 speed transmission with torque converter, "Martinshorn" siren

// Russian trucks --------
//#include "vehicles/Ural4320.h" // URAL 4320 6x6 V8 Diesel military truck (TODO, rework required)
//#include "vehicles/Ural375D.h" // URAL 375D 6x6 V8 petrol military truck
//#include "vehicles/GAZ66.h" // GAZ-66 V8 petrol military truck

// Russian tanks -------
//#include "vehicles/IS3.h" // IS-3 WW2 battle tank, V12 Diesel (dual ESC mode, good bass speaker required))

// US motorcycles --------
//#include "vehicles/HarleyDavidsonFXSB.h" // Harley Davidson FXSB V2 motorcycle

// US cars --------
//#include "vehicles/ChevyNovaCoupeV8.h" // 1975 Chevy Nova Coupe V8
//#include "vehicles/1965FordMustangV8.h"// 1965 Ford Mustang V8 (TODO rework rev, too fast?)

// EU cars --------
//#include "vehicles/VwBeetle.h" // VW Käfer / Beetle
//#include "vehicles/JaguarXJS.h" // Jaguar XJS V12, manual transmission
//#include "vehicles/JaguarXJSautomatic.h" // Jaguar XJS V12, automatic transmission
//#include "vehicles/MGBGtV8.h" // MGB GT V8, manual transmission
#include "vehicles/LaFerrari.h" // Ferrari LaFerrari, V12, 6 speed double clutch transmission

// US SUV --------
//#include "vehicles/JeepGrandCherokeeTrackhawk.h" // Jeep Grand Cherokee Trackhawk V8 monster SUV with supercharger, 6 speed automatic
//#include "vehicles/FordPowerstroke.h" // Ford Powerstroke 7.3l V8 Diesel, 6 speed automatic (good bass speaker required)

// EU SUV --------
//#include "vehicles/DefenderV8Automatic.h" // Land Rover Defender 90 V8 automatic (very nice V8 with lots of bass)
//#include "vehicles/DefenderV8CrawlerAutomatic.h" // Land Rover Defender 90 V8 automatic crawler
//#include "vehicles/DefenderTd5.h" // Land Rover Defender 90 Td5 R5 Diesel

// US locomotives --------
//#include "vehicles/UnionPacific2002.h" // Union Pacific 2002 SD70M locomotive with enormous, low revving 16 cylinder Diesel

// Planes --------
//#include "vehicles/MesserschmittBf109.h" // Messerschmitt BF 109 WW2 German V12 plane
