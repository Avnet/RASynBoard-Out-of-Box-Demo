/* generated thread header file - do not edit */
#ifndef SYSTEM_CMD_THREAD_H_
#define SYSTEM_CMD_THREAD_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void system_cmd_thread_entry(void * pvParameters);
                #else
extern void system_cmd_thread_entry(void *pvParameters);
#endif
FSP_HEADER
FSP_FOOTER
#endif /* SYSTEM_CMD_THREAD_H_ */
