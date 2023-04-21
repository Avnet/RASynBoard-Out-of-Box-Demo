/*
 * bsp_led.c
 *
 *  Created on: Dec 17, 2022
 *      Author: 047759
 */

#include "bsp_api.h"
#include "led.h"
#include "bsp_pin_cfg.h"

/** Array of LED IOPORT pins. */
static const uint16_t g_bsp_prv_leds[] =
{
    (uint16_t) LED_B,
    (uint16_t) LED_G,
    (uint16_t) LED_R,
};


/** Structure with LED information for this board. */
const bsp_leds_t g_bsp_leds =
{
    .led_count = (uint16_t) ((sizeof(g_bsp_prv_leds) / sizeof(g_bsp_prv_leds[0]))),
    .p_leds    = &g_bsp_prv_leds[0]
};

/** Function to turn Led on or off for this board. */
void turn_led(bsp_led_t which, bsp_led_status_t status)
{
    if(which >= g_bsp_leds.led_count )
    {
        return ;
    }

    if( BSP_LEDOFF == status )
    {
        R_BSP_PinWrite(g_bsp_leds.p_leds[which], BSP_IO_LEVEL_HIGH);
    }
    else
    {
        R_BSP_PinWrite(g_bsp_leds.p_leds[which], BSP_IO_LEVEL_LOW);
    }
}

