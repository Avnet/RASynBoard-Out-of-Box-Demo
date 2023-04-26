#include "led_threadx.h"
#include <stdio.h>

#define User_Led_on()   R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_LOW)
#define User_Led_off()  R_BSP_PinWrite(LED_USER, BSP_IO_LEVEL_HIGH)

/* LED IDLE Thread entry function */
/* pvParameters contains TaskHandle_t */
void led_threadx_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    /* Wait for NDP120 to run successfully */
    xSemaphoreTake(g_binary_semaphore, portMAX_DELAY);
    printf("FreeRTOS led_thread running\n");

    xTimerStart(g_timer, 0);
    /* TODO: add your own code here */
    while (1)
    {
        xSemaphoreTake(g_binary_semaphore, portMAX_DELAY);
        User_Led_on();
        vTaskDelay (50);
        User_Led_off();
        vTaskDelay (50);

        User_Led_on();
        vTaskDelay (50);
        User_Led_off();
        vTaskDelay (50);

        User_Led_on();
        vTaskDelay (50);
        User_Led_off();
    }
}

void g_timer_callback(TimerHandle_t xTimer)
{
    static uint16_t tim = 0;

    if (tim % 2 ){
        User_Led_on();
        xTimerChangePeriod(g_timer, pdMS_TO_TICKS(200UL), 0);
    } else {
        User_Led_off();
        xTimerChangePeriod(g_timer, pdMS_TO_TICKS(2000UL), 0);
    }
    tim ++;
}
