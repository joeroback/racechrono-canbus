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
#include "../racechrono-canbus.hpp"

#include "decoder.hpp"

namespace canbus
{

/**
 * decode for BMW g8x series vehicles.
 * 
 * Spreadsheet of g8x CAN-bus signals, data formats, units, formulas (factors + offsets)
 * https://docs.google.com/spreadsheets/d/1O491j8jlEOYR6voFPAYZ2NpbNyCKAgqbxt4HVw0bSDA/edit?usp=sharing
 */
class decoder_bmwg8x
    : public decoder
{
    CPP_NOCOPY(decoder_bmwg8x);
    CPP_NOMOVE(decoder_bmwg8x);

public:
    static decoder_bmwg8x& get() noexcept
    {
        static decoder_bmwg8x instance;
        return instance;
    }

    ~decoder_bmwg8x() noexcept override
    {
    }

    const char* name() const noexcept override
    {
        return "BMW G8X Decoder";
    }

    twai_timing_config_t timing() const noexcept override
    {
        return TWAI_TIMING_CONFIG_500KBITS();
    }

    twai_filter_config_t filter() const noexcept override
    {
        // TODO: test this filter out
        //
        //   ID 0x0?? - 0000 0000 0000
        //   ID 0x1?? - 0001 0000 0000
        //   ID 0x2?? - 0010 0000 0000
        //   ID 0x3?? - 0011 0000 0000
        // MASK 0x400 - 0100 0000 0000
        //              X0XX XXXX XXXX
        //   X == wildcard / do not care
        //
        // use hardware filter to trim down overall messages
        // allow 0x000 to 0x3FF and the second filter allow none 0x000
        return { .acceptance_code = (0x000 << 21) | (0x000 << 5),
                 .acceptance_mask = ~((0x400 << 21) | (0x7ff << 5)),
                 .single_filter = false };

        // return TWAI_FILTER_CONFIG_ACCEPT_ALL();
    }

    uint16_t rate(uint32_t pid) const noexcept override
    {
        switch (pid)
        {
            // RPM - 100hz
            case 0x0A5:
                return 3;
            // THROTTLE (%) / ACCELERATOR PEDAL (%) - 100hz
            case 0x0D9:
                return 3;
            // BRAKE PRESSURE - 50hz
            case 0x0EF:
                return 2;
            // ABS / ASC
            case 0x173:
                return 2;
            // LONGITUDINAL ACCELERATION - 50hz
            case 0x199:
                return 2;
            // LATERAL ACCELERATION - 50hz
            case 0x19A:
                return 2;
            // YAW RATE - 50hz
            case 0x19F:
                return 2;
            // SPEED - 50hz
            case 0x1A1:
                return 2;
            // BATTERY VOLTAGE
            case 0x281:
                return 1;
            // Fuel
            case 0x2C4:
                return 1;
            // AIR_TEMP - 1hz
            case 0x2CA:
                return 1;
            // STEERING ANGLE - 5hz
            case 0x301:
                return 1;
            // Fuel Used, Fuel Indicator Lamp, Fuel Range
            case 0x330:
                return 1;
            // STEERING ANGLE - 25hz (301 vs 302 both contain angle like units, 301 is steering on F-series)
            // case 0x302:
            //     return 1;
            // GEAR, WATER_TEMP, OIL_TEMP, Max RPM - 1hz
            // gear changes trigger immediate update
            case 0x3F9:
                return 1;
            default:
                return rate_default;
        }
    }

private:
    explicit decoder_bmwg8x() noexcept
        : decoder(14)
    {
        // pre-sorted list of pids
        // list must be sorted by ID, as binary search is used
        // easy enough to pre-sort this list here
        size_t idx = 0;
        _ids[idx++] = { 0x0A5, rate_disabled, 0 }; // RPM
        _ids[idx++] = { 0x0D9, rate_disabled, 0 }; // THROTTLE
        _ids[idx++] = { 0x0EF, rate_disabled, 0 }; // BRAKE PRESSURE
        _ids[idx++] = { 0x173, rate_disabled, 0 }; // ABS / ASC
        _ids[idx++] = { 0x199, rate_disabled, 0 }; // LONGITUDINAL ACCELERATION
        _ids[idx++] = { 0x19A, rate_disabled, 0 }; // LATERAL ACCELERATION
        _ids[idx++] = { 0x19F, rate_disabled, 0 }; // YAW RATE
        _ids[idx++] = { 0x1A1, rate_disabled, 0 }; // SPEED
        _ids[idx++] = { 0x281, rate_disabled, 0 }; // BATTERY VOLTAGE
        _ids[idx++] = { 0x2C4, rate_disabled, 0 }; // FUEL RAW
        _ids[idx++] = { 0x2CA, rate_disabled, 0 }; // AIR_TEMP
        _ids[idx++] = { 0x301, rate_disabled, 0 }; // STEERING ANGLE
        _ids[idx++] = { 0x330, rate_disabled, 0 }; // FUEL USED, FUEL LAMP, FUEL RANGE
        _ids[idx++] = { 0x3F9, rate_disabled, 0 }; // GEAR, OIL_TEMP, WATER_TEMP (alternative)

        // make sure pids are in sorted order
        uint32_t id = 0;
        for (size_t i = 0; i < idx; i++)
        {
            RCASSERT(id < _ids[i].id);
            id = _ids[i].id;
        }
    }
};

} // namespace canbus

#if defined(CONFIG_CANBUS_DECODER_BMWG8X)
canbus::decoder& CANDEC = canbus::decoder_bmwg8x::get();
#endif
