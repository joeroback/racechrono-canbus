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

#include <driver/ledc.h>

namespace led
{

/**
 * Basic LED controller for the various ESP32 boards.
 */
class led final
{
    CPP_NOCOPY(led);
    CPP_NOMOVE(led);

public:
    /**
     * Get LED instance (singleton).
     */
    static led& get() noexcept;

    ~led() noexcept = default;

    /**
     * Turn on built-in LED
     */
    void builtin_on() noexcept;

    /**
     * Turn off built-in LED
     */
    void builtin_off() noexcept;

private:
    explicit led() noexcept;

private:
    ledc_timer_config_t _timer_config;
    ledc_channel_config_t _channel_config;
};

} // namespace led

extern led::led& LED;
