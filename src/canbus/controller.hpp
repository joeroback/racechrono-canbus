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
#include "../logging/logging.hpp"
#include "../utils/timer.hpp"

#include "frame.hpp"

#include <atomic>

namespace canbus
{

/**
 * CAN-bus controller. this is a singleton class, as there is only
 * one controller on a standard ESP32 board.
 */
class controller final
{
    CPP_NOCOPY(controller);
    CPP_NOMOVE(controller);

public:
    ~controller() noexcept = default;

    /**
     * get controller instance
     */
    static controller& get() noexcept;

    /**
     * return true is controller is initialized and running (interrupt handler installed and controller active)
     */
    __always_inline bool running() const noexcept { return _running; }

    /**
     * print any controller stats
     */
#if defined(DEBUG)
    void stats() noexcept;
#else
    void stats() noexcept {}
#endif

    /**
     * install controller driver
     */
    bool install() noexcept;

    /**
     * uninstall controller driver
     */
    bool uninstall() noexcept;

    /**
     * start controller
     */
    bool start() noexcept;

    /**
     * stop controller
     */
    bool stop() noexcept;

    /**
     * receive a frame from the internal buffer
     */
    bool recv(frame& f) noexcept;

private:
    explicit controller() noexcept;

    /**
     * interrupt service handler
     */
    static void IRAM_ATTR isr(void* arg);

    /**
     * interrupt service handler
     */
    void isr() noexcept;

    __always_inline void ENTER_CRITICAL() noexcept { portENTER_CRITICAL(&_lock); }
    __always_inline void EXIT_CRITICAL() noexcept { portEXIT_CRITICAL(&_lock); }
    __always_inline void ENTER_CRITICAL_ISR() noexcept { portENTER_CRITICAL_ISR(&_lock); }
    __always_inline void EXIT_CRITICAL_ISR() noexcept { portEXIT_CRITICAL_ISR(&_lock); }

private:
    portMUX_TYPE _lock;
    bool _running;
    QueueHandle_t _queue;
    utils::timer _stats_timer;
    std::atomic<uint32_t> _ir_count;
    std::atomic<uint32_t> _er_count;
    std::atomic<uint32_t> _cb_count;
    std::atomic<uint32_t> _rc_count;
    intr_handle_t _isr_handle;
    static constexpr uint32_t _queue_length = 8;
    static constexpr uint32_t _queue_item_size = sizeof(canbus::frame);
    uint8_t _queue_storage[_queue_length * _queue_item_size];
    StaticQueue_t _static_queue;
};

} // namespace canbus

extern canbus::controller& CANCTLR;
