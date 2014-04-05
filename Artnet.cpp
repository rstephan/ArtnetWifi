#include <Artnet.h>

Artnet::Artnet() {}

void Artnet::begin(byte mac[], byte ip[], unsigned int port)
{
  Ethernet.begin(mac,ip);
  Udp.begin(port);
}

int Artnet::read(char packetBuffer[])
{
  packetSize = Udp.parsePacket();
  
  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  { 
      Udp.read(packetBuffer, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      for (byte i = 0 ; i < 9 ; i++)
      {
        if (packetBuffer[i] != ART_NET_ID[i])
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
  else
  {
    return 0;
  }
}


void Artnet::printPacketHeader()
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

void Artnet::printPacketContent(char packetBuffer[])
{
  for (int i = 18 ; i < dmxDataLength ; i++){
    Serial.print(packetBuffer[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}
