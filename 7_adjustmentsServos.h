/* SERVO OUTPUT SETTINGS ************************************************************************************************
 *  
 * The CH1 - CH6 headers are used as outputs in BUS communication mode (SBUS, IBUS, PPM)
 * This allows to use as super compact "bus-only" receiver
 * Set the endpoints here in microseconds (equal to -45 to 45° servo angle)
 * !! WARNING: Don't connect a receiver to the "CH1 - CH6" headers, if BUS communication is selected. Ihis will short them out!!
 */

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
const uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 1000; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements
