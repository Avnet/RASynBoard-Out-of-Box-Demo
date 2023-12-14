/* generated common source file - do not edit */
#include "common_data.h"
icu_instance_ctrl_t g_ndp_ext_irq05_ctrl;
const external_irq_cfg_t g_ndp_ext_irq05_cfg = { .channel = 5, .trigger =
		EXTERNAL_IRQ_TRIG_RISING, .filter_enable = true, .pclk_div =
		EXTERNAL_IRQ_PCLK_DIV_BY_64, .p_callback = ndp_irq_service,
/** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
		.p_context = &NULL,
#endif
		.p_extend = NULL, .ipl = (14),
#if defined(VECTOR_NUMBER_ICU_IRQ5)
    .irq                 = VECTOR_NUMBER_ICU_IRQ5,
#else
		.irq = FSP_INVALID_VECTOR,
#endif
		};
/* Instance structure to use this module. */
const external_irq_instance_t g_ndp_ext_irq05 = { .p_ctrl =
		&g_ndp_ext_irq05_ctrl, .p_cfg = &g_ndp_ext_irq05_cfg, .p_api =
		&g_external_irq_on_icu };
icu_instance_ctrl_t g_btn_ext_irq13_ctrl;
const external_irq_cfg_t g_btn_ext_irq13_cfg = { .channel = 13, .trigger =
		EXTERNAL_IRQ_TRIG_BOTH_EDGE, .filter_enable = true, .pclk_div =
		EXTERNAL_IRQ_PCLK_DIV_BY_64, .p_callback = button_callback,
/** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
		.p_context = &NULL,
#endif
		.p_extend = NULL, .ipl = (13),
#if defined(VECTOR_NUMBER_ICU_IRQ13)
    .irq                 = VECTOR_NUMBER_ICU_IRQ13,
#else
		.irq = FSP_INVALID_VECTOR,
#endif
		};
/* Instance structure to use this module. */
const external_irq_instance_t g_btn_ext_irq13 = { .p_ctrl =
		&g_btn_ext_irq13_ctrl, .p_cfg = &g_btn_ext_irq13_cfg, .p_api =
		&g_external_irq_on_icu };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport = { .p_api = &g_ioport_on_ioport, .p_ctrl =
		&g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
SemaphoreHandle_t g_binary_semaphore;
#if 1
StaticSemaphore_t g_binary_semaphore_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
EventGroupHandle_t g_ndp_event_group;
#if 1
StaticEventGroup_t g_ndp_event_group_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
TimerHandle_t g_timer;
#if 1
StaticTimer_t g_timer_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
QueueHandle_t g_led_queue;
#if 1
StaticQueue_t g_led_queue_memory;
uint8_t g_led_queue_queue_memory[4 * 10];
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
SemaphoreHandle_t g_ndp_mutex;
#if 1
StaticSemaphore_t g_ndp_mutex_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
SemaphoreHandle_t g_xInitialSemaphore;
#if 1
StaticSemaphore_t g_xInitialSemaphore_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
EventGroupHandle_t g_https_extended_msg_event_group;
#if 1
StaticEventGroup_t g_https_extended_msg_event_group_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
QueueHandle_t g_telemetry_queue;
#if 1
StaticQueue_t g_telemetry_queue_memory;
uint8_t g_telemetry_queue_queue_memory[8 * 128];
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
SemaphoreHandle_t g_sd_mutex;
#if 1
StaticSemaphore_t g_sd_mutex_memory;
#endif
void rtos_startup_err_callback(void *p_instance, void *p_data);
void g_common_init(void) {
	g_binary_semaphore =
#if 1
			xSemaphoreCreateBinaryStatic(&g_binary_semaphore_memory);
#else
                xSemaphoreCreateBinary();
                #endif
	if (NULL == g_binary_semaphore) {
		rtos_startup_err_callback(g_binary_semaphore, 0);
	}
	g_ndp_event_group =
#if 1
			xEventGroupCreateStatic(&g_ndp_event_group_memory);
#else
                xEventGroupCreate();
                #endif
	if (NULL == g_ndp_event_group) {
		rtos_startup_err_callback(g_ndp_event_group, 0);
	}
	g_timer =
#if 1
			xTimerCreateStatic(
#else
                xTimerCreate(
                #endif
					"LoopTimer", 2, pdTRUE, NULL, g_timer_callback
#if 1
					, &g_timer_memory
#endif
					);
	if (NULL == g_timer) {
		rtos_startup_err_callback(g_timer, 0);
	}
	g_led_queue =
#if 1
			xQueueCreateStatic(
#else
                xQueueCreate(
                #endif
					10, 4
#if 1
					, &g_led_queue_queue_memory[0], &g_led_queue_memory
#endif
					);
	if (NULL == g_led_queue) {
		rtos_startup_err_callback(g_led_queue, 0);
	}
	g_ndp_mutex =
#if 0
                #if 1
                xSemaphoreCreateRecursiveMutexStatic(&g_ndp_mutex_memory);
                #else
                xSemaphoreCreateRecursiveMutex();
                #endif
                #else
#if 1
			xSemaphoreCreateMutexStatic(&g_ndp_mutex_memory);
#else
                xSemaphoreCreateMutex();
                #endif
#endif
	if (NULL == g_ndp_mutex) {
		rtos_startup_err_callback(g_ndp_mutex, 0);
	}
	g_xInitialSemaphore =
#if 1
			xSemaphoreCreateBinaryStatic(&g_xInitialSemaphore_memory);
#else
                xSemaphoreCreateBinary();
                #endif
	if (NULL == g_xInitialSemaphore) {
		rtos_startup_err_callback(g_xInitialSemaphore, 0);
	}
	g_https_extended_msg_event_group =
#if 1
			xEventGroupCreateStatic(&g_https_extended_msg_event_group_memory);
#else
                xEventGroupCreate();
                #endif
	if (NULL == g_https_extended_msg_event_group) {
		rtos_startup_err_callback(g_https_extended_msg_event_group, 0);
	}
	g_telemetry_queue =
#if 1
			xQueueCreateStatic(
#else
                xQueueCreate(
                #endif
					128, 8
#if 1
					, &g_telemetry_queue_queue_memory[0],
					&g_telemetry_queue_memory
#endif
					);
	if (NULL == g_telemetry_queue) {
		rtos_startup_err_callback(g_telemetry_queue, 0);
	}
	g_sd_mutex =
#if 0
                #if 1
                xSemaphoreCreateRecursiveMutexStatic(&g_sd_mutex_memory);
                #else
                xSemaphoreCreateRecursiveMutex();
                #endif
                #else
#if 1
			xSemaphoreCreateMutexStatic(&g_sd_mutex_memory);
#else
                xSemaphoreCreateMutex();
                #endif
#endif
	if (NULL == g_sd_mutex) {
		rtos_startup_err_callback(g_sd_mutex, 0);
	}
}
