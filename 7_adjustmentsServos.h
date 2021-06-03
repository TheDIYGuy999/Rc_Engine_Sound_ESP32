/* SERVO OUTPUT SETTINGS ************************************************************************************************
 *  
 * The CH1 - CH6 headers are used as outputs in BUS communication mode (SBUS, IBUS, PPM)
 * This allows to use as super compact "bus-only" receiver
 * Set the endpoints here in 1000 - 2000 microseconds (equal to -45 to 45° servo angle)
 * !! WARNING: Don't connect a receiver to the "CH1 - CH6" headers, if BUS communication is selected. Ihis will short them out!!
 */

 // Select the vehicle configuration you have:
#define SERVOS_DEFAULT // <------- Select (remove //) one of the remote configurations below
//#define SERVOS_LANDY

// Default servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_DEFAULT

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
const uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements

#endif

// Land Rover Defender servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_LANDY

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 1880, CH1C = 1480, CH1R = 1080; // CH1 steering left 1875, center 1475, right 1075
const uint16_t CH2L = 978, CH2C = 1833, CH2R = 1833; // CH2 transmission gear 1 978, 2 1833, 3 1833
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 300; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements

#endif
