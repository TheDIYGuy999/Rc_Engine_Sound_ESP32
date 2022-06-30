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

#include "sbus.h"  // NOLINT
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

// -----------------------------------------------------------------------------------
void SbusRx::begin(const int8_t rxpin, const int8_t txpin, bool inverted, uint32_t baud) {
    BAUD_ = baud; // Set baudrate (allows fine tuning, if you have troubles)
    uart_->begin(BAUD_, SERIAL_8E2, rxpin, txpin, inverted); // Start the bus
    uart_->flush(); // flush the bus
}

// -----------------------------------------------------------------------------------
bool SbusRx::read() {
    /* Read through all available packets to get the newest */
    new_data_ = false;
    do {
        if (parse()) {
            new_data_ = true;
        }
    } while (uart_->available());
    /* Parse new data, if available */
    if (new_data_) {
        /* Grab the channel data */
        ch_[0]  = static_cast<uint16_t>(buf_[1]       | buf_[2]  << 8                    & 0x07FF);
        ch_[1]  = static_cast<uint16_t>(buf_[2]  >> 3 | buf_[3]  << 5                    & 0x07FF);
        ch_[2]  = static_cast<uint16_t>(buf_[3]  >> 6 | buf_[4]  << 2  |buf_[5] << 10    & 0x07FF);
        ch_[3]  = static_cast<uint16_t>(buf_[5]  >> 1 | buf_[6]  << 7                    & 0x07FF);
        ch_[4]  = static_cast<uint16_t>(buf_[6]  >> 4 | buf_[7]  << 4                    & 0x07FF);
        ch_[5]  = static_cast<uint16_t>(buf_[7]  >> 7 | buf_[8]  << 1  |buf_[9] << 9     & 0x07FF);
        ch_[6]  = static_cast<uint16_t>(buf_[9]  >> 2 | buf_[10] << 6                    & 0x07FF);
        ch_[7]  = static_cast<uint16_t>(buf_[10] >> 5 | buf_[11] << 3                    & 0x07FF);
        ch_[8]  = static_cast<uint16_t>(buf_[12]      | buf_[13] << 8                    & 0x07FF);
        ch_[9]  = static_cast<uint16_t>(buf_[13] >> 3 | buf_[14] << 5                    & 0x07FF);
        ch_[10] = static_cast<uint16_t>(buf_[14] >> 6 | buf_[15] << 2  |buf_[16] << 10   & 0x07FF);
        ch_[11] = static_cast<uint16_t>(buf_[16] >> 1 | buf_[17] << 7                    & 0x07FF);
        ch_[12] = static_cast<uint16_t>(buf_[17] >> 4 | buf_[18] << 4                    & 0x07FF);
        ch_[13] = static_cast<uint16_t>(buf_[18] >> 7 | buf_[19] << 1  |buf_[20] << 9    & 0x07FF);
        ch_[14] = static_cast<uint16_t>(buf_[20] >> 2 | buf_[21] << 6                    & 0x07FF);
        ch_[15] = static_cast<uint16_t>(buf_[21] >> 5 | buf_[22] << 3                    & 0x07FF);
        /* CH 17 */
        ch17_ = buf_[23] & CH17_MASK_;
        /* CH 18 */
        ch18_ = buf_[23] & CH18_MASK_;
        /* Grab the lost frame */
        lost_frame_ = buf_[23] & LOST_FRAME_MASK_;
        /* Grab the failsafe */
        failsafe_ = buf_[23] & FAILSAFE_MASK_;
    }
    return new_data_;
}

// -----------------------------------------------------------------------------------
bool SbusRx::parse() {
    /* Parse (analyze) messages */
    while (uart_->available() > 0) {
        cur_byte_ = uart_->read();
        if (state_ == 0) { // First byte -----
            if ((cur_byte_ == HEADER_) && ((prev_byte_ == FOOTER_) || ((prev_byte_ & 0x0F) == FOOTER2_))) {
                state_++;
                _sbusMicros = micros();
            } else {
                state_ = 0; // Go back, if header incorrect
            }
        } else { // Following bytes (read data) -----
            if (state_ < BUF_LEN_ - 1) {
                buf_[state_++] = cur_byte_;
            }
            // End byte & no timeout -----
            if (state_ == BUF_LEN_ - 1) {
                state_ = 0;
                if (((cur_byte_ == FOOTER_) || ((cur_byte_ & 0x0F) == FOOTER2_)) && (micros() - _sbusMicros < SBUS_PACKET_TIMEOUT_US)) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        prev_byte_ = cur_byte_;
    }
    return false;
}


}  // namespace bfs
