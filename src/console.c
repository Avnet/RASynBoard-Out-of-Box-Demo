/*
 * console.c
 *
 *  Created on: 2022年11月24日
 *      Author: Wenxue
 */

#include "console.h"

volatile bool           g_console_txComplete = false; /* Tx complete flags */

fsp_err_t console_init(void)
{
#ifdef DEBUG_CONSOLE_UART
    fsp_err_t err = FSP_SUCCESS;

    err = R_SCI_UART_Open (g_console_ctrl, g_console_cfg);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
#endif
    return FSP_SUCCESS;
}

fsp_err_t console_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err =  R_SCI_UART_Close (g_console_ctrl);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

void console_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
            g_console_txComplete = true;
            break;

        default:
            break;
    }
}

/* redirecting output */
#ifdef DEBUG_CONSOLE_UART
int _write(int fd, char *pBuffer, int size)
{
    fsp_err_t              err = FSP_SUCCESS;

    (void)fd;

    g_console_txComplete = false;
    err = R_SCI_UART_Write(g_console_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    if(FSP_SUCCESS != err)
        __BKPT();

    while(g_console_txComplete == false)
    {
    }
    return size;
}
#endif

