#include "sd.h"
#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "clock.h"
#include "run.h"
#include "main.h"
#include "ff.h"

FRESULT SD_setFileTime(char* patho, DateTime* dateTime)
{
    FILINFO infoo;

    // fatfso counts date from 1980, +20 to year needed
    infoo.fdate = (WORD)(((dateTime->year + 20) << 9) | (dateTime->month << 5) | (dateTime->dayM));
    // fatfso stores seconds as 0:30, /2 division needed
    infoo.ftime = (WORD)((dateTime->hour << 11) | (dateTime->minute << 5) | (dateTime->second / 2));

    //f_utime((const TCHAR*)patho, &infoo); // << 	WHY ISNT THIS WORKING...../.......
    return FR_OK;
}


FRESULT SD_init()
{
	if (BSP_SD_Init() != MSD_OK) return FR_NOT_READY;

	if (FATFS_UnLinkDriver(SDPath) != 0) return FR_NOT_READY;

	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) return FR_NOT_READY;

	f_mount(0, SDPath, 0);
	FRESULT mountStatus = f_mount(&fso, SDPath, 0);
	if (mountStatus != FR_OK)
	{
		f_mount(0, SDPath, 0);
		return mountStatus;
	}

	return FR_OK;
}
FRESULT SD_deinit()
{
	//if (FATFS_UnLinkDriver(SDPath) != 0) return FR_NOT_READY;
	f_mount(0, SDPath, 0);

	return FR_OK;
}

FRESULT SD_newFile(char* path)
{
	stato = f_open(&fileo, path, FA_CREATE_ALWAYS);
	if (stato != FR_OK)
	{
		f_mount(0, SDPath, 0);
		return stato;
	}

	//DateTime now = getTime();
	//SD_setFileTime(path, &now);

	f_close(&fileo);
	return stato;
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
	stato = f_open(&fileo, path,  FA_WRITE | FA_OPEN_APPEND);
	if (stato != FR_OK) return stato;

	stato = f_write(&fileo, content, strlen(content), &testByteo);
	if (stato != FR_OK) return stato;

	return f_close(&fileo);
}

FRESULT SD_readFileLine(char* path, uint32_t lineIndex, char* buffer)
{
	stato = f_open(&fileo, path, FA_OPEN_EXISTING | FA_READ);
	if (stato != FR_OK)
	{
		//f_mount(0, SDPath, 0);
		return stato;
	}

	//########################
	// to implement
	//########################

	return f_close(&fileo);
}
FRESULT SD_readFileLines(char* path, uint32_t lineIndex, uint32_t lineCount, char* buffer)
{
	//########################
	// to implement
	//########################
	return FR_OK;
}

FRESULT SD_newDirectory(char* path)
{
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ implement: tree -> multiple '/'
	return f_mkdir(path);
}
