#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <SD.h>

Artnet artnet;
File datafile;

const int chipSelect = 4;
bool record = 0;
uint8_t filebuffer[8];
uint16_t position = 0;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 2, 2};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  Serial.begin(115200);
  artnet.begin(mac, ip);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();

}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  Serial.println(universe);
  if (universe == 15)
  {
    if (data[0] == 255)
    {
      Serial.println("Record !");
      datafile = SD.open("data.txt", FILE_WRITE);
      record = 1;
    }
    if (data[0] == 0)
    {
      Serial.println("Stop !");
      record = 0;
      datafile.close();
    }
    if (data[0] == 2)
    {
      datafile = SD.open("data.txt", FILE_READ);
      datafile.readBytes(filebuffer, 8);
      for (byte i = 0; i < 8 ; i++)
      {
        Serial.print(filebuffer[i]);
        Serial.print(" ");
      }

      Serial.println();
    }

  }

  if (record == 1 && universe == 0)
  {
    Serial.println("Writing to file");
    datafile.write(data, length);
  }
  
}