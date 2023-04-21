/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define LED_R (BSP_IO_PORT_00_PIN_01) /* red led */
#define CS_NDP (BSP_IO_PORT_01_PIN_03) /* SPI_NDP_CS */
#define CS_SD (BSP_IO_PORT_01_PIN_04) /* SPI_uSD_CS */
#define DA16600_RstPin (BSP_IO_PORT_01_PIN_06) /* WLBT_RST */
#define LED_B (BSP_IO_PORT_01_PIN_11) /* blue led */
#define LED_USER (BSP_IO_PORT_01_PIN_12) /* Green LED on core board */
#define LED_G (BSP_IO_PORT_02_PIN_07) /* green led */
extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA6M4AF3CFM.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif /* BSP_PIN_CFG_H_ */
