#include "sdcard.h"
#include "ff.h"

#ifdef  SPI_SDCARD_TEST

FATFS fatfs;
// state Holders
uint8_t state_mount = 0;
uint8_t state_unmount = 0;

void InitSdcard(void)
{
	MX_FATFS_Init();
}

void sdcardErrorHandle(uint8_t error_code , char *error_message)
{
		// Handle Errors
		(void) (error_code);
		(void) (error_message);
}

void _mountSdcard(void)
{
	if(state_mount == SDCARD_MOUNTED)
			return;
	else
	{
			if(f_mount(&fatfs , "" , 0) != FR_OK)
			{
				sdcardErrorHandle(SDCARD_ERROR_MOUNT , "Failed to mount sdcard");
				state_mount = SDCARD_NOT_MOUNTED;
			}
			else
			{
				state_mount = SDCARD_MOUNTED;
				state_unmount = SDCARD_NOT_UNMOUNTED;
			}
	}
}

void _unmountSdcard(void)
{
	if(state_unmount == SDCARD_UNMOUNTED)
			return;
	else
	{
			if(f_mount(NULL , "" , 1) != FR_OK)
			{
				sdcardErrorHandle(SDCARD_ERROR_MOUNT , "Failed to unmount sdcard");
				state_unmount = SDCARD_NOT_UNMOUNTED;
			}
			else
			{
				state_unmount = SDCARD_UNMOUNTED;
				state_mount = SDCARD_NOT_MOUNTED;
			}
	}
}

char filename[]={"0:/test-write.txt"};
char wtext[]={"Hello,World!\r\n"};
int SDcardWriteTest(void)
{
    FRESULT res;
    FIL fsrc;
    uint32_t  bw;
	//UINT bw;

    _mountSdcard();
    res=f_open(&fsrc, (const TCHAR*)filename, FA_OPEN_ALWAYS | FA_WRITE);

    if(res!=FR_OK)
    {
        printf("f_open error!");
    }

    res = f_lseek(&fsrc , f_size(&fsrc));
    if(res!=FR_OK)
    {
        printf("f_lseek error!");
    }

    {
        int cnt;
        for(cnt = 0; cnt < 4; cnt++)
            res=f_write(&fsrc, wtext, 512, &bw);
		
//		f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);
		
        if(res!=FR_OK)
        {
            printf("f_write error!\r\n");
        }
    }

    res=f_close(&fsrc);
    _unmountSdcard();

    return res;
}

void SPI_Sdcard_Test(void)
{
    InitSdcard();

    if(0 == SDcardWriteTest()){
        printf("uSD test PASS\r\n");
    }else{
        printf("uSD test FAIL\r\n");
    }
}
#endif
