/*
 * ndp_irq_service.h
 *
 *  Created on: Dec 22, 2022
 *      Author: 047759
 */

#ifndef NDP_IRQ_SERVICE_H_
#define NDP_IRQ_SERVICE_H_

/* Generic bit definitions. */
#define BIT_0          ( 0x01 )
#define BIT_1          ( 0x02 )
#define BIT_2          ( 0x04 )
/* The bits in the event group used to signal interrupt events to NDP task. */
#define  EVENT_BIT_VOICE          BIT_0

int ndp_irq_init(void);
int ndp_irq_enable(void);
void ndp_icu_deinit(void);

#endif /* NDP_IRQ_SERVICE_H_ */
