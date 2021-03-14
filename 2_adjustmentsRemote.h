
// Select the remote configuration you have:
#define FLYSKY_FS_I6X // <------- Select (remove //) one of the remote configurations below
//#define MICRO_RC // <------- Don't use this with standard remotes!


// BOARD SETTINGS ********************************************************************************************************************
// Choose the board version
//#define PROTOTYPE_36 // 36 or 30 pin board (do not uncomment it or it will cause boot issues!)

// COMMUNICATION SETTINGS  ***********************************************************************************************************
// Choose the receiver communication mode (never uncomment more than one!) !!! ADJUST THEM BEFORE CONNECTING YOUR RECEIVER AND ESC !!!

// PWM servo signal communication (CH1 - CH4, 35, PPM headers, 6 channels) --------
// PWM mode active, if SBUS, IBUS, SERIAL and PPM are disabled (// in front of #define)

// SBUS communication (SBUS header, 13 channels. This is my preferred communication protocol)--------
#define SBUS_COMMUNICATION // control signals are coming in via the SBUS interface (comment it out for classic PWM RC signals)
boolean sbusInverted = true; // false = wired to non standard (inverted) SBUS signal (for example from my "Micro RC" receiver)

// IBUS communication (RX header, 13 channels not recommended, NO FAILSAFE, if bad contact in iBUS wiring!) --------
//#define IBUS_COMMUNICATION // control signals are coming in via the IBUS interface (comment it out for classic PWM RC signals)

// PPM communication (RX header, 8 channels, working fine, but channel signals are a bit jittery) --------
//#define PPM_COMMUNICATION // control signals are coming in via the PPM interface (comment it out for classic PWM RC signals)

// CONFIGURATIONS ********************************************************************************************************************
/*
  // Channel settings -----
  // Channnel assignment
  // Assign your remote channels (the green ones in the excel sheet "adjustmentsRemote.xlsx") and the sound controller channels (the blue ones)
  // for each function. Depending on your wiring and communication mode, not all channels are usable.
  // Example for Flysky:  -> connect receiver CH6 with sound controller CH2 etc. Also make sure, you plug in your PWM wires accordingly

  // Assign the remote channels and the sound controller channels (change the remote channel numbers in #define accordingly)

  // Channels reversed or not: select reversed or not by changing true / false for each channel (if the channel direction is wrong)

  // Channels auto zero adjustment or not: select auto zero calibration or not by changing true / false for each channel.
  // (don't use it for channels without spring centered neutral position, for potentiometers or for switches)

  // Channels signal range calibration -----
  const uint16_t pulseNeutral = 30; // 1500 +/- this value (around 30) is the neutral range
  const uint16_t pulseSpan = 480; // in theory 500 (1500 center position +/-500 = 1000 - 2000ms) usually about 480

  // Automatic or manual modes -----
  //#define AUTO_LIGHTS // Lights controlled by engine state or controller CH5
  //#define AUTO_ENGINE_ON_OFF // Engine switching on / off automatically via throttle stick and timer or manually by controller CH5
  //#define AUTO_INDICATORS // Indicators triggered automatically by steering angle or manually by controller CH6
*/

// Flysky FS-i6X remote --------------------------------------------------------------------------------------------------------------
#ifdef FLYSKY_FS_I6X

// Channel assignment
// Remote channel ####### // Sound controller channel ##########################################
#define STEERING 1        // CH1 steering
#define GEARBOX 6         // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 3        // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5            // CH4 horn and bluelight / siren
#define FUNCTION_R 2      // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L 4      // CH6 indicators, hazards
#define POT2 8            // CH7 pot 2
#define MODE1 7           // CH8 mode 1 switch
#define MODE2 9           // CH9 mode 2 switch
#define MOMENTARY1 16     // CH10
#define HAZARDS 16        // CH11
#define INDICATOR_LEFT 16 // CH12
#define INDICATOR_RIGHT 16// CH13

// Channels reversed or not
boolean channelReversed[14] = {
  false, // CH0 (unused)
  false, // CH1
  false, // CH2
  false, // CH3
  false, // CH4
  true, // CH5
  false, // CH6
  false, // CH7
  false, // CH8
  false, // CH9
  false, // CH10
  false, // CH11
  false, // CH12
  false
}; // CH13

// Channels auto zero adjustment or not
boolean channelAutoZero[14] = {
  false, // CH0 (unused)
  true, // CH1
  false, // CH2
  true, // CH3
  false, // CH4
  true, // CH5
  true, // CH6
  false, // CH7
  false, // CH8
  false, // CH9
  false, // CH10
  false, // CH11
  false, // CH12
  false
}; // CH13

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
//#define AUTO_LIGHTS
//#define AUTO_ENGINE_ON_OFF
//#define AUTO_INDICATORS

#endif


// "Micro RC" DIY Arduino remote -----------------------------------------------------------------------------------------------------
#ifdef MICRO_RC

// Channel assignment
// Remote channel ####### // Sound controller channel ##########################################
#define STEERING 1        // CH1 steering
#define GEARBOX 2         // CH2 3 position switch for gearbox (left throttle in tracked mode)
#define THROTTLE 3        // CH3 throttle & brake (right throttle in tracked mode)
#define HORN 5            // CH4 horn and bluelight / siren
#define FUNCTION_R 4      // CH5 jake brake, high / low beam, headlight flasher, engine on / off
#define FUNCTION_L 16     // CH6 indicators, hazards
#define POT2 16           // CH7 pot2
#define MODE1 6           // CH8 mode 1 switch
#define MODE2 7           // CH9 mode 2 switch
#define MOMENTARY1 8      // CH10
#define HAZARDS 9         // CH11
#define INDICATOR_LEFT 10 // CH12
#define INDICATOR_RIGHT 11// CH13

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
  true, // CH8
  true, // CH9
  false, // CH10
  false, // CH11
  false, // CH12
  false
}; // CH13

// Channels auto zero adjustment or not (don't use it for channels without spring centered neutral position or switches)
boolean channelAutoZero[14] = {
  false, // CH0 (unused)
  true, // CH1
  false, // CH2
  true, // CH3
  false, // CH4
  true, // CH5
  true, // CH6
  false, // CH7
  false, // CH8
  false, // CH9
  false, // CH10
  false, // CH11
  false, // CH12
  false
}; // CH13

// Channels signal range calibration -----
const uint16_t pulseNeutral = 30;
const uint16_t pulseSpan = 480;

// Automatic or manual modes -----
//#define AUTO_LIGHTS
//#define AUTO_ENGINE_ON_OFF
#define AUTO_INDICATORS

#endif
