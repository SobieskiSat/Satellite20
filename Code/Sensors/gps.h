#ifndef SENSORS_GPS_H
#define SENSORS_GPS_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "clock.h"

/**************************************************************************/
/**
Comment out the definition of NMEA_EXTENSIONS to make the library use as
little memory as possible for GPS functionality only.*/
#define NMEA_EXTENSIONS // if defined will include more NMEA sentences

#define MAXLINELENGTH 120 // how long are max NMEA lines to parse?
#define NMEA_MAX_SENTENCE_ID 20 // maximum length of a sentence ID name, including terminating 0
#define NMEA_MAX_SOURCE_ID 3 // maximum length of a source ID name, including terminating 0

/**************************************************************************/
/**
Different commands to set the update rate from once a second (1 Hz) to 10 times
a second (10Hz) Note that these only control the rate at which the position is
echoed, to actually speed up the position fix you must also send one of the
position fix rate commands below too. */
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ "$PMTK220,10000*2F" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ "$PMTK220,5000*1B" // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ "$PMTK220,1000*1F" //  1 Hz
#define PMTK_SET_NMEA_UPDATE_2HZ "$PMTK220,500*2B"  //  2 Hz
#define PMTK_SET_NMEA_UPDATE_5HZ "$PMTK220,200*2C"  //  5 Hz
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F" // 10 Hz
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ "$PMTK300,10000,0,0,0,0*2C" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ "$PMTK300,5000,0,0,0,0*18" // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ "$PMTK300,1000,0,0,0,0*1C" // 1 Hz
#define PMTK_API_SET_FIX_CTL_5HZ "$PMTK300,200,0,0,0,0*2F"  // 5 Hz
// Can't fix position faster than 5 times a second!

#define PMTK_SET_BAUD_115200 "$PMTK251,115200*1F" // 115200 bps
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"   //  57600 bps
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"     //   9600 bps

#define PMTK_SET_NMEA_OUTPUT_GLLONLY "$PMTK314,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on only the GPGLL sentence
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on only the GPRMC sentence
#define PMTK_SET_NMEA_OUTPUT_VTGONLY "$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on only the GPVTG
#define PMTK_SET_NMEA_OUTPUT_GGAONLY "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on just the GPGGA
#define PMTK_SET_NMEA_OUTPUT_GSAONLY "$PMTK314,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on just the GPGSA
#define PMTK_SET_NMEA_OUTPUT_GSVONLY "$PMTK314,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on just the GPGSV
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28" // turn on GPRMC and GPGGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGAGSA "$PMTK314,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29" // turn on GPRMC, GPGGA and GPGSA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28" // turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28" // turn off output

// to generate your own sentences, check out the MTK command datasheet and use a
// checksum calculator such as the awesome
// http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG "$PMTK185,0*22" // Start logging data
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"  // Stop logging data
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C" // Acknowledge the start or stop command
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"  // Query the logging status
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22" // Erase the log flash data
#define LOCUS_OVERLAP 0 // If flash is full, log will overwrite old data with new logs
#define LOCUS_FULLSTOP 1 // If flash is full, logging will stop

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E" // Enable search for SBAS satellite (only works with 1Hz output rate)
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E" // Use WAAS for DGPS correction data

#define PMTK_STANDBY "$PMTK161,0*28" // standby command & boot successful message
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36" // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"             // Wake up

#define PMTK_Q_RELEASE "$PMTK605*31" // ask for the release and version
#define PMTK_Q_RELEASE_RESPONSE (char *)"$GPGSA,A,1,,,,,,,,,,,,,,,*1E" // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ << add that

#define PGCMD_ANTENNA "$CDCMD,33,1*7C" // request for updates on antenna status
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D" // don't show antenna status messages

#define MAXWAITSENTENCE 10 // how long to wait when we're looking for a response
/**************************************************************************/

/// type for resulting code from running check()
typedef enum
{
	NMEA_BAD = 0,            // passed none of the checks
	NMEA_HAS_DOLLAR = 1,     // has a dollar sign in the first position
	NMEA_HAS_CHECKSUM = 2,   // has a valid checksum at the end
	NMEA_HAS_NAME = 4,       // there is a token after the $ followed by a comma
	NMEA_HAS_SOURCE = 10,    // has a recognized source ID
	NMEA_HAS_SENTENCE = 20,  // has a recognized sentence ID
	NMEA_HAS_SENTENCE_P = 40 // has a recognized parseable sentence ID
} nmea_check_t;

typedef struct
{
	// these were cleared, 0 and {0} \./
	int thisCheck; // the results of the check on the current sentence
	char thisSource[NMEA_MAX_SOURCE_ID]; // the first two letters of the current sentence, e.g. WI, GP
	char thisSentence[NMEA_MAX_SENTENCE_ID]; // the next three letters of the current sentence, e.g. GLL, RMC
	char lastSource[NMEA_MAX_SOURCE_ID]; // the results of the check on the most recent successfully parsed sentence
	char lastSentence[NMEA_MAX_SENTENCE_ID]; // the next three letters of the most recent successfully parsed sentence, e.g. GLL, RMC

	DateTime gpsTime;
	float latitude;  // Floating point latitude value in degrees/minutes as received from the GPS (DDMM.MMMM)
	float longitude; // Floating point longitude value in degrees/minutes as received from the GPS (DDDMM.MMMM)
	
	/** Fixed point latitude and longitude value with degrees stored in units of
	1/100000 degrees, and minutes stored in units of 1/100000 degrees.  See pull
	#13 for more details:
	https://github.com/adafruit/Adafruit-GPS-Library/pull/13 */
	int32_t latitude_fixed;  // Fixed point latitude in decimal degrees
	int32_t longitude_fixed; // Fixed point longitude in decimal degrees

	float latitudeDegrees;  // Latitude in decimal degrees
	float longitudeDegrees; // Longitude in decimal degrees
	float geoidheight;      // Diff between geoid height and WGS84 height
	float altitude;         // Altitude in meters above MSL
	float speed;            // Current speed over ground in knots
	float angle;            // Course in degrees from true north
	float magvariation;     // Magnetic variation in degrees (vs. true north)
	float HDOP;     // Horizontal Dilution of Precision - relative accuracy of horizontal position
	float VDOP;     // Vertical Dilution of Precision - relative accuracy of vertical position
	float PDOP;     // Position Dilution of Precision - Complex maths derives a simple, single number for each kind of DOP

	// these were 'X' \./
	char lat; // N/S
	char lon; // E/W
	char mag; // Magnetic variation direction

	bool fix;    // Have a fix?
	uint8_t fixquality;    // Fix quality (0, 1, 2 = Invalid, GPS, DGPS)
	uint8_t fixquality_3d; // 3D fix quality (1, 3, 3 = Nofix, 2D fix, 3D fix)
	uint8_t satellites;    // Number of satellites in use
	
	uint16_t LOCUS_serial;  // Log serial number
	uint16_t LOCUS_records; // Log number of data record
	uint8_t LOCUS_type;     // Log type, 0: Overlap, 1: FullStop
	uint8_t LOCUS_mode;     // Logging mode, 0x08 interval logger
	uint8_t LOCUS_config;   // Contents of configuration
	uint8_t LOCUS_interval; // Interval setting
	uint8_t LOCUS_distance; // Distance setting
	uint8_t LOCUS_speed;    // Speed setting
	uint8_t LOCUS_status;   // 0: Logging, 1: Stop logging
	uint8_t LOCUS_percent;  // Log life used percentage
	
	// these were cleared, 0 and {0} \./
	// NMEA additional public variables
	char txtTXT[63]; // text content from most recent TXT sentence
	int txtTot;        // total TXT sentences in group
	int txtID;         // id of the text message
	int txtN;          // the TXT sentence number
	
	UART_HandleTypeDef* uart;

	volatile char line1[MAXLINELENGTH]; // We double buffer: read one line in and leave one for the main program
	volatile char line2[MAXLINELENGTH]; // Second buffer
	// this was cleared, 0 \./
	volatile uint8_t lineidx;   // our index into filling the current line
	volatile char* currentline;     // Pointer to current line buffer
	volatile char* lastline;        // Pointer to previous line buffer
	volatile bool recvdflag;     // Received flag
	volatile bool inStandbyMode; // In standby flag
	
	// used by check() for validity tests, room for future expansion

	// these have to be set! \./
	char* sources[5]; // valid source ids
	char* sentences_parsed[5]; // parseable sentence ids
	char* sentences_known[1]; // known, but not parseable sentence ids
	/*
	const char* sources[5] = {"II", "WI", "GP", "GN", "ZZZ"}; // valid source ids
	const char* sentences_parsed[5] = {"GGA", "GLL", "GSA", "RMC", "ZZZ"}; // parseable sentence ids
	const char* sentences_known[1] = {"ZZZ"}; // known, but not parseable sentence ids
	 */

	// Make all of these times far in the past by setting them near the middle of
	// the millis() range. Timing assumes that sentences are parsed promptly.

	// these were 2000000000UL \./
	uint32_t lastUpdate; // millis() when last full sentence successfully parsed
	uint32_t lastFix;  // millis() when last fix received
	uint32_t lastTime; // millis() when last time received
	uint32_t lastDate; // millis() when last date received
	uint32_t recvdTime; // millis() when last full sentence received
	uint32_t sentTime; // millis() when first character of last full sentence received
	bool paused;
	bool active;

	uint8_t uartBuffer[1];

} GPS;

bool GPS_begin(GPS* inst, uint32_t baud_or_i2caddr);
bool GPS_init(GPS* inst);

char* GPS_lastNMEA(GPS* inst);
bool GPS_newNMEAreceived(GPS* inst);

uint8_t GPS_parseHex(GPS* inst, char c);
bool GPS_parse(GPS* inst, char*);
char* GPS_parseStr(GPS* inst, char* buff, char* p, int n);
bool GPS_isEmpty(GPS* inst, char* pStart);
void GPS_parseTime(GPS* inst, char*);
void GPS_parseLat(GPS* inst, char*);
bool GPS_parseLatDir(GPS* inst, char*);
void GPS_parseLon(GPS* inst, char*);
bool GPS_parseLonDir(GPS* inst, char*);
bool GPS_parseFix(GPS* inst, char*);
uint8_t GPS_parseResponse(GPS* inst, char* response);
const char* GPS_tokenOnList(GPS* inst, char* token, const char** list);

void GPS_addChecksum(GPS* inst, char *buff);
float GPS_secondsSinceFix(GPS* inst);
float GPS_secondsSinceTime(GPS* inst);
float GPS_secondsSinceDate(GPS* inst);

char GPS_read(GPS* inst);
bool GPS_write(GPS* inst, uint8_t);
void GPS_sendCommand(GPS* inst,char* );

bool GPS_check(GPS* inst, char* nmea);
bool GPS_available(GPS* inst);
void GPS_pause(GPS* inst, bool b);
bool GPS_wakeup(GPS* inst);
bool GPS_standby(GPS* inst);

// (optional arguments) max = MAXWAITSENTENCE, usingInterrupts = false
bool GPS_waitForSentence(GPS* inst, const char* wait, uint8_t max, bool usingInterrupts);
bool LOCUS_StartLogger(GPS* inst);
bool LOCUS_StopLogger(GPS* inst);
bool LOCUS_ReadStatus(GPS* inst);

#ifdef NMEA_EXTENSIONS
// NMEA additional public functions
// (optional arguments) ref = 'R'
char* GPS_build(GPS* inst, char* nmea, const char* thisSource, const char* thisSentence, char ref);
void GPS_resetSentTime(GPS* inst);
#endif

#endif
