/*
 * ndp_irq_service.c
 *
 *  Created on: Dec 22, 2022
 *      Author: 047759
 */

#include <stdio.h>
#include "ndp_irq_service.h"
#include "bsp_api.h"
#include "common_data.h"
#include "led.h"
#include "event_groups.h"

int ndp_irq_init(void)
{
    int err = FSP_SUCCESS;

    /* Open ICU interrupt */
    err = R_ICU_ExternalIrqOpen(&g_ndp_ext_irq05_ctrl, &g_ndp_ext_irq05_cfg);
    if (FSP_SUCCESS != err)
    {
        printf ("\r\n**R_ICU_ExternalIrqOpen API FAILED**\r\n");
    }

    return err;
}

int ndp_irq_enable(void)
{
    int err = FSP_SUCCESS;

    /* Enable external irq */
    err = R_ICU_ExternalIrqEnable(&g_ndp_ext_irq05_ctrl);
    if (FSP_SUCCESS != err)
    {
        printf ("\r\n**R_ICU_ExternalIrqEnable API FAILED**\r\n");
    }

    return err;
}

int ndp_irq_disable(void)
{
    int err = FSP_SUCCESS;

    /* Enable external irq */
    err = R_ICU_ExternalIrqDisable(&g_ndp_ext_irq05_ctrl);
    if (FSP_SUCCESS != err)
    {
        printf ("\r\n**R_ICU_ExternalIrqDisable API FAILED**\r\n");
    }

    return err;
}

void ndp_icu_deinit(void)
{
    R_ICU_ExternalIrqClose(&g_ndp_ext_irq05_ctrl);
}

void ndp_irq_service(external_irq_callback_args_t *p_args)
{
    (void)p_args;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xEventGroupSetBitsFromISR(g_ndp_event_group, EVENT_BIT_VOICE, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

