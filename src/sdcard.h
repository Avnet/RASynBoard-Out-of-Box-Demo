#ifndef __SDCARD_H
#define __SDCARD_H

#include "hal_data.h"
#include "fatfs.h"
#include "stdio.h"


#define SDCARD_ERROR_OPEN_NORMAL	 		0x1
#define SDCARD_ERROR_OPEN_WRITE_MODE	0x2
#define SDCARD_ERROR_OPEN_READ_MODE	 	0x3
#define SDCARD_ERROR_CLOSE 						0x4
#define SDCARD_ERROR_WRITE 						0x5
#define SDCARD_ERROR_READ  						0x6
#define SDCARD_ERROR_MOUNT 						0x7
#define SDCARD_ERROR_UNMOUNT 					0x8
#define SDCARD_ERROR_READ_SIZE 				0x9
#define SDCARD_ERROR_CAPACITY 				0x10

#define SDCARD_MOUNTED 								0x01
#define SDCARD_NOT_MOUNTED 						0x00
#define SDCARD_UNMOUNTED 							0x01
#define SDCARD_NOT_UNMOUNTED 					0x00

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

		READ_FILE = 0 ,
		WRITE_FILE ,
		READ_WRITE_FILE,

} openType_t;

typedef enum {

		FILE_EXSITS ,
		FILE_NOT_EXISTS

} file_state_t;

typedef struct {

		uint32_t totalsize;
		uint32_t freesize;
		uint8_t percent;

} storage_info_t;

//#define  SPI_SDCARD_TEST

#ifdef  SPI_SDCARD_TEST
void SPI_Sdcard_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
