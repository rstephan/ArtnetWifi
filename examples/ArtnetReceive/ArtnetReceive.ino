#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <mac.h>

Artnet artnet;

char packetBuffer[MAX_BUFFER_ARTNET];

byte ip[] = {192, 168, 2, 2};

void setup()
{
  Serial.begin(115200);
  read_mac();
  artnet.begin(mac, ip, packetBuffer);
}

void loop()
{
  if (artnet.read() == ART_DMX)
  {
      artnet.printPacketHeader();
      artnet.printPacketContent();
  }
    
}