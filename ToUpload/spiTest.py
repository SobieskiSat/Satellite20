from machine import SPI
from pyb import Pin
from pyb import delay
import pyb

NSS = Pin('X9', Pin.OUT)
RFBUSY = Pin('X3', Pin.IN)
NRESET = Pin('X4', Pin.OUT)
DIO1 = Pin('X2', Pin.IN)

spi = SPI(1)

def run():

	NSS.on()
	NRESET.off()
	spi.init(baudrate=4000000, polarity=0, phase=0, firstbit=SPI.MSB)
	
	resetDevice()
	
	delay(2000)
		
	# WRITE REGISTER
	checkBusy()
	NSS.off()
	spi.write(bytes([0x18]))
	spi.write(bytes([0x09]))
	spi.write(bytes([0x07]))

	spi.write(bytes([0x10]))
	NSS.on()
	checkBusy()

	# READ REGISTER
	data = bytearray(5)
	checkBusy()
	NSS.off()
	spi.write_readinto(bytes([0x19, 0x09, 0x07, 0xFF, 0xFF]), data)
	print("Data recv: ")
	print(data)
	NSS.on()
	checkBusy()	

	result = (data[4] == 0x10)
	print("Result: " + str(result))

def resetDevice():
	delay(20)
	NRESET.off()
	delay(50)
	NRESET.on()
	delay(20)

def checkBusy():
	busy_timeout_cnt = 0x00

	while (RFBUSY.value()):
		delay(1)
		busy_timeout_cnt += 1

		if (busy_timeout_cnt > 5):
			print("ERROR - Busy Timeout!")
			
			break