from pyb import SPI
from pyb import sleep_ms as delay
from machine import Pin

from SX1280LT import SX1280Class
import Settings
from Constants import *

# FUNCTIONS
def printPacket():
	message = ""
	for char in RXBUFFER:
		#message.write(char)
		message += char
	print("Received: " + message)
def packet_is_OK():
	RXpacketCount += 1
	RXBUFFER = SX1280LT.readPacketFLRC(RXBUFFER_SIZE);
	
	if len(received) == 0: print("Buffer Error")
	else: printPacket()

	print("RSSI: " + str(PacketRSSI) + "dBm")
	print("Length: " + str(RXPacketL))
	print("Packets: " + str(RXpacketCount))
	print("Errors: " + str(errors))
	IRQStatus = SX1280LT.readIrqStatus()
	print("IRQreg: " + str(hex(IRQStatus)))
def packet_is_Error():
	# cropped
	errors += 1
	print("PacketError")
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
	SX1280.setDioIrqParams(IRQ_RADIO_ALL, IRQ_TX_DONE, 0, 0)
	SX1280.setSyncWord1(Sample_Syncword)
# FUNCTIONS END

spi = SPI(spiRail, SPI.MASTER, baudrate=4000000, polarity=0, phase=0, firstbit=SPI.MSB)

SX1280 = SX1280Class(spi, pinNSS, pinNRESET, pinRFBUSY, pinDIO1, pinDIO2, pinDIO3)

RXpacketCount = 0
errors = 0
RXPacketL = 0
PacketRSSI = 0
RXBUFFER = [None] * RXBUFFER_SIZE

LED1 = Pin(pinLED1)
#LED1 = pyb.LED(1)
led_Flash(2, 125)

if SX1280.begin():
	print("Device found")
	led_Flash(2, 150)
	delay(1000)
else:
	print("No device responding")
	while True:
		led_Flash(50, 50)
		
setup_FLRC()
print("FLRC Receiver ready")


while True:
	SX1280.setRx(PERIOBASE_01_MS, 0)

	while (!SX1280.DIO1.value())
	
	LED1.on()

	SX1280.readPacketReceptionFLRC()
	RXPacketL = SX1280.RXPacketL()
	PacketRSSI = SX1280.PacketRSSI

	if SX1280.readIrqStatus() == (IRQ_RX_DONE + IRQ_SYNCWORD_VALID): packet_is_OK()
	else: packet_is_Error()

	LED1.off()
