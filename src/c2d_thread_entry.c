#include "c2d_thread.h"
#include "rm_atcmd.h"
#include "fat_load.h"
#include <stdio.h>

/* C2D Thread entry function */
/* pvParameters contains TaskHandle_t */
void c2d_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    c2dQueueMsg_t newMsg;

    // If we're not connecting to a cloud solution, then we don't need this thread.
    if(CLOUD_NONE == get_target_cloud()){

        vTaskDelete(NULL);
    }


    // Spin on the Cloud 2 Device message queue
    while (pdTRUE)
    {

        // Incomming message looks like . . .
        // "AT+NWMQMSG='{\"msgCount\": 0, \"inferenceIdx\": 0, \"inferenceStr\": \"NN0:ok-syntiant\"}'\r\r\nOK\r\n\n", '\0'

        // Pull the data from the queue
        xQueueReceive(g_c2d_queue, &newMsg, portMAX_DELAY);
        printf("RX C2D message!\n");
        printf("%d: %s\n",newMsg.msgLen, newMsg.msgPtr);


        // Free the message memory
        vPortFree((void*) newMsg.msgPtr);

        vTaskDelay(100);
    }
}
