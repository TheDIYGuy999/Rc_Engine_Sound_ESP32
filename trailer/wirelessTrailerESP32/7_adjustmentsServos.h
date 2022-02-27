#include <Arduino.h>

/* SERVO OUTPUT SETTINGS ************************************************************************************************
 *  
 */
 
 // Select the vehicle configuration you have:
#define SERVOS_DEFAULT // <------- Select (remove //) one of the remote configurations below


// Default servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_DEFAULT

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

#define LEGS_ESC_MODE // use this, if your legs are driven by an ESC instead of a servo
//#define RAMPS_ESC_MODE // use this, if your ramps are driven by an ESC instead of a servo

// Servo limits 
const uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 legs down, off, up
const uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 ramp down, off, up
const uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
const uint16_t CH4L = 1300, CH4R = 1700; // No usage

// Servo ramp times (not for ESC mode)
const uint16_t LEGS_RAMP_TIME = 5000; // 0 = fastest speed, enlarge it to around 5000 for "scale" servo movements
const uint16_t RAMPS_RAMP_TIME = 5000; // 0 = fastest speed, enlarge it to around 5000 for "scale" servo movements

#endif
