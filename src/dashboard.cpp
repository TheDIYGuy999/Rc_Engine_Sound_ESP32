/*
 * dashboard.cpp - Library to drive a 80x160px color LCD as rc truck dashboard
 *
 * https://github.com/Gamadril/ESP32_LASE
 * MIT License
 */

#include "dashboard.h"
#include <TFT_eSPI.h>

Dashboard::Dashboard() {}

void Dashboard::init()
{
  _tft.init();
  _tft.setRotation(3);
  _tft.fillScreen(TFT_BLACK);
  drawFrame();
  setSpeed(0);
  setRPM(0);
  setFuelLevel(0);
  setAdBlueLevel(0);
}

void Dashboard::setLeftIndicator(boolean on)
{
  drawLeftIndicator(on ? TFT_DARKGREEN : TFT_BLACK);
}

void Dashboard::setRightIndicator(boolean on)
{
  drawRightIndicator(on ? TFT_DARKGREEN : TFT_BLACK);
}

void Dashboard::setSpeed(uint16_t value) { drawSpeed(value); }

void Dashboard::setRPM(uint16_t value) { drawRPM(value); }

void Dashboard::setFuelLevel(uint16_t value) { drawFuel(value); }

void Dashboard::setAdBlueLevel(uint16_t value) { drawAdBlue(value); }

void Dashboard::setLowBeamIndicator(boolean on)
{
  drawLowBeamIndicator(on ? TFT_GREEN : TFT_BLACK);
}

void Dashboard::setHighBeamIndicator(boolean on)
{
  drawHighBeamIndicator(on ? TFT_BLUE : TFT_BLACK);
}

void Dashboard::setFogLightIndicator(boolean on)
{
  drawFogLightIndicator(on ? TFT_GREEN : TFT_BLACK);
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
  if (color == lastVal) {
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
  if (color == lastVal) {
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
  if (color == lastVal) {
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
  if (color == lastVal) {
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
  if (color == lastVal) {
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
