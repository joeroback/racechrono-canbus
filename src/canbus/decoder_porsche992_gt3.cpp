//// MIT License
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
 * decoder for Porsche 992 GT3 vehicles. (possibly other 992 models)
 * 
 */
class decoder_porsche992
    : public decoder
{
    CPP_NOCOPY(decoder_porsche992);
    CPP_NOMOVE(decoder_porsche992);

public:
    static decoder_porsche992& get() noexcept
    {
        static decoder_porsche992 instance;
        return instance;
    }

    ~decoder_porsche992() noexcept override
    {
    }

    const char* name() const noexcept override
    {
        return "Porsche 992 Decoder";
    }

    twai_timing_config_t timing() const noexcept override
    {
        return TWAI_TIMING_CONFIG_500KBITS();
    }

    twai_filter_config_t filter() const noexcept override
    {
        return TWAI_FILTER_CONFIG_ACCEPT_ALL();
    }

    uint16_t rate(uint32_t pid) const noexcept override
    {
        switch (pid)
        {
            case 0x1F1:
                return 1;
            default:
                return rate_default;
        }
    }

private:
    explicit decoder_porsche992() noexcept
        : decoder(3)
    {
        // pre-sorted list of pids
        // list must be sorted by ID, as binary search is used
        // easy enough to pre-sort this list here
        size_t idx = 0;
        _ids[idx++] = { 0x1F1, rate_disabled, 0 };
        _ids[idx++] = { 0x1F2, rate_disabled, 0 };
        _ids[idx++] = { 0x1F3, rate_disabled, 0 };

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

#if defined(CONFIG_CANBUS_DECODER_PORSCHE992)
canbus::decoder& CANDEC = canbus::decoder_porsche992::get();
#endif
