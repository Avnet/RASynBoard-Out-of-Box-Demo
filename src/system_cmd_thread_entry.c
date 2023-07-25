#include "system_cmd_thread.h"
#include "led.h"
#include <stdio.h>
#include "fat_load.h"

#define   wifi_sleep()			R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_LOW)
#define   wifi_wakup()			R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_HIGH)

// Local globals
static uint32_t sleep_time = pdMS_TO_TICKS(500UL);
static uint32_t blink_time = pdMS_TO_TICKS(50UL);

static void r_lpm_sleep (void);
static void enter_low_power_mode (void);

/* SysCMD Thread entry function */
/* pvParameters contains TaskHandle_t */
void system_cmd_thread_entry(void *pvParameters)
{
	uint32_t led_event;
    FSP_PARAMETER_NOT_USED (pvParameters);

    // Check to see if we started up in low power mode.  If so, enter low power mode right away
    if(get_low_power_mode() == ALWAYS_ENTER_LP_MODE){

        printf("\n\n\nCall enter_low_power_mode()\n\n\n");
        enter_low_power_mode();
    }

    // Loop until we receive a new message on the g_led_queue
    while (1)
    {
        turn_led(BSP_LEDBLUE, BSP_LEDOFF);
        turn_led(BSP_LEDGREEN, BSP_LEDOFF);
        turn_led(BSP_LEDRED, BSP_LEDOFF);

		xQueueReceive(g_led_queue, &led_event, portMAX_DELAY);
		switch (led_event) {
			case LED_COLOR_RED:
				turn_led(BSP_LEDRED, BSP_LEDON);
				break;
			case LED_COLOR_GREEN:
				turn_led(BSP_LEDGREEN, BSP_LEDON);
				break;
			case LED_COLOR_BLUE:
				turn_led(BSP_LEDBLUE, BSP_LEDON);
				break;
			case LED_COLOR_YELLOW:
				turn_led(BSP_LEDGREEN, BSP_LEDON);
				turn_led(BSP_LEDRED, BSP_LEDON);
				break;
			case LED_COLOR_CYAN:
				turn_led(BSP_LEDBLUE, BSP_LEDON);
				turn_led(BSP_LEDGREEN, BSP_LEDON);
				break;
			case LED_COLOR_MAGENTA:
				turn_led(BSP_LEDRED, BSP_LEDON);
				turn_led(BSP_LEDBLUE, BSP_LEDON);
				break;
			case LED_BLINK_DOUBLE_BLUE:
	            enter_low_power_mode ();
				break;
			default :
				break;
		    }

		// If we're in low power mode, go back to low power
		// after each feature detection
		if(get_low_power_mode() == ALWAYS_ENTER_LP_MODE){

		    enter_low_power_mode ();
		}
        vTaskDelay (sleep_time);
    }
}

static void r_lpm_sleep (void)// also software standby mode
{
    fsp_err_t err = R_LPM_Open(&g_lpm0_ctrl, &g_lpm0_cfg);
    /* Handle any errors. This function should be defined by the user. */
    assert(FSP_SUCCESS == err);
    err = R_LPM_LowPowerModeEnter(&g_lpm0_ctrl);
    assert(FSP_SUCCESS == err);
}

static void enter_low_power_mode (void)
{
    // Sleep to allow the detection LED to light before powering down
    vTaskDelay (sleep_time);

    turn_led(BSP_LEDBLUE, BSP_LEDOFF);
    turn_led(BSP_LEDGREEN, BSP_LEDOFF);
    turn_led(BSP_LEDRED, BSP_LEDOFF);

    printf("Enter sleep mode\n");
    /* two rapid blinks */
    turn_led(BSP_LEDBLUE, BSP_LEDON);
    vTaskDelay (blink_time);
    turn_led(BSP_LEDBLUE, BSP_LEDOFF);
    vTaskDelay (blink_time);
    turn_led(BSP_LEDBLUE, BSP_LEDON);
    vTaskDelay (blink_time);
    turn_led(BSP_LEDBLUE, BSP_LEDOFF);
    /* Enter sleep mode */
    vTaskDelay (2000);
    R_USB_Close (&g_basic_ctrl);/* close usb */
    wifi_sleep();
    r_lpm_sleep();
    wifi_wakup();
    R_USB_Open (&g_basic_ctrl, &g_basic_cfg);/* open usb */
}
