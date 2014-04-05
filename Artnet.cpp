#include <Artnet.h>

Artnet::Artnet() {}

void Artnet::begin(byte mac[], byte ip[], unsigned int port, char* packetBuffer)
{
  Ethernet.begin(mac,ip);
  Udp.begin(port);
  _packetBuffer = packetBuffer;
}

int Artnet::read()
{
  packetSize = Udp.parsePacket();
  
  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  { 
      Udp.read(_packetBuffer, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      for (byte i = 0 ; i < 9 ; i++)
      {
        if (_packetBuffer[i] != ART_NET_ID[i])
          return 0;
      }
        
      opcode = _packetBuffer[8] | _packetBuffer[9] << 8; 
      sequence = _packetBuffer[12];
      incomingUniverse = _packetBuffer[14] | _packetBuffer[15] << 8;  
      dmxDataLength = _packetBuffer[17] | _packetBuffer[16] << 8;

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
  for (int i = ART_DMX_START ; i < dmxDataLength ; i++){
    Serial.print(_packetBuffer[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}
