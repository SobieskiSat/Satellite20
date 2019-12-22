from micropython import const
from Constants import *

spiRail = 1
# rail 1 = X5, X6, X7, X8
# rail 2 = Y5, Y6, Y7, Y8

pinNSS = 'X9'
pinRFBUSY = 'X3'
pinNRESET = 'X2'
#pinLED1 = const(#)
pinDIO1 = 'X4'
pinDIO2 = const(-1) 
pinDIO3 = const(-1)

Frequency = const(2445000000)                    #frequency of transmissions
Offset = 0                                #offset frequency for calibration purposes  

BandwidthBitRate = FLRC_BR_1_300_BW_1_2    #FLRC bandwidth and bit rate, 1.3Mbs               
CodingRate = FLRC_CR_1_2                   #FLRC coding rate
BT = RADIO_MOD_SHAPING_BT_1_0		       #FLRC BT
Sample_Syncword = const(0x01234567)               #FLRC uses syncword

TXBUFFER_SIZE = const(16)
RXBUFFER_SIZE = const(16)
PowerTX = 1
packet_delay = 1000
