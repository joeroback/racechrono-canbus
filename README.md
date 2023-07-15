# RaceChrono DIY CAN-bus ESP32 Device

## Disclaimer

CAN-bus is the central nervous system of a car. Modifying it in anyway can harm the car, cause things like
"check engine" lights, electrical or mechanical damage of the car or components, loss of control, injuries, or
even death.

By using any information, hardware designs or code in this project you assume any and all risk, and release any
liability from the author(s) and contributors to this project.

## Debugging

Turn on debugging messages by uncommenting out `#define DEBUG` in `src/racechrono-canbus.hpp`.

## Outline

ESP32 CAN-bus device for [RaceChrono](https://racechrono.com) on iOS/Android.

* [ESP32 Device Assembly](docs/ESP32.md)
* [CAN-bus Hacking with Raspberry Pi](docs/CANbusHacking.md)
* Vehicles
  * [Wiring Supported Vehicles](docs/WiringVehicles.md)
  * [Adding New Vehicles](docs/AddingNewVehicles.md)
* [Arduino Setup](docs/Arduino.md)

## TODO

- [ ] Add histograms to candump-parse tool
- [ ] Bluetooth LE optimizations
- [ ] Compare Bluetooth LE 4.0 vs 5.0 performance
- [ ] Add support for NeoPixel RGB LED on ESP32s3 board

## Credits

* https://github.com/aollin/racechrono-ble-diy-device
* https://github.com/timurrrr/RaceChronoDiyBleDevice
* https://github.com/espressif/arduino-esp32
