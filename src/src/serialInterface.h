
#include <Arduino.h>

//
// =======================================================================================================
// SERIAL INTERFACE
// =======================================================================================================
//

void serialInterface()
{
#ifdef WEMOS_D1_MINI_ESP32 // Only available on this board!

  static bool parseSerial = false;
  String inp;

  // Serial commands availabale in buffer?
  if (Serial.available() > 0)
  {
    // Read command from serial port
    inp = Serial.readStringUntil('\n');
    parseSerial = true;
  }

  if (parseSerial)
  {
    parseSerial = false;
    char cmd = inp[0];             // First sign is the command
    String dat = inp.substring(1); // Second sign upwards is the value
    switch (cmd)
    {

    case 'p':
      escPulseSpan = constrain(dat.toInt(), 500, 1200);
      Serial.printf("ESC pulse span: %i \n", escPulseSpan);
      eepromWrite();
      delay(1000);
      ESP.restart();
      break;

    case 't':
      escTakeoffPunch = constrain(dat.toInt(), 0, 150);
      Serial.printf("ESC takeoff punch: %i \n", escTakeoffPunch);
      eepromWrite();
      delay(1000);
      ESP.restart();
      break;

    default:
      Serial.println("Invalid command! Valid commands are: (p700 = ESC pulse span 500 - 1200, t50 = ESC takeoff punch 0 - 150)");
      break;
    }
  }
#endif
}