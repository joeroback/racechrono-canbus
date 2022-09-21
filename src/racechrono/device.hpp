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
#include "../utils/timer.hpp"

#include <BLE2902.h>
#include <BLECharacteristic.h>
#include <BLEDevice.h>

namespace racechrono
{

/**
 * RaceChrono Bluetooth LE device.
 */
class device final
    : public BLEServerCallbacks
{
    CPP_NOCOPY(device);
    CPP_NOMOVE(device);

public:
    ~device() noexcept override = default;

    /**
     * Get instance (singleton)
     */
    static device& get() noexcept;

    /**
     * @return true is connected to RaceChrono app
     */
    __always_inline bool connected() const noexcept
    {
        return _server->getConnectedCount() > 0;
    }

    /**
     * @return true is Bluetooth LE stack is started
     */
    __always_inline bool started() const noexcept
    {
        return false;
    }

    /**
     * Start Bluetooth LE stack.
     * @return true is sucessful; otherwise false.
     */
    bool start(BLECharacteristicCallbacks* callbacks) noexcept;

    /**
     * Send \p data of size \p len over Bluetooth LE stack as an LE notification.
     */
    __always_inline void send(uint8_t* data, size_t len) noexcept
    {
        if (_client_connected)
        {
            _canbus_frames->setValue(data, len);
            _canbus_frames->notify();
            ++_ble_count;
        }
    }

    /**
     * BLE callback for when a client (RaceChrono app) connects
     */
    void onConnect(BLEServer*) override;

    /**
     * BLE callback for when a client (RaceChrono app) disconnects
     */
    void onDisconnect(BLEServer*) override;

    /**
     * print any bluetooth stats
     */
#if defined(DEBUG)
    void stats() noexcept;
#else
    void stats() noexcept {}
#endif

private:
    // RaceChrono BLE service UUID
    static constexpr uint16_t racechrono_service_uuid = 0x1ff8;

    // RaceChrono uses two BLE characteristics:
    // 0x01 to be notified of data received for those PIDs
    // 0x02 to request which PIDs to send and how frequently
    static constexpr uint16_t can_bus_characteristic_uuid = 0x1;
    static constexpr uint16_t pid_characteristic_uuid = 0x2;

    explicit device() noexcept
        : _server(nullptr)
        , _service(nullptr)
        , _pid_requests(nullptr)
        , _canbus_frames(nullptr)
        , _2902_desc{}
        , _client_connected(false)
        , _stats_timer{}
        , _ble_count(0UL)
    {
        _2902_desc.setNotifications(true);
    }

private:
    BLEServer* _server;
    BLEService* _service;
    BLECharacteristic* _pid_requests;
    BLECharacteristic* _canbus_frames;
    BLE2902 _2902_desc;
    bool _client_connected;
    utils::timer _stats_timer;
    unsigned long _ble_count;
};

} // namespace racechrono

extern racechrono::device& RCDEV;
