#ifndef COMPONENTS_SD_H_
#define COMPONENTS_SD_H_

#include <stdio.h>
#include <string.h>
#include "fatfs.h"
#include "stm32f4xx_hal.h"
#include "clock.h"

FATFS fso;
FIL fileo;
UINT testByteo;
FRESULT stato;

FRESULT SD_init();
FRESULT SD_deinit();

FRESULT SD_newFile(char* path);
FRESULT SD_clearFile(char* path);
FRESULT SD_deleteFile(char* path);

FRESULT SD_writeToFile(char* path, char* content);

FRESULT SD_setFileTime(char* patho, DateTime* dateTime);

FRESULT SD_newDirectory(char* path);

//########################
// to implement
//########################
FRESULT SD_readFileLine(char* path, uint32_t lineIndex, char* buffer);
FRESULT SD_readFileLines(char* path, uint32_t lineIndex, uint32_t lineCount, char* buffer);

#endif /* COMPONENTS_SD_H_ */
