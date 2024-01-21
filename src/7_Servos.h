#include <Arduino.h>

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
// #define SERVOS_LANDY_MN_MODEL
// #define SERVOS_LANDY_DOUBLE_EAGLE
// #define SERVOS_C34
// #define SERVOS_URAL
// #define SERVOS_RGT_EX86100
// #define SERVOS_ACTROS
// #define SERVOS_KING_HAULER
// #define SERVOS_RACING_TRUCK
// #define SERVOS_MECCANO_DUMPER
//#define SERVOS_OPEN_RC_TRACTOR

// Default servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_DEFAULT

#define CH3_BEACON              // Rotating Beacons are connected to Servo CH3. BUS mode only! https://www.ebay.ch/itm/303979210629
#define MODE2_TRAILER_UNLOCKING // The mode 2 button is used for trailer unlocking by servo CH4 (sound1 triggering will not work!)

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
uint16_t CH4L = 1350, CH4R = 1650;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// MN Model 1:12 Land Rover Defender servo configuration profile -------------------------------------------------------------------
#ifdef SERVOS_LANDY

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1840, CH1C = 1485, CH1R = 1090; // CH1 steering left 1880, center 1480, right 1080
uint16_t CH2L = 978, CH2C = 1833, CH2R = 1833;  // CH2 transmission gear 1 978, 2 1833, 3 1833
uint16_t CH3L = 1300, CH3C = 1450, CH3R = 1600; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// Double Eagle 1:8 Land Rover Defender servo configuration profile -------------------------------------------------------------------
#ifdef SERVOS_LANDY_DOUBLE_EAGLE

#define MODE2_WINCH // Mode 2 is used for winch mode, if defined. The winch is controlled by the CH4 pot and connected to Servo CH3. BUS mode only!
// #define NO_WINCH_DELAY // Use this, if you don't want a winch on / off ramp

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 900, CH1C = 1600, CH1R = 2200;  // CH1 steering left 900, center 1600, right 2200
uint16_t CH2L = 1900, CH2C = 1000, CH2R = 1000; // CH2 transmission gear 1 1900, 2 1000, 3 1000
uint16_t CH3L = 2000, CH3C = 1500, CH3R = 1000; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// WPL C34 Toyota Land Cruiser configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_C34

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1940, CH1C = 1600, CH1R = 1140; // CH1 steering left 1990, center 1640, right 1090
uint16_t CH2L = 670, CH2C = 1800, CH2R = 1800;  // CH2 transmission gear 1 978, 2 1800, 3 1800
uint16_t CH3L = 1300, CH3C = 1450, CH3R = 1600; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// WPL Ural servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_URAL

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1990, CH1C = 1640, CH1R = 1090; // CH1 steering left 1990, center 1640, right 1090
uint16_t CH2L = 670, CH2C = 1800, CH2R = 1800;  // CH2 transmission gear 1 978, 2 1800, 3 1800
uint16_t CH3L = 1300, CH3C = 1450, CH3R = 1600; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// RGT EX86100 servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_RGT_EX86100

#define MODE2_WINCH    // Mode 2 is used for winch mode, if defined. The winch is controlled by the CH4 pot and connected to Servo CH3. BUS mode only!
#define NO_WINCH_DELAY // Use this, if you don't want a winch on / off ramp

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 2100, CH1C = 1660, CH1R = 1190; // CH1 steering left 2000, center 1660, right 1190
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1435, CH3C = 1495, CH3R = 1560; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// Hercules Hobby Actros 3363 -------------------------------------------------------------------------------------------
#ifdef SERVOS_ACTROS

#define CH3_BEACON              // Rotating Beacons are connected to Servo CH3. BUS mode only! https://www.ebay.ch/itm/303979210629
#define MODE2_TRAILER_UNLOCKING // The mode 2 button is used for trailer unlocking by servo CH4 (sound1 triggering will not work!)

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
uint16_t CH4L = 1350, CH4R = 1650;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// TAMIYA King Hauler -------------------------------------------------------------------------------------------
#ifdef SERVOS_KING_HAULER

#define CH3_BEACON              // Rotating Beacons are connected to Servo CH3. BUS mode only! https://www.ebay.ch/itm/303979210629
#define MODE2_TRAILER_UNLOCKING // The mode 2 button is used for trailer 5th wheel unlocking by servo CH4 (sound1 triggering will not work!)

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
uint16_t CH4L = 1500, CH4R = 1100;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 1000; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// Carson Mercedes Racing Truck -------------------------------------------------------------------------------------------
#ifdef SERVOS_RACING_TRUCK

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1850, CH1C = 1500, CH1R = 1120; // CH1 steering left, center, right
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// Meccano 3 Ton Dumper -------------------------------------------------------------------------------------------
#ifdef SERVOS_MECCANO_DUMPER

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1000, CH1C = 1500, CH1R = 2000; // CH1 steering left, center, right
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 1000, CH3C = 1500, CH3R = 2000; // CH3 Beacons (modes are switched, if position changes from 1000 to 2000us)
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 6000; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif

// Open RC Tractor servo configuration profile -------------------------------------------------------------------------------------------
#ifdef SERVOS_OPEN_RC_TRACTOR

//#define MODE2_WINCH    // Mode 2 is used for winch mode, if defined. The winch is controlled by the CH4 pot and connected to Servo CH3. BUS mode only!
//#define NO_WINCH_DELAY // Use this, if you don't want a winch on / off ramp
#define MODE2_HYDRAULIC    // Mode 2 is used for hydraulic mode, if defined. The hydraulic is controlled by the CH4 pot and connected to Servo CH3. BUS mode only!

// Servo frequency
const uint8_t SERVO_FREQUENCY = 50; // usually 50Hz, some servos may run smoother @ 100Hz

// WARNING: never connect receiver PWM signals to the "CH" pins in BUS communication mode!

// Servo limits
uint16_t CH1L = 1620, CH1C = 1480, CH1R = 1300; // CH1 steering left 1620, center 1460, right 1300
uint16_t CH2L = 1000, CH2C = 1500, CH2R = 2000; // CH2 transmission gear 1, 2, 3
uint16_t CH3L = 2000, CH3C = 1500, CH3R = 1000; // CH3 winch pull, off, release
uint16_t CH4L = 1300, CH4R = 1700;              // CH4 trailer coupler (5th. wheel) locked, unlocked

// Servo ramp time
uint16_t STEERING_RAMP_TIME = 0; // 0 = fastest speed, enlarge it to around 3000 for "scale" servo movements

#endif
