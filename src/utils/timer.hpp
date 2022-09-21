// MIT License
//
// Copyright (c) 2022 Joe Roback <joe.roback@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include "../racechrono-canbus.hpp"

namespace utils
{

/**
 * Basic timer class. Uses microseconds instead of milliseconds to avoid
 * division by 1000 on ESP32 boards when calling millis()
 */
class timer final
{
public:
    explicit timer() noexcept
        : _ts(micros())
    {}

    ~timer() noexcept = default;

    /**
     * Check is timer is elapsed by \p timeout microseconds.
     * @return the actual number of microseconds elapsed since timer started. 0 if timer has not been triggered.
     */
    unsigned long elapsed(unsigned long timeout) noexcept
    {
        unsigned long now = micros();
        unsigned long delta = now - _ts;

        if (delta >= timeout)
        {
            _ts = now;
            return delta;
        }

        return 0;
    }

private:
    unsigned long _ts;
};

} // namespace utils
