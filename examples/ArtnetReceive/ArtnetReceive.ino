#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

Artnet artnet;

byte ip[] = {192, 168, 2, 2};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  Serial.begin(115200);
  artnet.begin(mac, ip);
}

void loop()
{
  if (artnet.read() == ART_DMX)
  {
      artnet.printPacketHeader();
      artnet.printPacketContent();
      uint8_t* data = artnet.getArtnetPacket();
      Serial.println(data[19]);
  }
    
}