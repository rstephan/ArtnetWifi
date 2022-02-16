/*
This example will receive multiple universes via Art-Net and control a strip of
WS2812 LEDs via the FastLED library: https://github.com/FastLED/FastLED
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/
#include "ArtnetWifi.h"
#include <Arduino.h>
#include <FastLED.h>

// Wifi settings
const char *SSID = "ssid";
const char *PASSWORD = "pAsSwOrD";

// LED settings
const int LED_COUNT = 300;               // CHANGE FOR YOUR SETUP
const int CHANNEL_COUNT = LED_COUNT * 3; // Total number of channels you want to receive (1 led = 3 channels)
const byte DATA_PIN = 2;
CRGB leds[LED_COUNT];

// Art-Net settings
// CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const int START_UNIVERSE = 0;
const int UNIVERSE_COUNT = CHANNEL_COUNT / 512 + ((CHANNEL_COUNT % 512) ? 1 : 0);
bool universesReceived[UNIVERSE_COUNT];
bool writeLeds = true;
ArtnetWifi artnet;

bool connectToWifi(void)
{
  bool state = true;
  int i = 0;

  WiFi.begin(SSID, PASSWORD);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (i > 20)
    {
      state = false;
      break;
    }
    i++;
  }
  if (state)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void testLeds()
{
  std::vector<CRGB> colors{CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
  for (const auto color : colors)
  {
    FastLED.showColor(color);
    delay(500);
  }
}

void setChannel(unsigned universe, unsigned channel, uint8_t value)
{
  // Get the index of leds array corresponding to the given universe and channel
  unsigned ledIndex = (universe * 512 + channel) / 3;
  // Avoid array index being out of bounds, since the last universe may not be completely represented in the leds
  // array.
  if (ledIndex >= LED_COUNT)
  {
    return;
  }
  // Get the index of the color channel
  // 0 -> first channel (e.g. red), 1 -> second channel (e.g. green), 2 -> third channel (e.g. blue)
  unsigned colorChannelIndex = (universe * 512 + channel) % 3;
  leds[ledIndex][colorChannelIndex] = value;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
  // Range check
  if (universe < START_UNIVERSE)
  {
    Serial.println("Error: Received universe index less than start index!");
    return;
  }
  unsigned index = universe - START_UNIVERSE;
  if (index >= UNIVERSE_COUNT)
  {
    Serial.println("Error: Received universe index greater than end index!");
    return;
  }
  // Store which universe has got in
  universesReceived[index] = true;
  // Read received universe and put each channel into the right part of the display buffer
  for (int i = 0; i < length; i++)
  {
    setChannel(index, i, data[i]);
  }
  // Check if all universes were received. We should then write the display buffer to the leds
  writeLeds = true;
  for (int i = 0; i < UNIVERSE_COUNT; i++)
  {
    if (universesReceived[i])
    {
      writeLeds = false;
      break;
    }
  }
  // Write to the LEDs if all universes where received
  if (writeLeds)
  {
    FastLED.show();
    // Reset universeReceived to false
    memset(universesReceived, false, UNIVERSE_COUNT);
  }
}

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, LED_COUNT);
  testLeds();
  connectToWifi();
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void loop()
{
  // We call the read function inside the loop
  artnet.read();
}
