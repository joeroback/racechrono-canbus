# Arduino Devices Supported

* [Adafruit ESP32-S3 Feather with 4MB Flash 2MB PSRAM](https://www.adafruit.com/product/5477)
* [Adafruit ESP32 Feather V2](https://www.adafruit.com/product/5400)
* [Adafruit HUZZAH32 ESP32 Feather Board](https://www.adafruit.com/product/3405)

# Assembling CAN-bus Arduino Device

While the ESP32 contains a SJA-1000 like CAN-bus controller, it still requires a CAN-bus transceiver. There are a few
ways to do this, some cheap, some easy. I choose the easy route. All the Adafruit devices listed above are all feather
form factor, so using a Feather Wing CAN-bus transceiver is fairly plug-and-play, just requires some basic soldering.
Here is a list of equipment needed.

* [CAN Bus FeatherWing For ESP32 by SK Pang Electronics](https://copperhilltech.com/can-bus-featherwing-for-esp32/)
* [Straight Single Row PCB Board Female Pin Header Sockets](https://smile.amazon.com/dp/B09BDX9L66)
* [EEMB 3.7v LiPo Battery 620mAh](https://smile.amazon.com/dp/B095VVR4LB)
* [MAIYUM 63-37 Tin Lead Rosin Core Solder Wire](https://smile.amazon.com/gp/product/B075WBDYZZ)
* [Desoldering Wick Braid](https://smile.amazon.com/dp/B094GZ6CPZ)
* [Heat Shrink Tubing](https://smile.amazon.com/dp/B084GDLSCK)
* [22 AWG JST SM 2 Pin Plugs](https://smile.amazon.com/gp/product/B06WGN56V2)
* [22 AWG Stranded Wire](https://smile.amazon.com/LotFancy-Stranded-Electrical-Flexible-insulated/dp/B07Q3G1VG2)
* [Hakko FX888D-23BY Digital Soldering Station](https://smile.amazon.com/gp/product/B00ANZRT4M)
* [LED Lighted Magnifying Helping Hands](https://smile.amazon.com/gp/product/B07V6CWRQ3)

Here is a video of soldering the ESP32 board and the feather wing CAN-bus transceiver.

[![ESP32 Assembly Video](https://img.youtube.com/vi//0.jpg)](https://www.youtube.com/watch?v=)
