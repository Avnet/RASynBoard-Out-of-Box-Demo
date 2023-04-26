#include "usb_pcdc_vcom.h"

//#define USB_COM_ECHO

#define LINE_CODING_LENGTH          (0x07U)
#define READ_BUF_SIZE               (8U)
#define DATA_LEN                       (64U)

#define TASK_STACK_SIZE_BYTE        (8192U)
#define DATA_BUFFER_LENGTH_BYTES    (128U)

/* Buffer to store user data */
uint8_t user_data_buf[DATA_LEN] = {0};
/* Flag to indicate USB resume/suspend status */
static bool  b_usb_attach = false;
/* Variable to store UART settings */
volatile usb_pcdc_linecoding_t g_line_coding = { 0, 0, 0, 8, 0 };

/* FreeRTOS module */
TaskHandle_t usb_pcdc_thread;
SemaphoreHandle_t g_usb_semaphore;
StreamBufferHandle_t g_streambuffer;
static portTASK_FUNCTION_PROTO(usb_pcdc_thread_entry, pvParameters );

/* print by USB VCOM */
int usb_pcdc_print(char *pBuffer, int size)
{
	if (true == b_usb_attach)
	{
		xStreamBufferSend( g_streambuffer, ( void * )pBuffer, size,  0 );
	}

    return size;
}


void start_usb_pcdc_thread( void )
{
	/* create a task to send data via usb */
	xTaskCreate(usb_pcdc_thread_entry,
		(const char*) "USB PCDC Thread", 
		TASK_STACK_SIZE_BYTE / 4, // in words
		NULL, //pvParameters
		configMAX_PRIORITIES - 2, //uxPriority
		&usb_pcdc_thread);
	/* create a emaphore with a count value of 1 */
	g_usb_semaphore = xSemaphoreCreateCounting( 1, 1 );
	/* create stream buffea stream buffer to save data */
    g_streambuffer = xStreamBufferCreate(DATA_BUFFER_LENGTH_BYTES, 3 );
    //vTaskDelay (10);
}

/* USB PCDC Thread entry function */
void usb_pcdc_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
	fsp_err_t err = FSP_SUCCESS;
	uint8_t   rcv_data[DATA_BUFFER_LENGTH_BYTES];
	size_t rcv_len;
	
	/* Open USB instance */
	err = R_USB_Open (&g_basic_ctrl, &g_basic_cfg);
	if (FSP_SUCCESS != err)
	{
		return ;
	}

    while (1) {
		/* Wait for previous USB transfer to complete */
		xSemaphoreTake (g_usb_semaphore, 10000);
		/* Wait for reading data from Stream Buffer */
		rcv_len = xStreamBufferReceive(g_streambuffer, ( void * ) rcv_data, sizeof(rcv_data ), portMAX_DELAY );
		if (rcv_len > 0)
		{
			rcv_data[rcv_len] = '\0';
			/* Write data to host machine */
			R_USB_Write (&g_basic_ctrl, (uint8_t *)rcv_data, (uint32_t)rcv_len, USB_CLASS_PCDC);
		}
        vTaskDelay (2);
    }
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize initialized USB instance
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close module */
    err = R_USB_Close (&g_basic_ctrl);
    if (FSP_SUCCESS != err)
    {
    }
}

/*******************************************************************************************************************//**
 * @brief       USB PCDC with freertos callback
 * @param[IN]   usb_event_info_t    usb event
 * @param[IN]   usb_hdl_t           task
 * @param[IN]   usb_onoff_t         state
 * @retval      None
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_pcdc_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (task);
    FSP_PARAMETER_NOT_USED (state);

    switch (p_pcdc_event->event)
    {
        /* USB Read complete Class Request */
        case USB_STATUS_READ_COMPLETE:
        {
            if(b_usb_attach)
            {
                R_USB_Read (&g_basic_ctrl, user_data_buf, READ_BUF_SIZE, USB_CLASS_PCDC);
#ifdef USB_COM_ECHO
				/* Echo the received data */
				xStreamBufferSend( g_streambuffer, ( void * ) user_data_buf, p_pcdc_event->data_size,  0 );
#endif
            }
        }
        break;

        /*  Write Complete Class Request */
        case USB_STATUS_WRITE_COMPLETE:
        {
			xSemaphoreGive(g_usb_semaphore);
        }
        break;

		/* Configured state class request */
        case USB_STATUS_CONFIGURED:
        {
            /* Read data from tera term */
            fsp_err_t err = R_USB_Read (&g_basic_ctrl, user_data_buf, DATA_LEN, USB_CLASS_PCDC);
            if (FSP_SUCCESS != err)
            {
                deinit_usb();
            }
            return;
        }
        break;

        /* Receive Class Request */
        case USB_STATUS_REQUEST:
        {
            /* Check for the specific CDC class request IDs */
            if (USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Get the class request.*/
                fsp_err_t err = R_USB_PeriControlDataGet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
                if(FSP_SUCCESS == err)
                {
                }
            }

            else if (USB_PCDC_GET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Set the class request.*/
                fsp_err_t err = R_USB_PeriControlDataSet (&g_basic_ctrl, (uint8_t*) &g_line_coding, LINE_CODING_LENGTH);
                if (FSP_SUCCESS == err)
                {
                }
                else
                {
                    deinit_usb();
                }
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                /* Set the usb status as ACK response.*/
                fsp_err_t err = R_USB_PeriControlStatusSet (&g_basic_ctrl, USB_SETUP_STATUS_ACK);
                if (FSP_SUCCESS != err)
                {
                    deinit_usb();
                }
            }
            else
            {
            }
            return;
        }
        break;
        /* Complete Class Request */
        case USB_STATUS_REQUEST_COMPLETE:
        {
            if(USB_PCDC_SET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
            }
            else if (USB_PCDC_GET_LINE_CODING == (p_pcdc_event->setup.request_type & USB_BREQUEST))
            {
            }
            else
            {
            }
            return;
        }
        break;

        /* Detach, Suspend State Class requests */
        case USB_STATUS_DETACH:
            /* Stop all read/write transactions using R_USB_Stop */
        case USB_STATUS_SUSPEND:
        {
            /* Reset the usb attached flag indicating usb is removed.*/
            b_usb_attach = false;
            memset (user_data_buf, 0, sizeof(user_data_buf));
            return;
        }
        /* Resume state */
        case USB_STATUS_RESUME:
        {
            /* set the usb attached flag*/
            b_usb_attach = true;
        }
        break;

        default:
            break;
    }
}
