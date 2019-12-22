#define Serial_Monitor_Baud 115200

#include <CanSatKit-fordual.h>

#include <SPI.h>
#include "Settings.h"
#include <SX1280_lite.h>

using namespace CanSatKit;
// CanSatKit variables
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,                  // frequency in MHz
            Bandwidth_125000_Hz,    // bandwidth - check with CanSat regulations to set allowed value
            SpreadingFactor_9,      // see provided presentations to determine which setting is the best
            CodingRate_4_8);        // see provided presentations to determine which setting is the best

// FLRC variables
SX1280Class SX1280LT;
uint32_t RXpacketCount = 0;
uint32_t errors;
uint8_t RXBUFFER[RXBUFFER_SIZE];
uint8_t RXPacketL;                               //stores length of packet received
int8_t  PacketRSSI;                              //stores RSSI of received packet
long lastRecv = 0;
int count = 0;


void setup()
{
  SerialUSB.begin(Serial_Monitor_Baud);

  SPI.begin();
  SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));

  // FLRC routine
  if (SX1280LT.begin(NSS, NRESET, RFBUSY, DIO1, -1, -1))
  {
    SerialUSB.println("Setup success!");
    delay(1000);
  }
  else
  {
    while (1)
    { 
      SerialUSB.println("No device responding");
      delay(1000);
    }
  }

  setup_FLRC();

  // CanSatKit routine
  radio.begin();
}


void loop()
{
  // FLRC routine
  SX1280LT.setRx(PERIOBASE_01_MS, 0);            //set no SX1280 RX timeout

  while (!digitalRead(DIO1));                    //wait for RxDone or timeout interrupt activating DIO1

  SX1280LT.readPacketReceptionFLRC();
  RXPacketL = SX1280LT.readRXPacketL();
  PacketRSSI = SX1280LT.readPacketRSSI();

  if (SX1280LT.readIrqStatus() == (IRQ_RX_DONE + IRQ_SYNCWORD_VALID))
  {
    uint8_t len;

    RXpacketCount++;
    len = SX1280LT.readPacketFLRC(RXBUFFER, RXBUFFER_SIZE);         //read the actual packet, maximum size specified in RXBUFFER_SIZE
    
    SerialUSB.println("2.4GHz @ " + String(PacketRSSI));
  }
  else
  {
     uint16_t IRQStatus;
     IRQStatus = SX1280LT.readIrqStatus();                    //get the IRQ status
     errors++;
     SX1280LT.printIrqStatus();
  }

  // CanSatKit routine
  char data[256];
  radio.receive(data);
  SerialUSB.print("433MHz @ ");
  SerialUSB.println(radio.get_rssi_last());
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
}
