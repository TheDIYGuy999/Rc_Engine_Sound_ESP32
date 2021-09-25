// VEHICLE SETTINGS ****************************************************************************************************
// Select the vehicle preset you want (uncomment the one you want, remove //, never more than one)

// Master --------
//#include "vehicles/00_Master.h" // This master preset file contains all available sound files, which are not used in existing vehicle presets

// US trucks --------
//#include "vehicles/CaboverCAT3408.h" // Cabover truck with Caterpillar 3408 V8 Diesel
//#include "vehicles/KenworthW900ADetroit8V71.h" // Kenworth W900A with Detroit 8V71 V8 2 stroke Diesel
//#include "vehicles/KenworthW900ACAT3408.h" // Kenworth W900A with Caterpillar 3408 V8 Diesel
//#include "vehicles/KenworthW900ACAT3408new.h" // Kenworth W900A with Caterpillar 3408 V8 Diesel (good bass speaker required)
//#include "vehicles/KenworthCummins335.h" // 1950ies Kenworth with Cummins 335 R6 Diesel
//#include "vehicles/MackSuperLiner.h" // MACK Super Liner
//#include "vehicles/M35.h"// AM General M35 "deuce and a half" military truck, turbocharged R6 universal fuel engine
//#include "vehicles/US_Firetruck.h"// US firetruck with CAT3408 V8 and Allison 6 speed automatic (horn & siren loop capable)
//#include "vehicles/FreightlinerCummins350.h" // Freightliner Cabover with Cummins 350 R6 Diesel

// EU trucks --------
//#include "vehicles/Tatra813.h" // Tatra 813 8x8 V12 Diesel military truck (old version for comparison, don't use it)
//#include "vehicles/Tatra813new.h" // Tatra 813 8x8 V12 Diesel military truck
//#include "vehicles/UnimogU1000.h" // Umimog U 1000 with turbocharged R6 Diesel incl. Feuerwehr "Martinshorn" siren
//#include "vehicles/MercedesActros1836.h" // Mercedes Actros 1863 or 3363 truck with R6 Diesel
#include "vehicles/MercedesActrosV6.h" // Mercedes Actros V6 Race Truck incl. tire squealing
//#include "vehicles/ScaniaV8_50ton.h" // SCANIA V8 50 ton truck. Unknown model. Lots of bass, but a bit noisy
//#include "vehicles/ScaniaV8.h" // SCANIA V8 truck, unknown model
//#include "vehicles/1000HpScaniaV8.h" // 1000 HP SCANIA V8 truck with open pipes. Insane sound! Good bass speakers reqired
//#include "vehicles/Scania143.h" // SCANIA 143 V8 - the legend! The best sounding in my opinion
//#include "vehicles/ScaniaV8Firetruck.h" // SCANIA V8 firetruck, automatic Allison 6 speed transmission with torque converter, "Martinshorn" siren
//#include "vehicles/VolvoFH16_750.h" // Volvo FH16 750 truck. Inline 6, 750 horses, open pipes!
//#include "vehicles/VolvoFH16_OpenPipe.h" // Volvo FH truck. Inline 6, open pipes, alternative version
//#include "vehicles/ManTgx.h" // MAN TGX 680 V8 truck
//#include "vehicles/ManKat.h" // MAN KAT V8 Diesel German Bundeswehr military truck

// Russian trucks --------
//#include "vehicles/Ural4320.h" // URAL 4320 6x6 V8 Diesel military truck
//#include "vehicles/Ural375D.h" // URAL 375D 6x6 V8 petrol military truck
//#include "vehicles/URAL375.h" // URAL 375D 6x6 V8 petrol military truck (new version with better V8 sound, but good bass speaker required)
//#include "vehicles/GAZ66.h" // GAZ-66 V8 petrol military truck

// Russian tanks -------
//#include "vehicles/IS3.h" // IS-3 WW2 battle tank, V12 Diesel (dual ESC mode, good bass speaker required)

// Tractors -------
//#include "vehicles/KirovetsK700.h" // Russian Kirovets K700 monster tractor. Extreme turbo sound!

// Excavators -------
//#include "vehicles/Caterpillar323Excavator.h" // Caterpillar 323 excavator (use "FLYSKY_FS_I6S_EXCAVATOR" remote profile)

// US motorcycles --------
//#include "vehicles/HarleyDavidsonFXSB.h" // Harley Davidson FXSB V2 motorcycle

// US cars --------
//#include "vehicles/ChevyNovaCoupeV8.h" // 1975 Chevy Nova Coupe V8
//#include "vehicles/1965FordMustangV8.h"// 1965 Ford Mustang V8
//#include "vehicles/Chevy468.h" // Chevy 468 big block V8 (needs rework!)

// EU cars --------
//#include "vehicles/VwBeetle.h" // VW Käfer / Beetle
//#include "vehicles/JaguarXJS.h" // Jaguar XJS V12, manual transmission
//#include "vehicles/JaguarXJSautomatic.h" // Jaguar XJS V12, automatic transmission
//#include "vehicles/MGBGtV8.h" // MGB GT V8, manual transmission
//#include "vehicles/LaFerrari.h" // Ferrari LaFerrari, V12, 6 speed double clutch transmission

// US SUV & Pickups --------
//#include "vehicles/JeepGrandCherokeeTrackhawk.h" // Jeep Grand Cherokee Trackhawk V8 monster SUV with supercharger, 6 speed automatic
//#include "vehicles/FordPowerstroke.h" // Ford Powerstroke 7.3l V8 Diesel, 6 speed automatic (good bass speaker required)
//#include "vehicles/RAM2500_Cummins12V.h" // Dodge RAM 2500 with inline 6, 12V Cummins 5.9l Diesel, manual transmission
//#include "vehicles/RAM2500_Cummins12Vautomatic.h" // Dodge RAM 2500 with inline 6, 12V Cummins 5.9l Diesel, automatic transmission
//#include "vehicles/GMCsierra.h" // GMC Sierra V8 pickup, 3 speed automatic transmission
//#include "vehicles/ChevyNovaCoupeV8_P407.h" // 1975 Chevy Nova Coupe V8, special version for HG-P407, 3 speed automatic transmission
//#include "vehicles/JeepWranglerRubicon392V8.h" // 2021 Jeep Wrangler Rubicon HEMI 392 V8 (not the best)
//#include "vehicles/JeepWranglerRubicon392V8_2.h" // 2021 Jeep Wrangler Rubicon HEMI 392 V8 (insane bass!)

// EU SUV --------
//#include "vehicles/DefenderV8Automatic.h" // Land Rover Defender 90 V8 automatic (very nice V8 with lots of bass)
//#include "vehicles/DefenderV8OpenPipeAutomatic.h" // Land Rover Defender 90 V8 automatic, open pipes (optimised for smaller speakers)
//#include "vehicles/DefenderV8CrawlerAutomatic.h" // Land Rover Defender 90 V8 automatic crawler
//#include "vehicles/DefenderTd5.h" // Land Rover Defender 90 Td5 R5 Diesel

// Asian SUV --------
//#include "vehicles/LandcruiserFJ40.h" // Landcruiser Fj40 with inline 6 petrol engine
//#include "vehicles/LandcruiserFJ40Diesel.h" // Landcruiser Fj40 with inline 6 12H Turbo Diesel engine
//#include "vehicles/LandcruiserFJ40Diesel2.h" // Landcruiser Fj40 with inline 6 12H Turbo Diesel engine
//#include "vehicles/HiluxDiesel.h" // Hilux Diesel with inline 6 12H Turbo Diesel engine (for HG-P407)

// US locomotives --------
//#include "vehicles/UnionPacific2002.h" // Union Pacific 2002 SD70M locomotive with enormous, low revving 16 cylinder Diesel

// Planes --------
//#include "vehicles/MesserschmittBf109.h" // Messerschmitt BF 109 WW2 German V12 plane

// Generic Diesels --------
//#include "vehicles/generic6zylDiesel.h" // Generic inline 6 Diesel, no turbo, manual transmission (optimised for smaller speakers)
