#include <Artnet.h>

Artnet::Artnet() {}

void Artnet::begin(byte mac[], byte ip[])
{
  Ethernet.begin(mac,ip);
  Udp.begin(ART_NET_PORT);
}

uint16_t Artnet::read()
{
  packetSize = Udp.parsePacket();
  
  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  { 
      Udp.read(artnetPacket, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      for (byte i = 0 ; i < 9 ; i++)
      {
        if (artnetPacket[i] != ART_NET_ID[i])
          return 0;
      }
        
      opcode = artnetPacket[8] | artnetPacket[9] << 8; 
      sequence = artnetPacket[12];
      incomingUniverse = artnetPacket[14] | artnetPacket[15] << 8;  
      dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;

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

void Artnet::printPacketContent()
{
  for (uint16_t i = ART_DMX_START ; i < dmxDataLength ; i++){
    Serial.print(artnetPacket[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}
