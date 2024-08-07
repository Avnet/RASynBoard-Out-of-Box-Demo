/*
 * console.c
 *
 *  Created on: 2022年11月24日
 *      Author: Wenxue
 */

#include "console.h"
#include "usb_pcdc_vcom.h"



/* redirecting output */
int _write(int fd, char *pBuffer, int size)
{
    usb_pcdc_print(pBuffer, size);
    return size;
}
