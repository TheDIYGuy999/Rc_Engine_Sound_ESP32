#include "dashboard.h"

#ifndef FREVIC_DASHBOARD // Standard dashboard *********************************************************************************************************************
/*
 * dashboard.cpp - Library to drive a 80x160px color LCD as rc truck dashboard
 *
 * https://github.com/Gamadril/ESP32_LASE
 * MIT License
 */

#include <TFT_eSPI.h>
#include "dashLogos.h"

Dashboard::Dashboard() {}

void Dashboard::init(uint8_t value)
{
    _tft.init();
    _tft.setRotation(value); // 3 = normal, 1 = upside down
    _tft.fillScreen(TFT_BLACK);
    drawLogo(0, 0, logo_thediyguy, 80, 80);
    _tft.drawCentreString("Powered by", 120, 34, 1);
    _tft.drawCentreString("TheDIYGuy999", 120, 46, 1);
    delay(1500);
    _tft.fillScreen(TFT_BLACK);
    drawFrame();
    setSpeed(0);
    setRPM(0);
    setFuelLevel(0);
    setAdBlueLevel(0);
    setLeftIndicator(false);
    setRightIndicator(false);
    setLowBeamIndicator(false);
    setHighBeamIndicator(false);
    setFogLightIndicator(false);
}

void Dashboard::setLeftIndicator(boolean on)
{
    drawLeftIndicator(on ? TFT_DARKGREEN : offColor);
}

void Dashboard::setRightIndicator(boolean on)
{
    drawRightIndicator(on ? TFT_DARKGREEN : offColor);
}

void Dashboard::setSpeed(uint16_t value) { drawSpeed(value); }

void Dashboard::setRPM(uint16_t value) { drawRPM(value); }

void Dashboard::setFuelLevel(uint16_t value) { drawFuel(value); }

void Dashboard::setAdBlueLevel(uint16_t value) { drawAdBlue(value); }

void Dashboard::setLowBeamIndicator(boolean on)
{
    drawLowBeamIndicator(on ? TFT_GREEN : offColor);
}

void Dashboard::setHighBeamIndicator(boolean on)
{
    drawHighBeamIndicator(on ? TFT_BLUE : offColor);
}

void Dashboard::setFogLightIndicator(boolean on)
{
    drawFogLightIndicator(on ? TFT_GREEN : offColor);
}

void Dashboard::setGear(int8_t value)
{
    char symbol = ' ';
    if (value == -1)
    {
        symbol = 'R';
    }
    else if (value == 0)
    {
        symbol = 'N';
    }
    else if (value > 0 && value < 10)
    {
        symbol = '0' + value;
    }
    drawGear(symbol);
}

void Dashboard::setVolt(float value, float threshold)
{
    drawVolt(value, threshold);
}

void Dashboard::drawFrame()
{
    // speed
    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, TFT_LIGHTGREY);
    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 1,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, TFT_LIGHTGREY);

    for (int16_t i = 0; i <= SPEED_CIRCLE_SEGMENTS; i++)
    {
        drawSep(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, 3,
                SPEED_CIRCLE_ANGLE_START + i * SPEED_CIRCLE_SEGMENT_ANGLE, SPEED_CIRCLE_RADIUS,
                TFT_LIGHTGREY);
    }

    // rpm
    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS, RPM_CIRCLE_ANGLE_START,
            RPM_CIRCLE_ANGLE_END, TFT_LIGHTGREY);
    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 1, RPM_CIRCLE_ANGLE_START,
            RPM_CIRCLE_ANGLE_END, TFT_LIGHTGREY);

    uint32_t color;
    for (int16_t i = 0; i <= 5; i++)
    {
        if (i == 5)
        {
            color = TFT_RED;
        }
        else if (i == 4)
        {
            color = TFT_YELLOW;
        }
        else if (i == 2)
        {
            color = TFT_GREEN;
        }
        else
        {
            color = TFT_LIGHTGREY;
        }

        drawSep(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, 3, RPM_CIRCLE_ANGLE_START + i * 45,
                RPM_CIRCLE_RADIUS, color);
    }

    // fuel
    drawArc(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, FUEL_CIRCLE_RADIUS, FUEL_CIRCLE_ANGLE_START,
            FUEL_CIRCLE_ANGLE_END, TFT_LIGHTGREY);
    for (int16_t i = 0; i <= 4; i++)
    {
        drawSep(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, 3,
                FUEL_CIRCLE_ANGLE_START + i * FUEL_CIRCLE_SEGMENT_ANGLE * 4, FUEL_CIRCLE_RADIUS,
                TFT_LIGHTGREY);
    }
    drawSep(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, 3, FUEL_CIRCLE_ANGLE_START,
            FUEL_CIRCLE_RADIUS, TFT_RED);
    drawSep(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, 3, FUEL_CIRCLE_ANGLE_START + 7 * 2,
            FUEL_CIRCLE_RADIUS, TFT_RED);
    drawSep(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, 3, FUEL_CIRCLE_ANGLE_START + 7 * 4,
            FUEL_CIRCLE_RADIUS, TFT_RED);

    drawPump(FUEL_CIRCLE_CENTER_X + 4, FUEL_CIRCLE_CENTER_Y + 8, TFT_WHITE);

    // adblue
    drawArc(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, ADBLUE_CIRCLE_RADIUS,
            ADBLUE_CIRCLE_ANGLE_START, ADBLUE_CIRCLE_ANGLE_END, TFT_LIGHTGREY);
    for (int16_t i = 0; i <= 4; i++)
    {
        drawSep(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, 3,
                ADBLUE_CIRCLE_ANGLE_START + i * ADBLUE_CIRCLE_SEGMENT_ANGLE * 4, ADBLUE_CIRCLE_RADIUS,
                TFT_LIGHTGREY);
    }
    drawSep(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, 3, ADBLUE_CIRCLE_ANGLE_START,
            ADBLUE_CIRCLE_RADIUS, TFT_SKYBLUE);
    drawSep(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, 3, ADBLUE_CIRCLE_ANGLE_START + 7 * 2,
            ADBLUE_CIRCLE_RADIUS, TFT_SKYBLUE);
    drawSep(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, 3, ADBLUE_CIRCLE_ANGLE_START + 7 * 4,
            ADBLUE_CIRCLE_RADIUS, TFT_SKYBLUE);

    drawPump(ADBLUE_CIRCLE_CENTER_X + 4, ADBLUE_CIRCLE_CENTER_Y + 8, TFT_SKYBLUE);

    // gear
    drawGear('N');
}

void Dashboard::drawArc(int16_t cx, int16_t cy, int16_t r, int16_t deg_start, int16_t deg_end,
                        uint32_t color)
{
    float sx, sy;
    uint16_t x, y;

    for (int16_t i = deg_start; i <= deg_end; i++)
    {
        sx = cos(i * DEG2RAD);
        sy = sin(i * DEG2RAD);
        x = round(sx * r) + cx;
        y = round(sy * r) + cy;
        _tft.drawPixel(x, y, color);
    }
}

void Dashboard::drawSep(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos, uint16_t r,
                        uint32_t color)
{
    float sx, sy;
    uint16_t x0, y0, x1, y1;

    for (int8_t i = -1; i <= 1; i++)
    {
        sx = cos((i + deg_pos) * DEG2RAD);
        sy = sin((i + deg_pos) * DEG2RAD);
        x0 = round(sx * r) + cx;
        y0 = round(sy * r) + cy;
        x1 = round(sx * (r - len)) + cx;
        y1 = round(sy * (r - len)) + cy;
        _tft.drawLine(x0, y0, x1, y1, color);
    }
}

void Dashboard::drawNeedle(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos,
                           uint16_t radius_holder, uint32_t color_needle)
{
    float sx, sy;
    uint16_t x, y, xx, yy;

    _tft.fillCircle(cx, cy, radius_holder, TFT_DARKGREY);
    sx = cos(deg_pos * DEG2RAD);
    sy = sin(deg_pos * DEG2RAD);
    x = round(sx * len) + cx;
    y = round(sy * len) + cy;
    xx = -round(sx * 2) + cx;
    yy = -round(sy * 2) + cy;
    _tft.drawLine(x, y, xx, yy, color_needle);
}

void Dashboard::drawLeftIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    _tft.fillTriangle(INDICATOR_LEFT_CENTER_X - 4, INDICATOR_LEFT_CENTER_Y, INDICATOR_LEFT_CENTER_X,
                      INDICATOR_LEFT_CENTER_Y - 4, INDICATOR_LEFT_CENTER_X,
                      INDICATOR_LEFT_CENTER_Y + 4, color);
    _tft.fillRect(INDICATOR_LEFT_CENTER_X, INDICATOR_LEFT_CENTER_Y - 2, 4, 5, color);
}

void Dashboard::drawRightIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    _tft.fillTriangle(INDICATOR_RIGHT_CENTER_X + 4, INDICATOR_RIGHT_CENTER_Y,
                      INDICATOR_RIGHT_CENTER_X, INDICATOR_RIGHT_CENTER_Y - 4,
                      INDICATOR_RIGHT_CENTER_X, INDICATOR_RIGHT_CENTER_Y + 4, color);
    _tft.fillRect(INDICATOR_RIGHT_CENTER_X - 4, INDICATOR_RIGHT_CENTER_Y - 2, 4, 5, color);
}

void Dashboard::drawLowBeamIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 + 10;
    uint16_t y = SCREEN_HEIGHT - 20;

    uint8_t cols = sizeof(LOW_BEAM_ICON[0]);
    uint8_t rows = sizeof(LOW_BEAM_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (LOW_BEAM_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawHighBeamIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 - 7;
    uint16_t y = SCREEN_HEIGHT - 20;

    uint8_t cols = sizeof(HIGH_BEAM_ICON[0]);
    uint8_t rows = sizeof(HIGH_BEAM_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (HIGH_BEAM_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawFogLightIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 - 24;
    uint16_t y = SCREEN_HEIGHT - 20;

    uint8_t cols = sizeof(FOG_LIGHT_ICON[0]);
    uint8_t rows = sizeof(FOG_LIGHT_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (FOG_LIGHT_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawSpeed(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, SPEED_MIN, SPEED_MAX, SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END);
    int16_t deg = map(value, SPEED_MIN, SPEED_MAX, SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END);
    drawNeedle(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 4, last_deg, 3,
               TFT_BLACK);
    drawNeedle(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 4, deg, 3,
               TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawRPM(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg = map(lastVal, RPM_MIN, RPM_MAX, RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END);
    int16_t deg = map(value, RPM_MIN, RPM_MAX, RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END);
    drawNeedle(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 4, last_deg, 3,
               TFT_BLACK);
    drawNeedle(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 4, deg, 3, TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawFuel(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, FUEL_MIN, FUEL_MAX, FUEL_CIRCLE_ANGLE_START, FUEL_CIRCLE_ANGLE_END);
    int16_t deg = map(value, FUEL_MIN, FUEL_MAX, FUEL_CIRCLE_ANGLE_START, FUEL_CIRCLE_ANGLE_END);
    drawNeedle(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, FUEL_CIRCLE_RADIUS - 4, last_deg, 2,
               TFT_BLACK);
    drawNeedle(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, FUEL_CIRCLE_RADIUS - 4, deg, 2, TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawAdBlue(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, ADBLUE_MIN, ADBLUE_MAX, ADBLUE_CIRCLE_ANGLE_START, ADBLUE_CIRCLE_ANGLE_END);
    int16_t deg =
        map(value, ADBLUE_MIN, ADBLUE_MAX, ADBLUE_CIRCLE_ANGLE_START, ADBLUE_CIRCLE_ANGLE_END);
    drawNeedle(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, ADBLUE_CIRCLE_RADIUS - 4, last_deg, 2,
               TFT_BLACK);
    drawNeedle(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, ADBLUE_CIRCLE_RADIUS - 4, deg, 2,
               TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawPump(uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t cols = sizeof(PUMP_ICON[0]);
    uint8_t rows = sizeof(PUMP_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (PUMP_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawGear(char gear)
{
    static char lastVal = ' ';
    if (gear == lastVal)
    {
        return;
    }
    char str[2] = "\0";
    str[0] = gear;
    _tft.drawCentreString(str, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
    lastVal = gear;
}

void Dashboard::drawVolt(float volt, float threshold)
{
    static float lastVal = '1';
    if (abs(volt - lastVal) < 0.1)
        return; // Prevent display from being triggered too frequently
    if (volt <= threshold)
        _tft.setTextColor(TFT_RED, TFT_BLACK);
    else if (volt <= threshold + 0.5)
        _tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    else
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.setTextDatum(MC_DATUM);                    // Middle centre alignment
    _tft.setTextPadding(_tft.textWidth("99.9", 2)); // Clear old content
    _tft.drawFloat(volt, 1, SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) - 16, 2);
    _tft.setTextPadding(0);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    lastVal = volt;
}

// Draw logo
void Dashboard::drawLogo(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h)
{
    int row, col, buffidx = 0;
    for (row = x; row < h; row++)
    { // For each scanline...
        for (col = y; col < w; col++)
        { // For each pixel...
            _tft.drawPixel(col, row, pgm_read_word(bitmap + buffidx));
            buffidx++;
        }
    }
}
// End of standard dashboard *********************************************************************************************************************

#else // LCD dashboard from Frevic. See: https://www.rc-modellbau-portal.de/index.php?threads/esp32-arduino-rc-sound-und-licht-controller.7183/post-490891

/*
 * dashboard.cpp - Library to drive a 80x160px color LCD as rc truck dashboard
 *
 * https://github.com/Gamadril/ESP32_LASE
 * MIT License
 */

#include <TFT_eSPI.h>
#include <stdint.h>
#include "dashLogos.h"

Dashboard::Dashboard() {}

void Dashboard::init(uint8_t value)
{
    _tft.init();
    _tft.setRotation(value); // 3 = normal, 1 = upside down
    _tft.fillScreen(TFT_BLACK);
    drawLogo(0, 0, logo_thediyguy, 80, 80);
    _tft.drawCentreString("Powered by", 120, 34, 1);
    _tft.drawCentreString("TheDIYGuy999", 120, 46, 1);
    delay(1500);
    drawLogo(0, 0, logo_frevic, 160, 80);
    delay(1500);
    drawLogo(0, 0, logo_scania, 160, 80);
    delay(1500);
    _tft.fillScreen(TFT_BLACK);
    drawFrame();
    setSpeed(0);
    setRPM(0);
    setFuelLevel(0);
    setAdBlueLevel(0);
    setLeftIndicator(false);
    setRightIndicator(false);
    setLowBeamIndicator(false);
    setHighBeamIndicator(false);
    setFogLightIndicator(false);

    // Iniciar

    int a = 215;
    double rad = 0.01745;
    int r = SPEED_CIRCLE_RADIUS - 9;
    int cx = SPEED_CIRCLE_CENTER_X;
    int cy = SPEED_CIRCLE_CENTER_Y;
    for (int i = 0; i < 360; i++)
    {

        lx[i] = ((r)*cos(rad * a)) + cx;
        ly[i] = ((r)*sin(rad * a)) + cy;
        // Serial.println(String(i) + " x=>" + String(lx[i]) + " y=>" + String(ly[i]));

        a++;
        if (a == 360)
            a = 0;
    }
}

void Dashboard::setLeftIndicator(boolean on)
{
    drawLeftIndicator(on ? TFT_DARKGREEN : offColor);
}

void Dashboard::setRightIndicator(boolean on)
{
    drawRightIndicator(on ? TFT_DARKGREEN : offColor);
}

void Dashboard::setSpeed(uint16_t value) { drawSpeed(value); }

void Dashboard::setRPM(uint16_t value) { drawRPM(value); }

void Dashboard::setFuelLevel(uint16_t value) { drawFuel(value); }

void Dashboard::setAdBlueLevel(uint16_t value) { drawAdBlue(value); }

void Dashboard::setLowBeamIndicator(boolean on)
{
    drawLowBeamIndicator(on ? TFT_GREEN : offColor);
}

void Dashboard::setHighBeamIndicator(boolean on)
{
    drawHighBeamIndicator(on ? TFT_BLUE : offColor);
}

void Dashboard::setFogLightIndicator(boolean on)
{
    drawFogLightIndicator(on ? TFT_GREEN : offColor);
}

void Dashboard::setGear(int8_t value)
{
    char symbol = ' ';
    if (value == -1)
    {
        symbol = 'R';
    }
    else if (value == 0)
    {
        symbol = 'N';
    }
    else if (value > 0 && value < 10)
    {
        symbol = '0' + value;
    }
    drawGear(symbol);
}

void Dashboard::setVolt(float value, float threshold)
{
    drawVolt(value, threshold);
}

void Dashboard::drawFrame()
{

    unsigned short color1;
    unsigned short color2;

    // Central display ----
    for (int i = 0; i <= 3; i++)
    {
        _tft.fillRect(SCREEN_WIDTH / 4, 15 + (i * 12), (SCREEN_WIDTH / 4) * 2, 10, 0x0290);
    }

    _tft.fillRect(SCREEN_WIDTH / 2 - 10, 17, 4, 4, TFT_DARKGREY);
    _tft.fillRect(SCREEN_WIDTH / 2 - 5, 17, 4, 4, TFT_DARKGREY);
    _tft.fillRect(SCREEN_WIDTH / 2, 17, 4, 4, TFT_DARKGREY);
    _tft.fillRect(SCREEN_WIDTH / 2 + 5, 17, 4, 4, TFT_DARKGREY);

    drawPump(SCREEN_WIDTH / 2 - 3, 53, TFT_WHITE);
    drawPump(SCREEN_WIDTH / 2 - 3, 29, TFT_SKYBLUE);

    _tft.fillRect(SCREEN_WIDTH / 2 - 2, 42, 4, 4, TFT_DARKGREY);
    // Punto Rojo
    _tft.fillCircle(SCREEN_WIDTH - 5, 5, 2, TFT_RED);
    for (int i = 0; i <= 2; i++)
    {
        _tft.drawLine(3 + (i * 3), 3, 3 + (i * 3), 8, TFT_ORANGE);
    }

    _tft.fillCircle(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS + 2, TFT_BLACK);
    _tft.fillCircle(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS + 2, TFT_BLACK);

    // SPEED FREDY ----
    // Speed gauge outer ring segments
    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, 0x0290); // TFT_LIGHTGREY);
    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 1,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, 0x0290); // TFT_LIGHTGREY);
    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 2,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, 0x0290); // TFT_LIGHTGREY

    drawArc(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 4,
            SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END, TFT_WHITE); // TFT_LIGHTGREY);

    // Speed scale segments
    for (int16_t i = 0; i <= SPEED_CIRCLE_SEGMENTS; i++)
    {
        if (i < 9)
        {
            color1 = gaugeColor;
            color2 = TFT_WHITE;
        }
        else
        {
            color1 = purple;
            color2 = purple;
        }
        if (i % 2 == 0)
        {
            drawSep(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, 3,
                    SPEED_CIRCLE_ANGLE_START + i * SPEED_CIRCLE_SEGMENT_ANGLE, SPEED_CIRCLE_RADIUS - 5,
                    color1);
        }
        else
        {
            drawSep(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, 3,
                    SPEED_CIRCLE_ANGLE_START + i * SPEED_CIRCLE_SEGMENT_ANGLE, SPEED_CIRCLE_RADIUS - 5,
                    color2);
        }
    }

    // RPM Fredy ----
    // RPM gauge outer ring segments
    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS,
            RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END, 0x0290);
    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 1,
            RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END, 0x0290);
    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 2,
            RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END, 0x0290);

    drawArc(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 4,
            RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END, TFT_WHITE);

    // RPM scale segments
    for (int16_t i = 0; i <= RPM_CIRCLE_SEGMENTS; i++)
    {
        color1 = gaugeColor;
        color2 = TFT_WHITE;
        if (i % 2 == 0)
        {
            drawSep(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, 3,
                    SPEED_CIRCLE_ANGLE_START + i * RPM_CIRCLE_SEGMENT_ANGLE, RPM_CIRCLE_RADIUS - 5,
                    color1);
        }
        else
        {
            drawSep(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, 3,
                    RPM_CIRCLE_ANGLE_START + i * RPM_CIRCLE_SEGMENT_ANGLE, RPM_CIRCLE_RADIUS - 5,
                    color2);
        }
    }

    drawNumVel();
    drawNumrpm();

    _tft.fillCircle(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, 14, 0x0290);
    _tft.fillCircle(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, 12, TFT_BLACK);

    _tft.fillCircle(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, 14, 0x0290);
    _tft.fillCircle(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, 12, TFT_BLACK);

    _tft.setTextColor(TFT_ORANGE);  
    _tft.drawCentreString("rpm", RPM_CIRCLE_CENTER_X, 60, 1);
    _tft.drawCentreString("x100", RPM_CIRCLE_CENTER_X, 70, 1);
    _tft.drawCentreString("km/h", SPEED_CIRCLE_CENTER_X - 3, 60, 1);

    drawGear('N');
}

void Dashboard::drawNumVel()
{
    unsigned short color2;
    double rad = 0.01745;
    for (int16_t i = 0; i <= SPEED_CIRCLE_SEGMENTS; i++)
    {
        if (i < 9)
        {
            color2 = TFT_WHITE;
        }
        else
        {
            color2 = purple;
        }
        if (i % 2 == 0)
        {
            _tft.setTextColor(color2);
            int16_t a = SPEED_CIRCLE_ANGLE_START + i * SPEED_CIRCLE_SEGMENT_ANGLE;
            int16_t mx = ((SPEED_CIRCLE_RADIUS - 12) * cos(rad * a)) + SPEED_CIRCLE_CENTER_X;
            int16_t my = ((SPEED_CIRCLE_RADIUS - 12) * sin(rad * a)) + SPEED_CIRCLE_CENTER_Y;
            _tft.drawCentreString(String(i * 10), mx, my, 1);
        }
    }
}

void Dashboard::drawNumrpm()
{
    unsigned short color2 = TFT_WHITE;
    double rad = 0.01745;
    for (int16_t i = 0; i <= RPM_CIRCLE_SEGMENTS; i++)
    {
        if (i % 2 == 0)
        {
            _tft.setTextColor(color2);
            int16_t a = RPM_CIRCLE_ANGLE_START + i * RPM_CIRCLE_SEGMENT_ANGLE;
            int16_t mx = ((RPM_CIRCLE_RADIUS - 12) * cos(rad * a)) + RPM_CIRCLE_CENTER_X;
            int16_t my = ((RPM_CIRCLE_RADIUS - 12) * sin(rad * a)) + RPM_CIRCLE_CENTER_Y;
            _tft.drawCentreString(String(i * 5 / 2), mx, my, 1);
        }
    }
}

void Dashboard::drawArc(int16_t cx, int16_t cy, int16_t r, int16_t deg_start, int16_t deg_end,
                        uint32_t color)
{
    float sx, sy;
    uint16_t x, y;

    for (int16_t i = deg_start; i <= deg_end; i++)
    {
        sx = cos(i * DEG2RAD);
        sy = sin(i * DEG2RAD);
        x = round(sx * r) + cx;
        y = round(sy * r) + cy;
        _tft.drawPixel(x, y, color);
    }
}

void Dashboard::drawSep(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos, uint16_t r,
                        uint32_t color)
{
    float sx, sy;
    uint16_t x0, y0, x1, y1;

    for (int8_t i = -1; i <= 1; i++)
    {
        sx = cos((i + deg_pos) * DEG2RAD);
        sy = sin((i + deg_pos) * DEG2RAD);
        x0 = round(sx * r) + cx;
        y0 = round(sy * r) + cy;
        x1 = round(sx * (r - len)) + cx;
        y1 = round(sy * (r - len)) + cy;
        _tft.drawLine(x0, y0, x1, y1, color);
    }
}

void Dashboard::drawNeedle(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos,
                           uint16_t radius_holder, uint32_t color_needle)
{
    float sx, sy;
    uint16_t x, y, xx, yy;
    sx = cos((deg_pos)*DEG2RAD);
    sy = sin((deg_pos)*DEG2RAD);
    x = round(sx * len) + cx;
    y = round(sy * len) + cy;
    xx = -round(sx * 2) + cx;
    yy = -round(sy * 2) + cy;
    _tft.drawLine(x, y, xx, yy, color_needle);
}

// Needle function
void Dashboard::drawIndicadorFr(int16_t cx, int16_t cy, uint16_t len, int16_t deg_pos,
                                uint16_t radius_holder, uint32_t color_needle,
                                int16_t deg_pos_ant, uint32_t color_needle_ant, uint16_t pVelocidad)
{

    if (deg_pos == deg_pos_ant)
    {
        return;
    }
    float sx, sy;
    uint16_t x, y, xx, yy;

    // clear old needle
    sx = cos(deg_pos_ant * DEG2RAD);
    sy = sin(deg_pos_ant * DEG2RAD);
    x = round(sx * len) + cx;
    y = round(sy * len) + cy;

    xx = round(sx * (radius_holder - 2)) + cx;
    yy = round(sy * (radius_holder - 2)) + cy;

    _tft.drawLine(x, y, xx, yy, color_needle_ant);

    if (pVelocidad == 1)
    {
        drawNumVel();
    }
    if (pVelocidad == 2)
    {
        drawNumrpm();
    }

    // draw new needle
    sx = cos(deg_pos * DEG2RAD);
    sy = sin(deg_pos * DEG2RAD);
    x = round(sx * len) + cx;
    y = round(sy * len) + cy;

    xx = round(sx * (radius_holder - 2)) + cx;
    yy = round(sy * (radius_holder - 2)) + cy;

    _tft.drawLine(x, y, xx, yy, color_needle);
}

void Dashboard::drawLeftIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    _tft.fillTriangle(INDICATOR_LEFT_CENTER_X - 4, INDICATOR_LEFT_CENTER_Y, INDICATOR_LEFT_CENTER_X,
                      INDICATOR_LEFT_CENTER_Y - 4, INDICATOR_LEFT_CENTER_X,
                      INDICATOR_LEFT_CENTER_Y + 4, color);
    _tft.fillRect(INDICATOR_LEFT_CENTER_X, INDICATOR_LEFT_CENTER_Y - 2, 4, 5, color);
}

void Dashboard::drawRightIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    _tft.fillTriangle(INDICATOR_RIGHT_CENTER_X + 4, INDICATOR_RIGHT_CENTER_Y,
                      INDICATOR_RIGHT_CENTER_X, INDICATOR_RIGHT_CENTER_Y - 4,
                      INDICATOR_RIGHT_CENTER_X, INDICATOR_RIGHT_CENTER_Y + 4, color);
    _tft.fillRect(INDICATOR_RIGHT_CENTER_X - 4, INDICATOR_RIGHT_CENTER_Y - 2, 4, 5, color);
}

void Dashboard::drawLowBeamIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 + 10;
    // uint16_t y = SCREEN_HEIGHT - 20;
    // Fredy
    uint16_t y = INSTRUMENTOS_Y;

    uint8_t cols = sizeof(LOW_BEAM_ICON[0]);
    uint8_t rows = sizeof(LOW_BEAM_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (LOW_BEAM_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawHighBeamIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 - 7;
    // uint16_t y = SCREEN_HEIGHT - 20;
    // Fredy
    uint16_t y = INSTRUMENTOS_Y;

    uint8_t cols = sizeof(HIGH_BEAM_ICON[0]);
    uint8_t rows = sizeof(HIGH_BEAM_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (HIGH_BEAM_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawFogLightIndicator(uint32_t color)
{
    static uint32_t lastVal = TFT_BLACK;
    if (color == lastVal)
    {
        return;
    }
    lastVal = color;

    uint16_t x = SCREEN_WIDTH / 2 - 24;
    // uint16_t y = SCREEN_HEIGHT - 20;
    // Fredy
    uint16_t y = INSTRUMENTOS_Y;

    uint8_t cols = sizeof(FOG_LIGHT_ICON[0]);
    uint8_t rows = sizeof(FOG_LIGHT_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (FOG_LIGHT_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawSpeed(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, SPEED_MIN, SPEED_MAX, SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END);
    int16_t deg = map(value, SPEED_MIN, SPEED_MAX, SPEED_CIRCLE_ANGLE_START, SPEED_CIRCLE_ANGLE_END);
    // Speed needle
    drawIndicadorFr(SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y, SPEED_CIRCLE_RADIUS - 9, deg, 16,
                    needleColor, last_deg, TFT_BLACK, 1);

    _tft.setTextColor(TFT_BLACK);
    _tft.drawCentreString(String(lastVal), SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y - 4, 1);

    _tft.setTextColor(TFT_WHITE);
    _tft.drawCentreString(String(value), SPEED_CIRCLE_CENTER_X, SPEED_CIRCLE_CENTER_Y - 4, 1);

    lastVal = value;
}

void Dashboard::drawRPM(uint16_t value)
{
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg = map(lastVal, RPM_MIN, RPM_MAX, RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END);
    int16_t deg = map(value, RPM_MIN, RPM_MAX, RPM_CIRCLE_ANGLE_START, RPM_CIRCLE_ANGLE_END);
    // RPM needle
    drawIndicadorFr(RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y, RPM_CIRCLE_RADIUS - 9, deg, 16,
                    needleColor, last_deg, TFT_BLACK, 2);

    lastVal = value;
}

void Dashboard::drawFuel(uint16_t value)
{
    return;

    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, FUEL_MIN, FUEL_MAX, FUEL_CIRCLE_ANGLE_START, FUEL_CIRCLE_ANGLE_END);
    int16_t deg = map(value, FUEL_MIN, FUEL_MAX, FUEL_CIRCLE_ANGLE_START, FUEL_CIRCLE_ANGLE_END);
    drawNeedle(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, FUEL_CIRCLE_RADIUS - 4, last_deg, 2,
               TFT_BLACK);
    drawNeedle(FUEL_CIRCLE_CENTER_X, FUEL_CIRCLE_CENTER_Y, FUEL_CIRCLE_RADIUS - 4, deg, 2, TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawAdBlue(uint16_t value)
{
    return;
    static uint16_t lastVal = 0xFFFF;
    if (value == lastVal)
    {
        return;
    }

    int16_t last_deg =
        map(lastVal, ADBLUE_MIN, ADBLUE_MAX, ADBLUE_CIRCLE_ANGLE_START, ADBLUE_CIRCLE_ANGLE_END);
    int16_t deg =
        map(value, ADBLUE_MIN, ADBLUE_MAX, ADBLUE_CIRCLE_ANGLE_START, ADBLUE_CIRCLE_ANGLE_END);
    drawNeedle(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, ADBLUE_CIRCLE_RADIUS - 4, last_deg, 2,
               TFT_BLACK);
    drawNeedle(ADBLUE_CIRCLE_CENTER_X, ADBLUE_CIRCLE_CENTER_Y, ADBLUE_CIRCLE_RADIUS - 4, deg, 2,
               TFT_WHITE);

    lastVal = value;
}

void Dashboard::drawPump(uint16_t x, uint16_t y, uint32_t color)
{
    uint8_t cols = sizeof(PUMP_ICON[0]);
    uint8_t rows = sizeof(PUMP_ICON) / cols;
    for (uint8_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < cols; j++)
        {
            if (PUMP_ICON[i][j])
            {
                _tft.drawPixel(x + j, y + i, color);
            }
        }
    }
}

void Dashboard::drawGear(char gear)
{
    static char lastVal = ' ';
    if (gear == lastVal)
    {
        return;
    }
    char str[2] = "\0";
    str[0] = gear;

    _tft.setTextColor(TFT_BLACK);
    _tft.drawCentreString(String((lastVal)), RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y - 7, 2);

    _tft.setTextColor(TFT_WHITE);
    _tft.drawCentreString(str, RPM_CIRCLE_CENTER_X, RPM_CIRCLE_CENTER_Y - 7, 2);
    lastVal = gear;
}

void Dashboard::drawVolt(float volt, float threshold)
{
    static float lastVal = '1';
    if (abs(volt - lastVal) < 0.1)
        return; // Prevent display from being triggered too frequently

    _tft.setTextColor(TFT_BLACK);
    _tft.drawCentreString(String(lastVal) + "V", 33, 70, 1); // Clear old content

    // Set color
    if (volt <= threshold)
        _tft.setTextColor(TFT_RED);
    else if (volt <= threshold + 0.5)
        _tft.setTextColor(TFT_ORANGE);
    else
        _tft.setTextColor(TFT_WHITE);

    _tft.drawCentreString(String(volt) + "V", 33, 70, 1); // Show new content

    lastVal = volt;
}

// Draw Frevic logo
void Dashboard::drawLogo(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h)
{
    int row, col, buffidx = 0;
    for (row = x; row < h; row++)
    { // For each scanline...
        for (col = y; col < w; col++)
        { // For each pixel...
            _tft.drawPixel(col, row, pgm_read_word(bitmap + buffidx));
            buffidx++;
        }
    }
}

#endif // End of Vrevic dashboard *********************************************************************************************************************
