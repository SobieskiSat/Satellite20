#include "sd.h"
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "clock.h"

static FRESULT SD_setFileTime(char* path, DateTime* dateTime)
{
    FILINFO info;

    // fatfs counts date from 1980, +20 to year needed
    info.fdate = (WORD)(((dateTime->year + 20) << 9) | (dateTime->month << 5) | (dateTime->dayM));
    // fatfs stores seconds as 0:30, /2 division needed
    info.ftime = (WORD)((dateTime->hour << 11) | (dateTime->minute << 5) | (dateTime->second / 2));

    //return f_utime(path, &info);
    return FR_OK;
}


FRESULT SD_init()
{
	if (BSP_SD_Init() != MSD_OK) return FR_NOT_READY;

	if (FATFS_UnLinkDriver(SDPath) != 0) return FR_NOT_READY;

	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) return FR_NOT_READY;

	FATFS fs;
	FRESULT mountStatus = f_mount(&fs, SDPath, 0);
	if (mountStatus != FR_OK)
	{
		f_mount(0, SDPath, 0);
		return mountStatus;
	}

	return FR_OK;
}
FRESULT SD_deinit()
{
	if (FATFS_UnLinkDriver(SDPath) != 0) return FR_NOT_READY;
	f_mount(0, SDPath, 0);

	return FR_OK;
}

FRESULT SD_newFile(char* path)
{
	FIL file;
	FRESULT status;
	status = f_open(&file, path, FA_CREATE_NEW);
	if (status != FR_OK)
	{
		//f_mount(0, SDPath, 0);
		return status;
	}

	DateTime now = getTime();
	SD_setFileTime(path, &now);
	return f_close(&file);
}
FRESULT SD_clearFile(char* path)
{
	SD_deleteFile(path);
	return SD_newFile(path);
}
FRESULT SD_deleteFile(char* path)
{
	return f_unlink(path);
}


FRESULT SD_writeToFile(char* path, char* content)
{
	FIL file;
	FRESULT status;
	status = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	if (status != FR_OK) return status;

	UINT testByte = 0x00;
	status = f_write(&file, content, strlen(content), &testByte);
	if (status != FR_OK) return status;

	return f_close(&file);
}

FRESULT SD_readFileLine(char* path, uint32_t lineIndex, char* buffer)
{
	FIL file;
	FRESULT status;
	status = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	if (status != FR_OK)
	{
		//f_mount(0, SDPath, 0);
		return status;
	}

	//########################
	// to implement
	//########################

	return f_close(&file);
}
FRESULT SD_readFileLines(char* path, uint32_t lineIndex, uint32_t lineCount, char* buffer)
{
	//########################
	// to implement
	//########################
	return FR_OK;
}
