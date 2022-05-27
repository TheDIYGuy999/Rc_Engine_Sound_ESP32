/*
 * dashboard.h - Library to drive a 80x160px color LCD as rc truck dashboard
 *
 * https://github.com/Gamadril/ESP32_LASE
 * MIT License
 */

#ifndef DASHBOARD_h
#define DASHBOARD_h

//#include "Arduino.h"
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 80
#define DEG2RAD 0.0174532925
#define SPEED_MIN 0
#define SPEED_MAX 120
#define RPM_MIN 0
#define RPM_MAX 500
#define FUEL_MIN 0
#define FUEL_MAX 100
#define ADBLUE_MIN 0
#define ADBLUE_MAX 100

#define SPEED_CIRCLE_CENTER_X 40
#define SPEED_CIRCLE_CENTER_Y 35
#define SPEED_CIRCLE_RADIUS 25
#define SPEED_NEEDLE_LENGTH 20
#define SPEED_CIRCLE_ANGLE_START -203 // -180 - 23
#define SPEED_CIRCLE_SEGMENTS 12
#define SPEED_CIRCLE_SEGMENT_ANGLE 18.95
#define SPEED_CIRCLE_ANGLE_END                                                                     \
SPEED_CIRCLE_ANGLE_START + SPEED_CIRCLE_SEGMENTS *SPEED_CIRCLE_SEGMENT_ANGLE

#define RPM_CIRCLE_CENTER_X SCREEN_WIDTH - 40
#define RPM_CIRCLE_CENTER_Y 35
#define RPM_CIRCLE_RADIUS 25
#define RPM_CIRCLE_ANGLE_START -202 // -180 - 22
#define RPM_CIRCLE_SEGMENTS 25
#define RPM_CIRCLE_SEGMENT_ANGLE 9
#define RPM_CIRCLE_ANGLE_END RPM_CIRCLE_ANGLE_START + RPM_CIRCLE_SEGMENTS *RPM_CIRCLE_SEGMENT_ANGLE

#define FUEL_CIRCLE_CENTER_X 30
#define FUEL_CIRCLE_CENTER_Y 60
#define FUEL_CIRCLE_RADIUS 14
#define FUEL_CIRCLE_ANGLE_START -202 // -180 - 22
#define FUEL_CIRCLE_SEGMENTS 16
#define FUEL_CIRCLE_SEGMENT_ANGLE 14
#define FUEL_CIRCLE_ANGLE_END                                                                      \
FUEL_CIRCLE_ANGLE_START + FUEL_CIRCLE_SEGMENTS *FUEL_CIRCLE_SEGMENT_ANGLE

#define ADBLUE_CIRCLE_CENTER_X SCREEN_WIDTH - 30
#define ADBLUE_CIRCLE_CENTER_Y 60
#define ADBLUE_CIRCLE_RADIUS 14
#define ADBLUE_CIRCLE_ANGLE_START -202 // -180 - 22
#define ADBLUE_CIRCLE_SEGMENTS 16
#define ADBLUE_CIRCLE_SEGMENT_ANGLE 14
#define ADBLUE_CIRCLE_ANGLE_END                                                                    \
ADBLUE_CIRCLE_ANGLE_START + ADBLUE_CIRCLE_SEGMENTS *ADBLUE_CIRCLE_SEGMENT_ANGLE

#define INDICATOR_LEFT_CENTER_X SCREEN_WIDTH / 2 - 20
#define INDICATOR_LEFT_CENTER_Y 9

#define INDICATOR_RIGHT_CENTER_X SCREEN_WIDTH / 2 + 20
#define INDICATOR_RIGHT_CENTER_Y 9

const uint8_t LOW_BEAM_ICON[7][13] PROGMEM = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1},
    {0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}};

const uint8_t HIGH_BEAM_ICON[7][13] PROGMEM = {
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0}};

const uint8_t FOG_LIGHT_ICON[7][13] PROGMEM = {
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1},
    {0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1},
    {0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0}};

const uint8_t PUMP_ICON[6][7] PROGMEM = {
    {0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 0}};

/**
 * @brief Class acts as a wrapper around TFT_eSPI routines to draw dashboard elements.
 *
 */
class Dashboard {
public:
    /**
     * Default constructor
     */
    Dashboard();
    
    /**
     * Performs initialization of the display, rotates the coordinate system of the screen
     * (0,0) is the upper left corner if the connector is on the top.
     * Draws static dashboard elements and sets all values to 0.
     */
    void init(uint8_t value);
    
    /**
     * Sets the state of the left turn signal.
     * @param on - true to show the signal, false to deactivate it
     */
    void setLeftIndicator(boolean on);
    
    /**
     * Sets the state of the right turn signal.
     * @param on - true to show the signal, false to deactivate it
     */
    void setRightIndicator(boolean on);
    
    /**
     * Sets the state of the low beam lights.
     * @param on - true to show the indicator, false to deactivate it
     */
    void setLowBeamIndicator(boolean on);
    
    /**
     * Sets the state of the high beam lights.
     * @param on - true to show the indicator, false to deactivate it
     */
    void setHighBeamIndicator(boolean on);
    
    /**
     * Sets the state of the fog lights.
     * @param on - true to show the indicator, false to deactivate it
     */
    void setFogLightIndicator(boolean on);
    
    /**
     * Sets the speed to show by rotating the needle to the right position.
     * @param value - speed value between SPEED_MIN and SPEED_MAX
     */
    void setSpeed(uint16_t value);
    
    /**
     * Sets the RPM to show by rotating the needle to the right position.
     * @param value - RPM value between RPM_MIN and RPM_MAX
     */
    void setRPM(uint16_t value);
    
    /**
     * Sets the fuel level to show by rotating the needle to the right position.
     * @param value - fuel value between FUEL_MIN and FUEL_MAX
     */
    void setFuelLevel(uint16_t value);
    
    /**
     * Sets the AdBlue level to show by rotating the needle to the right position.
     * @param value - AdBlue value between ADBLUE_MIN and ADBLUE_MAX
     */
    void setAdBlueLevel(uint16_t value);
    
    /**
     * Sets the gear to show.
     * @param value - -1 for R, 0 for N, > 0 for gear number
     */
    void setGear(int8_t value);
    void setVolt(float value, float threshold);
    
private:
    TFT_eSPI _tft = TFT_eSPI();
    void drawFrame();
    void drawArc(int16_t cx, int16_t cy, int16_t r, int16_t deg_start, int16_t deg_end,
                 uint32_t color);
    void drawSep(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos, uint16_t r, uint32_t color);
    void drawNeedle(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos, uint16_t radius_holder,
                    uint32_t color_needle);
    void drawLeftIndicator(uint32_t color);
    void drawRightIndicator(uint32_t color);
    void drawLowBeamIndicator(uint32_t color);
    void drawHighBeamIndicator(uint32_t color);
    void drawFogLightIndicator(uint32_t color);
    void drawSpeed(uint16_t value);
    void drawRPM(uint16_t value);
    void drawFuel(uint16_t value);
    void drawAdBlue(uint16_t value);
    void drawPump(uint16_t x, uint16_t y, uint32_t color);
    void drawGear(char gear);
    void drawVolt(float volt, float threshold);
};

#endif
