# ArtnetWifi

An Art-Net library for Wifi-Arduino's (aka. ESP8266 or ESP32)

Note: this library assumes you are using a wifi module

Based on https://github.com/natcl/Artnet [master](https://github.com/natcl/Artnet/archive/master.zip)

## Installation

Place this in your `~/Documents/Arduino/libraries` folder.

## Examples

Different examples are provided, here is a summary of what each example does.

### ArtnetWifiDebug

Simple test for WiFi, serial and Art-Net.

Example output (Serial Monitor, 115200 Baud):
```
DMX: Univ: 0, Seq: 0, Data (48): 17 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ...
```

If this example is not working, don't try anything else!

### ArtnetWifiFastLED

This example will receive multiple universes via Art-Net and control a strip of WS2812 LEDs via the [FastLED library](https://github.com/FastLED/FastLED). It is similar to the NeoPixel example but it will work on the ESP32 and the ESP8266 controller as well.

### ArtnetWifiNeoPixel

**ESP8266 only!**
This example will receive multiple universes via Art-Net and control a strip of WS2811 LEDs via Adafruit's [NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel).

### ArtnetWifiTransmit

This is a simple transmitter. Send 3 byte over into the Art-Net, to make a RGB light ramp-up in white.

# Art-Net

Art-Net(tm) is a trademark of Artistic Licence Holdings Ltd. The Art-Net protocol and associated documentation is copyright Artistic Licence Holdings Ltd.

[Art-Net](http://www.artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf)
