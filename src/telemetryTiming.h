/*
 * telemetryTiming.h
 *
 *  Created on: Jul 8, 2024
 *      Author: 051520
 */
#ifndef TELEMETRYTIMING_H_
#define TELEMETRYTIMING_H_

#include "stdbool.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "ndp_thread_entry.h"
#include "fat_load.h"

#define IDLE_NETWORK_NUM 0
#define IDLE_INFERENCE_INDEX 7

/* The bits in the event group used to signal interrupt events to NDP task. */
#define EVENT_DELAY_AND_SEND_IDLE  ( 0x01 ) // Bit 0

void suppressNdp120Events(void);
bool getSupressNdp120Event(void);

#endif /* TELEMETRYTIMING_H_ */
