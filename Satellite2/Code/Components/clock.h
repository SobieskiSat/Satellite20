#ifndef COMPONENTS_CLOCK_H_
#define COMPONENTS_CLOCK_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main.h"

// TIM5 has 32-bit counter -> takes longer to overflow
static inline uint32_t millis() { return (uint32_t)TIM5->CNT; }
static inline uint32_t seconds() { return (uint32_t)(millis() / 1000); }

typedef struct
{
	uint8_t year;		// 00:99 (+2000)
	uint8_t month;		// 1:12
	//uint16_t dayY;		// 1:366
	uint8_t dayM;		// 1:31
	uint8_t dayW;		// 1:7

	uint8_t hour;		// 0:23
	uint8_t minute;		// 0:60
	uint8_t second;		// 0:60
	uint16_t msecond;	// 0:1000
} DateTime;

static inline bool setTime(DateTime* dateTime)
{
	RTC_TimeTypeDef rtc_time = {0};
	rtc_time.Seconds = dateTime->second;
	rtc_time.Minutes = dateTime->minute;
	rtc_time.Hours = dateTime->hour;
	if (HAL_RTC_SetTime(Get_RTC_Instance(), &rtc_time, RTC_FORMAT_BIN) != HAL_OK) return false;

	RTC_DateTypeDef rtc_date = {0};
	rtc_date.Year = dateTime->year;
	rtc_date.Month = dateTime->month; //conversion might be needed
	rtc_date.Date = dateTime->dayM;
	if (HAL_RTC_SetDate(Get_RTC_Instance(), &rtc_date, RTC_FORMAT_BIN) != HAL_OK) return false;
}

static inline DateTime getTime()
{
	RTC_TimeTypeDef rtc_time = {0};
	RTC_DateTypeDef rtc_date = {0};
	DateTime toReturn = {0};

	HAL_RTC_GetTime(Get_RTC_Instance(), &rtc_time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(Get_RTC_Instance(), &rtc_date, RTC_FORMAT_BIN);

	toReturn.year = rtc_date.Year;
	toReturn.month = rtc_date.Month - (rtc_date.Month >= 0x10 ? 0 : 6);	//conversion from silly BTC format
	//toReturn.dayY = rtc_date.
	toReturn.dayM = rtc_date.Date;
	toReturn.dayW = rtc_date.WeekDay;
	toReturn.hour = rtc_time.Hours;
	toReturn.minute = rtc_time.Minutes;
	toReturn.second = rtc_time.Seconds;
	toReturn.msecond = millis() % 1000;

	return toReturn;
}

#endif /* COMPONENTS_CLOCK_H_ */
