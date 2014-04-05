#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <mac.h>

Artnet artnet;

char packetBuffer[MAX_BUFFER_ARTNET];

byte ip[] = {192, 168, 2, 2};
unsigned int port = 6454;  

void setup()
{
  Serial.begin(115200);
  read_mac();
  artnet.begin(mac, ip, port);
}

void loop()
{
  if (artnet.read(packetBuffer) == ART_DMX)
  {
      artnet.printPacketHeader();
  }
    
}