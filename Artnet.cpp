#include <Artnet.h>

int packetSize;
int opcode;
char packetID[8] = "Art-Net"; // ID of the packet, should be "Art-Net" else ignore
byte sequence;
int incomingUniverse;
int dmxDataLength;

EthernetUDP Udp;
 
void artnetInit(byte mac[], byte ip[], unsigned int port)
{
  Ethernet.begin(mac,ip);
  Udp.begin(port);
}

int artnetReadPacket(char packetBuffer[])
{
  packetSize = Udp.parsePacket();
  
  if (packetSize)
  { 
      Udp.read(packetBuffer, MAX_BUFFER_ARTNET);
      if (parseArtnet(packetBuffer, packetSize) == ART_DMX)
        Serial.println("Got ARTDMX");
      if (parseArtnet(packetBuffer, packetSize) == ART_POLL)
        Serial.println("Got ARTPOLL");
      if (parseArtnet(packetBuffer, packetSize) == 0)
        return 0;
      
      return 1;
  }
  else
  {
    return 0;
  }
}

int parseArtnet(char packetBuffer[], int _packetSize)
{
  packetSize = _packetSize;

  // Check that packetID is "Art-Net" else ignore
  for (byte i = 0 ; i < 8 ; i++)
  {
    if (packetBuffer[i] != packetID[i])
      return 0;
  }
    
    

  opcode = packetBuffer[8] | packetBuffer[9] << 8; 
  sequence = packetBuffer[12];
  incomingUniverse = packetBuffer[14] | packetBuffer[15] << 8;  
  dmxDataLength = packetBuffer[17] | packetBuffer[16] << 8;

  if (opcode == ART_DMX)
    return ART_DMX;
  if (opcode == ART_POLL)
    return ART_POLL;

}

void printPacketHeader()
{
  Serial.print("packet size = ");
  Serial.print(packetSize);
  Serial.print("\topcode = ");
  Serial.print(opcode, HEX);
  Serial.print("\tuniverse number = ");
  Serial.print(incomingUniverse);
  Serial.print("\tdata length = ");
  Serial.print(dmxDataLength);
  Serial.print("\tsequence n0. = ");
  Serial.println(sequence);

}

void printPacketContent(char packetBuffer[])
{
  for (int i = 18 ; i < dmxDataLength ; i++){
    Serial.print(packetBuffer[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}
