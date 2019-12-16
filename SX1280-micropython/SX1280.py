from pyb import sleep_ms as delay
from machine import Pin
from Constants import *

class SX1280Class:
	
	def __init__(self, spi, pinNSS, pinNRESET, pinRFBUSY, pinDIO1, pinDIO2, pinDIO3):
		
		self.spi = spi
		self.NSS = Pin(pinNSS, Pin.OUT)
		self.NRESET = Pin(pinNRESET, Pin.OUT)
		self.RFBUSY = Pin(pinRFBUSY, Pin.IN)
		if (pinDIO1 != -1) self.DIO1 = Pin(pinDIO1, Pin.IN)
		if (pinDIO2 != -1) self.DIO2 = Pin(pinDIO2, Pin.IN)
		if (pinDIO3 != -1) self.DIO3 = Pin(pinDIO3, Pin.IN)
		
	def begin(self):
		self.resetDevice()
		return self.checkDevice()

# SECTION: Device methods
	
	def resetDevice(self):
		delay(20)
		self.NRESET.off()
		delay(50)
		self.NRESET.on()
		delay(20)	
	def checkDevice(self):
		data1 = self.readRegister(0x0907)
		self.writeRegister(0x0907, data1 + 1)
		data2 = self.readRegister(0x0907)
		self.writeRegister(0x0907, data1)
		
		return (data2 == (data1 + 1))		
	def checkBusy(self):
		timeout_cnt = 0
		
		while self.RFBUSY.value() == 1:
			delay(1)
			timeout_cnt += 1
			
			if timeout_cnt > 5:
				print("checkBusy - TIMEOUT")
				self.setStandby(0) #BAD - recusive
				self.resetDevice() #BAD - recusive
				self.config()
				break	
	def config(self):
		self.resetDevice()
		
		 #getter setter and event \/ below \/
		
		self.setStandby(MODE_STDBY_RC)
		self.setRegulatorMode(self.savedReguatorMode)
		self.setPacketType(self.savedPacketType)
		self.setRfFrequency(self.savedFrequency, self.savedOffset)
		self.setModulationParams(self.savedModParam1, self.savedModParam2, self.savedModParam3)
		self.setPacketParams(self.savedPacketParam1, self.savedPacketParam2, self.savedPacketParam3, self.savedPacketParam4, self.savedPacketParam5, self.savedPacketParam6, self.savedPacketParam7)
		self.setDioIrqParams(self.savedIrqMask, self.savedDio1Mask, self.savedDio2Mask, self.savedDio3Mask)
	def wake(self):
		self.NSS.off()
		delay(1)
		self.NSS.on()
		delay(1)

# SECTION: Register communication

	def readRegisters(self, address, size):
		data = [None] * size
		
		addr_h = address >> 8
		addr_l = address & 0x00FF
		
		self.checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(RADIO_READ_REGISTER);
		self.spi.send_recv(addr_h);               #MSB
		self.spi.send_recv(addr_l);               #LSB
		self.spi.send_recv(0xFF);
		for i in range(size):
			data[i] = self.spi.send_recv(0xFF);
			
		self.NSS.on()
		self.checkBusy()
		
		return data
	def readRegister(self, address):
		return self.readRegisters(address, 1)[0]	
	def writeRegisters(self, address, data, size):
		addr_l = address & 0xff
		addr_l = address >> 8
		
		self.checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(RADIO_WRITE_REGISTER)
		self.spi.send_recv(addr_h)
		self.spi.send_recv(addr_l)
		
		for i in range(size):
			self.spi.send_recv(data[i])
		
		self.NSS.on()
		self.checkBusy()	
	def writeRegister(self, address, value):
		self.writeRegisters(address, [value], 1)	
	def writeCommand(self, Opcode, data, size):
		checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(Opcode)
		for i in range(size):
			self.spi.send_recv(data[i])
		
		self.NSS.on()
		if Opcode != RADIO_SET_SLEEP: checkBusy()
	def readCommand(self, Opcode, size):
		data = [None] * size
		
		self.checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(Opcode)
		self.spi.send_recv(0xFF)
		for i in range(size):
			data[i] = self.spi.send_recv(0xFF)
		
		self.NSS.on()
		self.checkBusy()
	
# SECTION: Getters and setters
	
	def setStandby(self, standbyconfig):
		self.checkBusy()		#BAD BAD VEEERY BAD WHEN RUN FROM checkBusy() -> LOOPI DOOPI
		self.NSS.off()
		
		self.spi.send_recv(0x80)
		self.spi.send_recv(standbyconfig)
		
		self.NSS.on()
		self.checkBusy()
	def setRegulatorMode(self, mode): #getter setter and event
		self.savedReguatorMode = mode
		self.writeCommand(RADIO_SET_REGULATORMODE, mode, 1)
	def setPacketType(self, packettype): #getter setter and event
		self.packettype = packettype
		self.writeCommand(RADIO_SET_PACKETTYPE, packettype, 1)
	def setRfFrequency(self, frequency, offset):
		self.savedFrequency = frequency;
		self.savedOffset = offset;
		
		frequency = frequency + offset;
		buffer = [None] * 3
		freqtemp = double(frequency) / double(FREQ_STEP) #unneccessary?
		# original conversions: buffer[0] = ( uint8_t )( ( freqtemp >> 16 ) & 0xFF );
		buffer[0] = (freqtemp >> 16) & 0xFF
		buffer[1] = (freqtemp >> 8)  & 0xFF
		buffer[2] = (freqtemp) 		 & 0xFF
		self.writeCommand(RADIO_SET_RFFREQUENCY, buffer, 3)
	def setModulationParams(self, modParam1, modParam2, modParam3):
		self.savedModParam1 = modParam1
		self.savedModParam2 = modParam2
		self.savedModParam3 = modParam3
		
		buffer = [None] * 3
		buffer[0] = modParam1
		buffer[1] = modParam2
		buffer[2] = modParam3
		
		self.writeCommand(RADIO_SET_MODULATIONPARAMS, buffer, 3)
	def setPacketParams(self, packetParam1, packetParam2, packetParam3, packetParam4, packetParam5, packetParam6, packetParam7):
		self.savedPacketParam1 = packetParam1
		self.savedPacketParam2 = packetParam2
		self.savedPacketParam3 = packetParam3
		self.savedPacketParam4 = packetParam4
		self.savedPacketParam5 = packetParam5
		self.savedPacketParam6 = packetParam6
		self.savedPacketParam7 = packetParam7
		
		buffer = [None] * 7
		buffer[0] = packetParam1
		buffer[1] = packetParam2
		buffer[2] = packetParam3
		buffer[3] = packetParam4
		buffer[4] = packetParam5
		buffer[5] = packetParam6
		buffer[6] = packetParam7
		self.writeCommand(RADIO_SET_PACKETPARAMS, buffer, 7)	
	def setDioIrqParams(self, irqMask, dio1Mask, dio2Mask, dio3Mask):
		self.savedIrqMask = irqMask
		self.savedDio1Mask = dio1Mask
		self.savedDio2Mask = dio2Mask
		self.savedDio3Mask = dio3Mask

		buffer = [None] * 8
		# origianl conversion: buffer[0] = (uint8_t) (irqMask >> 8);
		buffer[0] = (irqMask >> 8)
		buffer[1] = (irqMask & 0xFF)
		buffer[2] = (dio1Mask >> 8)
		buffer[3] = (dio1Mask & 0xFF)
		buffer[4] = (dio2Mask >> 8)
		buffer[5] = (dio2Mask & 0xFF)
		buffer[6] = (dio3Mask >> 8)
		buffer[7] = (dio3Mask & 0xFF)
		self.writeCommand(RADIO_SET_DIOIRQPARAMS, buffer, 8)
	def setSleep(self, sleepconfig):
		self.setStandby(MODE_STDBY_RC)
		
		self.checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(RADIO_SET_SLEEP)
		self.spi.send_recv(sleepconfig)
		
		self.NSS.on()
		delay(1)
	def setBufferBaseAddress(self, txBaseAddress, rxBaseAddress):
		buffer = [None] * 2
		buffer[0] = txBaseAddress
		buffer[1] = rxBaseAddress
		self.writeCommand(RADIO_SET_BUFFERBASEADDRESS, buffer, 2)
	def setTxParams(self, TXpower, RampTime):
		self.savedTXPower = TXpower
		
		buffer = [None] * 2
		buffer[0] = (TXpower + 18)
		buffer[1] = RampTime #uint8_t
		self.writeCommand(RADIO_SET_TXPARAMS, buffer, 2)
	
# SECTION: FLRC methods

	def sendPacketFLRC(self, data, size, txtimeout, TXpower, DIO):
		if size not in range(6, 128): return False
		
		self.setStandby(MODE_STDBY_RC)
		self.setBufferBaseAddress(0, 0)
		
		self.checkBusy()
		self.NSS.off()
		
		self.spi.send_recv(RADIO_WRITE_BUFFER)
		self.spi.send_recv(0)
		for i in range(size);
			seld.spi.send_recv(data[i])
			
		self.NSS.on()
		self.checkBusy()
		
		self.TXPacketL = size
		self.setPacketParams(self.savedPacketParam1, self.savedPacketParam2, self.savedPacketParam3, self.savedPacketParam4, size, self.savedPacketParam6, self.savedPacketParam7)
		self.setTxParams(TXpower, RADIO_RAMP_02_US)
		
		self.setTx(PERIOBASE_01_MS, txtimeout)
		while(not DIO.value()) # wait for finished sending... not good
		return not (self.readIrqStatus() & IRQ_RX_TX_TIMEOUT)
	def readPacketFLRC(self, size):
		buffer = readCommand(RADIO_GET_RXBUFFERSTATUS, 2)
		self.RXPacketL = buffer[0] # Reception>????
		RXStart = buffer[1]
		
		if self.RXPacketL > size:
			self.RXPacketL = size
		
		data = [None] * RXPacketL
		RXEnd = RXStart + self.RXPacketL
		
		self.NSS.off()
		
		self.spi.send_recv(RADIO_READ_BUFFER)
		self.spi.send_recv(RXStart)
		self.spi.send_recv(0xFF)
		for i in range(RXStart, RXEnd):
			data[i] = self.spi.send_recv(0)
			
		self.NSS.on()
		return data
	
	def setSyncWord1(self, syncword):
		self.writeRegister(REG_FLRCSYNCWORD1_BASEADDR,     (syncword >> 24 ) & 0x000000FF)
		self.writeRegister(REG_FLRCSYNCWORD1_BASEADDR + 1, (syncword >> 16 ) & 0x000000FF)
		self.writeRegister(REG_FLRCSYNCWORD1_BASEADDR + 2, (syncword >> 8 )  & 0x000000FF)
		self.writeRegister(REG_FLRCSYNCWORD1_BASEADDR + 3,  syncword         & 0x000000FF)
	def setTX(self, periodBase, periodBaseCount):
		self.clearIrqStatus(IRQ_RADIO_ALL)
		
		buffer = [None] * 3
		buffer[0] = periodBase
		buffer[1] = (periodBaseCount >> 8) * 0x00FF #uint8_t
		buffer[2] = (periodBaseCount & 0x00FF) #uint8_t
		self.writeCommand(RADIO_SET_TX, buffer, 3)
	def setRX(self, periodBase, periodBaseCount):
		self.clearIrqStatus(IRQ_RADIO_ALL);

		buffer = [None] * 3		  
		buffer[0] = periodBase
		buffer[1] = (periodBaseCount >> 8) & 0x00FF #uint8_t
		buffer[2] = (periodBaseCount & 0x00FF) #uint8_t
		self.writeCommand(RADIO_SET_RX, buffer, 3)
	def readPacketReceptionFLRC(self):
		status = self.readCommand(RADIO_GET_PACKETSTATUS, 5)
		self.PacketRSSI = -status[0] / 2
	def readRXPacketL():
		buffer = self.readCommand(RADIO_GET_RXBUFFERSTATUS, 2)
		self.RXPacketL = buffer[0]
		return self.RXPacketL
		
# SECTION: IRQ
	
	def readIrqStatus(self):
		buffer = readCommand(RADIO_GET_IRQSTATUS, 2)
		return ((buffer[0] << 8) + buffer[1])
	def clearIrqStatus(self, irqMask):
		buffer = [None] * 2
		buffer[0] = (irqMask >> 8) #uint8_t
		buffer[1] = (irqMask & 0xFF) #uint8_t
		self.writeCommand(RADIO_CLR_IRQSTATUS, buffer, 2)
	