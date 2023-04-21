/*
 * ble_uart.c
 *
 *  Created on: 2022年11月24日
 *      Author: Wenxue
 */

#include "ble_uart.h"

volatile bool           g_uart3_txComplete = false; /* Tx complete flags */

fsp_err_t ble_uart_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_SCI_UART_Open (&g_uart3_ctrl, &g_uart3_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

fsp_err_t ble_uart_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err =  R_SCI_UART_Close (&g_uart3_ctrl);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

void ble_uart_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
            g_uart3_txComplete = true;
            break;

        default:
            break;
    }
}

int ble_send(char *pBuffer, int size)
{
    fsp_err_t              err = FSP_SUCCESS;

    g_uart3_txComplete = false;
    err = R_SCI_UART_Write(&g_uart3_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    if(FSP_SUCCESS != err)
        __BKPT();

    while(g_uart3_txComplete == false)
    {
    }
    return size;
}
