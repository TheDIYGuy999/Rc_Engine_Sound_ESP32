#include "SUMD.h"

SUMD::SUMD(HardwareSerial& serial) : _serial(&serial) {
  this->_serial->setRxBufferSize(SUMD_FRAME_LENGTH * 4);
}

void SUMD::begin(int8_t rxPin) {
  if (rxPin < 0) {
    log_e("Provide RX pin");
    return;
  }

  _serial->begin(SUMD_BAUD, SERIAL_8N1, rxPin);
}

int SUMD::read(uint16_t *channels, bool *failsafe, bool *frame_lost) {
  uint8_t frame[SUMD_FRAME_LENGTH]; 
  int16_t rb;
  int32_t idx = 0;

  while(true) {
    if (this->_serial->available() > 0) {
      // wait for SUMD Header
      rb = this->_serial->read();
      if (idx == 0 && (rb < 0 || rb != SUMD_VENDOR_ID)) {
        idx = 0;
        *frame_lost = true;
        continue;
      }
      
      frame[idx++] = rb;

      if (idx == SUMD_FRAME_LENGTH) {
         int res = this->_decode_frame(frame, channels, failsafe);
         *frame_lost = res > 0 ? true : false;
         return res;
      }
    }
  }

  return 0;
}

int32_t SUMD::_decode_frame(uint8_t *frame, uint16_t *channels, bool *failsafe) {
    if (frame[1] != SUMD_DATA_FRAME && frame[1] != SUMD_DATA_FRAME_FAILSAFE) {
      log_e("Unexpected status value");
      return 1;
    }

    *failsafe = frame[1] == SUMD_DATA_FRAME_FAILSAFE;

    if (frame[2] != SUMD_CHANNELS) {
      log_e("Unexpected number of channels: %d", frame[2]);
      return 2;
    }
    
    for (uint8_t i = 0; i < SUMD_CHANNELS; i++) {
      channels[i] = ((frame[3 + i * 2] << 8) | frame[4 + i * 2]) / 8;
    }

    uint16_t frame_crc = (frame[3 + SUMD_CHANNELS * 2] << 8) | frame[3 + SUMD_CHANNELS * 2 + 1];

    uint16_t crc = 0;
    for (uint8_t i = 0 ; i < 3 + SUMD_CHANNELS * 2; i++) {
       crc = this->CRC16(crc , frame[i]);
    }

    if (frame_crc != crc) {
       log_e("Wrong frame checksum.");
       //log_e("0x%04X != 0x%04X", frame_crc, crc);
       //log_e("0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X",
       //frame[0], frame[1], frame[2], frame[3], frame[4], frame[5], frame[6], frame[7], frame[8], frame[9], frame[10], frame[11], frame[12], frame[13], frame[14], frame[15], frame[16], frame[17], frame[18], frame[19], frame[20], frame[21], frame[22], frame[23], frame[24], frame[25], frame[26], frame[27], frame[28]);
       return 3;
    }

    return 0;
}

uint16_t SUMD::CRC16(uint16_t crc, uint8_t value) {
  crc = crc ^ (int16_t)value << 8;
  for (uint8_t i = 0; i < 8; i++) {
    crc = crc & 0x8000 ? (crc << 1) ^ CRC_POLYNOME : crc << 1;
  }
  return crc;
}
