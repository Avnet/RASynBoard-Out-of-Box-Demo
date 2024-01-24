/*
 * rm_atcmd.c
 *
 *  Created on: 2022年12月8日
 *      Author: Wenxue
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <rm_atcmd.h>

//#define DEBUG_ATCMD_PRINT
#ifdef  DEBUG_ATCMD_PRINT
#define atcmd_print(format,args...) printf(format, ##args)
#else
#define atcmd_print(format,args...) do{} while(0);
#endif

rm_atcmd_ctx_t          g_atcmd_ctx;
static volatile bool    s_atmodule_txComplete = false; /* Tx complete flags */
char httpsBuffer[HTTPS_BUFFER_SIZE]={'\0'};

static void atcmd_thread_entry(void *pvParameters);

fsp_err_t rm_atcmd_init(void)
{
    fsp_err_t         err = FSP_SUCCESS;
    BaseType_t        rv;

    memset(&g_atcmd_ctx, 0, sizeof(g_atcmd_ctx));
    g_atcmd_ctx.xCom = g_uart3;

    err = R_SCI_UART_Open (g_atcmd_ctx.xCom.p_ctrl, g_atcmd_ctx.xCom.p_cfg);
    if( FSP_SUCCESS != err )
        return err;

    /* Create UART receive stream buffer */
    g_atcmd_ctx.xUartRxBuffer = xStreamBufferCreate(ATBUF_SIZE, 1);
    if( !g_atcmd_ctx.xUartRxBuffer )
    {
        err = FSP_ERR_OUT_OF_MEMORY;
        goto cleanup;
    }

    /* Create asynchronous message receive stream buffer */
    g_atcmd_ctx.xAsyncBuffer = xStreamBufferCreate(ATBUF_SIZE, 1);
    if( !g_atcmd_ctx.xAsyncBuffer )
    {
        err = FSP_ERR_OUT_OF_MEMORY;
        goto cleanup;
    }

    /* Create UART receive over semaphore */
    g_atcmd_ctx.xSemaphore = xSemaphoreCreateBinary();
    if( !g_atcmd_ctx.xSemaphore )
    {
        err = FSP_ERR_OUT_OF_MEMORY;
        goto cleanup;
    }

    /* Create UART parser event group */
    g_atcmd_ctx.xEventGroup = xEventGroupCreate();
    if( !g_atcmd_ctx.xEventGroup )
    {
        err = FSP_ERR_OUT_OF_MEMORY;
        goto cleanup;
    }

    rv = xTaskCreate( atcmd_thread_entry,             // Task function
                      (const char*) "ATCMD Thread",   // Task name
                      2048 / 4,                       // Task stack depth in word(4B)
                      NULL,                           // Task parameters
                      1,                              // Task Priority
                     &g_atcmd_ctx.xAtcmdTask );       // Task Handler

    if( pdPASS != rv )
    {
        err = FSP_ERR_INTERNAL;
        goto cleanup;
    }

    vTaskDelay (500);

    // Initialize the extended message bit
    xEventGroupClearBits(g_https_extended_msg_event_group, EVENT_BIT_EXTENDED_MSG);


//    printf("AT command initial and create task okay\r\n");
    g_atcmd_ctx.xComOpen = true;

    return FSP_SUCCESS;

cleanup:
    if( g_atcmd_ctx.xEventGroup )
        vEventGroupDelete(g_atcmd_ctx.xEventGroup);

    if( g_atcmd_ctx.xSemaphore )
        vSemaphoreDelete(g_atcmd_ctx.xSemaphore);

    if (g_atcmd_ctx.xAsyncBuffer )
        vStreamBufferDelete(g_atcmd_ctx.xAsyncBuffer);

    if (g_atcmd_ctx.xUartRxBuffer )
        vStreamBufferDelete(g_atcmd_ctx.xUartRxBuffer);

    R_SCI_UART_Close(g_atcmd_ctx.xCom.p_ctrl);
    memset(&g_atcmd_ctx, 0, sizeof(g_atcmd_ctx));

    return err;
}

fsp_err_t rm_atcmd_deinit(void)
{
    if( !g_atcmd_ctx.xComOpen )
        return FSP_ERR_NOT_OPEN;

    vTaskDelete(g_atcmd_ctx.xAtcmdTask);

    if( g_atcmd_ctx.xEventGroup )
        vEventGroupDelete(g_atcmd_ctx.xEventGroup);

    if( g_atcmd_ctx.xSemaphore )
        vSemaphoreDelete(g_atcmd_ctx.xSemaphore);

    if (g_atcmd_ctx.xAsyncBuffer )
        vStreamBufferDelete(g_atcmd_ctx.xAsyncBuffer);

    if (g_atcmd_ctx.xUartRxBuffer )
        vStreamBufferDelete(g_atcmd_ctx.xUartRxBuffer);

    R_SCI_UART_Close (g_atcmd_ctx.xCom.p_ctrl);
    memset(&g_atcmd_ctx, 0, sizeof(g_atcmd_ctx));

    return FSP_SUCCESS;
}

void rm_atcmd_uart_callback(uart_callback_args_t *p_args)
{
    BaseType_t      xHigherPriorityTaskWoken;
    char            data_byte;

    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
            s_atmodule_txComplete = true;
            break;

        case UART_EVENT_RX_CHAR:
            data_byte = (char)p_args->data;

            if( xStreamBufferSpacesAvailable(g_atcmd_ctx.xUartRxBuffer) > 0 )
                xStreamBufferSendFromISR(g_atcmd_ctx.xUartRxBuffer, &data_byte, 1, &xHigherPriorityTaskWoken);

            if('\n' == data_byte)
                xSemaphoreGiveFromISR(g_atcmd_ctx.xSemaphore, &xHigherPriorityTaskWoken );

            /* context switch will be performed if xHigherPriorityTaskWoken is equal to pdTRUE.  */
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            break;

        default:
            break;
    }
}

typedef enum
{
    TYPE_EVENT,
    TYPE_DATA,
} msg_type_t;

static inline int parser_async_message(char *buf, char *keystr, msg_type_t type)
{
    char                  *ptr, *end;
    int                    bytes = 0;

    if( ! (ptr=strstr(buf, keystr)) )
        return 0;

    xEventGroupSetBits(g_atcmd_ctx.xEventGroup, EVENT_BIT_ASYNC);

    if(TYPE_DATA == type)
    {
        if( !(end=strstr(ptr+strlen(keystr), "\r\n")) )
            return 0;

        bytes = end - ptr + 2;

        if( xStreamBufferSpacesAvailable(g_atcmd_ctx.xAsyncBuffer) >= (size_t)bytes )
            xStreamBufferSend(g_atcmd_ctx.xAsyncBuffer, buf, (size_t)bytes, 0);
    }

    return 1;
}

static inline int parser_atcmd_message(char *buf)
{
    char                  *ptr, *end;
    int                   bytes = 0;
    size_t                i;

    if( '\0' == g_atcmd_ctx.xAtCmd[0] )
    {
        /* No AT command send */
        return 0;
    }

    /* AT echo command without '\n' */
    for( i=0; i<strlen(g_atcmd_ctx.xAtCmd); i++ )
    {
        if( g_atcmd_ctx.xAtCmd[i] == '\n' )
        {
            g_atcmd_ctx.xAtCmd[i] = '\0';
            break;
        }
    }

    /* Must set AT command echo on */
    if( !(ptr=strstr(buf, g_atcmd_ctx.xAtCmd)) )
    {
        return 0;
    }

    /* Normal response  :  <CR><LF>OK<CR><LF>
     * Error response   :  <CR><LF>ERROR:<error code><CR><LF>
     * Extended response:  <CR><LF>+XXX:[value1],[value2],…<CR><LF>OK<CR><LF>
     */
    if( (end=strstr(ptr, "\r\nOK\r\n")) )
    {
        end += strlen("\r\nOK\r\n");
        bytes = end - ptr;
    }
    else if( (end=strstr(ptr, "\r\nERROR")) )
    {
        if( (end=strstr(ptr, "\r\n")) )
        {
            end+=2;
            bytes = end - ptr;
        }
    }

    if( bytes )
    {
        strncpy(g_atcmd_ctx.xAtReplyBuf, ptr, (size_t)bytes);
        atcmd_print("Got AT reply: %s", g_atcmd_ctx.xAtReplyBuf);
        xEventGroupSetBits(g_atcmd_ctx.xEventGroup, EVENT_BIT_ATCMD);
        return 1;
    }
    return 0;
}

static void atcmd_thread_entry(void *pvParameters)
{
    uint32_t               bytes = 0;
    uint32_t               last_bytes = 0;
    char                   buf[ATBUF_SIZE];
    c2dQueueMsg_t newC2dMsg;

    FSP_PARAMETER_NOT_USED (pvParameters);

    printf("AT Command Thread Start running...\r\n");

    while(1)
    {
        /* Wait ISR receive data and send semaphore */
        xSemaphoreTake( g_atcmd_ctx.xSemaphore, portMAX_DELAY);

        last_bytes = xStreamBufferBytesAvailable(g_atcmd_ctx.xUartRxBuffer);
        if( !last_bytes )
        {
            printf("No data arrive but receive semaphore\r\n");
            continue;
        }

WAIT4_NEWDATA:
        /* Wait for 10ms to check got new data arrive or not */
        xSemaphoreTake( g_atcmd_ctx.xSemaphore, pdMS_TO_TICKS(10) );
        bytes = xStreamBufferBytesAvailable(g_atcmd_ctx.xUartRxBuffer);

        /* still got new data arrive in 10ms, receive again */
        if( bytes != last_bytes )
        {
            last_bytes = bytes;
            goto WAIT4_NEWDATA;
        }

        /* No new data arrive in 10ms, start parser arrive data now */

        bytes = bytes>ATBUF_SIZE ? ATBUF_SIZE : bytes;
        memset(buf, 0, ATBUF_SIZE);
        xStreamBufferReceive(g_atcmd_ctx.xUartRxBuffer, buf, bytes, pdMS_TO_TICKS(50));

#if 0 /* Don't care this event */
        /* module init done or wakeup: +INIT:DONE,0 */
        if( parser_async_message(buf, "\r\n+INIT:", TYPE_EVENT) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SYS_INIDONE;
        }
#endif

        /* got https response with some data */
        if( parser_async_message(buf, "\r\n+NWHTCDATA:", TYPE_EVENT) )
        {
            memset(httpsBuffer,0,HTTPS_BUFFER_SIZE);
            memcpy(httpsBuffer,buf,ATBUF_SIZE);
            xEventGroupSetBits(g_https_extended_msg_event_group, EVENT_BIT_EXTENDED_MSG);
            vTaskDelay(10);
        }

        // Process new Cloud to Device (c2d) message
        if( parser_async_message(buf, "\r\n+NWMQMSG:", TYPE_EVENT) )
        {

//            printf("RX C2D Message\n");

            memset(httpsBuffer,0,HTTPS_BUFFER_SIZE);
            memcpy(httpsBuffer,buf,strlen(buf));

            // Populate new queue node
            newC2dMsg.msgLen = strlen(httpsBuffer);
            newC2dMsg.msgPtr = pvPortMalloc(newC2dMsg.msgLen);

            // Verify we got memory from the heap
            if(NULL != newC2dMsg.msgPtr){

                // Copy the topic String to the heap memory and enqueu the new node
                strncpy(newC2dMsg.msgPtr, httpsBuffer, newC2dMsg.msgLen);
                xQueueSend(g_c2d_queue, (void *)&newC2dMsg, 0U);
            }

            vTaskDelay(10);

        }

        /* WiFi disconnect: +WFDAP:0,DEAUTH */
        if( parser_async_message(buf, "\r\n+WFDAP:", TYPE_EVENT) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SYS_DEAUTH;
        }

        /* Socket client mode, got server disconnect:  +TRXTC:1,192.168.2.58,12345  */
        if( parser_async_message(buf, "\r\n+TRXTC:", TYPE_EVENT) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SOCKC_CLOSE;
        }

        /* Socket client mode, server send data arrive: +TRDTC:1,192.168.2.58,12345,21,Hello, Socket Client! */
        if( parser_async_message(buf, "\r\n+TRDTC:", TYPE_DATA) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SOCKC_DATA;
        }

        /* Socket server mode, got client disconnect:  +TRCTS:0,192.168.2.58,50522 */
        if( parser_async_message(buf, "\r\n+TRXTS:", TYPE_DATA) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SOCKS_CLOSE;
        }

        /* Socket server mode, got client data arrive:  +TRDTS:0,192.168.2.58,50541,19,Hello from Client1! */
        if( parser_async_message(buf, "\r\n+TRDTS:", TYPE_DATA) )
        {
            g_atcmd_ctx.xEvents |= EVENT_SOCKS_CLOSE;
        }

        parser_atcmd_message(buf);  /* Parser AT command reply message */
    }
}


fsp_err_t rm_data_send(uint8_t *data, uint32_t bytes)
{
    fsp_err_t              err = FSP_SUCCESS;

    /* send user data */
    err = R_SCI_UART_Write(g_atcmd_ctx.xCom.p_ctrl, data, bytes);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* wait user data send over */
    while(s_atmodule_txComplete == false)
    {
    }

    return FSP_SUCCESS;
}


fsp_err_t rm_atcmd_send(char *at, uint32_t timeout, char *reply, size_t size)
{
    fsp_err_t              err = FSP_SUCCESS;
    EventBits_t            evbits;
    char                  *ptr, *end;

    if( !at || !strlen(at) )
        return FSP_ERR_INVALID_ARGUMENT;

    if( !g_atcmd_ctx.xComOpen )
        return FSP_ERR_NOT_OPEN;

    atcmd_print("Send AT command: %s\r\n", at);
    snprintf(g_atcmd_ctx.xAtCmd, ATCMD_SIZE, "%s%s", at, AT_SUFFIX);
    memset(g_atcmd_ctx.xAtReplyBuf, 0, ATCMD_SIZE);
    s_atmodule_txComplete = false;
    err = R_SCI_UART_Write(g_atcmd_ctx.xCom.p_ctrl, (uint8_t *)g_atcmd_ctx.xAtCmd, strlen(g_atcmd_ctx.xAtCmd));
    if( FSP_SUCCESS != err )
    {
        goto out;
    }

    /* Wait AT command send over */
    while(s_atmodule_txComplete == false)
    {
        vTaskDelay(50);
    }

    /* Wait AT command reply message */
    evbits = xEventGroupWaitBits(g_atcmd_ctx.xEventGroup, EVENT_BIT_ATCMD, pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));
    //printf("AT COMMAND REPLY BUF:%s\r\n",g_atcmd_ctx.xAtReplyBuf);
    if( !(evbits & EVENT_BIT_ATCMD) )
    {
        err = FSP_ERR_TIMEOUT;
        goto out;
    }
    /* only set echo on, reply will contains the AT command */
    ptr = strstr(g_atcmd_ctx.xAtReplyBuf, at);
    if( !ptr )
    {
        err = FSP_ERR_NOT_FOUND;
        goto out;
    }

    ptr += strlen(at);

    if( NULL != (end=strstr(ptr, AT_OKSTR)) )
    {
        if( reply && size>0 )
        {
            end += strlen(AT_OKSTR);
            memset(reply, 0, size);

            strncpy(reply, ptr, (size_t)(end-ptr));
            atcmd_print("AT command '%s' got reply: %s", at, reply);
        }

        err = FSP_SUCCESS;
        goto out;
    }

    if( NULL != (end=strstr(ptr, "\r\nERROR:")) )
    {
        err = FSP_ERR_INVALID_MODE;
        goto out;
    }

out:
    memset(g_atcmd_ctx.xAtCmd, 0, sizeof(g_atcmd_ctx.xAtCmd));
    memset(g_atcmd_ctx.xAtReplyBuf, 0, sizeof(g_atcmd_ctx.xAtReplyBuf));
    return err;
}

/*
 *  Description: Send AT command which will only reply by "OK" or "ERROR", such as AT:
 *                 Reply:   \r\nOK\r\n
 * Return Value: 0: OK     -X: Failure
 */
fsp_err_t rm_atcmd_check_ok(char *at, unsigned long timeout)
{
    return rm_atcmd_send(at, timeout, NULL, 0);
}


/*
 *  Description: Send AT command which will reply by the value  with a prefix "+CMD: " line, such as AT+CHIPNAME:
 *                  Reply:   \r\n+CHIPNAME:DA16600\r\nOK\r\n
 *
 * Return Value:  0: OK     -X: ERROR
 */
fsp_err_t rm_atcmd_check_value(char *at, unsigned long timeout, char *reply, size_t size)
{
    uint32_t                i = 0, len;
    int                     rv;
    char                    buf[ATCMD_SIZE];
    char                   *ptr, *end;

    if( !at || !reply || size<=0 )
        return FSP_ERR_INVALID_ARGUMENT;

    rv = rm_atcmd_send(at, timeout, buf, sizeof(buf));
    if( rv  )
    {
        return rv;
    }

    ptr=strchr(buf, '+');  /* found '+' before the value */
    if( !ptr )
        return FSP_ERR_NOT_FOUND;

    ptr++;   /* skip '+'  */

    ptr=strchr(buf, ':');  /* found ':' before the value */
    if( !ptr )
        return FSP_ERR_NOT_FOUND;

    ptr++;   /* skip ':'  */

    end = strstr(ptr, "\r\nOK");
    if( !end )
        return FSP_ERR_NOT_FOUND;

    memset(reply, 0, size);

    if( ! (len=strlen(ptr)) )
        return FSP_SUCCESS;


    while( ptr != end )
    {
        if( *ptr!='\"') /* remove " ...  */
            reply[i++] = *ptr;
        ptr ++;
    }

    return FSP_SUCCESS;
}
