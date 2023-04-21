/*
 * bottun.h
 *
 *  Created on: Dec 17, 2022
 *      Author: 047759
 */

#ifndef BOTTON_H_
#define BOTTON_H_

#define USER_BUTTON_IRQ_NUMBER 1

/* The bits in the event group used to signal interrupt events to NDP task. */
#define  EVENT_BIT_RISING         ( 0x10 )
#define  EVENT_BIT_FALLING         ( 0x20 )
#define  EVENT_BIT_FLASH         ( 0x02 )

extern int button_init(void);
extern void icu_deinit(void);

#endif /* BOTTON_H_ */
