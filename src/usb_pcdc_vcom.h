/*
 * usb_pcdc_vcom.c
 *
 *  Created on: 2023年4月10日
 *      Author: Nick
 */
#ifndef USB_PCDC_VCOM_H_
#define USB_PCDC_VCOM_H_
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"
//#include "hal_data.h"

/* Function declaration */
void start_usb_pcdc_thread( void );

#endif /* USB_PCDC_VCOM_H_ */
