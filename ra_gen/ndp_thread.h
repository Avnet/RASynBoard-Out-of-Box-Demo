/* generated thread header file - do not edit */
#ifndef NDP_THREAD_H_
#define NDP_THREAD_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void ndp_thread_entry(void * pvParameters);
                #else
extern void ndp_thread_entry(void *pvParameters);
#endif
#include "r_iic_master.h"
#include "r_i2c_master_api.h"
FSP_HEADER
/* I2C Master on IIC Instance. */
extern const i2c_master_instance_t g_i2c0_master;

/** Access the I2C Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iic_master_instance_ctrl_t g_i2c0_master_ctrl;
extern const i2c_master_cfg_t g_i2c0_master_cfg;

#ifndef i2c0_master_callback
void i2c0_master_callback(i2c_master_callback_args_t *p_args);
#endif
FSP_FOOTER
#endif /* NDP_THREAD_H_ */
