#define Serial_Monitor_Baud 115200

#include <SPI.h>
#include "Settings.h"
#include <SX1280LT.h>

SX1280Class SX1280LT;

boolean SendOK;
int8_t TestPower;
uint8_t TXPacketL;

long lastSend = 0;
int count = 0;
bool state = false;

void setup()
{
  SerialUSB.begin(Serial_Monitor_Baud);
  pinMode(LED_BUILTIN, OUTPUT);

  SPI.begin();
  SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));

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
}

void loop()
{
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
    if (count >= 1200)
    {
      digitalWrite(LED_BUILTIN, state);
      state = !state;
      long spe = TXPacketL * 8 * 1000 * 1000;
      long del = micros() - lastSend;
      float rate = spe / del;
      rate /= 1000 * 1000;
      SerialUSB.println(String(rate, 4) + " Mb/s, send@ " + String(PowerTX));
      count = 0;
    }
    lastSend = micros();
    count++;
  }
  else { packet_is_Error(); }
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
