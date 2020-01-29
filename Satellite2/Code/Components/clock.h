#ifndef COMPONENTS_CLOCK_H_
#define COMPONENTS_CLOCK_H_

#include "stm32f4xx_hal.h"

typedef struct
{
	uint8_t year;		// 00:99 (+2000)
	uint8_t month;		// 1:12
	uint16_t dayY;		// 1:366
	uint8_t dayM;		// 1:31
	uint8_t dayW;		// 1:7

	uint8_t hour;		// 0:23
	uint8_t minute;		// 0:60
	uint8_t second;		// 0:60
	uint16_t msecond;	// 0:1000
} DateTime;

// implement: start DateTime + loading from GPS

// implement: get current DateTime static DateTime getTime();

static inline uint32_t seconds() { return millis() / 1000; }
static inline uint32_t millis() { return TIM2->CNT; }


#endif /* COMPONENTS_CLOCK_H_ */
