from pyb import SPI
from pyb import delay
import pyb
from machine import Pin

from SX1280 import SX1280Class
from Settings import *
from Constants import *

LED1 = pyb.LED(4)

spi = SPI(spiRail)

SX1280 = SX1280Class()

sendOK = False
TXPower = 0
TXPacketL = 0


# FUNCTIONS
def packet_is_OK():
	print(" " + str(TXPacketL) + " Bytes SentOK")
def packet_is_Error():
	print("packet not send, error occured")
def sendPacket(packet):
	packet += "#"
	buffersize = len(packet) if len(packet) < TXBUFFER_SIZE else TXBUFFER_SIZE
	TXPacketL = buffersize
	LED1.on()

	if SX1280.sendPacketFLRC(packet, buffersize, 1000, PowerTX, Pin(pinDIO1)):
		LED1.off()
		return True
	else: return False
def led_Flash(flashes, delayMS):
	for i in range(0, 2 * flashes):
		LED1.toggle()
		delay(delayMS);
	LED1.off()
def setup_FLRC():
	SX1280.setStandby(MODE_STDBY_RC)
	SX1280.setRegulatorMode(USE_LDO)
	SX1280.setPacketType(PACKET_TYPE_FLRC)
	SX1280.setRfFrequency(Frequency, Offset)
	SX1280.setBufferBaseAddress(0, 0)
	SX1280.setModulationParams(BandwidthBitRate, CodingRate, BT)
	SX1280.setPacketParams(PREAMBLE_LENGTH_32_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 127, RADIO_CRC_3_BYTES, RADIO_WHITENING_OFF)
	SX1280.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0)
	SX1280.setSyncWord1(Sample_Syncword)
# FUNCTIONS END

def run():
	print("starting")
	delay(1000)
	print("now")

	delay(10)
	spi.init(SPI.MASTER, baudrate=2625000, polarity=0, phase=0, firstbit=SPI.MSB)
	delay(100)

	#LED1 = Pin(pinLED1)
	#led_Flash(2, 125)

	if (SX1280.begin(spi, pinNSS, pinNRESET, pinRFBUSY, pinDIO1, pinDIO2, pinDIO3) == True) or True:
		print("Device found")
		led_Flash(2, 150)
		delay(1000)
	else:
		print("No device responding")
		while True:
			led_Flash(50, 50)

	setup_FLRC()
	print("FLRC Transmitter ready")


	while True:
		LED1.off()
		print(str(TXPower) + "dBm")
		print("TestPacketStatus: ")

		if sendPacket("Hello world!") == 0: packet_is_OK()
		else: packet_is_Error();
		
		print(" ")
		delay(packet_delay)
