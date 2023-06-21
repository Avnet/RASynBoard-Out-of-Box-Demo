/*
 * fat_load.c
 *
 *  Created on: 2022年12月29日
 *      Author: david
 */

#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "fatfs.h"
#include "fat_load.h"
#include "led.h"
#include "minIni.h"
#include "spi_drv.h"


/* Parse config.ini to save the settings */
char mcu_file_name[32] = { MCU_FILE_NAME };
char dsp_file_name[64] = { DSP_FILE_NAME };
char model_file_name[64] = { MODEL_FILE_NAME };
char button_switch[32] = {"sound"};
int  led_event_color[] = { \
                    LED_COLOR_YELLOW,  \
                    LED_COLOR_CYAN, \
                    LED_COLOR_MAGENTA, \
                    LED_COLOR_RED, \
                    LED_COLOR_GREEN, \
                    LED_EVENT_NONE };

/* Local global variables */
static FATFS fatfs_obj;
static int boot_mode =  BOOT_MODE_NONE;
static int print_console_type = CONSOLE_UART;

void init_fatfs(void)
{
    MX_FATFS_Init();
}

int binary_loading(char * file_name)
{
    FRESULT res;
    FIL fil;
    //FILINFO fno;
    char path[64];


    sprintf(path, "0:/");
    strcat(path, file_name);

    printf("Loading %s\r\n",path);

    // mount
    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
    }

#if 0
    res = f_stat(path, &fno);
    if(res != FR_OK){
        printf("f_stat fail %d\r\n",res);
    }else{
        printf("Size: %lu\r\n", fno.fsize);
        printf("Timestamp: %u-%02u-%02u, %02u:%02u\r\n",
               (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
               fno.ftime >> 11, fno.ftime >> 5 & 63);
        printf("Attributes: %c%c%c%c%c\r\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',
               (fno.fattrib & AM_RDO) ? 'R' : '-',
               (fno.fattrib & AM_HID) ? 'H' : '-',
               (fno.fattrib & AM_SYS) ? 'S' : '-',
               (fno.fattrib & AM_ARC) ? 'A' : '-');

    }
#endif

    res = f_open(&fil, path, FA_READ);
    if(res != FR_OK){
        printf("f_open fail %d\r\n",res);
    }

    printf("%d bytes\r\n", f_size(&fil));


    res =  f_close(&fil);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }


    // unmount
    res = f_mount(NULL, "", 0);
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }

    return res;
}

uint32_t get_synpkg_size(char * file_name)
{
    FRESULT res;
    FIL fil;
    uint32_t size = 0;
    char path[64];

    sprintf(path, "0:/");
    strcat(path, file_name);

    // mount
    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
    }

    res = f_open(&fil, path, FA_READ);
    if(res != FR_OK){
        printf("f_open fail %d\r\n",res);
        return size;
    }

    size = f_size(&fil);

    res =  f_close(&fil);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }

    // unmount
    res = f_mount(NULL, "", 0);
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }

    return size;
}

uint32_t read_synpkg_block(char * file_name, uint32_t offset, uint8_t *buff,  uint32_t split_len)
{
    FRESULT res;
    FIL fil;
    char path[64];
    uint32_t br;

    sprintf(path, "0:/");
    strcat(path, file_name);

    // mount
    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
        return res;
    }

    res = f_open(&fil, path, FA_READ);
    if(res != FR_OK){
        printf("f_open fail %d\r\n",res);
        return res;
    }

    res = f_lseek(&fil, offset);
    if(res != FR_OK){
        printf("f_lseek fail %d\r\n",res);
        return res;
    }
    res = f_read(&fil, buff, split_len, &br);
    if(res != FR_OK){
        printf("f_read fail %d\r\n",res);
        return res;
    }

    res =  f_close(&fil);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }

    // unmount
    res = f_mount(NULL, "", 0);
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }
    return br;
}

int check_sdcard_env(void)
{
    int ret = -1;
    uint32_t size;

    size = get_synpkg_size(mcu_file_name);
    if (size == 0)
        return ret;

    size = get_synpkg_size(dsp_file_name);
    if (size == 0)
        return ret;

    size = get_synpkg_size(model_file_name);
    if (size == 0)
        return ret;

    return 0;
}

uint32_t cat_file(char * src_file, char * dst_file, int flag)
{
    FRESULT res;
    FIL fil_rd, fil_wr;
    char path[64];
	uint8_t buffer[512];
    uint32_t br, bw;

    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
        return res;
    }

	/* open src file to read */
	memset(path, 0, sizeof(path));
	sprintf(path, "0:/%s", src_file);
	res = f_open(&fil_rd, path, FA_READ);
	if(res != FR_OK){
		printf("f_open fail %d\r\n",res);
		return res;
	}

	/* open src file to read */
	memset(path, 0, sizeof(path));
	sprintf(path, "0:/%s", dst_file);
	if ( flag == 0) {
		/* create a new file */
		res = f_open(&fil_wr, path, FA_CREATE_ALWAYS | FA_WRITE);
	} else {
		/* append data to file */
		res = f_open(&fil_wr, path, FA_OPEN_APPEND | FA_WRITE);
	}
	if(res != FR_OK){
		printf("f_open fail %d\r\n",res);
		return res;
	}

	/* Copy source to destination */
    while (1) {
		res = f_read(&fil_rd, buffer, sizeof(buffer), &br); 
		if(res != FR_OK){
			printf("f_read fail %d\r\n",res);
			return res;
		}

        if (br == 0)
			break; /* error or eof */

        res = f_write(&fil_wr, buffer, br, &bw);
		if(res != FR_OK){
			printf("f_write fail %d\r\n",res);
			return res;
		}
        if (bw < br)
			break; /* error or disk full */
    }

    res =  f_close(&fil_rd);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }
    res =  f_close(&fil_wr);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }

    res = f_unmount("");
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }
    return bw;
}

uint32_t remove_file(char * file_name)
{
    FRESULT res;
    char path[64];

    sprintf(path, "0:/%s", file_name);

    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
        return res;
    }

	/* delete file */
	res = f_unlink(path);
	if(res != FR_OK){
		printf("f_unlink fail %d\r\n",res);
		return res;
	}

  res = f_unmount("");
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }
    return res;
}

uint32_t write_wav_file(char * file_name, uint8_t *buff,  uint32_t len,  int header)
{
    FRESULT res;
    FIL fil;
    char path[64];
    uint32_t bw;

    sprintf(path, "0:/%s", file_name);

    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
        return res;
    }

	if ( header == 1 ) {
		/* create a new file */
		res = f_open(&fil, path, FA_CREATE_ALWAYS | FA_WRITE);
	} else {
		/* append data to file */
		res = f_open(&fil, path, FA_OPEN_APPEND | FA_WRITE);
	}
	if(res != FR_OK){
		printf("f_open fail %d\r\n",res);
		return res;
	}


#if 0
	if ( header == 1 ) {
		/* create a new wav header */
		res = f_rewind(&fil);
		if(res != FR_OK){
			printf("f_rewind fail %d\r\n",res);
			return res;
		}
	}
#endif

    res = f_write(&fil, buff, len, &bw);
    if(res != FR_OK){
        printf("f_write fail %d\r\n",res);
        return res;
    }

    res =  f_close(&fil);
    if(res != FR_OK){
        printf("f_close fail %d\r\n",res);
    }

    res = f_mount(NULL, "", 0);
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }
    return bw;
}

static uint32_t read_config_file( void )
{
    FRESULT res;
    FILINFO fno;
    char inifile[] = "0:/config.ini";
    char color[16] = {0};
    char key[6] = {0};
    char section[24] = {0};
    char tip[64] = {0};
    int mode;

    // mount
    res = f_mount(&fatfs_obj, "", 1);
    if(res != FR_OK){
        printf("f_mount fail %d\r\n",res);
        return res;
    }

    /* checks the existence of a file */
    res = f_stat (inifile, &fno);
    if(res == FR_NO_FILE){
        f_unmount("");
        return res;
    }

	/* Read config.ini from sdcard */
	mode = ini_getl("NDP Firmware", "Mode", 0, inifile);
	sprintf(section, "Function_%d", mode);

	ini_gets(section, "Description", NULL, tip, sizeof(tip), inifile);
	printf("Select mode[%d]: %s\r\n", mode, tip);

	ini_gets(section, "MCU", MCU_FILE_NAME, \
						mcu_file_name, sizeof(mcu_file_name), inifile);
	ini_gets(section, "DSP", DSP_FILE_NAME, \
						dsp_file_name, sizeof(dsp_file_name), inifile);
	ini_gets(section, "DNN", MODEL_FILE_NAME, \
						model_file_name, sizeof(model_file_name), inifile);
	ini_gets(section, "Button_shift", "sound", \
						button_switch, sizeof(button_switch), inifile);

	/* Get led color accoding according to voice command */
	for (int idx = 0; idx < LED_EVENT_NUM; idx++)
	{
		sprintf(key, "IDX%d", idx);
		ini_gets("Led", key, "-", color, sizeof(color), inifile);

		if( strncmp(color, "red", 3) == 0)
		{
			led_event_color[idx] = LED_COLOR_RED;
		} else if( strncmp(color, "green", 5) == 0)
		{
			led_event_color[idx] = LED_COLOR_GREEN;
		} else if( strncmp(color, "blue", 4) == 0)
		{
			led_event_color[idx] = LED_COLOR_BLUE;
		} else if( strncmp(color, "cyan", 4) == 0)
		{
			led_event_color[idx] = LED_COLOR_CYAN;
		} else if( strncmp(color, "magenta", 6) == 0)
		{
		    led_event_color[idx] = LED_COLOR_MAGENTA;
		} else if( strncmp(color, "yellow", 6) == 0)
		{
			led_event_color[idx] = LED_COLOR_YELLOW;
		} else
		{
			led_event_color[idx] = LED_EVENT_NONE;
		}
	}
	print_console_type = ini_getl("Debug Print", "Port", CONSOLE_UART, inifile);

    // unmount
    res = f_mount(NULL, "", 0);
    if(res != FR_OK){
        printf("f_mount umount fail %d\r\n",res);
    }

    return res;
}

uint32_t get_synpkg_config_info( void )
{
	uint32_t res = 0;
	bool sdcard;

	sdcard = (sdmmc_exist_check() == 1) ?  true : false;
	if (sdcard){
		boot_mode = BOOT_MODE_SD;
	}else{
		boot_mode = BOOT_MODE_EMMC;
		print_console_type = CONSOLE_USB_CDC;
		return 0;
	}

	res = read_config_file();
	if(res != FR_OK){
		printf("read config.txt failed %d\n", res);
		return res;
	}

	printf("MCU : %s\r\n", mcu_file_name);
	printf("DSP : %s\r\n", dsp_file_name);
	printf("DNN : %s\r\n", model_file_name);

	return res;
}

uint32_t get_synpkg_boot_mode( void )
{
	return boot_mode;
}

int get_print_console_type( void )
{
    return print_console_type;
}
