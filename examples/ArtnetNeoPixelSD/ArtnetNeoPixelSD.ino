/*
Same as ArtnetNeoPixel.ino but with controls to record and playback sequences from an SD card. 
To record, send 255 to the first channel of universe 14. To stop, send 0 and to playback send 127.  
The limit of leds seems to be around 450 to get 44 fps. The playback routine is not optimzed yet.
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>

// Neopixel settings
const int numLeds = 200; // change for your setup
const byte dataPin = 2;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);

// Artnet settings
Artnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as zero.
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)
byte channelBuffer[numberOfChannels]; // Combined universes into a single array

// SD card
File datafile;
char fileName[] = "data.txt";
const int chipSelect = 4;
bool record = 0;
bool playback = 0;

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 2, 2};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  Serial.begin(115200);
  artnet.begin(mac, ip);
  leds.begin();
  initTest();

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
  }
  else
    Serial.println("initialization done.");

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
  if (playback)
  {
    while(datafile.available() && playback)
    {
      artnet.read();
      datafile.read(channelBuffer, numberOfChannels);
      for (int i = 0; i < numLeds; i++)
        leds.setPixelColor(i, channelBuffer[(i) * 3], channelBuffer[(i * 3) + 1], channelBuffer[(i * 3) + 2]);
      
      leds.show();
      delay(20);
    }
    playback = 0;
    datafile.close();
  }  
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  // set brightness of the whole strip 
  if (universe == 15)
  {
    leds.setBrightness(data[0]);
    leds.show();
  }
  
  if (universe == 14)
  {
    // record
    if (data[0] == 255)
    {
      if (SD.exists(fileName))
      {
        SD.remove(fileName);
      }
      datafile = SD.open(fileName, FILE_WRITE);
      record = 1;
      playback = 0;
    }
    // play
    if (data[0] == 127)
    {
      record = 0;
      playback = 1;
      datafile = SD.open(fileName, FILE_READ);
    }
    // stop
    if (data[0] == 0)
    { 
      record = 0;
      playback = 0;
      datafile.close();
    }
  }

  // Store which universe has got in
  if (universe < maxUniverses)
    universesReceived[universe] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }

  if (!playback)
  {
    // read universe and put into the right part of the display buffer
    for (int i = 0 ; i < length ; i++)
    {
      int bufferIndex = i + ((universe - startUniverse) * length);
      if (bufferIndex < numberOfChannels) // to verify
        channelBuffer[bufferIndex] = byte(data[i]);
    }
  }

  if (record && sendFrame)
  {
    datafile.write(channelBuffer, numberOfChannels);
    memset(universesReceived, 0, maxUniverses);
  } 

  if (!playback && !record)
  {
    // send to leds
    for (int i = 0; i < numLeds; i++)
    {
      leds.setPixelColor(i, channelBuffer[(i) * 3], channelBuffer[(i * 3) + 1], channelBuffer[(i * 3) + 2]);
    }      
    
    if (sendFrame)
    {
      leds.show();
      // Reset universeReceived to 0
      memset(universesReceived, 0, maxUniverses);
    }
  }
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0);
  leds.show();
}
