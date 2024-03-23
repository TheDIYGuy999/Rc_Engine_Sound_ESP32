#include <Arduino.h>

// Select (remove //) the remote configuration profile you have:
// #define FLYSKY_FS_I6X // <------- Flysky FS-i6x
 #define FLYSKY_FS_I6S_LOADER     // <------- Flysky FS-i6s for BURNIE222 Volvo L120H loader (use IBUS communication setting)
// #define FLYSKY_FS_I6S_EXCAVATOR // <------- Flysky FS-i6s for KABOLITE K336 hydraulic excavator (use IBUS communication setting)
// #define FLYSKY_GT5              // <------- Flysky GT5 / Reely GT6 EVO / Absima CR6P
// #define RGT_EX86100             // <------- MT-305 remote delivered with RGT EX86100 crawler (use PWM communication setting)
// #define GRAUPNER_MZ_12          // <------- Graupner MZ-12 PRO
// #define MICRO_RC                // <------- The car style DIY "Micro RC" remote. Don't use this with standard remotes!
// #define MICRO_RC_STICK          // <------- The stick based DIY "Micro RC" remote. Don't use this with standard remotes!

// For testing only!
// #define FLYSKY_FS_I6S_EXCAVATOR_TEST // <------- Flysky FS-i6s for KABOLITE K336 hydraulic excavator

// BOARD SETTINGS *****************************************************************************************************************************
// Choose the board version
// #define PROTOTYPE_36 // 36 or 30 pin board (do not uncomment it or it will cause boot issues!)

// COMMUNICATION SETTINGS  ********************************************************************************************************************
// Choose the receiver communication mode (never uncomment more than one!) !!! ADJUST THEM BEFORE CONNECTING YOUR RECEIVER AND ESC !!!

// PWM servo signal communication (CH1 - CH6 headers, 6 channels) --------
// PWM mode active, if SBUS, IBUS, SUMD and PPM are disabled (// in front of #define)

// SBUS communication (RX header, 13 channels. This is my preferred communication protocol)--------
// #define SBUS_COMMUNICATION // control signals are coming in via the SBUS interface (comment it out for classic PWM RC signals)
// NOTE: "boolean sbusInverted = true / false" was moved to the remote configuration profiles, so you don't have to change it
uint32_t sbusBaud = 100000;         // Standard is 100000. Try to lower it, if your channels are coming in unstable. Working range is about 96000 - 104000.
#define EMBEDDED_SBUS               // Embedded SBUS code is used instead of SBUS library, if defined (recommended)
uint16_t sbusFailsafeTimeout = 100; // Failsafe is triggered after this timeout in milliseconds (about 100)

// IBUS communication (RX header, 13 channels not recommended, NO FAILSAFE, if bad contact in iBUS wiring!) --------
#define IBUS_COMMUNICATION // control signals are coming in via the IBUS interface (comment it out for classic PWM RC signals)

// SUMD communication (RX header, 12 channels, For Graupner remotes) --------
// #define SUMD_COMMUNICATION // control signals are coming in via the SUMD interface (comment it out for classic PWM RC signals)

// PPM communication (RX header, 8 channels, working fine, but channel signals are a bit jittery) --------
// #define PPM_COMMUNICATION // control signals are coming in via the PPM interface (comment it out for classic PWM RC signals)

// CHANNEL LINEARITY SETTINGS  ****************************************************************************************************************

#define EXPONENTIAL_THROTTLE // Exponential throttle curve. Ideal for enhanced slow speed control in crawlers
// #define EXPONENTIAL_STEERING // Exponential steering curve. More steering accuracy around center position

// CHANNEL AVERAGING (EXPERIMENTAL!) **********************************************************************************************************
// #define CHANNEL_AVERAGING // This is recommended, if you have issues with unstable channels

// CONFIGURATION PROFILES *********************************************************************************************************************
/*
  // Channel settings -----
  // Channnel assignment
  // Assign your remote channels (the green ones in the excel sheet "adjustmentsRemote.xlsx") and the sound controller channels (the blue ones)
  // for each function. Depending on your wiring and communication mode, not all channels are usable.
  // Example for Flysky:  -> connect receiver CH6 with sound controller CH2 etc. Also make sure, you plug in your PWM wires accordingly

  // Assign the remote channels and the sound controller channels (change the remote channel numbers in #define accordingly)

  // Channels reversed or not: select reversed or not by changing true / false for each channel (if the channel direction is wrong)

  // Channels auto zero adjustment or not: select auto zero calibration or not by changing true / false for each channel.
  // (don't use it for channels without spring centered neutral position @ 1500 microseconds, for potentiometers or for switches)

  // Channels signal range calibration -----
  const uint16_t pulseNeutral = 30; // 1500 +/- this value (around 30) is the neutral range
  const uint16_t pulseSpan = 480; // in theory 500 (1500 center position +/-500 = 1000 - 2000ms) usually about 480

  // Automatic or manual modes -----
  //#define AUTO_LIGHTS // Lights controlled by engine state or controller CH5
  //#define AUTO_ENGINE_ON_OFF // Engine switching on / off automatically via throttle stick and timer or manually by controller CH5
  //#define AUTO_INDICATORS // Indicators triggered automatically by steering angle or manually by controller CH6
*/

// IMPORTANT information for the Flysky FS-i6/X/S radios:
//
// (1) Go to "Functions" -> "Switches assign" and select SWA, SWB or SWD for "Fly mode".
// This will be the switch to access secondary functions on the FUNCTION_R and FUNCTION_L axes (by temporarily limiting their range/dual rate to 75%).
// This switch should NOT be assigned to anything else in "Aux. channels" (and as such not assigned anywhere below)! Most people use SWA, which is also the default.
//
// (2) Now go to "Functions" -> "Dual rate/exp.".
// Note, that "Normal" changes to "Sport" as soon as the selected Fly mode switch is toggled.
//
// (3) Now switch to "Sport" and set the "Rate" of "Ch2" from 100 to 75.
// Do the exact same thing for "Ch4", but don't change "Ch1" (this one should still be 100 in both modes)!

// Flysky FS-i6X remote configuration profile ---------------------------------------------------------------------------------------------------
#ifdef FLYSKY_FS_I6X

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 steering
#define GEARBOX 6            // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 3           // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5               // CH4 horn and bluelight / siren
#define FUNCTION_R 2         // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L 4         // CH6 indicators, hazards
#define POT2 8               // CH7 pot 2
#define MODE1 7              // CH8 mode 1 switch
#define MODE2 9              // CH9 mode 2 switch
#define MOMENTARY1 NONE      // CH10
#define HAZARDS NONE         // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    false, // CH2
    true,  // CH3
    false, // CH4
    true,  // CH5
    true,  // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
// #define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// Flysky FS-i6S remote configuration profile (for loaders only) --------------------------------------------------------------------------------------
#ifdef FLYSKY_FS_I6S_LOADER

// NOTE: The vehicle file needs to contain #define LOADER_MODE

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 bucket
#define GEARBOX 2            // CH2 lift
#define THROTTLE 3           // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 9               // CH4 horn and bluelight / siren 3 pos switch SWB
#define FUNCTION_R 7         // CH5 jake brake, high / low beam, headlight flasher, engine on / off VRB
#define FUNCTION_L 6         // CH6 indicators, hazards VRA
#define POT2 NONE               // CH7 pot 2
#define MODE1 NONE              // CH8 mode 1 switch
#define MODE2 NONE              // CH9 mode 2 switch
#define MOMENTARY1 NONE      // CH10
#define HAZARDS NONE         // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    false, // CH2
    true,  // CH3
    false, // CH4
    true,  // CH5
    true,  // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
// #define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// Flysky FS-i6S remote configuration profile (for excavators only) ---------------------------------------------------------------------------------
#ifdef FLYSKY_FS_I6S_EXCAVATOR

// NOTE: The vehicle file needs to contain #define EXCAVATOR_MODE

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 bucket
#define GEARBOX 2            // CH2 dipper
#define THROTTLE 9           // CH3 3 position switch off, idle, rev. 9 (6 for test)
#define HORN 10              // CH4 horn
#define FUNCTION_R 3         // CH5 boom
#define FUNCTION_L 6         // CH6 left track 6 (none for test)
#define POT2 7               // CH7 right track
#define MODE1 4              // CH8 swing
#define MODE2 NONE           // CH9
#define MOMENTARY1 NONE      // CH10
#define HAZARDS NONE         // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    true,  // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    true,  // CH6
    true,  // CH7
    true,  // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
// #define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// Flysky FS-i6S remote configuration profile (for excavators only) ---------------------------------------------------------------------------------
#ifdef FLYSKY_FS_I6S_EXCAVATOR_TEST

// NOTE: The vehicle file needs to contain #define EXCAVATOR_MODE

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 bucket
#define GEARBOX 2            // CH2 dipper
#define THROTTLE 6           // CH3 3 position switch off, idle, rev. 9 (6 for test)
#define HORN 10              // CH4 horn
#define FUNCTION_R 3         // CH5 boom
#define FUNCTION_L NONE      // CH6 left track 6 (none for test)
#define POT2 NONE            // CH7 right track
#define MODE1 4              // CH8 swing
#define MODE2 NONE           // CH9
#define MOMENTARY1 NONE      // CH10
#define HAZARDS NONE         // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    true,  // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    true,  // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
// #define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// Flysky GT5 / Reely GT6 EVO / Absima CR6P remote configuration profile (thanks to BlackbirdXL1 for making this profile)-----------------------
#ifdef FLYSKY_GT5

/* Communication settings (above):-----------------------
    #define IBUS_COMMUNICATION // Use IBUS (tested with FS-IA6B receiver)
*/

/* Transmitter settings -----------------------
    Menu EPA:
    select AUX 3
    L.F.U. 100%, R.B.D. 75%
    select AUX 6
    L.F.U. 75%, R.B.D. 75%

    SVC off
    CRAWL off

    Save
*/

/* Remote channel functions -----------------------
   Channel 1 = Steering and automatic indicators
   Channel 2 = Throttle & brake- reversing-lights
   Channel 3 = Push button on the transmitter grip = hazards on / off
   Channel 4 = 3 positiion switch on the transmitter grip = gearbox shifting
   Channel 5 = left pot: left turn = blue lights & siren, right turn = horn
   Channel 6 = right pot: left turn = engine start / stop, right turn = light sequences switching (hold it in end position, then return to center pos.)
*/

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 steering
#define GEARBOX 4            // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 2           // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5               // CH4 horn and bluelight / siren
#define FUNCTION_R 6         // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L 1         // CH6 indicators, hazards
#define POT2 NONE            // CH7 pot 2
#define MODE1 NONE           // CH8 mode 1 switch
#define MODE2 NONE           // CH9 mode 2 switch
#define MOMENTARY1 NONE      // CH10
#define HAZARDS 3            // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    false, // CH2
    true,  // CH3
    false, // CH4
    true,  // CH5
    true,  // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// RGT MT-305 configuration profile (comes with EX86100) -----------------------
#ifdef RGT_EX86100

/* Communication settings (above):-----------------------
   Use PWM commmunication mode
*/

/* Transmitter settings -----------------------
    CH1 & 2 reverse: R
    CH2 EPA (LO & HI): max.
    CH1 DR: about 60% (so that the steerning servo is not pushing against end stops)
    CH1 center position: 0
    CH2 center position: around 0, so that vehicle is driving a straight line

    NOTE: do not adjust the settings above without rebooting the ESP32!

*/

/* Remote channel functions -----------------------
   Channel 1 = Steering and automatic indicators
   Channel 2 = Throttle & brake- reversing-lights
   Channel 3 = 2 position switch on the transmitter grip = horn on / off
*/

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1           // CH1 steering
#define GEARBOX NONE         // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 2           // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 3               // CH4 horn and bluelight / siren
#define FUNCTION_R NONE      // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L NONE      // CH6 indicators, hazards
#define POT2 NONE            // CH7 pot 2
#define MODE1 NONE           // CH8 mode 1 switch
#define MODE2 NONE           // CH9 mode 2 switch
#define MOMENTARY1 NONE      // CH10
#define HAZARDS NONE         // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    true,  // CH3
    true,  // CH4
    true,  // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1
    false, // CH2
    true,  // CH3
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 500;

// Automatic or manual modes -----
#define AUTO_LIGHTS
#define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// Graupner mz-12 PRO remote configuration profile ---------------------------------------------------------------------------------------------------
#ifdef GRAUPNER_MZ_12

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 4           // CH1 steering
#define GEARBOX 5            // CH2 3 position switch for gearbox
#define THROTTLE 1           // CH3 throttle & brake
#define HORN 7               // CH4 horn
#define FUNCTION_R 3         // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L 2         // CH6 indicators, hazards
#define POT2 8               // CH7 pot 2
#define MODE1 6              // CH8 mode 1 switch
#define MODE2 NONE           // CH9 mode 2 switch
#define MOMENTARY1 NONE      // CH10
#define HAZARDS 9            // CH11
#define INDICATOR_LEFT NONE  // CH12
#define INDICATOR_RIGHT NONE // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = true; // true = standard (non inverted) SBUS signal

#endif

// "Micro RC" (the car style one) DIY Arduino remote configuration profile -------------------------------------------------------------------------------------------
#ifdef MICRO_RC

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1         // CH1 steering
#define GEARBOX 2          // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 3         // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5             // CH4 horn and bluelight / siren
#define FUNCTION_R 4       // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L NONE    // CH6 indicators, hazards
#define POT2 16            // CH7 pot2
#define MODE1 6            // CH8 mode 1 switch
#define MODE2 7            // CH9 mode 2 switch
#define MOMENTARY1 8       // CH10
#define HAZARDS 9          // CH11
#define INDICATOR_LEFT 10  // CH12
#define INDICATOR_RIGHT 11 // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    true,  // CH8
    true,  // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1 true
    false, // CH2
    true,  // CH3 true
    false, // CH4
    true,  // CH5 true
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
// #define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = false; // false = non standard (inverted) SBUS signal

#endif

// "Micro RC" (The stick based one) DIY Arduino remote configuration profile -------------------------------------------------------------------------------------------
#ifdef MICRO_RC_STICK

// Channel assignment (use NONE for non existing channels!)
// Remote channel #######   // Sound controller channel ##########################################
#define STEERING 1         // CH1 steering
#define GEARBOX NONE       // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 3         // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5             // CH4 horn and bluelight / siren
#define FUNCTION_R NONE    // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L NONE    // CH6 indicators, hazards
#define POT2 16            // CH7 pot2
#define MODE1 6            // CH8 mode 1 switch
#define MODE2 7            // CH9 mode 2 switch
#define MOMENTARY1 8       // CH10
#define HAZARDS 9          // CH11
#define INDICATOR_LEFT 10  // CH12
#define INDICATOR_RIGHT 11 // CH13

// Channels reversed or not
boolean channelReversed[14] = {
    false, // CH0 (unused)
    false, // CH1
    false, // CH2
    false, // CH3
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    true,  // CH8
    true,  // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position, switches or unused channels)
boolean channelAutoZero[14] = {
    false, // CH0 (unused)
    true,  // CH1 true
    false, // CH2
    true,  // CH3 true
    false, // CH4
    false, // CH5
    false, // CH6
    false, // CH7
    false, // CH8
    false, // CH9
    false, // CH10
    false, // CH11
    false, // CH12
    false  // CH13
};

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
#define AUTO_LIGHTS
// #define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

// SBUS mode ----
boolean sbusInverted = false; // false = non standard (inverted) SBUS signal

#endif
