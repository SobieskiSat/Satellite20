from machine import SPI
from pyb import Pin
from pyb import delay
import pyb

NSS = Pin('X9', Pin.OUT)
RFBUSY = Pin('X3', Pin.IN)
NRESET = Pin('X4', Pin.OUT)
LED1 = pyb.LED(4)
DIO1 = Pin('X2', Pin.IN)

spi = SPI(1)

def run():
	delay(2000)

	NSS.on()
	NRESET.off()
	spi.init(baudrate=4000000, polarity=0, phase=0, firstbit=SPI.MSB)

	print("begin()")
	print("SX1280Class constructor instantiated successfully")
	print("NSS ")
	print(NSS)
	print("NRESET ")
	print(NRESET)
	print("RFBUSY ")
	print(RFBUSY)
	print("DIO1 ")
	print(DIO1)
	
	resetDevice()

	Regdata1 = bytearray(1)
	Regdata2 = bytearray(1)
	Regdata1 = SXreadRegister()
	SXwriteRegister(0x10)
	Regdata2 = SXreadRegister()
	SXwriteRegister(0x00)

	result = (Regdata2 == 0x10)
	print("Result: " + str(result))

def resetDevice():
	print("resetDevice()")

	delay(20)
	NRESET.off()
	delay(50)
	NRESET.on()
	delay(20)

def SXreadRegister():
	print("readRegister()")

	buff = bytearray(1)

	SXcheckBusy()
	NSS.off()
	
	print("Write: 0x19")
	spi.write(bytes([0x19]))
	print("Write: 0x09")
	spi.write(bytes([0x09]))
	print("Write: 0x07")
	spi.write(bytes([0x07]))
	print("Write: 0xFF")
	spi.write(bytes([0xFF]))
	print("Write: 0xFF")
	spi.write_readinto(bytes([0xFF]), buff)
	print("Data recv: ")
	print(buff)
	
	NSS.on()
	SXcheckBusy()
	
	return buff

def SXwriteRegister(buffer):
	print("writeRegister()")

	SXcheckBusy()
	NSS.off()
	
	print("Write: 0x18")
	spi.write(bytes([0x18]))
	print("Write: 0x09")
	spi.write(bytes([0x09]))
	print("Write: 0x07")
	spi.write(bytes([0x07]))

	print("Write: " + str(buffer))
	spi.write(bytes([buffer]))
	
	NSS.on()
	SXcheckBusy()

def SXcheckBusy():
	print("checkBusy()")

	busy_timeout_cnt = 0x00

	while (RFBUSY.value()):
		delay(1)
		busy_timeout_cnt += 1

		if (busy_timeout_cnt > 5):
			print("ERROR - Busy Timeout!")
			#setStandby(0)
			#resetDevice() 
			#config()
		break