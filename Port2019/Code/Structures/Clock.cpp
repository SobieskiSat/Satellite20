#include "Clock.h"

void Clock::begin(char* initializer)
{
	// initializer format: DDMMYYhhmmssms
	Start.Day = initializer.substring(0, 1).toInt();
	Start.Month = initializer.substring(2, 3).toInt();
	Start.Year = initializer.substring(4, 5).toInt();
	Start.Hour = initializer.substring(6, 7).toInt();
	Start.Minutes = initializer.substring(8, 9).toInt();
	Start.Seconds = initializer.substring(10, 11).toInt();
	Start.Milliseconds = initializer.substring(12, 13).toInt();
		
	lastTick = millis();
	Tick = millis();
}

void Clock::begin(Time initializer)
{
	Start = initializer;
	Current = initializer;
		
	lastTick = millis();
	Tick = millis();
}
	
void Clock::update()
{
	lastTick = Tick;
	Tick = millis();
		
	Current = Current + (Tick - lastTick);
}
