#ifndef SUMD_h
#define SUMD_h

#include "Arduino.h"

#define SUMD_BAUD 115200
#define SUMD_CHANNELS 12
#define SUMD_FRAME_LENGTH (3 + SUMD_CHANNELS * 2 + 2) // SUMD_Header + SUMD_Data + SUMD_CRC 

#define SUMD_VENDOR_ID 0xA8           // Graupner
#define SUMD_DATA_FRAME 0x01          // valid and live data frame
#define SUMD_DATA_FRAME_FAILSAFE 0x81 // valid data frame with transmitter in fail safe condition
#define CRC_POLYNOME 0x1021

class SUMD {
public:
  SUMD(HardwareSerial& serial);

  void begin(int8_t rxPin = -1);
  int read(uint16_t *channels, bool *failsafe, bool* frame_lost);

private:
  HardwareSerial *_serial;
  int32_t         _decode_frame(uint8_t *frame, uint16_t *channels, bool *failsafe);
  uint16_t        _data[SUMD_CHANNELS];
  uint32_t        _frameDetectTime;
  uint16_t        CRC16(uint16_t crc, uint8_t value);
};
#endif
