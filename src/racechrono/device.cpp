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

#include <cstdio>

#include "device.hpp"

namespace racechrono
{

device& device::get() noexcept
{
    static device instance;
    return instance;
}

bool device::start(BLECharacteristicCallbacks* callbacks) noexcept
{
    infoln("Bluetooth LE starting...");

    char name[32];

    uint8_t mac[6];
    esp_err_t ret = esp_read_mac(mac, ESP_MAC_BT);

    if (ret == ESP_OK)
    {
        infoln("Bluetooth LE MAC: %02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        snprintf(name, sizeof(name), "RaceChrono %02X:%02X:%02X", mac[3], mac[4], mac[5]);
    }
    else
    {
        errorln("ERROR: Unable to determine bluetooth MAC address! reason: %d", ret);
        snprintf(name, sizeof(name), "RaceChrono DIY");
    }

    BLEDevice::init(name);
    BLEDevice::setPower(BLE_PWR_LVL);

    _server = BLEDevice::createServer();
    _server->setCallbacks(this);

    _service = _server->createService(racechrono_service_uuid);
    _pid_requests = _service->createCharacteristic(pid_characteristic_uuid, BLECharacteristic::PROPERTY_WRITE);
    _pid_requests->setCallbacks(callbacks);
    _canbus_frames = _service->createCharacteristic(can_bus_characteristic_uuid,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    _canbus_frames->addDescriptor(&_2902_desc);
    _service->start();

    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(_service->getUUID());
    advertising->setScanResponse(false);
    BLEDevice::startAdvertising();

    infoln("Bluetooth LE started!");

    return true;
}

void device::onConnect(BLEServer*)
{
    infoln("Bluetooth LE client connected!");
    _client_connected = true;
}

void device::onDisconnect(BLEServer*)
{
    // once connection is made, BLE stops advertising, so on disconnect, start advertising again..
    infoln("Bluetooth LE client disconnected!");
    _client_connected = false;
    BLEDevice::startAdvertising();
}

#if defined(DEBUG)
void device::stats() noexcept
{
    if (logging::logger::get().level() >= logging::log_level::info)
    {
        unsigned long delta = _stats_timer.elapsed(CONFIG_RC_STATS_TIMEOUT);

        if (delta > 0UL)
        {
            float msg_per_sec = (static_cast<float>(exchange(_ble_count, 0UL)) / static_cast<float>(delta)) * 1e6f;
            infoln(" Bluetooth LE msg/s: %.2f", msg_per_sec);
        }
    }
}
#endif

} // namespace racechrono

racechrono::device& RCDEV = racechrono::device::get();
