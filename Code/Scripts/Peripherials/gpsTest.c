// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test GPS module
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

#include "run.h"
#include "gps.h"
#include "clock.h"


GPS gps;
float lato, lono;
uint32_t timenow;

static bool gpsTest_begin(void)
{
	println("[GPS] Begin...");
	gps.uart = Get_UART3_Instance();
	GPS_init(&gps);
	GPS_sendCommand(&gps, PMTK_SET_NMEA_OUTPUT_ALLDATA);
	GPS_sendCommand(&gps, PMTK_SET_NMEA_UPDATE_5HZ);

	HAL_Delay(1000);

	// Ask for firmware version
	GPS_sendCommand(&gps, PMTK_Q_RELEASE);
	// TO ADD: chceck firmware response

	return true;
}

static void gpsTest_getData(void)
{
	char c = GPS_read(&gps);
	if (GPS_newNMEAreceived(&gps))
	{
		if (GPS_parse(&gps, GPS_lastNMEA(&gps)))
		{
			if (gps.latitudeDegrees != 0)
			{
				lato = gps.latitudeDegrees;
			}
			if (gps.longitudeDegrees != 0)
			{
				lono = gps.longitudeDegrees;
			}
			return;
		}
	}

}
static void gpsTest_printData(void)
{
	// read data from the GPS in the 'main loop'
	char c = GPS_read(&gps);
	// if you want to debug, this is a good time to do it!
	char ca[1] = {c};
	//if (GPSECHO && c) print(ca);
	print(ca);
	// if a sentence is received, we can check the checksum, parse it...
	if (GPS_newNMEAreceived(&gps))
	{
		// a tricky thing here is if we print the NMEA sentence, or data
		// we end up not listening and catching other sentences!
		// so be very wary if using OUTPUT_ALLDATA and trying to print out data
		//println(GPS_lastNMEA(&gps)); // this also sets the newNMEAreceived() flag to false
		if (!GPS_parse(&gps, GPS_lastNMEA(&gps))) // this also sets the newNMEAreceived() flag to false
		  return; // we can fail to parse a sentence in which case we should just wait for another
	}
	// if millis() or timenow wraps around, we'll just reset it
	if (timenow > millis()) timenow = millis();

	// approximately every 2 seconds or so, print out the current stats
	if (millis() - timenow > 200)
	{
		timenow = millis(); // reset the timenow
		print("\r\nTime: ");
		if (gps.gpsTime.hour < 10) { print_char('0'); }
		print_int(gps.gpsTime.hour); print_char(':');
		if (gps.gpsTime.minute < 10) { print_char('0'); }
		print_int(gps.gpsTime.minute); print_char(':');
		if (gps.gpsTime.second < 10) { print_char('0'); }
		print_int(gps.gpsTime.second); print_char('.');
		if (gps.gpsTime.msecond < 10)
		{
			print("00");
		}
		else if (gps.gpsTime.msecond > 9 && gps.gpsTime.msecond < 100)
		{
			print("0");
		}
		print_int(gps.gpsTime.msecond);
		print("\r\nDate: ");
		print_int(gps.gpsTime.dayM); print_char('/');
		print_int(gps.gpsTime.month); print("/20");
		print_int(gps.gpsTime.year); println("");
		print("\r\nFix: "); print_int((int)gps.fix);
		print(" quality: "); print_int((int)gps.fixquality); println("");
		if (gps.fix)
		{
			print("Location: ");
			print_float(gps.latitudeDegrees); print_char(gps.lat);
			if (gps.latitudeDegrees != 0)
			{
				lato = gps.latitudeDegrees;
			}
			print(", ");
			print_float(gps.longitudeDegrees); print_char(gps.lon);
			if (gps.longitudeDegrees != 0)
			{
				lono = gps.longitudeDegrees;
			}
			print("Speed (knots): "); print_float(gps.speed);
			print("\r\nAngle: "); print_float(gps.angle);
			print("\r\nAltitude: "); print_float(gps.altitude);
			print("\r\nSatellites: "); print_int((int)gps.satellites);
		}
	}
}
