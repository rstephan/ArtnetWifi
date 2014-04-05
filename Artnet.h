#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define ART_POLL 0x2000
#define ART_DMX 0x5000
#define MAX_BUFFER_ARTNET 530

void artnetInit(byte mac[], byte ip[], unsigned int port);
int artnetReadPacket(char packetBuffer[]);
void printPacketHeader();
void printPacketContent(char packetBuffer[]);