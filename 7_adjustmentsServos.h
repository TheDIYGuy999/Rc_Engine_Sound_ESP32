/* SERVO OUTPUT SETTINGS ************************************************************************************************
 *  
 * The CH1 - CH6 headers are used as outputs in BUS communication mode (SBUS, IBUS, PPM)
 * This allows to use as super compact "bus-only" receiver
 * Set the endpoints here in 1000 - 2000 microseconds (equal to -45 to 45° servo angle)
 * !! WARNING: Don't connect a receiver to the "CH1 - CH6" headers, if BUS communication is selected. Ihis will short them out!!
 * 
 * Uncommenting "#define SERVO_DEBUG" in the main tab allows to calibrate the servo positions easily:
 * 1. select the "SERVOS_DEFAULT" servo configuration
 * 2. upload the sketch
 * 3. connect the servo you want to calibrate to the steeting channel CH2 on the sound controller
 * 4. turn your steering wheel until you cave the position you want
 * 5. write down the microseconds reading, which is displayed in the Arduino IDE serial monitor
 * 6. do it for every position
 * 7. make a servo configuration profile, using these values
 * 8. select this profile and upload the sketch
 * 9. that's it!
 */
 
 // Select the vehicle configuration you have:
#define SERVOS_DEFAULT // <------- Select (remove //) one of the remote configurations below
//#define SERVOS_LANDY
//#define SERVOS_RGT_EX86100
//#define SERVOS_ACTROS

// Default servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_DEFAULT

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
const uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
const uint16_t CH3L = 1300, CH3C = 1450, CH3R = 1600; // CH3 winch pull, off, release
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
const uint16_t CH3L = 1300, CH3C = 1450, CH3R = 1600; // CH3 winch pull, off, release
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 300; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements

#endif

// RGT EX86100 servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_RGT_EX86100

#define MODE2_WINCH // Mode 2 is used for winch mode, if defined. The winch is controlled by the CH4 pot and connected to Servo CH3. BUS mode only!

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 2000, CH1C = 1660, CH1R = 1190; // CH1 steering left, center, right
const uint16_t CH2L = 1000, CH2C = 1500, CH2R  = 2000; // CH2 transmission gear 1, 2, 3
const uint16_t CH3L = 1435, CH3C = 1495, CH3R = 1560; // CH3 winch pull, off, release
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 300; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements

#endif

// Hercules Hobby Actros 3363 -------------------------------------------------------------------------------------------
#ifdef SERVOS_ACTROS

#define CH3_BEACON // Rotating Beacons are connected to Servo CH3. BUS mode only! https://www.ebay.ch/itm/303979210629

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz
 
// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits 
const uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
const uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
const uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
const uint16_t CH4L = 1300, CH4R = 1700; // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time 
const uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 1000 for "scale" servo movements

#endif
