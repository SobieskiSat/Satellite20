#include "gps.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "run.h"
#include "loging.c"

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
   __typeof__ (b) _b = (b); \
 _a < _b ? _a : _b; })

static bool strStartsWith(const char* str, const char* prefix);

bool GPS_write(GPS* inst, uint8_t c)
{
	uint8_t ca[1] = {c};
	HAL_UART_Transmit(inst->uart, ca, 1, HAL_MAX_DELAY);			// transmit byte
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);	// wait for finished transmission
	return true;
}

char GPS_read(GPS* inst)
{
	static uint32_t firstChar = 0;	// first character received in current sentence
	uint32_t tStart = millis();		// as close as we can get to time char was sent
	char c = 0;
	if (inst->paused) return c;

	c = (char)inst->uartBuffer[0];
	//char ca[2] = {c, '\0'};
	//print(ca);

	inst->currentline[inst->lineidx++] = c;
	// [!!!] \/ wtf is that
	if (inst->lineidx >= MAXLINELENGTH) inst->lineidx = MAXLINELENGTH - 1; // ensure there is someplace to put the next received character

	if (c == '\n')
	{
		inst->currentline[inst->lineidx] = 0;	// add endline character

		if (inst->currentline == inst->line1)
		{
			inst->currentline = inst->line2;
			inst->lastline = inst->line1;
		}
		else
		{
			inst->currentline = inst->line1;
			inst->lastline = inst->line2;
		}

		inst->lineidx = 0;
		inst->recvdflag = true;
		inst->recvdTime = millis();	// time we got the end of the string
		inst->sentTime = firstChar;
		firstChar = 0;				// there are no characters yet
		return c;					// wait until next character to set time
	}

	if (firstChar == 0) firstChar = tStart;
	return c;
}

/**************************************************************************/
/*!
		@brief Initialization code used by all constructor types
*/
/**************************************************************************/
bool GPS_init(GPS* inst)
{
	// UART instance, code ##########################$$$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@@@@@@@@@@@@
	//println("[GPS] init()");
	inst->active = false;
	inst->recvdflag = false;
	inst->inStandbyMode = false;
	inst->paused = false;
	inst->lineidx = 0;
	inst->currentline = inst->line1;
	inst->lastline = inst->line2;

	// uint8_t
	inst->gpsTime.hour = 0;
	inst->gpsTime.minute = 0;
	inst->gpsTime.second = 0;
	inst->gpsTime.year = 0;
	inst->gpsTime.month = 0;
	inst->gpsTime.dayM = 0;
	inst->fixquality = 0;
	inst->fixquality_3d = 0;
	inst->satellites = 0;
	// bool
	inst->fix = false;
	// char
	inst->lat = 0;
	inst->lon = 0;
	inst->mag = 0;
	// uint16_t
	inst->gpsTime.msecond = 0;
	// float
	inst->latitude = 0.0;
	inst->longitude = 0.0;
	inst->geoidheight = 0.0;
	inst->altitude = 0.0;
	inst->speed = 0.0;
	inst->angle = 0.0;
	inst->magvariation = 0.0;
	inst->HDOP = 0.0;
	inst->VDOP = 0.0;
	inst->PDOP = 0.0;

	inst->lastUpdate = 0; // millis() when last full sentence successfully parsed
	inst->lastFix = 0;  // millis() when last fix received
	inst->lastTime = 0; // millis() when last time received
	inst->lastDate = 0; // millis() when last date received
	inst->recvdTime = 0; // millis() when last full sentence received
	inst->sentTime = 0; // millis() when first character of last full sentence received

	inst->lat = 'X';
	inst->lon = 'X';
	inst->mag = 'X';

	inst->thisCheck = 0;
	memset(inst->thisSource, 0x00, NMEA_MAX_SOURCE_ID);
	memset(inst->thisSentence, 0x00, NMEA_MAX_SENTENCE_ID);
	memset(inst->lastSource, 0x00, NMEA_MAX_SOURCE_ID);
	memset(inst->lastSentence, 0x00, NMEA_MAX_SENTENCE_ID);

	memset(inst->txtTXT, 0x00, 63);
	inst->txtTot = 0;
	inst->txtID = 0;
	inst->txtN = 0;

	inst->sources[0] = "II";
	inst->sources[1] = "WI";
	inst->sources[2] = "GP";
	inst->sources[3] = "GN";
	inst->sources[4] = "ZZZ";
	inst->sentences_parsed[0] = "GGA";
	inst->sentences_parsed[1] = "GLL";
	inst->sentences_parsed[2] = "GSA";
	inst->sentences_parsed[3] = "RMC";
	inst->sentences_parsed[4] = "ZZZ";
	inst->sentences_known[0] = "ZZZ";

	uint8_t msg_len = 0;
	char message[50] = {0};
	msg_len = sprintf(message, "$PMTK314,1,1,0,1,5,5,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
	HAL_UART_Transmit(inst->uart, message, msg_len, 1000);			// transmit bytes
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);	// wait for finished transmission
	delay(100);

	msg_len = sprintf(message, "$PMTK101*32\r\n");
	HAL_UART_Transmit(inst->uart, message, msg_len, 1000);			// transmit bytes
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);	// wait for finished transmission
	delay(100);

	msg_len = sprintf(message, "$PMTK286,1*23\r\n");
	HAL_UART_Transmit(inst->uart, message, msg_len, 1000);			// transmit bytes
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);	// wait for finished transmission
	delay(100);

	HAL_UART_Receive_IT(inst->uart, inst->uartBuffer, 1);

	uint32_t timeout = millis();
	while (millis() - timeout <= 3000)
	{
		if (GPS_newNMEAreceived(inst) && GPS_parse(inst, GPS_lastNMEA(inst)))
		{
			inst->active = true;
			return true;
		}
	}

	inst->active = false;
	return false;
}

bool GPS_update(GPS* inst)
{
	if (GPS_newNMEAreceived(inst))
	{
		if (GPS_parse(inst, GPS_lastNMEA(inst))) { inst->newData = true; return true; }
	}
	return false;
}

/**************************************************************************/
/*!
		@brief Send a command to the GPS device
		@param str Pointer to a string holding the command to send
*/
/**************************************************************************/
void GPS_sendCommand(GPS* inst, char* str)
{
	//println("[GPS] sendCommand()");
	// NOT THIS //printLN, code !!!!!!!!!!!!!!@@@@@@@@@@@@@@@@@@@#################$$$$$$$$$$$$$$$$$$$$$$$$$$
	HAL_UART_Transmit(inst->uart, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	//println("[GPS] sendCommand()... waiting");
	// wait for finished transmission
	while (HAL_UART_GetState(inst->uart) != HAL_UART_STATE_READY);
	uint8_t nla[2] = {(uint8_t)('\r'), (uint8_t)('\n')};
	HAL_UART_Transmit(inst->uart, nla, 2, HAL_MAX_DELAY);
	//println("[GPS] finished");
}


/**************************************************************************/
/*!
		@brief Parse a NMEA string
		@param nmea Pointer to the NMEA string
		@return True if we parsed it, false if it has an invalid checksum or invalid
	 data
*/
/**************************************************************************/
bool GPS_parse(GPS* inst, char* nmea)
{
	// do checksum check
	if (!GPS_check(inst, nmea)) return false;
	// passed the check, so there's a valid source in thisSource and a valid
	// sentence in thisSentence

	// look for a few common sentences
	char* p = nmea; // Pointer to move through the sentence -- good parsers are
									// non-destructive
	p = strchr(p, ',') + 1; // Skip to the character after the next comma, then check sentence.

	if (!strcmp(inst->thisSentence, "GGA"))
	{
		// found GGA
		// get time
		GPS_parseTime(inst, p);

		// parse out latitude
		p = strchr(p, ',') + 1;
		GPS_parseLat(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLatDir(inst, p)) return false;

		// parse out longitude
		p = strchr(p, ',') + 1;
		GPS_parseLon(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLonDir(inst, p)) return false;

		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p))
		{
			inst->fixquality = atoi(p);
			if (inst->fixquality > 0)
			{
				inst->fix = true;
				inst->lastFix = inst->sentTime;
			}
			else inst->fix = false;
		}

		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->satellites = atoi(p);

		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->HDOP = (float)atof(p);

		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->altitude = (float)atof(p);

		p = strchr(p, ',') + 1;
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->geoidheight = (float)atof(p);
	}

	else if (!strcmp(inst->thisSentence, "RMC"))
	{
		// found RMC
		// get time
		GPS_parseTime(inst, p);

		// fix or no fix
		p = strchr(p, ',') + 1;
		if (!GPS_parseFix(inst, p)) return false;

		// parse out latitude
		p = strchr(p, ',') + 1;
		GPS_parseLat(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLatDir(inst, p)) return false;

		// parse out longitude
		p = strchr(p, ',') + 1;
		GPS_parseLon(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLonDir(inst, p)) return false;

		// speed
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->speed = (float)atof(p);

		// angle
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->angle = (float)atof(p);

		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p))
		{
			uint32_t fulldate = (float)atof(p);
			inst->gpsTime.dayM = fulldate / 10000;
			inst->gpsTime.month = (fulldate % 10000) / 100;
			inst->gpsTime.year = (fulldate % 100);
			inst->lastDate = inst->sentTime;
		}
	}

	else if (!strcmp(inst->thisSentence, "GLL"))
	{
		// found GLL
		// parse out latitude
		GPS_parseLat(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLatDir(inst, p)) return false;

		// parse out longitude
		p = strchr(p, ',') + 1;
		GPS_parseLon(inst, p);
		p = strchr(p, ',') + 1;
		if (!GPS_parseLonDir(inst, p)) return false;

		// get time
		p = strchr(p, ',') + 1;
		GPS_parseTime(inst, p);

		// fix or no fix
		p = strchr(p, ',') + 1;
		if (!GPS_parseFix(inst, p)) return false;
	}

	else if (!strcmp(inst->thisSentence, "GSA"))
	{
		// found GSA
		// parse out Auto selection, but ignore them
		// parse out 3d fixquality
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->fixquality_3d = atoi(p);
		// skip 12 Satellite PDNs without interpreting them
		for (int i = 0; i < 12; i++)
		{
			p = strchr(p, ',') + 1;
		}
		
		// parse out PDOP
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->PDOP = (float)atof(p);
		
		// parse out HDOP, we also parse this from the GGA sentence. Chipset should
		// report the same for both
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->HDOP = (float)atof(p);
		
		// parse out VDOP
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->VDOP = (float)atof(p);
	}

#ifdef NMEA_EXTENSIONS // Sentences not required for basic GPS functionality
	else if (!strcmp(inst->thisSentence, "TXT"))
	{ //*******************************TXT
		if (!GPS_isEmpty(inst, p)) inst->txtTot = atoi(p);
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->txtN = atoi(p);
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) inst->txtID = atoi(p);
		p = strchr(p, ',') + 1;
		if (!GPS_isEmpty(inst, p)) GPS_parseStr(inst, inst->txtTXT, p, 61); // copy the text to NMEA TXT max of 61 characters
	}
#endif // NMEA_EXTENSIONS

	// we dont parse the remaining, yet!
	else return false;

	// Record the successful parsing of where the last data came from and when
	strcpy(inst->lastSource, inst->thisSource);
	strcpy(inst->lastSentence, inst->thisSentence);
	inst->lastUpdate = millis();
	return true;
}

/**************************************************************************/
/*!
		@brief Check an NMEA string for basic format, valid source ID and valid
		and valid sentence ID. Update the values of thisCheck, thisSource and
		thisSentence.
		@param nmea Pointer to the NMEA string
		@return True if well formed, false if it has problems
*/
/**************************************************************************/
bool GPS_check(GPS* inst, char* nmea)
{
	inst->thisCheck = 0; // new check
	if (*nmea != '$') return false; // doesn't start with $
	else inst->thisCheck += NMEA_HAS_DOLLAR;
	// do checksum check -- first look if we even have one -- ignore all but last
	//* 
	char* ast = nmea; // not strchr(nmea,'*'); for first* 
	while (*ast) ast++; // go to the end
	while (*ast != '*' && ast > nmea) ast--; // then back to*	if it's there
	if (*ast != '*') return false; // there is no asterisk
	else
	{
		uint16_t sum = GPS_parseHex(inst, *(ast + 1)) * 16; // extract checksum
		sum += GPS_parseHex(inst, *(ast + 2));
		char* p = nmea; // check checksum
		for (char* p1 = p + 1; p1 < ast; p1++)
		{
			sum ^=* p1;
		}
		if (sum != 0) return false; // bad checksum :(
		else inst->thisCheck += NMEA_HAS_CHECKSUM;
	}
	// extract source of variable length
	char* p = nmea + 1;
	const char* src = GPS_tokenOnList(inst, p, inst->sources);
	if (src)
	{
		strcpy(inst->thisSource, src);
		inst->thisCheck += NMEA_HAS_SOURCE;
	}
	else return false;
	p += strlen(src);
	// extract sentence id and check if parsed
	const char* snc = GPS_tokenOnList(inst, p, inst->sentences_parsed);
	if (snc)
	{
		strcpy(inst->thisSentence, snc);
		inst->thisCheck += NMEA_HAS_SENTENCE_P + NMEA_HAS_SENTENCE;
	}
	else
	{ // check if known
		snc = GPS_tokenOnList(inst, p, inst->sentences_known);
		if (snc)
		{
			strcpy(inst->thisSentence, snc);
			inst->thisCheck += NMEA_HAS_SENTENCE;
			return false;
		}
	}
	return true; // passed all the tests
}

/**************************************************************************/
/*!
		@brief Check if a token at the start of a string is on a list.
		@param token Pointer to the string
		@param list A list of strings, with the final entry starting "ZZ"
		@return Pointer to the found token, or NULL if it fails
*/
/**************************************************************************/
const char* GPS_tokenOnList(GPS* inst, char* token, const char* *list)
{
	int i = 0; // index in the list
	while (strncmp(list[i], "ZZ", 2) && i < 1000)
	{ // stop at terminator and don't crash without it
		// test for a match on the sentence name
		if (!strncmp((const char* )list[i], (const char* )token, strlen(list[i]))) return list[i];
		i++;
	}
	return NULL; // couldn't find a match
}

/**************************************************************************/
/*!
		@brief Parse a string token from pointer p to the next comma, asterisk
		or end of string.
		@param buff Pointer to the buffer to store the string in
		@param p Pointer into a string
		@param n Max permitted size of string including terminating 0
		@return Pointer to the string buffer
*/
/**************************************************************************/
char* GPS_parseStr(GPS* inst, char* buff, char* p, int n)
{
	char* e = strchr(p, ',');
	int len = 0;
	if (e) 
	{
		len = min(e - p, n - 1);
		strncpy(buff, p, len); // copy up to the comma
		buff[len] = 0;
	}
	else
	{
		e = strchr(p, '*');
		if (e)
		{
			len = min(e - p, n - 1);
			strncpy(buff, p, len); // or up to the* 
			buff[e - p] = 0;
		}
		else
		{
			len = min((int)strlen(p), n - 1);
			strncpy(buff, p, len); // or to the end or max capacity
		}
	}
	return buff;
}

/**************************************************************************/
/*!
		@brief Is the field empty, or should we try conversion? Won't work
		for a text field that starts with an asterisk or a comma, but that
		probably violates the NMEA-183 standard.
		@param pStart Pointer to the location of the token in the NMEA string
		@return true if empty field, false if something there
*/
/**************************************************************************/
bool GPS_isEmpty(GPS* inst, char* pStart)
{
	if (',' != *pStart && '*' != *pStart && pStart != NULL) return false;
	else return true;
}

/**************************************************************************/
/*!
		@brief Add* CS where CS is the two character hex checksum for all but
		the first character in the string. The checksum is the result of an
		exclusive or of all the characters in the string. Also useful if you
		are creating new PMTK strings for controlling a GPS module and need a
		checksum added.
		@param buff Pointer to the string, which must be long enough
		@return none
*/
/**************************************************************************/
void GPS_addChecksum(GPS* inst, char* buff)
{
	char cs = 0;
	int i = 1;
	while (buff[i])
	{
		cs ^= buff[i];
		i++;
	}
	sprintf(buff, "%s*%02X", buff, cs);
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for time
		@param p Pointer to the location of the token in the NMEA string
*/
/**************************************************************************/
void GPS_parseTime(GPS* inst, char* p)
{
	// get time
	uint32_t time = atol(p);
	inst->gpsTime.hour = time / 10000;
	inst->gpsTime.minute = (time % 10000) / 100;
	inst->gpsTime.second = (time % 100);

	p = strchr(p, '.') + 1;
	inst->gpsTime.msecond = (uint32_t)atoi(p);
	inst->lastTime = inst->sentTime;
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for latitude angle
		@param p Pointer to the location of the token in the NMEA string
*/
/**************************************************************************/
void GPS_parseLat(GPS* inst, char* p)
{
	//int32_t degree;
	//long minutes;
	char degreebuff[10];
	if (!GPS_isEmpty(inst, p))
	{
		strncpy(degreebuff, p, 2);
		p += 2;
		degreebuff[2] = '\0';
		long degree = atol(degreebuff) * 10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3;										// skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		long minutes = 50*	atol(degreebuff) / 3;
		inst->latitude_fixed = degree + minutes;
		inst->latitude = degree / 100000 + minutes * 0.000006;
		inst->latitudeDegrees = (inst->latitude - 100 * (int)(inst->latitude / 100)) / 60.0;
		inst->latitudeDegrees += (int)(inst->latitude / 100);
	}
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for latitude direction
		@param p Pointer to the location of the token in the NMEA string
		@return True if we parsed it, false if it has invalid data
*/
/**************************************************************************/
bool GPS_parseLatDir(GPS* inst, char* p)
{
	if (p[0] == 'S')
	{
		inst->lat = 'S';
		inst->latitudeDegrees *= -1.0;
		inst->latitude_fixed *= -1;
	}
	else if (p[0] == 'N') inst->lat = 'N';
	else if (p[0] == ',') inst->lat = 0;
	else return false;
	
	return true;
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for longitude angle
		@param p Pointer to the location of the token in the NMEA string
*/
/**************************************************************************/
void GPS_parseLon(GPS* inst, char* p)
{
	int32_t degree;
	long minutes;
	char degreebuff[10];
	if (!GPS_isEmpty(inst, p))
	{
		strncpy(degreebuff, p, 3);
		p += 3;
		degreebuff[3] = '\0';
		degree = atol(degreebuff)*	10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3;										// skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		minutes = 50 * atol(degreebuff) / 3;
		inst->longitude_fixed = degree + minutes;
		inst->longitude = degree / 100000 + minutes * 0.000006f;
		inst->longitudeDegrees = (inst->longitude - 100 * (int)(inst->longitude / 100)) / 60.0;
		inst->longitudeDegrees += (int)(inst->longitude / 100);
	}
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for longitude direction
		@param p Pointer to the location of the token in the NMEA string
		@return True if we parsed it, false if it has invalid data
*/
/**************************************************************************/
bool GPS_parseLonDir(GPS* inst, char* p)
{
	if (!GPS_isEmpty(inst, p))
	{
		if (p[0] == 'W')
		{
			inst->lon = 'W';
			inst->longitudeDegrees *= -1.0;
			inst->longitude_fixed *= -1;
		}
		else if (p[0] == 'E') inst->lon = 'E';
		else if (p[0] == ',') inst->lon = 0;
		else return false;
	}
	return true;
}

/**************************************************************************/
/*!
		@brief Parse a part of an NMEA string for whether there is a fix
		@param p Pointer to the location of the token in the NMEA string
		@return True if we parsed it, false if it has invalid data
*/
/**************************************************************************/
bool GPS_parseFix(GPS* inst, char* p)
{
	if (p[0] == 'A')
	{
		inst->fix = true;
		inst->lastFix = inst->sentTime;
	}
	else if (p[0] == 'V') inst->fix = false;
	else return false;
	return true;
}

/**************************************************************************/
/*!
		@brief Time in seconds since the last position fix was obtained. Will
		fail by rolling over to zero after one millis() cycle, about 6-1/2 weeks.
		@return float value in seconds since last fix.
*/
/**************************************************************************/
float GPS_secondsSinceFix(GPS* inst) { return (millis() - inst->lastFix) / 1000.; }

/**************************************************************************/
/*!
		@brief Time in seconds since the last GPS time was obtained. Will fail
		by rolling over to zero after one millis() cycle, about 6-1/2 weeks.
		@return float value in seconds since last GPS time.
*/
/**************************************************************************/
float GPS_secondsSinceTime(GPS* inst) { return (millis() - inst->lastTime) / 1000.; }

/**************************************************************************/
/*!
		@brief Time in seconds since the last GPS date was obtained. Will fail
		by rolling over to zero after one millis() cycle, about 6-1/2 weeks.
		@return float value in seconds since last GPS date.
*/
/**************************************************************************/
float GPS_secondsSinceDate(GPS* inst) { return (millis() - inst->lastDate) / 1000.; }



/**************************************************************************/
/*!
		@brief Check to see if a new NMEA line has been received
		@return True if received, false if not
*/
/**************************************************************************/
bool GPS_newNMEAreceived(GPS* inst) { return inst->recvdflag; }

/**************************************************************************/
/*!
		@brief Pause/unpause receiving new data
		@param p True = pause, false = unpause
*/
/**************************************************************************/
void GPS_pause(GPS* inst, bool p) { inst->paused = p; }

/**************************************************************************/
/*!
		@brief Returns the last NMEA line received and unsets the received flag
		@return Pointer to the last line string
*/
/**************************************************************************/
char* GPS_lastNMEA(GPS* inst)
{
	inst->recvdflag = false;
	return (char*)inst->lastline;
}

/**************************************************************************/
/*!
		@brief Parse a hex character and return the appropriate decimal value
		@param c Hex character, e.g. '0' or 'B'
		@return Integer value of the hex character. Returns 0 if c is not a proper
	 character
*/
/**************************************************************************/
// read a Hex value and return the decimal equivalent
uint8_t GPS_parseHex(GPS* inst, char c)
{
	if (c < '0') return 0;
	if (c <= '9') return c - '0';
	if (c < 'A') return 0;
	if (c <= 'F') return (c - 'A') + 10;
	// if (c > 'F')
	return 0;
}

/**************************************************************************/
/*!
		@brief Wait for a specified sentence from the device
		@param wait4me Pointer to a string holding the desired response
		@param max How long to wait, default is MAXWAITSENTENCE
		@param usingInterrupts True if using interrupts to read from the GPS
	 (default is false)
		@return True if we got what we wanted, false otherwise
*/
/**************************************************************************/
bool GPS_waitForSentence(GPS* inst, const char* wait4me, uint8_t max, bool usingInterrupts)
{
	uint8_t i = 0;
	while (i < max)
	{
		if (!usingInterrupts) GPS_read(inst);

		if (GPS_newNMEAreceived(inst))
		{
			char* nmea = GPS_lastNMEA(inst);
			i++;

			if (strStartsWith(nmea, wait4me))
				return true;
		}
	}

	return false;
}

/**************************************************************************/
/*!
		@brief Start the LOCUS logger
		@return True on success, false if it failed
*/
/**************************************************************************/
bool GPS_LOCUS_StartLogger(GPS* inst)
{
	GPS_sendCommand(inst, PMTK_LOCUS_STARTLOG);
	inst->recvdflag = false;
	return GPS_waitForSentence(inst, PMTK_LOCUS_STARTSTOPACK, MAXWAITSENTENCE, false);
}

/**************************************************************************/
/*!
		@brief Stop the LOCUS logger
		@return True on success, false if it failed
*/
/**************************************************************************/
bool GPS_LOCUS_StopLogger(GPS* inst) {
	GPS_sendCommand(inst, PMTK_LOCUS_STOPLOG);
	inst->recvdflag = false;
	return GPS_waitForSentence(inst, PMTK_LOCUS_STARTSTOPACK, MAXWAITSENTENCE, false);
}

/**************************************************************************/
/*!
		@brief Read the logger status
		@return True if we read the data, false if there was no response
*/
/**************************************************************************/
bool GPS_LOCUS_ReadStatus(GPS* inst) {
	GPS_sendCommand(inst, PMTK_LOCUS_QUERY_STATUS);

	if (!GPS_waitForSentence(inst, "$PMTKLOG", MAXWAITSENTENCE, false))
		return false;

	char* response = GPS_lastNMEA(inst);
	uint16_t parsed[10];
	uint8_t i;

	for (i = 0; i < 10; i++)
		parsed[i] = -1;

	response = strchr(response, ',');
	for (i = 0; i < 10; i++) {
		if (!response || (response[0] == 0) || (response[0] == '*'))
			break;
		response++;
		parsed[i] = 0;
		while ((response[0] != ',') && (response[0] != '*') && (response[0] != 0))
		{
			parsed[i] *= 10;
			char c = response[0];
			//if (isDigit(c))
			if (((uint8_t)c) >= 0x30 && ((uint8_t)c) <= 0x39)
				parsed[i] += c - '0';
			else
				parsed[i] = c;
			response++;
		}
	}
	inst->LOCUS_serial = parsed[0];
	inst->LOCUS_type = parsed[1];
	//if (isAlpha(parsed[2]))
	if ((parsed[2] >= 0x41 && parsed[2] <= 0x5A) || (parsed[2] >= 0x61 && parsed[2] <= 0x7A))
	{
		parsed[2] = parsed[2] - 'a' + 10;
	}
	inst->LOCUS_mode = parsed[2];
	inst->LOCUS_config = parsed[3];
	inst->LOCUS_interval = parsed[4];
	inst->LOCUS_distance = parsed[5];
	inst->LOCUS_speed = parsed[6];
	inst->LOCUS_status = !parsed[7];
	inst->LOCUS_records = parsed[8];
	inst->LOCUS_percent = parsed[9];

	return true;
}

/**************************************************************************/
/*!
		@brief Standby Mode Switches
		@return False if already in standby, true if it entered standby
*/
/**************************************************************************/
bool GPS_standby(GPS* inst)
{
	if (inst->inStandbyMode)
	{
		return false; // Returns false if already in standby mode, so that you do
									// not wake it up by sending commands to GPS
	}
	else
	{
		inst->inStandbyMode = true;
		GPS_sendCommand(inst, PMTK_STANDBY);
		// return waitForSentence(PMTK_STANDBY_SUCCESS);	// don't seem to be fast
		// enough to catch the message, or something else just is not working
		return true;
	}
}

/**************************************************************************/
/*!
		@brief Wake the sensor up
		@return True if woken up, false if not in standby or failed to wake
*/
/**************************************************************************/
bool GPS_wakeup(GPS* inst)
{
	if (inst->inStandbyMode)
	{
		inst->inStandbyMode = false;
		GPS_sendCommand(inst, ""); // send byte to wake it up
		return GPS_waitForSentence(inst, PMTK_AWAKE, MAXWAITSENTENCE, false);
	}
	else
	{
		return false; // Returns false if not in standby mode, nothing to wakeup
	}
}

/**************************************************************************/
/*!
		@brief Checks whether a string starts with a specified prefix
		@param str Pointer to a string
		@param prefix Pointer to the prefix
		@return True if str starts with prefix, false otherwise
*/
/**************************************************************************/
static bool strStartsWith(const char* str, const char* prefix)
{
	while (*prefix)
	{
		if (*prefix++ != *str++) return false;
	}
	return true;
}

#ifdef NMEA_EXTENSIONS
/**************************************************************************/
/*!
		@brief Fakes time of receipt of a sentence. Use between build() and parse()
		to make the timing look like the sentence arrived from the GPS.
*/
/**************************************************************************/
void GPS_resetSentTime(GPS* inst) { inst->sentTime = millis(); }

/**************************************************************************/
/*!
		@brief Build an NMEA sentence string based on the relevant variables.
		Sentences start with a $, then a two character source identifier, then
		a three character sentence name that defines the format, then a comma
		and more comma separated fields defined by the sentence name. There are
		many sentences listed that are not yet supported. Most of these sentence
		definitions were found at http://fort21.ru/download/NMEAdescription.pdf

		build() will work with other lengths for source and sentence to allow
		extension to building proprietary sentences like $PMTK220,100*2F.

		build() will not work properly in an environment that does not support
		the %f floating point formatter in sprintf(), and will return NULL.

		build() adds Carriage Return and Line Feed to sentences to conform to
		NMEA-183, so send your output with a //print, not a //println.

		Some of the data in these test sentences may be arbitrary, e.g. for the
		TXT sentence which has a more complicated protocol for multiple lines
		sent as a message set. Also, the data in the class variables are presumed
		to be valid, so these sentences may contain values that are stale, or
		the result of initialization rather than measurement.

		@param nmea Pointer to the NMEA string buffer. Must be big enough to
								hold the sentence. No guarantee what will be in it if the
								building of the sentence fails.
		@param thisSource Pointer to the source name string (2 upper case)
		@param thisSentence Pointer to the sentence name string (3 upper case)
		@param ref Reference for the sentence, usually relative (R) or true (T)
		@return Pointer to sentence if successful, NULL if fails
*/
/**************************************************************************/
char* GPS_build(GPS* inst, char* nmea, const char* thisSource, const char* thisSentence, char ref) {
	sprintf(nmea, "%6.2f", 123.45); // fail if sprintf() doesn't handle floats
	if (strcmp(nmea, "123.45"))
		return NULL;
 * nmea = '$';
	char* p = nmea + 1; // Pointer to move through the sentence
	strncpy(p, thisSource, strlen(thisSource));
	p += strlen(thisSource);
	strncpy(p, thisSentence, strlen(thisSentence));
	p += strlen(thisSentence);
 * p = ',';
	p += 1; // Now $XXSSS, and need to add argument fields
	// This may look inefficient, but an M0 will get down the list in about 1 us /
	// strcmp()! Put the GPS sentences from Adafruit_GPS at the top to make
	// pruning excess code easier. Otherwise, keep them alphabetical for ease of
	// reading.

	if (!strcmp(thisSentence, "GGA")) { //********************************************GGA
		// GGA Global Positioning System Fix Data. Time, Position and fix related
		// data for a GPS receiver
		//			 1				 2			 3 4				5 6 7	8	 9	10 11 12 13	14	15
		//			 |				 |			 | |				| | |	|	 |	 | |	 | |	 |		|
		//$--GGA,hhmmss.ss,ddmm.mm,a,dddmm.mm,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
		// 1) Time (UTC)
		// 2) Latitude
		// 3) N or S (North or South)
		// 4) Longitude
		// 5) E or W (East or West)
		// 6) GPS Quality Indicator, 0 - fix not available, 1 - GPS fix, 2 -
		// Differential GPS fix 7) Number of satellites in view, 00 - 12 8)
		// Horizontal Dilution of precision 9) Antenna Altitude above/below
		// mean-sea-level (geoid) 10) Units of antenna altitude, meters 11) Geoidal
		// separation, the difference between the WGS-84 earth
		//		ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below
		//		ellipsoid
		// 12) Units of geoidal separation, meters
		// 13) Age of differential GPS data, time in seconds since last SC104
		//		type 1 or 9 update, null field when DGPS is not used
		// 14) Differential reference station ID, 0000-1023
		// 15) Checksum
		sprintf(p, "%09.2f,%09.4f,%c,%010.4f,%c,%d,%02d,%f,%f,M,%f,M,,",
				inst->gpsTime.hour * 10000UL + inst->gpsTime.minute * 100UL + inst->gpsTime.second + inst->gpsTime.msecond / 1000.,
				inst->latitude, inst->lat, inst->longitude, inst->lon, inst->fixquality, inst->satellites, inst->HDOP,
				inst->altitude, inst->geoidheight);

	}
	else if (!strcmp(thisSentence, "GLL"))
	{ //********************************************GLL
		// GLL Geographic Position – Latitude/Longitude
		//			 1			 2 3				4 5				 6 7
		//			 |			 | |				| |				 | |
		//$--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,A*hh
		// 1) Latitude ddmm.mm format
		// 2) N or S (North or South)
		// 3) Longitude dddmm.mm format
		// 4) E or W (East or West)
		// 5) Time (UTC)
		// 6) Status A - Data Valid, V - Data Invalid
		// 7) Checksum
		sprintf(p, "%09.4f,%c,%010.4f,%c,%09.2f,A", inst->latitude, inst->lat, inst->longitude, inst->lon,
				inst->gpsTime.hour * 10000UL + inst->gpsTime.minute * 100UL + inst->gpsTime.second + inst->gpsTime.msecond / 1000.);

	}
	else if (!strcmp(thisSentence, "GSA"))
	{ //********************************************
		// GSA GPS DOP and active satellites
		//			 1 2 3												14 15	16	17 18
		//			 | | |												 | |	 |	 |	 |
		//$--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh
		// 1) Selection mode
		// 2) Mode
		// 3) ID of 1st satellite used for fix
		// 4) ID of 2nd satellite used for fix
		// ...
		// 14) ID of 12th satellite used for fix
		// 15) PDOP in meters
		// 16) HDOP in meters
		// 17) VDOP in meters
		// 18) Checksum
		return NULL;

	}
	else if (!strcmp(thisSentence, "RMC"))
	{ //********************************************RMC
		// RMC Recommended Minimum Navigation Information
		//																														12
		//			 1				 2 3			 4 5				6 7	 8	 9		 10	11 |
		//			 |				 | |			 | |				| |	 |	 |			|	 | |
		//$--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxx,x.x,a*hh
		// 1) Time (UTC)
		// 2) Status, V = Navigation receiver warning
		// 3) Latitude
		// 4) N or S
		// 5) Longitude
		// 6) E or W
		// 7) Speed over ground, knots
		// 8) Track made good, degrees true
		// 9) Date, ddmmyy
		// 10) Magnetic Variation, degrees
		// 11) E or W
		// 12) Checksum
		sprintf(p, "%09.2f,A,%09.4f,%c,%010.4f,%c,%f,%f,%06d,%f,%c",
				inst->gpsTime.hour*	10000UL + inst->gpsTime.minute * 100UL + inst->gpsTime.second + inst->gpsTime.msecond / 1000.,
				inst->latitude, inst->lat, inst->longitude, inst->lon, inst->speed, inst->angle,
				inst->gpsTime.dayM * 10000 + inst->gpsTime.month * 100 + inst->gpsTime.year, inst->magvariation, inst->mag);

	}
	else if (!strcmp(thisSentence, "TXT"))
	{ //********************************************TXT
		// as mentioned in https://github.com/adafruit/Adafruit_GPS/issues/95
		// TXT Text Transmission
		//			 1	2	3	4		5
		//			 |	|	|	|		|
		//$--TXT,xx,xx,xx,c--c*hh
		// 1) Total Number of Sentences 01-99
		// 2) Sentence Number 01-99
		// 3) Text Identifier 01-99
		// 4) Text String, max 61 characters
		// 5) Checksum
		sprintf(p, "01,01,23,This is the text of the sample message");

	}
	else
	{
		return NULL; // didn't find a match for the build request
	}

	GPS_addChecksum(inst, nmea); // Successful completion
	sprintf(nmea, "%s\r\n", nmea); // Add Carriage Return and Line Feed to comply with NMEA-183
	return nmea;	 // return pointer to finished product
}

#endif // NMEA_EXTENSIONS
