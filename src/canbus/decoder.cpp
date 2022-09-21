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
#include "../logging/logging.hpp"

#include <algorithm>

#include "decoder.hpp"

namespace canbus
{

decoder::decoder(size_t size) noexcept
    : _ids(new ID[size])
    , _size(size)
{
}

decoder::~decoder() noexcept
{
    delete[] _ids;
}

twai_filter_config_t decoder::filter() const noexcept
{
    return TWAI_FILTER_CONFIG_ACCEPT_ALL();
}

bool decoder::can_decode(uint32_t id) const noexcept
{
    return find(id) != cend();
}

bool decoder::should_decode(uint32_t id) noexcept
{
    auto entry = find(id);

    // not found, cannot decode
    if (entry == end())
    {
        return false;
    }

    // rate set to zero, id is disabled
    if (entry->rate == 0)
    {
        return false;
    }

    ++(entry->n);

    // if we hit our interval for this id, let's decode one
    if (entry->n >= entry->rate)
    {
        entry->n = 0;
        return true;
    }

    return false;
}

void decoder::deny_all() noexcept
{
    for (size_t i = 0; i < size(); i++)
    {
        _ids[i].rate = rate_disabled;
    }
}

void decoder::allow_all() noexcept
{
    for (size_t i = 0; i < size(); i++)
    {
        _ids[i].rate = rate(_ids[i].id);
    }
}

void decoder::allow_id(uint32_t id) noexcept
{
    auto entry = find(id);

    if (entry != end())
    {
        entry->rate = rate(id);
    }
}

void decoder::onWrite(BLECharacteristic* characteristic)
{
    const uint8_t* data = characteristic->getData();
    const size_t len = characteristic->getLength();

    if (len < 1)
    {
        warnln("ID request EMPTY CMD");
        return;
    }

    uint8_t command = data[0];

    debugln("ID request CMD %u LEN %u", command, len);

    // NOTE: currently ignoring notify interval from RaceChrono app
    switch (command)
    {
        case 0:
            if (len == 1)
            {
                verboseln("ID request DENY all");
                deny_all();
            }
            break;
        case 1:
            if (len == 3)
            {
                uint16_t notifyIntervalMs = data[1] << 8 | data[2];
                verboseln("ID request ALLOW all INTERVAL %u ms", notifyIntervalMs);
                allow_all();
            }
            break;
        case 2:
            if (len == 7)
            {
                uint16_t notifyIntervalMs = data[1] << 8 | data[2];
                uint32_t id = data[3] << 24 | data[4] << 16 | data[5] << 8 | data[6];
                verboseln("ID request ALLOW ID %u INTERVAL %u ms", id, notifyIntervalMs);
                allow_id(id);
            }
            break;
        default:
            warnln("ID request UNKNOWN CMD %u", command);
            break;
    }
}

decoder::ID* decoder::find(uint32_t id) noexcept
{
    auto entry = std::lower_bound(begin(), end(), id);
    if (entry != end() && entry->id == id)
    {
        return entry;
    }
    return end();
}

decoder::ID const* decoder::find(uint32_t id) const noexcept
{
    auto entry = std::lower_bound(cbegin(), cend(), id);
    if (entry != cend() && entry->id == id)
    {
        return entry;
    }
    return cend();
}

} // namespace canbus
