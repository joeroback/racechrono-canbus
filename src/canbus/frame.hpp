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

#include <cstdint>

namespace canbus
{

/**
 * frame format (11-bit/29-bit)
 */
enum frame_format : uint8_t
{
    standard = 0x0, //!< standard frame, using 11 bit identifer
    extended = 0x1, //!< extended frame, using 29 bit identifer
};

/**
 * frame remote transmission request (rtr)
 */
enum frame_rtr : uint8_t
{
    data   = 0x0, //!< data frame.
    remote = 0x1, //!< remote frame.
};

/**
 * frame information
 */
union frame_info
{
    uint8_t u8;
    struct
    {
        uint8_t dlc: 4;             //!< data length code (0 to 8) of the frame
        uint8_t self_reception: 1;  //!< this frame should be transmitted using self reception command
        uint8_t single_shot: 1;     //!< this frame should be transmitted using single shot command
        uint8_t rtr: 1;             //!< remote transmission request
        uint8_t frame_format: 1;    //!< format of the frame (1 = extended, 0 = standard)
    };
};

/**
 * CAN frame
 *
 * id/data are packed together so that it can be sent over
 * bluetooth le to racechrono without any extra copies...
 */
struct frame
{
    frame_info info;     //!< Frame info
    uint32_t id;         //!< PID
    union {
        uint8_t u8[8];   //!< payload byte access
        uint32_t u32[2]; //!< payload u32 access
    } data;
} __attribute__ ((__packed__));

static_assert(std::is_standard_layout<frame>::value, "not standard layout");
static_assert(std::is_move_constructible<frame>::value, "not move constructible");
static_assert(std::is_move_assignable<frame>::value, "not move assignable");
static_assert(sizeof(frame) == 13, "not 13 bytes");

} // namespace canbus
