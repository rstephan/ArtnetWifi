/*The MIT License (MIT)

Copyright (c) 2014 Nathanaël Lécaudé
https://github.com/natcl/Artnet, http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811

Copyright (c) 2016 Stephan Ruloff
https://github.com/rstephan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <ArtnetWifi.h>


const char ArtnetWifi::artnetId[] = ART_NET_ID;

ArtnetWifi::ArtnetWifi() :
  nodeIp(),
  broadcastIp(),
  shortName(""),
  longName("") {}

void ArtnetWifi::begin(String hostname)
{
  Udp.begin(ART_NET_PORT);
  host = hostname;
  sequence = 1;
  phisical = 0;
}

uint16_t ArtnetWifi::read(void)
{
  sendArtPollReplyPacket();

  packetSize = Udp.parsePacket();

  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  { 
      Udp.read(artnetPacket, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      if (memcmp(artnetPacket, artnetId, sizeof(artnetId)) != 0) {
        return 0;
      }

      opcode = artnetPacket[8] | artnetPacket[9] << 8; 

      if (opcode == ART_DMX)
      {
        sequence = artnetPacket[12];
        incomingUniverse = artnetPacket[14] | artnetPacket[15] << 8;  
        dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;

        if (artDmxCallback) (*artDmxCallback)(incomingUniverse, dmxDataLength, sequence, artnetPacket + ART_DMX_START);
        return ART_DMX;
      }
      if (opcode == ART_POLL)
      {
        return ART_POLL; 
      }
  }

  return 0;
}

uint16_t ArtnetWifi::makePacket(void)
{
  uint16_t len;
  uint16_t version;

  memcpy(artnetPacket, artnetId, sizeof(artnetId));
  opcode = ART_DMX;
  artnetPacket[8] = opcode;
  artnetPacket[9] = opcode >> 8;
  version = 14;
  artnetPacket[11] = version;
  artnetPacket[10] = version >> 8;
  artnetPacket[12] = sequence;
  sequence++;
  if (!sequence) {
    sequence = 1;
  }
  artnetPacket[13] = phisical;
  artnetPacket[14] = outgoingUniverse;
  artnetPacket[15] = outgoingUniverse >> 8; 
  len = dmxDataLength + (dmxDataLength % 2); // make a even number 
  artnetPacket[17] = len;
  artnetPacket[16] = len >> 8;
  
  return len;
}

bool ArtnetWifi::setArtPollReplyInformation(IPAddress nodeIp, IPAddress broadcastIp, String shortName, String longName)
{
  if (shortName.length() > 17 || longName.length() > 63)
  {
    return false;
  }

  this->nodeIp = nodeIp;
  this->broadcastIp = broadcastIp;
  this->shortName = shortName;
  this->longName = longName;

  return true;
}

void ArtnetWifi::sendArtPollReplyPacket(void)
{
  if ((uint32_t) broadcastIp == 0 || millis() < nextPollReply)
  {
    return;
  }

  // only actually send an ArtPollReply packet every 2 seconds
  nextPollReply = millis() + 2000;


  uint16_t len;

  // Art-Net ID
  memcpy(artnetPacket, artnetId, sizeof(artnetId));

  // OpCode
  opcode = ART_POLL_REPLY;
  artnetPacket[8] = opcode; // low byte
  artnetPacket[9] = opcode >> 8; // high byte

  // IP address
  artnetPacket[10] = nodeIp[0];
  artnetPacket[11] = nodeIp[1];
  artnetPacket[12] = nodeIp[2];
  artnetPacket[13] = nodeIp[3];

  // Port
  artnetPacket[14] = 0x36; // low byte
  artnetPacket[15] = 0x19; // high byte

  // VersionInfo
  uint16_t version = 14;
  artnetPacket[16] = version >> 8; // high byte
  artnetPacket[17] = version; // low byte

  // NetSwitch / SubSwitch
  artnetPacket[18] = 0;
  artnetPacket[19] = 0;

  // Oem
  artnetPacket[20] = 0; // high byte
  artnetPacket[21] = 0; // low byte

  // Ubea Version
  artnetPacket[22] = 0;

  // Status1
  artnetPacket[23] = 0;

  // EstaMan
  artnetPacket[24] = 0; // low byte
  artnetPacket[25] = 0; // high byte

  // ShortName
  len = shortName.length();
  memcpy(artnetPacket + 26, shortName.c_str(), len);
  memset(artnetPacket + 26 + len, 0, 18 - len);

  // LongName
  len = longName.length();
  memcpy(artnetPacket + 44, longName.c_str(), len);
  memset(artnetPacket + 44 + len, 0, 64 - len);

  // NodeReport
  memset(artnetPacket + 108, 0, 64);

  // NumPorts
  artnetPacket[172] = 0; // high byte
  artnetPacket[173] = 1; // low byte

  // PortTypes[4]
  artnetPacket[174] = 0x80;
  artnetPacket[175] = 0;
  artnetPacket[176] = 0;
  artnetPacket[177] = 0;

  // GoodInput[4]
  // GoodOutput[4]
  // SwIn[4]
  // SwOut[4]
  // SwVideo
  // SwMacro
  // SwRemote
  // Spare[3]
  // Style
  // MAC address (6 bytes)
  // BindIp[4]
  // BindIndex
  memset(&artnetPacket[178], 0, 34);

  // Status2
  artnetPacket[212] = 0x01;

  // Filler
  memset(&artnetPacket[213], 0, 26);

  len = 239;


  Udp.beginPacket(broadcastIp, ART_NET_PORT);
  Udp.write(artnetPacket, len);
  Udp.endPacket();
}

int ArtnetWifi::write(void)
{
  uint16_t len;

  len = makePacket();
  Udp.beginPacket(host.c_str(), ART_NET_PORT);
  Udp.write(artnetPacket, ART_DMX_START + len);

  return Udp.endPacket();
}

int ArtnetWifi::write(IPAddress ip)
{
  uint16_t len;

  len = makePacket();
  Udp.beginPacket(ip, ART_NET_PORT);
  Udp.write(artnetPacket, ART_DMX_START + len);

  return Udp.endPacket();
}

void ArtnetWifi::setByte(uint16_t pos, uint8_t value)
{
  if (pos > 512) {
    return;
  }
  artnetPacket[ART_DMX_START + pos] = value;
}

void ArtnetWifi::printPacketHeader(void)
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

void ArtnetWifi::printPacketContent(void)
{
  for (uint16_t i = ART_DMX_START ; i < dmxDataLength ; i++){
    Serial.print(artnetPacket[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}
