#define Serial_Monitor_Baud 115200

#include <CanSatKit-fordual.h>

#include <SPI.h>
#include "Settings.h"
#include <SX1280_lite.h>

// CanSatKit variables
using namespace CanSatKit;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);
Frame frame;

// FLRC variables
SX1280Class SX1280LT;
uint8_t TXPacketL;

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
  uint8_t buffersize;
  uint8_t buff[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  //buff[126] = '#';                                //overwrite null character at end of buffer so we can see it in RX
  
  if (sizeof(buff) > TXBUFFER_SIZE)
  {
    buffersize = TXBUFFER_SIZE;
  }
  else
  {
    buffersize = sizeof(buff);
  }
  
  TXPacketL = buffersize;

  if (SX1280LT.sendPacketFLRC(buff, buffersize, 1000, PowerTX, DIO1))
  {
    SerialUSB.println("2.4GHz send");
  }

  delay(200);

  // CanSatKit routine
  if (radio.tx_fifo_empty())
  {
    frame.print("Hello CanSat!");
    radio.transmit(frame);
    frame.clear();
    SerialUSB.println("433MHz send");
  }

  delay(200);
}


void packet_is_Error()
{
  uint16_t IRQStatus;
  IRQStatus = SX1280LT.readIrqStatus();                    //get the IRQ status
  SX1280LT.printIrqStatus();
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
  SX1280LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);              //set for IRQ on TX done and timeout on DIO1
  SX1280LT.setSyncWord1(Sample_Syncword);
}
