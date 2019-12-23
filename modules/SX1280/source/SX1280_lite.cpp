//cansat@kopernik.org.pl

#include <SX1280_lite.h>
#include <SPI.h>

SX1280Class::SX1280Class()
{
  //Anything you need when instantiating your object goes here
}


bool SX1280Class::begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3)
{
  pinInit(pinNSS, pinNRESET, pinRFBUSY, pinDIO1, pinDIO2, pinDIO3);

  _NSS = pinNSS;
  _NRESET = pinNRESET;
  _RFBUSY = pinRFBUSY;
  _DIO1 = pinDIO1;
  _DIO2 = pinDIO2;
  _DIO3 = pinDIO3;

  resetDevice();
  if (checkDevice())
  {
    return true;
  }

  return false;
}


void SX1280Class::pinInit(int8_t _NSS, int8_t _NRESET, int8_t _RFBUSY, int8_t _DIO1, int8_t _DIO2, int8_t _DIO3)
{
  pinMode(_NSS, OUTPUT);
  digitalWrite(_NSS, HIGH);
  pinMode(_NRESET, OUTPUT);
  digitalWrite(_NRESET, LOW);
  pinMode(_RFBUSY, INPUT);
  pinMode( _DIO1, INPUT);
}


void SX1280Class::resetDevice()
{
  delay(20);
  digitalWrite(_NRESET, LOW);
  delay(50);
  digitalWrite(_NRESET, HIGH);
  delay(20);
}

bool SX1280Class::checkDevice()
{
  uint8_t Regdata1, Regdata2;
  Regdata1 = readRegister(0x0907);               //mid byte of frequency setting
  writeRegister(0x0907, (Regdata1 + 1));
  Regdata2 = readRegister(0x0907);               //read changed value back
  writeRegister(0x0907, Regdata1);               //restore register to original value

  return (Regdata2 == (Regdata1 + 1));
}


void SX1280Class::checkBusy()
{
  uint8_t busy_timeout_cnt;
  busy_timeout_cnt = 0;
  
  while (digitalRead(_RFBUSY))
  {
    busy_timeout_cnt++;

    if (busy_timeout_cnt > 5000)
    {
      SerialUSB.println(F("ERROR - Busy Timeout!"));
      setStandby(0);
      resetDevice();                        
      config();
      break;
    }
  }
  
}

void SX1280Class::wake()
{
digitalWrite(_NSS, LOW);
delay(1);
digitalWrite(_NSS, HIGH);
delay(1);
}

bool SX1280Class::config()
{
  resetDevice();
  setStandby(MODE_STDBY_RC);
  setRegulatorMode(savedRegulatorMode);
  setPacketType(savedPacketType);
  setRfFrequency(savedFrequency, savedOffset);
  setModulationParams(savedModParam1, savedModParam2, savedModParam3);
  setPacketParams(savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, savedPacketParam5, savedPacketParam6, savedPacketParam7);
  setDioIrqParams(savedIrqMask, savedDio1Mask, savedDio2Mask, savedDio3Mask);       //set for IRQ on RX done on DIO1
  return true;
}

void SX1280Class::setSleep(uint8_t sleepconfig)
{
  setStandby(MODE_STDBY_RC);
  checkBusy();
  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_SET_SLEEP);
  SPI.transfer(sleepconfig);
  digitalWrite(_NSS, HIGH);
  delay(1);
}


void SX1280Class::setStandby(uint8_t standbyconfig)
{
  uint8_t Opcode = 0x80;

  checkBusy();
  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);
  SPI.transfer(standbyconfig);
  digitalWrite(_NSS, HIGH);
  checkBusy();
}


void SX1280Class::setPacketType(uint8_t packettype )
{
  savedPacketType = packettype;

  writeCommand(RADIO_SET_PACKETTYPE, &packettype, 1);
}

void SX1280Class::setRegulatorMode(uint8_t mode)
{
  savedRegulatorMode = mode;

  writeCommand(RADIO_SET_REGULATORMODE, &mode, 1);
}


void SX1280Class::setRfFrequency(uint32_t frequency, int32_t offset)
{
  savedFrequency = frequency;
  savedOffset = offset;

  frequency = frequency + offset;
  uint8_t buffer[3];
  uint32_t freqtemp = 0;
  freqtemp = ( uint32_t )( (double) frequency / (double)FREQ_STEP);
  buffer[0] = ( uint8_t )( ( freqtemp >> 16 ) & 0xFF );
  buffer[1] = ( uint8_t )( ( freqtemp >> 8 ) & 0xFF );
  buffer[2] = ( uint8_t )( freqtemp & 0xFF );
  writeCommand(RADIO_SET_RFFREQUENCY, buffer, 3);
}


void SX1280Class::setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5, uint8_t packetParam6, uint8_t packetParam7)
{
  savedPacketParam1 = packetParam1;
  savedPacketParam2 = packetParam2;
  savedPacketParam3 = packetParam3;
  savedPacketParam4 = packetParam4;
  savedPacketParam5 = packetParam5;
  savedPacketParam6 = packetParam6;
  savedPacketParam7 = packetParam7;

  uint8_t buffer[7];
  buffer[0] = packetParam1;
  buffer[1] = packetParam2;
  buffer[2] = packetParam3;
  buffer[3] = packetParam4;
  buffer[4] = packetParam5;
  buffer[5] = packetParam6;
  buffer[6] = packetParam7;
  writeCommand(RADIO_SET_PACKETPARAMS, buffer, 7);

}


void SX1280Class::setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3)
{
  uint8_t buffer[3];

  savedModParam1 = modParam1;
  savedModParam2 = modParam2;
  savedModParam3 = modParam3;

  buffer[0] = modParam1;
  buffer[1] = modParam2;
  buffer[2] = modParam3;

  writeCommand(RADIO_SET_MODULATIONPARAMS, buffer, 3);
}


void SX1280Class::writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size)
{
  uint8_t index;
  checkBusy();
  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);

  for (index = 0; index < size; index++)
  {
    SPI.transfer(buffer[index]);
  }
  digitalWrite(_NSS, HIGH);

  if (Opcode != RADIO_SET_SLEEP)
  {
    checkBusy();
  }
}

void SX1280Class::setSyncWord1(uint32_t syncword)
{
  // For FLRC packet type, the SyncWord is one byte shorter and
  // the base address is shifted by one byte
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR, ( syncword >> 24 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 1, ( syncword >> 16 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 2, ( syncword >> 8 ) & 0x000000FF );
  writeRegister( REG_FLRCSYNCWORD1_BASEADDR + 3, syncword & 0x000000FF );
}

void SX1280Class::printIrqStatus()
{
  uint16_t _IrqStatus;
  _IrqStatus = readIrqStatus();

  //0x0001
  if (_IrqStatus & IRQ_TX_DONE)
  {
    SerialUSB.print(F(",IRQ_TX_DONE"));
  }

  //0x0002
  if (_IrqStatus & IRQ_RX_DONE)
  {
    SerialUSB.print(F(",IRQ_RX_DONE"));
  }

  //0x0004
  if (_IrqStatus & IRQ_SYNCWORD_VALID)
  {
    SerialUSB.print(F(",IRQ_SYNCWORD_VALID"));
  }

  //0x0008
  if (_IrqStatus & IRQ_SYNCWORD_ERROR)
  {
    SerialUSB.print(F(",IRQ_SYNCWORD_ERROR"));
  }

  //0x0010
  if (_IrqStatus & IRQ_HEADER_VALID)
  {
    SerialUSB.print(F(",IRQ_HEADER_VALID"));
  }

  //0x0020
  if (_IrqStatus & IRQ_HEADER_ERROR)
  {
    SerialUSB.print(F(",IRQ_HEADER_ERROR"));
  }

  //0x0040
  if (_IrqStatus & IRQ_CRC_ERROR)
  {
    SerialUSB.print(F(",IRQ_CRC_ERROR"));
  }

  //0x0080
  if (_IrqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE)
  {
    SerialUSB.print(F(",IRQ_RANGING_SLAVE_RESPONSE_DONE"));
  }

  //0x0100
  if (_IrqStatus & IRQ_RANGING_SLAVE_REQUEST_DISCARDED)
  {
    SerialUSB.print(",IRQ_RANGING_SLAVE_REQUEST_DISCARDED");
  }

  //0x0200
  if (_IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID)
  {
    SerialUSB.print(F(",IRQ_RANGING_MASTER_RESULT_VALID"));
  }

  //0x0400
  if (_IrqStatus & IRQ_RANGING_MASTER_RESULT_TIMEOUT)
  {
    SerialUSB.print(F(",IRQ_RANGING_MASTER_RESULT_TIMEOUT"));
  }

  //0x0800
  if (_IrqStatus & IRQ_RANGING_SLAVE_REQUEST_VALID)
  {
    SerialUSB.print(F(",IRQ_RANGING_SLAVE_REQUEST_VALID"));
  }

  //0x1000
  if (_IrqStatus & IRQ_CAD_DONE)
  {
    SerialUSB.print(F(",IRQ_CAD_DONE"));
  }

  //0x2000
  if (_IrqStatus & IRQ_CAD_ACTIVITY_DETECTED)
  {
    SerialUSB.print(F(",IRQ_CAD_ACTIVITY_DETECTED"));
  }

  //0x4000
  if (_IrqStatus & IRQ_RX_TX_TIMEOUT)
  {
    SerialUSB.print(F(",IRQ_RX_TX_TIMEOUT"));
  }

  //0x8000
  if (_IrqStatus & IRQ_PREAMBLE_DETECTED)
  {
    SerialUSB.print(F(",IRQ_PREAMBLE_DETECTED"));
  }
}


bool SX1280Class::readPacketCRCError()
{
  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_CRC_ERROR)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX1280Class::readPacketHeaderValid()
{
  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_HEADER_VALID)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX1280Class::readPacketHeaderError()
{
  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_HEADER_ERROR)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX1280Class::readRXDone()
{
  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_RX_DONE)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SX1280Class::readTXDone()
{
  uint16_t IRQreg;
  IRQreg = readIrqStatus();

  if (IRQreg & IRQ_TX_DONE)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void SX1280Class::readRXBufferStatus()
{
  uint8_t buffer[2];
  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
}


uint8_t SX1280Class::readPacketFLRC(uint8_t *RXbuffer, uint8_t size)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readPacket()"));
#endif

  uint8_t index, regdata, RXStart, RXEnd;
  uint8_t buffer[2];

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
  RXStart = buffer[1];
  
  if (_RXPacketL > size)                 //check passed buffer is big enough for packet
  {
  _RXPacketL = size;                     //truncate packet if not enough space
  }

  RXEnd = RXStart + _RXPacketL;          //calculate rxend

  digitalWrite(_NSS, LOW);               //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXStart);
  SPI.transfer(0xFF);

  for (index = RXStart; index < RXEnd; index++)
  {
    regdata = SPI.transfer(0);
    RXbuffer[index] = regdata;
  }

  digitalWrite(_NSS, HIGH);
  return _RXPacketL;                     //so we can check for packet having enough buffer space
}

bool SX1280Class::sendPacketFLRC(uint8_t *TXbuffer, uint8_t size, int32_t txtimeoutmS, int8_t TXpower, uint8_t _DIO)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("sendPacketFLRC()"));
#endif
  uint8_t index;
  uint8_t bufferdata;
  
  if (size == 0)
  {
   return false;
  }
  
  if ((size > 127) || (size < 6 ))
  {
    return false;
  }
  
  setStandby(MODE_STDBY_RC);
  setBufferBaseAddress(0, 0);
  checkBusy();
  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_BUFFER);
  SPI.transfer(0);

  for (index = 0; index < size; index++)
  {
    bufferdata = TXbuffer[index];
    SPI.transfer(bufferdata);

  }

  digitalWrite(_NSS, HIGH);
  checkBusy();
  _TXPacketL = size;
  setPacketParams(savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, size, savedPacketParam6, savedPacketParam7);
  setTxParams(TXpower, RADIO_RAMP_02_US);
  setTx(PERIOBASE_01_MS, txtimeoutmS);                            //this starts the TX

  while (!digitalRead(_DIO));                                     //Wait for DIO to go high

  if (readIrqStatus() & IRQ_RX_TX_TIMEOUT )                       //check for timeout
  {
    return false;
  }
  else
  {
    return true;
  }
}


int32_t SX1280Class::getFrequencyErrorRegValue()
{
  int32_t FrequencyError;
  uint32_t regmsb, regmid, reglsb, allreg;
  
  setStandby(MODE_STDBY_XOSC);
  
  regmsb = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB );
  regmsb = regmsb & 0x0F;       //clear bit 20 which is always set
  
  regmid = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB + 1 );
  
  reglsb = readRegister( REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB + 2 );
  setStandby(MODE_STDBY_RC);
    
  allreg = (uint32_t) ( regmsb << 16 ) | ( regmid << 8 ) | reglsb;

  if (allreg & 0x80000)
  {
  FrequencyError = (0xFFFFF - allreg) * -1;
  }
  else
  {
  FrequencyError = allreg; 
  }

  return FrequencyError;
}


void SX1280Class::setTxParams(int8_t TXpower, uint8_t RampTime)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setTxParams()"));
#endif

  uint8_t buffer[2];

  savedTXPower = TXpower;

  //power register is set to 0 to 31 which is -18dBm to +12dBm
  //buffer[0] = (TXpower + 18);
  buffer[0] = 0x1F;
  buffer[1] = (uint8_t)RampTime;
  writeCommand(RADIO_SET_TXPARAMS, buffer, 2);
}


void SX1280Class::setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setBufferBaseAddress()"));
#endif

  uint8_t buffer[2];

  buffer[0] = txBaseAddress;
  buffer[1] = rxBaseAddress;
  writeCommand(RADIO_SET_BUFFERBASEADDRESS, buffer, 2);
}


void SX1280Class::setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setDioIrqParams()"));
#endif

  savedIrqMask = irqMask;
  savedDio1Mask = dio1Mask;
  savedDio2Mask = dio2Mask;
  savedDio3Mask = dio3Mask;

  uint8_t buffer[8];

  buffer[0] = (uint8_t) (irqMask >> 8);
  buffer[1] = (uint8_t) (irqMask & 0xFF);
  buffer[2] = (uint8_t) (dio1Mask >> 8);
  buffer[3] = (uint8_t) (dio1Mask & 0xFF);
  buffer[4] = (uint8_t) (dio2Mask >> 8);
  buffer[5] = (uint8_t) (dio2Mask & 0xFF);
  buffer[6] = (uint8_t) (dio3Mask >> 8);
  buffer[7] = (uint8_t) (dio3Mask & 0xFF);
  writeCommand(RADIO_SET_DIOIRQPARAMS, buffer, 8);
}


void SX1280Class::setHighSensitivity()
{
  //set bits 7,6 of REG_LNA_REGIME
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setHighSensitivity()"));
#endif

  writeRegister(REG_LNA_REGIME, (readRegister(REG_LNA_REGIME) | 0xC0));
}


void SX1280Class::setLowPowerRX()
{
  //clear bits 7,6 of REG_LNA_REGIME
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setLowPowerRX()"));
#endif

  writeRegister(REG_LNA_REGIME, (readRegister(REG_LNA_REGIME) & 0x3F));
}


void SX1280Class::setRx(uint8_t _periodBase, uint16_t _periodBaseCount)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("setRx()"));
#endif

  
  uint8_t buffer[3];
  
  clearIrqStatus(IRQ_RADIO_ALL);                             //clear all interrupt flags
  buffer[0] = _periodBase;
  buffer[1] = ( uint8_t ) ((_periodBaseCount >> 8 ) & 0x00FF);
  buffer[2] = ( uint8_t ) (_periodBaseCount & 0x00FF);
  writeCommand(RADIO_SET_RX, buffer, 3);
}


uint16_t SX1280Class::readIrqStatus()
{
#ifdef SX1280DEBUG
  //SerialUSB.print(F("readIrqStatus()"));
#endif

  uint16_t temp;
  uint8_t buffer[2];

  readCommand(RADIO_GET_IRQSTATUS, buffer, 2);
  temp = ((buffer[0] << 8) + buffer[1]);
  return temp;
}


bool SX1280Class::packetOK(uint16_t mask)
{
#ifdef SX1280DEBUG
  //SerialUSB.print(F("packetOK()"));
#endif

  uint16_t temp;
  temp = readIrqStatus();

  if (temp == mask)
  {
    return true;
  }
  else
  {
    return false;
  }
}


void SX1280Class::readRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readRegisters()"));
#endif

  uint16_t index;
  uint8_t addr_l, addr_h;

  addr_h = address >> 8;
  addr_l = address & 0x00FF;
  checkBusy();

#ifdef SX1280DEBUG
  //SerialUSB.println(F("ReadRegisters "));
  //SerialUSB.print(addr_h, HEX);
  //SerialUSB.print(addr_l, HEX);
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_READ_REGISTER);
  SPI.transfer(addr_h);               //MSB
  SPI.transfer(addr_l);               //LSB
  SPI.transfer(0xFF);
  for (index = 0; index < size; index++)
  {
#ifdef SX1280DEBUG
    //SerialUSB.println(F(" "));
    //SerialUSB.print(*(buffer + index));
#endif
    *(buffer + index) = SPI.transfer(0xFF);
  }
#ifdef SX1280DEBUG
  //SerialUSB.println();
#endif
  digitalWrite(_NSS, HIGH);
  checkBusy();
}


uint8_t SX1280Class::readRegister(uint16_t address)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readRegister()"));
#endif

  uint8_t data;

  readRegisters(address, &data, 1);
  return data;
}


void SX1280Class::clearIrqStatus(uint16_t irqMask)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("clearIrqStatus()"));
#endif

  uint8_t buffer[2];

  buffer[0] = (uint8_t) (irqMask >> 8);
  buffer[1] = (uint8_t) (irqMask & 0xFF);
  writeCommand(RADIO_CLR_IRQSTATUS, buffer, 2);
}


void SX1280Class::readCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readCommand()"));
#endif

  uint8_t i;
  checkBusy();

  digitalWrite(_NSS, LOW);
  SPI.transfer(Opcode);
  SPI.transfer(0xFF);

  for ( i = 0; i < size; i++ )
  {
    *(buffer + i) = SPI.transfer(0xFF);
  }
  digitalWrite(_NSS, HIGH);

  checkBusy();
}


uint8_t SX1280Class::readPacketStatus3FLRC()
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readPacketStatus3FLRC()"));
#endif

  uint8_t status[5];

  readCommand(RADIO_GET_PACKETSTATUS, status, 5);
  return status[3];
}


uint8_t SX1280Class::readPacketStatus2FLRC()
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readPacketStatus2FLRC()"));
#endif

  uint8_t status[5];

  readCommand(RADIO_GET_PACKETSTATUS, status, 5);
  return status[2];
}


void SX1280Class::readPacketReceptionFLRC()
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readPacketReceptionFLRC()"));
#endif

  uint8_t status[5];

  readCommand(RADIO_GET_PACKETSTATUS, status, 5);
  _PacketRSSI = -status[0] / 2;
}


void SX1280Class::writeRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("writeRegisters()"));
#endif
  uint8_t addr_l, addr_h;
  uint8_t i;

  addr_l = address & 0xff;
  addr_h = address >> 8;
  checkBusy();

#ifdef SX1280DEBUG
  //SerialUSB.println(F("WriteRegisters "));
  //SerialUSB.print(addr_h, HEX);
  //SerialUSB.print(addr_l, HEX);
#endif

  digitalWrite(_NSS, LOW);
  SPI.transfer(RADIO_WRITE_REGISTER);
  SPI.transfer(addr_h);   //MSB
  SPI.transfer(addr_l);   //LSB

  for (i = 0; i < size; i++)
  {
#ifdef SX1280DEBUG
    //SerialUSB.println(F(" "));
    //SerialUSB.print(buffer[i]);
#endif
    SPI.transfer(buffer[i]);
  }

#ifdef SX1280DEBUG
  //SerialUSB.println();
#endif
  digitalWrite(_NSS, HIGH);
  checkBusy();
}


void SX1280Class::writeRegister(uint16_t address, uint8_t value)
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("writeRegister()"));
#endif

  writeRegisters( address, &value, 1 );
}



void SX1280Class::setTx(uint8_t _periodBase, uint16_t _periodBaseCount)
{

#ifdef SX1280DEBUG
  //SerialUSB.println(F("setTx()"));
#endif
  
  uint8_t buffer[3];
  
  clearIrqStatus(IRQ_RADIO_ALL);                             //clear all interrupt flags 
  buffer[0] = _periodBase;
  buffer[1] = ( uint8_t )( ( _periodBaseCount >> 8 ) & 0x00FF );
  buffer[2] = ( uint8_t )( _periodBaseCount & 0x00FF );
  writeCommand(RADIO_SET_TX, buffer, 3 );
}


uint8_t SX1280Class::readRXPacketL()
{
#ifdef SX1280DEBUG
  //SerialUSB.println(F("readRXPacketL()"));
#endif

  uint8_t buffer[2];

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];
  return _RXPacketL;
}


uint8_t SX1280Class::readPacketRSSI()
{
  return _PacketRSSI;
}


uint8_t SX1280Class::readPacketSNR()
{
  return _PacketSNR;
}




void SX1280Class::printSavedModulationParams()
{
  //SerialUSB.print(F("SavedModulationParams "));
  //SerialUSB.print(savedModParam1, HEX);
  //SerialUSB.print(F(" "));
  //SerialUSB.print(savedModParam2, HEX);
  //SerialUSB.print(F(" "));
  //SerialUSB.print(savedModParam3, HEX);
}


uint8_t SX1280Class::readsavedModParam1()
{
  return savedModParam1;
}


uint8_t SX1280Class::readsavedModParam2()
{
  return savedModParam2;
}


uint8_t SX1280Class::readsavedModParam3()
{
  return savedModParam3;
}


uint8_t SX1280Class::readsavedPower()
{
  return savedTXPower;
}


uint32_t SX1280Class::getFreqInt()
{
  //get the current set device frequency, return as long integer
  uint8_t Msb, Mid, Lsb;
  uint32_t uinttemp;
  float floattemp;
  Msb = readRegister(REG_RFFrequency23_16);
  Mid = readRegister(REG_RFFrequency15_8);
  Lsb = readRegister(REG_RFFrequency7_0);
  floattemp = ((Msb * 0x10000ul) + (Mid * 0x100ul) + Lsb);
  floattemp = ((floattemp * FREQ_STEP) / 1000000ul);
  uinttemp = (uint32_t)(floattemp * 1000000);
  return uinttemp;
}


uint32_t SX1280Class::returnBandwidth(uint8_t data)
{
  switch (data)
  {
    case LORA_BW_0200:
      return 203125;

    case LORA_BW_0400:
      return 406250;

    case LORA_BW_0800:
      return 812500;

    case LORA_BW_1600:
      return 1625000;

    default:
      break;
  }

  return 0x0;                      //so that a bandwidth not set can be identified
}


uint8_t SX1280Class::returnSF(uint8_t data)
{
  return (data >> 4);
}

uint32_t SX1280Class::getLoRaBandwidth()
{
  uint32_t bwValue = 0;

  switch (savedModParam2)
  {
    case LORA_BW_0200:
      bwValue = 203125;
      break;
    case LORA_BW_0400:
      bwValue = 406250;
      break;
    case LORA_BW_0800:
      bwValue = 812500;
      break;
    case LORA_BW_1600:
      bwValue = 1625000;
      break;
    default:
      bwValue = 0;
  }
  return bwValue;
}