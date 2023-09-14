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

#include <type_traits>

#include <hal/twai_types.h>
#include <BLECharacteristic.h>

namespace canbus
{


namespace detail
{

struct ID
{
    uint32_t id;    // CAN bus ID
    uint16_t rate;  // rate of IDs to record
    uint16_t n;     // number of times seen ID

    bool operator==(ID const& rhs) const noexcept
    {
        return id == rhs.id;
    }

    bool operator==(uint32_t const rhs) const noexcept
    {
        return id == rhs;
    }

    bool operator<(ID const& rhs) const noexcept
    {
        return id < rhs.id;
    }

    bool operator<(uint32_t const rhs) const noexcept
    {
        return id < rhs;
    }
};

static_assert(std::is_standard_layout<ID>::value, "not standard layout");
static_assert(std::is_move_constructible<ID>::value, "not move constructible");
static_assert(std::is_move_assignable<ID>::value, "not move assignable");

} // namespace detail

/**
 * CAN-bus decoder class. Most override for each type of CAN-bus
 * one wants to decode. Base class provides basic scaffolding for
 * rate handling, but sub-classes need to implement the actual
 * per id decoding.
 */
class decoder
    : public BLECharacteristicCallbacks
{
    CPP_NOCOPY(decoder);
    CPP_NOMOVE(decoder);

protected:
    using ID = detail::ID;

    static constexpr uint16_t rate_disabled = 0;
    static constexpr uint16_t rate_default = 1;

public:
    virtual ~decoder() noexcept;

    /**
     * return the name of this decoder (for logging purposes)
     */
    virtual const char* name() const noexcept = 0;

    /**
     * override this in sub-class for a given vehicle CAN-bus timing.
     * @see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html#bit-timing
     */
    virtual twai_timing_config_t timing() const noexcept = 0;

    /**
     * override this to use the CAN-bus controller built-in hardware filter. The default
     * behavior is to accept all CAN-bus frames.
     * @see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html#acceptance-filter
     */
    virtual twai_filter_config_t filter() const noexcept;

    /**
     * @return true is \p id can be decoded
     */
    bool can_decode(uint32_t id) const noexcept;

    /**
     * should an \p id be decoded? the decoder must know how to decode the id
     * and the rate limit must be triggered as well.
     * @return true is \p id should be decoded
     */
    bool should_decode(uint32_t id) noexcept;

protected:
    explicit decoder(size_t size) noexcept;

    void deny_all() noexcept;

    void allow_all() noexcept;

    void allow_id(uint32_t id) noexcept;

    virtual uint16_t rate(uint32_t id) const noexcept = 0;

    __always_inline size_t size() const noexcept
    {
        return _size;
    }

    __always_inline ID* begin() noexcept
    {
        return _ids;
    }

    __always_inline ID* end() noexcept
    {
        return _ids + _size;
    }

    __always_inline ID const* cbegin() const noexcept
    {
        return _ids;
    }

    __always_inline ID const* cend() const noexcept
    {
        return _ids + _size;
    }

    ID* find(uint32_t id) noexcept;

    ID const* find(uint32_t id) const noexcept;

private:
    /**
     * RaceChrono app will write LE message when it wants a certain ID.
     * This callback from BLE stack will handle that.
     */
    void onWrite(BLECharacteristic* pCharacteristic) override;

protected:
    ID* _ids; // not using std::vector as it seemed broken on most arduino libc++ implementations...
    size_t _size;
};

} // namespace canbus

extern canbus::decoder& CANDEC;
