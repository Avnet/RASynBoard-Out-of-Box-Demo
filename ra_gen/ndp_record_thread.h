/* generated thread header file - do not edit */
#ifndef NDP_RECORD_THREAD_H_
#define NDP_RECORD_THREAD_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void ndp_record_thread_entry(void * pvParameters);
                #else
extern void ndp_record_thread_entry(void *pvParameters);
#endif
FSP_HEADER
FSP_FOOTER
#endif /* NDP_RECORD_THREAD_H_ */
