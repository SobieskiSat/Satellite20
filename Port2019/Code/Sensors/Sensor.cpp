#include <Sensors/Sensoro.h>

using namespace SobieskiSat;

bool Sensor::setUpdateDelay(int ms)
{
	if (minDelay > ms || ms < 0)
	{
		//sendLog("usd:" + String(ms), *this); // usd - unsuccesful set delay
		updateDelay = minDelay;
		return false;
	}
	updateDelay = ms;
	//sendLog("ssd:" + String(ms), *this); // ssd - succesful set delay
	return true;
}

void Sensor::SDbufferClear()
{
	SDbuffer = "";
	//packetReady = false;
}
/*
bool Sensor::IsPacketReady()
{
	if (packetCount >= packetSize)
	{
		packetCount = 0;
		packetReady = true;
		return true;
	}
	return false;
}
*/
/*
void Sensor::loadToSDBuffer(std::initializer_list<float> inputs)
{
	for (auto element : inputs)
	{
		SDbuffer += String(element, 4);
		SDbuffer += "\t";
	}
	SDbuffer +="@" + String(millis(), DEC) + "\n";
}
*/
