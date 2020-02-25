// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//		A code to test SD card and SDIO
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "run.h"
#include "fatfs.h"
#include "clock.h"
#include "sd.h"

bool sdActive;

static bool sdTest_begin(void)
{

	if (SD_init() == FR_OK)
	{
		println("[SD] Detected");
		print("[SD] Creating test file...");
		if (SD_newFile("/TEST.TXT") == FR_OK)
		{
			println("successful.");
			bool status = true;
			status |= (SD_writeToFile("/TEST.TXT", "Created on: ") == FR_OK);

			DateTime fileCreated = getTime();
			char dateStr[26] = {0};
			sprintf(dateStr, "%d-%d-20%d %d:%d:%d:%d", fileCreated.dayM, fileCreated.month, fileCreated.year,
													   fileCreated.hour, fileCreated.minute, fileCreated.second, fileCreated.msecond);

			status |= (SD_writeToFile("/TEST.TXT", dateStr) == FR_OK);

			if (status) println("[SD] Content writing successful!");
			else println("[SD] Content writing unsuccessful!");
		}
		else println("unsuccessful :(");
	}
	else
	{
		println("[SD] Init fail!");
		sdActive = false;
		return false;
	}

	sdActive = true;
	return true;
}

static void sdTest_loop(void)
{
	char log_path[] = "/HAM/HAMOOD.TXT";
	char buf[20];

	sprintf((char*)buf, "hamood hadibi!!!!\r\n");


	print("Init:"); print_int(SD_init()); println("");
	f_mkdir("/HAM");
	print("Write: "); print_int(SD_writeToFile(log_path, buf)); println("");
	SD_deinit();

    HAL_Delay(1000);
}
