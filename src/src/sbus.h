/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2021 Bolder Flight Systems Inc (modified by TheDIYGuy999)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#ifndef SRC_SBUS_H_
#define SRC_SBUS_H_

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include "core/core.h"
#endif
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <array>

namespace bfs {

class SbusRx {
 private:
  /* Communication */
  HardwareSerial *uart_;
  uint32_t BAUD_ = 100000;
  /* Message len */
  static constexpr int8_t BUF_LEN_ = 25;
  static constexpr uint32_t SBUS_PACKET_TIMEOUT_US = 7000; // 3000 is the minimum working, but causing failsafe in noisy vehicles, so 7000 is recommended
  /* SBUS message defs */
  static constexpr int8_t NUM_SBUS_CH_ = 16;
  static constexpr uint8_t HEADER_ = 0x0F;
  static constexpr uint8_t FOOTER_ = 0x00;
  static constexpr uint8_t FOOTER2_ = 0x04;
  static constexpr uint8_t CH17_MASK_ = 0x01;
  static constexpr uint8_t CH18_MASK_ = 0x02;
  static constexpr uint8_t LOST_FRAME_MASK_ = 0x04;
  static constexpr uint8_t FAILSAFE_MASK_ = 0x08;
  /* Parsing state tracking */
  uint32_t _sbusMicros = micros();
  int8_t state_ = 0;
  uint8_t prev_byte_ = FOOTER_;
  uint8_t cur_byte_;
  /* Buffer for storing messages */
  uint8_t buf_[BUF_LEN_];
  /* Data */
  bool new_data_;
  std::array<int16_t, NUM_SBUS_CH_> ch_;
  bool failsafe_ = false, lost_frame_ = false, ch17_ = false, ch18_ = false;
  bool parse();

 public:
  explicit SbusRx(HardwareSerial *bus) : uart_(bus) {}
  void begin(const int8_t rxpin = 16, const int8_t txpin = 17, bool inverted = true, uint32_t baud = 100000); // 16, 17 = UART 2, if not specified
  bool read();
  static constexpr int8_t NUM_CH() {return NUM_SBUS_CH_;}
  inline std::array<int16_t, NUM_SBUS_CH_> ch() const {return ch_;}
  inline bool failsafe() const {return failsafe_;}
  inline bool lost_frame() const {return lost_frame_;}
  inline bool ch17() const {return ch17_;}
  inline bool ch18() const {return ch18_;}
};

}  // namespace bfs

#endif  // SRC_SBUS_H_
