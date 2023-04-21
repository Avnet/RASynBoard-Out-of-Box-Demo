/*
 * bsp_led.h
 *
 *  Created on: Dec 17, 2022
 *      Author: 047759
 */

#ifndef BSP_LED_H_
#define BSP_LED_H_

/** Information on how many LEDs and what pins they are on. */
typedef struct st_bsp_leds
{
    uint16_t         led_count;        ///< The number of LEDs on this board
    uint16_t const * p_leds;           ///< Pointer to an array of IOPORT pins for controlling LEDs
} bsp_leds_t;

/** Available user-controllable LEDs on this board. These enums can be can be used to index into the array of LED pins
 * found in the bsp_leds_t structure. */
typedef enum e_bsp_led
{
    BSP_LEDBLUE,                      ///< LED1
    BSP_LEDGREEN,                     ///< LED2
    BSP_LEDRED,                       ///< LED2
} bsp_led_t;

/** Available user-controllable LEDs on this board. These enums can be used to turn on/off LED. */
typedef enum e_bsp_led_status
{
    BSP_LEDOFF,                      ///< Turn off LED
    BSP_LEDON,                       ///< Turn on  LED
} bsp_led_status_t;

typedef enum queue_led_event
{
    LED_COLOR_RED = 1,
    LED_COLOR_GREEN = 2,
    LED_COLOR_BLUE = 3,
    LED_COLOR_YELLOW= 10,
    LED_COLOR_CYAN = 11,
    LED_COLOR_MAGENTA = 12,
    LED_BLINK_DOUBLE_BLUE = 20,
    LED_EVENT_NONE,
} queue_led_event_t;

extern const bsp_leds_t g_bsp_leds;

extern void turn_led(bsp_led_t which, bsp_led_status_t status);


#endif /* BSP_LED_H_ */
