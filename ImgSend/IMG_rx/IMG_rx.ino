#define Serial_Monitor_Baud 500000
#include <SPI.h>
#include "Settings.h"
#include <SX1280_lite.h>

#define packetLength (120 + 2) // = 640 packets per frame + 2 bytes for index

SX1280Class SX1280LT;

//uint32_t RXpacketCount = 0;
//uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];

uint8_t RXPacketL;                               //stores length of packet received
int8_t  PacketRSSI;                              //stores RSSI of received packet
uint16_t packetCo = 0;
uint16_t IRQStatus;

bool printBytes = false;

void setup()
{
  SerialUSB.begin(Serial_Monitor_Baud);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  SPI.begin();
  SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));

  if (SX1280LT.begin(NSS, NRESET, RFBUSY, DIO1, -1, -1)) { delay(1000); }
  else { while (1) { delay(1000); } }

  setup_FLRC();
}


void loop()
{
  SX1280LT.setRx(PERIOBASE_01_MS, 0xFFFF);

  if (printBytes)
  {
    for (int i = 0; i < packetLength; i++)
    {
      SerialUSB.write(RXBUFFER[i]);
      // false packet detection
      
      if (digitalRead(DIO1))
      {
        digitalWrite(LED_BUILTIN, HIGH);
        SX1280LT.readPacketReceptionFLRC();
        RXPacketL = SX1280LT.readRXPacketL();
        IRQStatus = SX1280LT.readIrqStatus();
        if ((IRQStatus == (IRQ_RX_DONE + IRQ_SYNCWORD_VALID) || (IRQ_RX_DONE & IRQ_CRC_ERROR)) && RXPacketL == packetLength)
        {
          SX1280LT.setRx(PERIOBASE_01_MS, 0xFFFF); //skip
          //while (true) { SerialUSB.println("not fast enough"); digitalWrite(LED_BUILTIN, HIGH); delay(500); }
        }
        else SX1280LT.setRx(PERIOBASE_01_MS, 0xFFFF);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    SerialUSB.write(PacketRSSI);
  }

  while (!digitalRead(DIO1));
  
  SX1280LT.readPacketReceptionFLRC();
  //RXPacketL = SX1280LT.readRXPacketL();
  PacketRSSI = SX1280LT.readPacketRSSI();
  IRQStatus = SX1280LT.readIrqStatus();

  if (IRQStatus == (IRQ_RX_DONE + IRQ_SYNCWORD_VALID) || IRQStatus == (IRQ_RX_DONE + IRQ_CRC_ERROR))
  {
    SX1280LT.readPacketFLRC(RXBUFFER, RXBUFFER_SIZE);
    printBytes = true;
  }
  else { printBytes = false; }
}

void setup_FLRC()
{
  SX1280LT.setStandby(MODE_STDBY_XOSC);
  SX1280LT.setRegulatorMode(USE_LDO);
  SX1280LT.setPacketType(PACKET_TYPE_FLRC);
  SX1280LT.setRfFrequency(Frequency, Offset);
  SX1280LT.setBufferBaseAddress(0, 0);
  SX1280LT.setModulationParams(BandwidthBitRate, CodingRate, BT);
  SX1280LT.setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF);
  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, IRQ_RX_DONE, 0, 0);
  SX1280LT.setSyncWord1(Sample_Syncword);
  SX1280LT.setHighSensitivity();
}
