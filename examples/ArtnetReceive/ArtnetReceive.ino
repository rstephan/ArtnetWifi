#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <mac.h>

char packetBuffer[MAX_BUFFER_ARTNET];

byte ip[] = {192, 168, 2, 2};
unsigned int port = 6454;  

void setup()
{
	Serial.begin(115200);
	read_mac();
	artnetInit(mac, ip, port);
}

void loop()
{
	if (artnetReadPacket(packetBuffer))
	{
		printPacketHeader();
	}
	
}