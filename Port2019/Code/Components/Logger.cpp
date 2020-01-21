#include <Sensors/Sensoro.h>
#include "Clock.h"
#include "Time.h"
#include "Logger.h"

using namespace SobieskiSat;

bool Logger::begin()
{
	Initialized = false;
	if (SD.begin(SD_pin))
	{
		File sessions = SD.open("SESSIONS.TXT");
		if (!sessions)
		{
			sessions = SD.open("SESSIONS.TXT", FILE_WRITE);
			if (sessions)
			{
				sessions.println("0");
				sessions.close();
				SD.mkdir("0");
				rootDir = "0/";
				Initialized = true;
				return true;
			}
			else return false;
		}
		
		int sessionNo = sessions.readStringUntil('\n').toInt() + 1;
		sessions.close();
		SD.remove("SESSIONS.TXT");
		sessions = SD.open("SESSIONS.TXT", FILE_WRITE);
		if (sessions)
		{
			sessions.println(String(sessionNo));
			sessions.close();
			SD.mkdir(String(sessionNo));
			rootDir = String(sessionNo) + "/";
			Initialized = true;
			return true;
		}
		
		return false;
	}
	else return false;
}

bool Logger::save(Sensor& sensor)
{
	if (Initialized)
	{
	if (sensor.SDbuffer != "")
	{
		File file = SD.open(rootDir + sensor.fileName, FILE_WRITE);
		if (file)
		{
			file.print(sensor.SDbuffer);
			sensor.SDbufferClear();
			file.close();
			return true;
		}
		return false;
	}
	}
	return false;
}

bool Logger::logSensor(String message, Sensor& sender)
{
	buffer += "[" + String(sender.ID) + "] " + message + "\r\n";
	//SerialUSB.println("[" + String(sender.ID) + "] " + message);
}

void Logger::addToBuffer(String str, bool onlyUSB)
{
	if (onlyUSB)
	{
		//if (printUSB) SerialUSB.println(str);
		return;
	}
	else
	{
		if (Initialized) buffer += str;
		//if (printUSB) SerialUSB.println(str);
	}
}

bool Logger::saveBuffer()
{
	if (buffer != "")
	{
		File file = SD.open(rootDir + "LOG.TXT", FILE_WRITE);
		if (file)
		{
			file.print(buffer);
			file.close();
			buffer = "";
			//SerialUSB.println("[L] Logged to SD.");
			return true;
		}
	}
	return false;
}
