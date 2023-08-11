/*
 * rm_atcmd.h
 *
 *  Created on: 2022年12月8日
 *      Author: Wenxue
 */

#ifndef RM_ATCMD_H_
#define RM_ATCMD_H_

#include <rm_wifi_api.h>
#include "r_sci_uart.h"
#include "hal_data.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "event_groups.h"

#define ATBUF_SIZE              1536
#define ATCMD_SIZE              256  /* AT command/Reply message length */

#define EVENT_SYS_INIDONE       0x01 /* module init done or wakeup */
#define EVENT_SYS_DEAUTH        0x02 /* WiFi disconnect */
#define EVENT_SOCKC_CLOSE       0x04 /* Socket client mode, server disconnect */
#define EVENT_SOCKC_DATA        0x08 /* Socket client mode, server send data arrive */
#define EVENT_SOCKS_CLOSE       0x10 /* Socket server mode, client disconnect */
#define EVENT_SOCKS_DATA        0x20 /* Socket server mode, client send data arrive */

#define HTTPS_BUFFER_SIZE 2048
extern char httpsBuffer[HTTPS_BUFFER_SIZE];

// The bits in the event group used to signal when an extended https response has been received.
#define  EVENT_BIT_EXTENDED_MSG    (1 << 0)


extern int temperature;
extern int pressure;

fsp_err_t loadAWS_certificates(const char *cert);

fsp_err_t discovery();
fsp_err_t identity(char* identityURL);
fsp_err_t AWSmqttTest(char *Host, char *PubT, char *SubT, char *username);
char* buildAWSTelemetry();

int getTempSensor();
int getPressureSensor();

/* Event Group bits */
#define EVENT_BIT_ASYNC      1<<0 /* Asynchronous message */
#define EVENT_BIT_ATCMD      1<<0 /* AT command reply message */

typedef struct rm_atcmd_ctx_s
{
    uart_instance_t          xCom;                    /* UART instance */
    bool                     xComOpen;                /* UART opened or not */
    StreamBufferHandle_t     xUartRxBuffer;           /* UART receive stream buffer */
    SemaphoreHandle_t        xSemaphore;              /* UART receive over semaphore */
    TaskHandle_t             xAtcmdTask;              /* UART receive data parser task */
    StreamBufferHandle_t     xAsyncBuffer;            /* System event data stream buffer */
    char                     xAtCmd[256];             /* AT command buffer */
    char                     xAtReplyBuf[256];        /* AT command reply buffer */
    EventGroupHandle_t       xEventGroup;             /* UART receive data parser event group */
    uint8_t                  xEvents;                 /* UART receive data parser event */
}rm_atcmd_ctx_t;

extern rm_atcmd_ctx_t   g_atcmd_ctx;

#define check_event_flag(x)     g_atcmd_ctx.xEvents & (x)
#define clear_event_flag(x)     g_atcmd_ctx.xEvents &= (uint8_t)~(x)

/* Function declaration */
void rm_atcmd_uart_callback(uart_callback_args_t *p_args);
fsp_err_t rm_atcmd_init(void);
fsp_err_t rm_atcmd_deinit(void);


/* AT command reply message got expect or error string */
#define AT_OKSTR                 "\r\nOK\r\n"     /* expect string always be OK */
#define AT_ERRSTR                "\r\nERROR"      /* error string always be ERROR */

/* AT command should be end by $AT_SUFFIX */
#define AT_SUFFIX                "\r\n"

/*  Description: this function used to send an AT command from serial port and wait for reply message from
 *               the communication module, and it will return once get expet/error string or timeout.
 *
 *    Arugments:
 *              $at: the AT command need to be sent, without "\r\n"
 *         $timeout: wait for module reply for AT command timeout value, unit micro seconds(ms)
 *           $reply: the module reply message output buffer
 *            $size: the output buffer ($reply) size
 *
 */
fsp_err_t rm_atcmd_send(char *at, uint32_t timeout, char *reply, size_t size);


/*  Description: this function used to send user data
 *
 *    Arugments:
 *            $data: the user data need to be sent
 *             $len: the user data bytes
 */

fsp_err_t rm_data_send(uint8_t *data, uint32_t bytes);

/*
 *  Description: Send AT command and check reply by "OK" or "ERROR", such as AT:
 *                 Reply:   \r\nOK\r\n
 * Return Value: 0: OK     -X: Failure
 */
fsp_err_t rm_atcmd_check_ok(char *at, unsigned long timeout);

/*
 *  Description: Send AT command which will reply by the value  with a prefix "+CMD: " line, such as AT+CHIPNAME:
 *                  Reply:   \r\n+CHIPNAME:DA16600\r\nOK\r\n
 *
 * Return Value:  0: OK     -X: ERROR
 */
fsp_err_t rm_atcmd_check_value(char *at, unsigned long timeout, char *reply, size_t size);

#endif /* RM_ATCMD_H_ */
