/*
 * DA9231.h
 *
 *  Created on: Dec 14, 2022
 *      Author: 047759
 */

#ifndef DA9231_H_
#define DA9231_H_

// Buck and LDO Control
#define DA9231_REG_EVENT             0x00
#define DA9231_REG_STATUS            0x02
#define DA9231_REG_MASK              0x03
#define DA9231_REG_GPO               0x04
#define DA9231_REG_BUCK              0x05
#define DA9231_REG_BUCK_CFG          0x06
#define DA9231_REG_LS_LDO            0x07
#define DA9231_REG_FAULT_CTL         0x08
#define DA9231_REG_PIN_MONTOR        0x0A

// System Module
#define DA9231_REG_SYS_RST_EVENT     0x01
#define DA9231_REG_SYS_SRST          0x09
#define DA9231_REG_SYS_DEVICE_ID     0x80
#define DA9231_REG_SYS_VARIANT_ID    0x81
#define DA9231_REG_SYS_CONFIG_ID     0x82

#define BUCK_VOLT        0x18  // 1.8V
#define VOUT_RANGE_HI    0x40  // 1.30 V <= VBUCK <= 1.90 V
#define BUCK_EN          0x80

#define EN_LS_LDO        0x80
#define LS_LDO           0x04  // 1.1V

#define SRST             0x05
#define RESET_EVENT      0x01

int DA9231_open(void);
void DA9231_close(void);
int DA9231_enable(void);
int DA9231_disable(void);
int DA9231_dump_regs(void);


#endif /* DA9231_H_ */
