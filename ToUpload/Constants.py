#*************************************************************
# LoRa Modem Settings
#*************************************************************

#LoRa spreading factors
LORA_SF5 = const(0x50)
LORA_SF6 = const(0x60)
LORA_SF7 = const(0x70)
LORA_SF8 = const(0x80)
LORA_SF9 = const(0x90)
LORA_SF10 = const(0xA0)
LORA_SF11 = const(0xB0)
LORA_SF12 = const(0xC0)

#LoRa bandwidths
LORA_BW_0200 = const(0x34)               #actually 203125hz
LORA_BW_0400 = const(0x26)              #actually 406250hz
LORA_BW_0800 = const(0x18)               #actually 812500hz
LORA_BW_1600 = const(0x0A)               #actually 1625000hz

#LoRa coding rates
LORA_CR_4_5 = const(0x01)
LORA_CR_4_6 = const(0x02)
LORA_CR_4_7 = const(0x03)
LORA_CR_4_8 = const(0x04)

#LoRa CAD settings
LORA_CAD_01_SYMBOL = const(0x00)
LORA_CAD_02_SYMBOL = const(0x20)
LORA_CAD_04_SYMBOL = const(0x40)
LORA_CAD_08_SYMBOL = const(0x60)
LORA_CAD_16_SYMBOL = const(0x80)

#LoRa Packet Types
LORA_PACKET_VARIABLE_LENGTH = const(0x00)
LORA_PACKET_FIXED_LENGTH = const(0x80)
LORA_PACKET_EXPLICIT = LORA_PACKET_VARIABLE_LENGTH
LORA_PACKET_IMPLICIT = LORA_PACKET_FIXED_LENGTH

#LoRa packet CRC settings
LORA_CRC_ON = const(0x20)
LORA_CRC_OFF = const(0x00)

#LoRa IQ Setings
LORA_IQ_NORMAL = const(0x40)
LORA_IQ_INVERTED = const(0x00)


FREQ_STEP = 198.364
FREQ_ERROR_CORRECTION = 1.55




#*************************************************************
# SX1280 Interrupt flags
#*************************************************************

IRQ_RADIO_NONE = const(0x0000)
IRQ_TX_DONE = const(0x0001)
IRQ_RX_DONE = const(0x0002)
IRQ_SYNCWORD_VALID = const(0x0004)
IRQ_SYNCWORD_ERROR = const(0x0008)
IRQ_HEADER_VALID = const(0x0010)
IRQ_HEADER_ERROR = const(0x0020)
IRQ_CRC_ERROR = const(0x0040)
IRQ_RANGING_SLAVE_RESPONSE_DONE = const(0x0080)

IRQ_RANGING_SLAVE_REQUEST_DISCARDED = const(0x0100)
IRQ_RANGING_MASTER_RESULT_VALID = const(0x0200)
IRQ_RANGING_MASTER_RESULT_TIMEOUT = const(0x0400)
IRQ_RANGING_SLAVE_REQUEST_VALID = const(0x0800)
IRQ_CAD_DONE = const(0x1000)
IRQ_CAD_ACTIVITY_DETECTED = const(0x2000)
IRQ_RX_TX_TIMEOUT = const(0x4000)
IRQ_PREAMBLE_DETECTED = const(0x8000)
IRQ_RADIO_ALL = const(0xFFFF)


#*************************************************************
# SX1280 Commands
#************************************************************

RADIO_GET_PACKETTYPE = const(0x03)
RADIO_GET_IRQSTATUS = const(0x15)
RADIO_GET_RXBUFFERSTATUS = const(0x17)
RADIO_WRITE_REGISTER = const(0x18)
RADIO_READ_REGISTER = const(0x19)
RADIO_WRITE_BUFFER = const(0x1A)
RADIO_READ_BUFFER = const(0x1B)
RADIO_GET_PACKETSTATUS = const(0x1D)
RADIO_GET_RSSIINST = const(0x1F)
RADIO_SET_STANDBY = const(0x80)
RADIO_SET_RX = const(0x82)
RADIO_SET_TX = const(0x83)
RADIO_SET_SLEEP = const(0x84)
RADIO_SET_RFFREQUENCY = const(0x86)
RADIO_SET_CADPARAMS = const(0x88)
RADIO_CALIBRATE = const(0x89)
RADIO_SET_PACKETTYPE = const(0x8A)
RADIO_SET_MODULATIONPARAMS = const(0x8B)
RADIO_SET_PACKETPARAMS = const(0x8C)
RADIO_SET_DIOIRQPARAMS = const(0x8D)
RADIO_SET_TXPARAMS = const(0x8E)
RADIO_SET_BUFFERBASEADDRESS = const(0x8F)
RADIO_SET_RXDUTYCYCLE = const(0x94)
RADIO_SET_REGULATORMODE = const(0x96)
RADIO_CLR_IRQSTATUS = const(0x97)
RADIO_SET_AUTOTX = const(0x98)
RADIO_SET_LONGPREAMBLE = const(0x9B)
RADIO_SET_UARTSPEED = const(0x9D)
RADIO_SET_AUTOFS = const(0x9E)
RADIO_SET_RANGING_ROLE = const(0xA3)
RADIO_GET_STATUS = const(0xC0)
RADIO_SET_FS = const(0xC1)
RADIO_SET_CAD = const(0xC5)
RADIO_SET_TXCONTINUOUSWAVE = const(0xD1)
RADIO_SET_TXCONTINUOUSPREAMBLE = const(0xD2)
RADIO_SET_SAVECONTEXT = const(0xD5)


#*************************************************************
# SX1280 Registers
#*************************************************************

REG_LNA_REGIME = const(0x0891)
REG_LR_PAYLOADLENGTH = const(0x901)
REG_LR_PACKETPARAMS = const(0x903)
REG_RFFrequency23_16 = const(0x906)
REG_RFFrequency15_8 = const(0x907)
REG_RFFrequency7_0 = const(0x908)
REG_LR_REQUESTRANGINGADDR = const(0x0912)

REG_LR_DEVICERANGINGADDR = const(0x0916)
REG_LR_RANGINGRESULTCONFIG = const(0x0924)
REG_LR_RANGINGRERXTXDELAYCAL = const(0x092C)
REG_LR_RANGINGIDCHECKLENGTH = const(0x0931)
REG_LR_ESTIMATED_FREQUENCY_ERROR_MSB = const(0x954)
REG_LR_ESTIMATED_FREQUENCY_ERROR_MID = const(0x955)
REG_LR_ESTIMATED_FREQUENCY_ERROR_LSB = const(0x956)
REG_LR_RANGINGRESULTBASEADDR = const(0x0961)
REG_LR_SYNCWORDTOLERANCE = const(0x09CD)
REG_LR_SYNCWORDBASEADDRESS1 = const(0x09CE)
REG_FLRCSYNCWORD1_BASEADDR = const(0x09CF)
REG_LR_SYNCWORDBASEADDRESS2 = const(0x09D3)
REG_FLRCSYNCWORD2_BASEADDR = const(0x09D4)
REG_LR_SYNCWORDBASEADDRESS3 = const(0x09D8)

REG_LR_ESTIMATED_FREQUENCY_ERROR_MASK = const(0x0FFFFF)

#SX1280 Packet Types
PACKET_TYPE_GFSK = const(0x00)
PACKET_TYPE_LORA = const(0x01)
PACKET_TYPE_RANGING = const(0x02)
PACKET_TYPE_FLRC = const(0x03)
PACKET_TYPE_BLE = const(0x04)

#SX1280 Standby modes
MODE_STDBY_RC = const(0x00)
MODE_STDBY_XOSC = const(0x01)

#TX and RX timeout based periods
PERIOBASE_15_US = const(0x00)
PERIOBASE_62_US = const(0x01)
PERIOBASE_01_MS = const(0x02)
PERIOBASE_04_MS = const(0x03)

#TX ramp periods
RADIO_RAMP_02_US = const(0x00)
RADIO_RAMP_04_US = const(0x20)
RADIO_RAMP_06_US = const(0x40)
RADIO_RAMP_08_US = const(0x60)
RADIO_RAMP_10_US = const(0x80)
RADIO_RAMP_12_US = const(0xA0)
RADIO_RAMP_16_US = const(0xC0)
RADIO_RAMP_20_US = const(0xE0)

#SX1280 Power settings)
USE_LDO = const(0x00)
USE_DCDC = const(0x01)


#*************************************************************
#GFSK  modem settings
#*************************************************************

GFS_BLE_BR_2_000_BW_2_4 = const(0x04)
GFS_BLE_BR_1_600_BW_2_4 = const(0x28)
GFS_BLE_BR_1_000_BW_2_4 = const(0x4C)
GFS_BLE_BR_1_000_BW_1_2 = const(0x45)
GFS_BLE_BR_0_800_BW_2_4 = const(0x70)
GFS_BLE_BR_0_800_BW_1_2 = const(0x69)
GFS_BLE_BR_0_500_BW_1_2 = const(0x8D)
GFS_BLE_BR_0_500_BW_0_6 = const(0x86)
GFS_BLE_BR_0_400_BW_1_2 = const(0xB1)
GFS_BLE_BR_0_400_BW_0_6 = const(0xAA)
GFS_BLE_BR_0_250_BW_0_6 = const(0xCE)
GFS_BLE_BR_0_250_BW_0_3 = const(0xC7)
GFS_BLE_BR_0_125_BW_0_3 = const(0xEF)

GFS_BLE_MOD_IND_0_35 = 0
GFS_BLE_MOD_IND_0_50 = 1
GFS_BLE_MOD_IND_0_75 = 2
GFS_BLE_MOD_IND_1_00 = 3
GFS_BLE_MOD_IND_1_25 = 4
GFS_BLE_MOD_IND_1_50 = 5
GFS_BLE_MOD_IND_1_75 = 6
GFS_BLE_MOD_IND_2_00 = 7
GFS_BLE_MOD_IND_2_25 = 8
GFS_BLE_MOD_IND_2_50 = 9
GFS_BLE_MOD_IND_2_75 = 10
GFS_BLE_MOD_IND_3_00 = 11
GFS_BLE_MOD_IND_3_25 = 12
GFS_BLE_MOD_IND_3_50 = 13
GFS_BLE_MOD_IND_3_75 = 14
GFS_BLE_MOD_IND_4_00 = 15

PREAMBLE_LENGTH_04_BITS = const(0x00)      #4 bits
PREAMBLE_LENGTH_08_BITS = const(0x10)      #8 bits
PREAMBLE_LENGTH_12_BITS = const(0x20)      #12 bits
PREAMBLE_LENGTH_16_BITS = const(0x30)      #16 bits
PREAMBLE_LENGTH_20_BITS = const(0x40)      #20 bits
PREAMBLE_LENGTH_24_BITS = const(0x50)      #24 bits
PREAMBLE_LENGTH_28_BITS = const(0x60)      #28 bits
PREAMBLE_LENGTH_32_BITS = const(0x70)      #32 bits

GFS_SYNCWORD_LENGTH_1_BYTE = const(0x00)      #Sync word length 1 byte
GFS_SYNCWORD_LENGTH_2_BYTE = const(0x02)      #Sync word length 2 bytes
GFS_SYNCWORD_LENGTH_3_BYTE = const(0x04)      #Sync word length 3 bytes
GFS_SYNCWORD_LENGTH_4_BYTE = const(0x06)      #Sync word length 4 bytes
GFS_SYNCWORD_LENGTH_5_BYTE = const(0x08)      #Sync word length 5 bytes  

RADIO_RX_MATCH_SYNCWORD_OFF = const(0x00)      #no search for SyncWord
RADIO_RX_MATCH_SYNCWORD_1 = const(0x10)
RADIO_RX_MATCH_SYNCWORD_2 = const(0x20)
RADIO_RX_MATCH_SYNCWORD_1_2 = const(0x30)
RADIO_RX_MATCH_SYNCWORD_3 = const(0x40)
RADIO_RX_MATCH_SYNCWORD_1_3 = const(0x50)
RADIO_RX_MATCH_SYNCWORD_2_3 = const(0x60)
RADIO_RX_MATCH_SYNCWORD_1_2_3 = const(0x70)

RADIO_PACKET_FIXED_LENGTH = const(0x00)      #The packet is fixed length, klnown on both RX and TX, no header
RADIO_PACKET_VARIABLE_LENGTH = const(0x20)      #The packet is variable size, header included

RADIO_CRC_OFF = const(0x00)
RADIO_CRC_1_BYTES = const(0x10)
RADIO_CRC_2_BYTES = const(0x20)
RADIO_CRC_3_BYTES = const(0x30)

RADIO_WHITENING_ON = const(0x00)
RADIO_WHITENING_OFF = const(0x08)

#End GFSK ****************************************************

#*************************************************************
#FLRC  modem settings
#*************************************************************

FLRC_SYNC_NOSYNC = const(0x00)
FLRC_SYNC_WORD_LEN_P32S = const(0x04)

FLRC_BR_1_300_BW_1_2 = const(0x45)   #1.3Mbs
FLRC_BR_1_000_BW_1_2 = const(0x69)   #1.04Mbs 
FLRC_BR_0_650_BW_0_6 = const(0x86)   #0.65Mbs
FLRC_BR_0_520_BW_0_6 = const(0xAA)   #0.52Mbs
FLRC_BR_0_325_BW_0_3 = const(0xC7)   #0.325Mbs
FLRC_BR_0_260_BW_0_3 = const(0xEB)   #0.26Mbs

FLRC_CR_1_2 = const(0x00)           #coding rate 1:2
FLRC_CR_3_4 = const(0x02)           #coding rate 3:4
FLRC_CR_1_0 = const(0x04)          #coding rate 1

BT_DIS = const(0x00)                #No filtering
BT_1 = const(0x10)                #1
BT_0_5 = const(0x20)                #0.5

RADIO_MOD_SHAPING_BT_OFF = const(0x00)
RADIO_MOD_SHAPING_BT_1_0 = const(0x10)
RADIO_MOD_SHAPING_BT_0_5 = const(0x20)


#Table 13-45: PacketStatus2 in FLRC Packet
PacketCtrlBusy = const(0x01)
PacketReceived = const(0x02)
HeaderReceived = const(0x04)
AbortError = const(0x08)
CrcError = const(0x10)
LengthError = const(0x20)
SyncError = const(0x40)
Reserved = const(0x80)


#Table 13-46: PacketStatus3 in FLRC Packet
PktSent = const(0x01)
rxpiderr = const(0x08)
rx_no_ack = const(0x10)

#FLRC default packetparamns
FLRC_Default_AGCPreambleLength = PREAMBLE_LENGTH_32_BITS #packetParam1
FLRC_Default_SyncWordLength = FLRC_SYNC_WORD_LEN_P32S #packetParam2
FLRC_Default_SyncWordMatch = RADIO_RX_MATCH_SYNCWORD_1 #packetParam3
FLRC_Default_PacketType = RADIO_PACKET_VARIABLE_LENGTH #packetParam4
FLRC_Default_PayloadLength = 16 #packetParam5
FLRC_Default_CrcLength = RADIO_CRC_3_BYTES #packetParam6
FLRC_Default_Whitening = RADIO_WHITENING_OFF #packetParam7


#Table 11-15 Sleep modes
RETAIN_INSTRUCTION_RAM = const(0x04)
RETAIN_DATABUFFER = const(0x02)
RETAIN_DATA_RAM = const(0x01)
RETAIN_None = const(0x00)


RAMP_TIME = RADIO_RAMP_02_US
PERIODBASE = PERIOBASE_01_MS
PERIODBASE_COUNT_15_8 = const(0)
PERIODBASE_COUNT_7_0 = const(0)