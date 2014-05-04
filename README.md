# Artnet

An Art-Net library for Teensy and Arduino

Note: this library assumes you are using the standard Ethernet library

## Installation

You can download the [master](https://github.com/natcl/Artnet/archive/master.zip) and place the folder in your `~/Documents/Arduino/libraries` folder.

## Examples

Different examples are provided, here is a summary of what each example does.

### ArtnetNeopixel

This example will receive multiple universes via Artnet and control a strip of ws2811 leds via Adafruit's [NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel).

### ArtnetReceive

This is a basic example that will print out the header and the content of an ArtDmx packet.  This example uses the read() function and the different getter functions to read the data.

### ArtnetReceiveCallback

This is similar to ArtnetReceive but uses a callback to read the data.

## Acknowledgements

Many thanks to virtualdave and [mortonkopf](http://orchardelica.com/wp/artnet-multiple-universe-with-teensy-3-and-octows2811) on the pjrc [forums](http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811?highlight=artnet) for the original sketches !