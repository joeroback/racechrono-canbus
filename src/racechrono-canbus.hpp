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

#include <Arduino.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

/// define to build the decoder for the BMW g8x
#define CONFIG_CANBUS_DECODER_BMWG8X 1

/// statistics timeout in microseconds
#define CONFIG_RC_STATS_TIMEOUT 5000000

/// if DEBUG is defined, logger will be enabled and print to serial console
// #define DEBUG

/// _x branch is likely to be true
#define RCLIKELY(_x)    __builtin_expect(!!(_x), 1)

/// _x branch is unlikely to be true
#define RCUNLIKELY(_x)  __builtin_expect(!!(_x), 0)

/// assert _expr is true (always not just DEBUG mode)
#define RCASSERT(_expr) do {                  \
    if (RCUNLIKELY(!(_expr))) { abort(); }    \
} while (0)

// Adafruit ESP32 Feather (Huzzah32)
#if defined(ARDUINO_FEATHER_ESP32)
#define CAN_RX_PIN GPIO_NUM_26
#define CAN_TX_PIN GPIO_NUM_25
#define BLE_PWR_LVL ESP_PWR_LVL_P9
#endif

// Adafruit Feather S3
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
#define CAN_RX_PIN GPIO_NUM_18
#define CAN_TX_PIN GPIO_NUM_17
#define BLE_PWR_LVL ESP_PWR_LVL_P12
#endif

// ESP32 Dev Module
#if defined(ARDUINO_ESP32_DEV)
#define LED_BUILTIN 13
#define CAN_RX_PIN GPIO_NUM_26
#define CAN_TX_PIN GPIO_NUM_25
#define BLE_PWR_LVL ESP_PWR_LVL_P9
#endif

/// disable copy
#define CPP_NOCOPY(_name)                     \
    _name(_name const&) = delete;             \
    _name& operator=(_name const&) = delete

/// disable move
#define CPP_NOMOVE(_name)                     \
    _name(_name&&) = delete;                  \
    _name& operator=(_name&&) = delete

// C++11 ugh
template <typename T, typename U = T>
inline T exchange(T& obj, U&& new_val)
{
    T old_val = std::move(obj);
    obj = std::forward<U>(new_val);
    return old_val;
}
