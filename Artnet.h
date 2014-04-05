#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define ART_POLL 0x2000
#define ART_DMX 0x5000
#define MAX_BUFFER_ARTNET 530

//int packetSize;
//int opcode;
//char packetID[8]; // ID of the packet, should be "Art-Net" else ignore
//byte sequence;
//int incomingUniverse;
//int dmxDataLength;

void artnetInit(byte mac[], byte ip[], unsigned int port);
int artnetReadPacket(char packetBuffer[]);
int parseArtnet(char packetBuffer[], int _packetSize);
void printPacketHeader();
void printPacketContent(char packetBuffer[]);