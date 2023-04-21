/*
 * button.c
 *
 *  Created on: Dec 17, 2022
 *      Author: 047759
 */

#include <stdio.h>
#include "button.h"
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "hal_data.h"

int button_init(void)
{
    int err = FSP_SUCCESS;

    /* Open and enable button interrupt */
    err = R_ICU_ExternalIrqOpen(&g_btn_ext_irq13_ctrl, &g_btn_ext_irq13_cfg);
    if (FSP_SUCCESS != err)
    {
        printf ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
        return err;
    }

    err = R_ICU_ExternalIrqEnable(&g_btn_ext_irq13_ctrl);
    if (FSP_SUCCESS != err)
    {
        printf ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
        return err;
    }
    return err;
}


void icu_deinit(void)
{
    R_ICU_ExternalIrqClose(&g_btn_ext_irq13_ctrl);
}

void button_callback(external_irq_callback_args_t *p_args)
{
    (void)p_args;
    static uint32_t edge = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t set_bit = 0;

    if ( edge % 2 ) {
        set_bit = EVENT_BIT_FALLING;
    } else {
        set_bit = EVENT_BIT_RISING;
        xEventGroupClearBitsFromISR(g_ndp_event_group, EVENT_BIT_FALLING);
    }
    xEventGroupSetBitsFromISR(g_ndp_event_group, set_bit, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	edge ++;
}
