from micropython import const
from Constants import *

spiRail = const(1)
# rail 1 = X5, X6, X7, X8
# rail 2 = Y5, Y6, Y7, Y8

pinNSS = const(#)
pinRFBUSY = const(#)
pinNRESET = const(#)
pinLED1 = const(#)
pinDIO1 = const(#)
pinDIO2 = const(-1) 
pinDIO3 = const(-1)

Frequency = const(2445000000)
Offset = const(0)
Bandwidth = LORA_BW_0400
SpreadingFactor = LORA_SF7
CodeRate = LORA_CR_4_5

TXBUFFER_SIZE = const(16)
RXBUFFER_SIZE = const(16)
TXpower = const(-18)
packet_delay = 1000      
