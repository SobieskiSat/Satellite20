#include <stdio.h>
#include <string.h>
#include "main.h"
#include "run.h"
#include "fatfs.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stm32f4xx_hal.h"

static int counte = 0;
static char buf[20];

void setup()
{
	LED_animation();
}

void loop()
{
	HAL_GPIO_TogglePin(LED_BLU_GPIO_Port, LED_BLU_Pin);
	HAL_Delay(1000);

	FRESULT fres;
	uint16_t raw_temp;
	float temp_c;
	char log_path[] = "/TEMPLOG.TXT";
	char buf[20];

	println("Enter append...");
	fres = AppendToFile(log_path, strlen(log_path), buf, strlen(buf));
	println("After append....");

	if (fres != FR_OK)
	{
		println("Fres not ok!");
		BlinkLED(200, 3);
	}

	println("Detect test!");
	uint8_t detecte = BSP_SD_IsDetected();
	printLen = sprintf(printBuffer, "%d", detecte);
	while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen) == USBD_BUSY);
}

void LED_animation()
{
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_YEL_GPIO_Port, LED_YEL_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_SET);

	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_YEL_GPIO_Port, LED_YEL_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
}

static void println(char* str)
{
	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_SET);

	printLen = strlen(str);
	uint8_t i;
	for (i = 0; i < printLen; i++)
	{
		printBuffer[i] = str[i];
	}
	printBuffer[printLen] = '\n';
	printBuffer[printLen + 1] = '\r';
	printBuffer[printLen + 2] = '\0';
 	while (CDC_Transmit_FS((uint8_t*) printBuffer, printLen + 3) == USBD_BUSY);

	HAL_GPIO_WritePin(LED_BLU_GPIO_Port, LED_BLU_Pin, GPIO_PIN_RESET);
};

FRESULT AppendToFile(char* path, size_t path_len, char* msg, size_t msg_len) {

	char buffer[128];
	static FATFS g_sFatFs;
	FRESULT fresult;
	FIL file;
	int len;
	UINT bytes_written;


	//mount SD card
	fresult = f_mount(&g_sFatFs, "0", 1);

	//open file on SD card
	fresult = f_open(&file, "file.txt", FA_OPEN_ALWAYS | FA_WRITE);


	//go to the end of the file
	//fresult = f_lseek(&file, file.fsize);

	//generate some string
	len = sprintf( buffer, "Hello World!\r\n");

	//write data to the file
	fresult = f_write(&file, buffer, len, &bytes_written);

	//close file
	fresult = f_close (&file);
	return fresult;
}

void BlinkLED(uint32_t blink_delay, uint8_t num_blinks)
{
	for (int i = 0; i < num_blinks; i++)
	{
		HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_SET);
		HAL_Delay(blink_delay);
		HAL_GPIO_WritePin(LED_GRN_GPIO_Port, LED_GRN_Pin, GPIO_PIN_RESET);
		HAL_Delay(blink_delay);
	}
}
