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
#include "src/racechrono-canbus.hpp"
#include "src/canbus/controller.hpp"
#include "src/canbus/decoder.hpp"
#include "src/canbus/frame.hpp"
#include "src/led/led.hpp"
#include "src/racechrono/device.hpp"

#include <cstdint>

using namespace logging;

namespace
{

constexpr uint32_t core0_stack_size = 6 * 1024;
StaticTask_t core0_buffer;
StackType_t core0_stack[core0_stack_size];
TaskHandle_t core0_handle;
volatile bool core0_started = false;

void core0(void*);

}

void setup()
{
    LED.builtin_off();

    Serial.begin(115200);

    delay(1500);
    Serial.print("Starting up on core ");
    Serial.println(xPortGetCoreID());
    Serial.flush();

#if defined(DEBUG)
    delay(5000);
#endif

    // set logging level
    logger::get().set_level(log_level::info);

    //
    // ATTENTION:
    //   All Bluetooth LE related activity must be pinned to core 0
    //   All CAN-bus related activity must be pinned to core 1
    //

    assert(xPortGetCoreID() == 1);

    core0_handle = xTaskCreateStaticPinnedToCore(
        core0,
        "racechrono",
        core0_stack_size,
        nullptr,
        tskIDLE_PRIORITY,
        core0_stack,
        &core0_buffer,
        0
    );
    RCASSERT(core0_handle);

    // wait for core 0 to start (not strictly necessary)
    while (!core0_started) { delay(100); }

    // setup can-bus on core 1 (default), interrupt handler will be serviced on core 1
    if (CANCTLR.install())
    {
        if (CANCTLR.start())
        {
            LED.builtin_on();
        }
        else
        {
            errorln("ERROR: CAN bus controller startup failed!");
            esp_restart();
        }
    }
    else
    {
        errorln("ERROR: CAN bus driver install failed!");
        esp_restart();
    }
}

namespace
{

// core 0 - receive can frames from queue, send over bluetooth le
void core0(void*)
{
    bootln("Starting %s", CANDEC.name());

    // xQueue copies all data, so we can use a static buffer here
    static canbus::frame f;

    // start up bluetooth le connection
    if (RCDEV.start(&CANDEC))
    {
        core0_started = true;

        while (true)
        {
            while (CANCTLR.recv(f))
            {
#if defined(DEBUG)
                uint32_t id = f.id;
                uint8_t len = f.info.dlc;
                verboseln("PID 0x%03x LEN %u", id, len);
#endif
                RCDEV.send(reinterpret_cast<uint8_t*>(&f.id), sizeof(uint32_t) + f.info.dlc);
            }
            RCDEV.stats();
        }
    }
    else
    {
        errorln("ERROR: RaceChrono bluetooth device startup failed!");
        esp_restart();
    }

    // should never reach here...
    RCASSERT(false);
}

} // namespace

// core 1 - can-bus interrupt handler is running here, but also print some stats
void loop()
{
    // print out stats
    CANCTLR.stats();
}
