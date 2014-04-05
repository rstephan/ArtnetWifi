#ifndef ARTNET_H
#define ARTNET_H

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// UDP specific
#define ART_NET_PORT 6454
// Opcodes
#define ART_POLL 0x2000
#define ART_DMX 0x5000
// Buffers
#define MAX_BUFFER_ARTNET 530
// Packet
#define ART_NET_ID "Art-Net\0"
#define ART_DMX_START 18

class Artnet
{
public:
  Artnet();

  void begin(byte mac[], byte ip[], char* packetBuffer);
  int read();
  void printPacketHeader();
  void printPacketContent();

private:
  EthernetUDP Udp;
  int packetSize;
  int opcode;
  byte sequence;
  int incomingUniverse;
  int dmxDataLength;
  char *_packetBuffer;
};

#endif
