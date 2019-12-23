#ifndef SX1280_lite_h
#define SX1280_lite_h

#include "Arduino.h"
#include <SX1280_lite_Includes.h>

class SX1280Class  {
  public:
    SX1280Class();

    bool begin(int8_t pinNSS, int8_t pinNRESET, int8_t pinRFBUSY, int8_t pinDIO1, int8_t pinDIO2, int8_t pinDIO3);
    void pinInit(int8_t _NSS, int8_t _NRESET, int8_t _RFBUSY, int8_t _DIO1, int8_t _DIO2, int8_t _DIO3);
    void resetDevice();
    bool checkDevice();
    void checkBusy();
    void wake();
    bool config();
	
    void setSleep(uint8_t sleepconfig); 
    void setStandby(uint8_t StdbyConfig);
    void setRegulatorMode(uint8_t mode);
	void setRfFrequency( uint32_t frequency, int32_t offset );
    void setPacketParams(uint8_t packetParam1, uint8_t  packetParam2, uint8_t packetParam3, uint8_t packetParam4, uint8_t packetParam5, uint8_t packetParam6, uint8_t packetParam7);
    void setModulationParams(uint8_t modParam1, uint8_t modParam2, uint8_t  modParam3);
    

    void writeCommand(uint8_t Opcode, uint8_t *buffer, uint16_t size );
    void readCommand( uint8_t Opcode, uint8_t *buffer, uint16_t size );  
    void writeRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    void writeRegister( uint16_t address, uint8_t value );
    void readRegisters( uint16_t address, uint8_t *buffer, uint16_t size );
    uint8_t readRegister( uint16_t address );
    void printSavedModulationParams();

    uint32_t getFreqInt();
    int32_t getFrequencyErrorRegValue();
    int32_t getFrequencyErrorHz();
    void setTxParams(int8_t TXpower, uint8_t RampTime);
    void setTx(uint8_t _periodBase, uint16_t _periodBaseCount);
    bool readTXDone();
    void setRx(uint8_t _periodBase, uint16_t _periodBaseCount);
    void setLowPowerRX();
    void setHighSensitivity();
    bool readRXDone();
    void readRXBufferStatus();
    uint8_t readRXPacketL();
    uint8_t readPacketRSSI();
    uint8_t readPacketSNR();

    void setBufferBaseAddress(uint8_t txBaseAddress, uint8_t rxBaseAddress);
    void setPacketType(uint8_t PacketType);

    void clearIrqStatus( uint16_t irq );
    uint16_t readIrqStatus();
    void setDioIrqParams(uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask );
    void printIrqStatus();

    bool readPacketCRCError();
    bool readPacketHeaderValid();
    bool readPacketHeaderError();

    uint32_t returnBandwidth(uint8_t data);
    uint8_t returnSF(uint8_t data);
    uint8_t readsavedPower();

    bool packetOK(uint16_t mask);

    uint8_t readsavedModParam1();
    uint8_t readsavedModParam2();
    uint8_t readsavedModParam3();
	
	uint32_t getLoRaBandwidth();
    
    //*******************************************************************************
    //FLRC specific Routines
    //*******************************************************************************

    bool sendPacketFLRC(uint8_t *txbuffer, uint8_t size, int32_t txtimeoutmS, int8_t txpower, uint8_t _DIO);
    bool sendPacketAddressedFLRC(uint8_t *txbuffer, uint8_t size, char txPacketType, char txDestination, char txSource, int32_t TXTimeoutmS, int8_t TXPower, uint8_t DIO);
    uint8_t readPacketAddressedFLRC(uint8_t *rxbuffer, uint8_t size);
    uint8_t readPacketFLRC(uint8_t *rxbuffer, uint8_t _size);
    void readPacketReceptionFLRC();
    uint8_t readPacketStatus3FLRC();
    uint8_t readPacketStatus2FLRC();
    void setSyncWord1( uint32_t syncword);

  private:

    int8_t _NSS, _NRESET, _RFBUSY, _DIO1, _DIO2, _DIO3;
    int8_t _RXEN, _TXEN;
    uint8_t _RXPacketL;             //length of packet received
    uint8_t _RXPacketType;          //type number of received packet
    uint8_t _RXDestination;         //destination address of received packet
    uint8_t _RXSource;              //source address of received packet
    int8_t  _PacketRSSI;            //RSSI of received packet
    int8_t  _PacketSNR;             //signal to noise ratio of received packet
    int8_t  _TXPacketL;             //transmitted packet length
    uint8_t _RXcount;               //used to keep track of the bytes read from SX1280 buffer during readFloat() etc
    uint8_t _TXcount;               //used to keep track of the bytes written to SX1280 buffer during writeFloat() etc
    bool _rxtxpinmode = false;      //set to true if RX and TX pin mode is used.

    //config variables are 36 bytes, allows for device to be reset and reconfigured via config();
    uint8_t  savedRegulatorMode;
    uint8_t  savedPacketType;
    uint32_t savedFrequency, savedOffset;
    uint8_t  savedModParam1, savedModParam2, savedModParam3;
    uint8_t  savedPacketParam1, savedPacketParam2, savedPacketParam3, savedPacketParam4, savedPacketParam5, savedPacketParam6, savedPacketParam7;
    uint16_t savedIrqMask, savedDio1Mask, savedDio2Mask, savedDio3Mask;
    int8_t   savedTXPower;

};
#endif
