#ifndef SOBIESKISATLIBRARY_DATETIME_H
#define SOBIESKISATLIBRARY_DATETIME_H

#include <string.h>

struct Time
{
	int Milliseconds;
	int Seconds;
	int Minutes;
	int Hour;
	int Day;
	int Month;
	int Year;

	Time operator+(int milliseconds)
	{
		Milliseconds += milliseconds;
		if (overflowAdd(Milliseconds, Seconds, 999))
		if (overflowAdd(Seconds, Minutes, 59))
		if (overflowAdd(Minutes, Hour, 59))
			overflowAdd(Hour, Day, 23);
		return *this;
	}
		
	bool overflowAdd(int &first, int &second, int limit)
	{
		bool toReturn = false;
		while (first > limit)
		{
			toReturn = true;
			first -= limit + 1;
			second++;
		}
		return toReturn;
	}
};
	   
#endif
