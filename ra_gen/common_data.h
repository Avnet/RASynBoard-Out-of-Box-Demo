/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
/** External IRQ on ICU Instance. */
extern const external_irq_instance_t g_ndp_ext_irq05;

/** Access the ICU instance using these structures when calling API functions directly (::p_api is not used). */
extern icu_instance_ctrl_t g_ndp_ext_irq05_ctrl;
extern const external_irq_cfg_t g_ndp_ext_irq05_cfg;

#ifndef ndp_irq_service
void ndp_irq_service(external_irq_callback_args_t *p_args);
#endif
/** External IRQ on ICU Instance. */
extern const external_irq_instance_t g_btn_ext_irq13;

/** Access the ICU instance using these structures when calling API functions directly (::p_api is not used). */
extern icu_instance_ctrl_t g_btn_ext_irq13_ctrl;
extern const external_irq_cfg_t g_btn_ext_irq13_cfg;

#ifndef button_callback
void button_callback(external_irq_callback_args_t *p_args);
#endif
/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
extern SemaphoreHandle_t g_binary_semaphore;
extern EventGroupHandle_t g_ndp_event_group;
extern TimerHandle_t g_timer;
void g_timer_callback(TimerHandle_t xTimer);
extern QueueHandle_t g_led_queue;
extern SemaphoreHandle_t g_ndp_mutex;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
