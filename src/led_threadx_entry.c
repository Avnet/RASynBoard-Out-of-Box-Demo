#include "led_threadx.h"
/* LED IDLE Thread entry function */
/* pvParameters contains TaskHandle_t */
void led_threadx_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    R_BSP_PinAccessEnable(); /* Enable access to the PFS registers. */
    while (1)
    {
        R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_LOW);
        vTaskDelay (300);
        R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_HIGH);
        vTaskDelay (300);
    }
}
