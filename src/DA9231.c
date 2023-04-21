/*
 * DA9231.c
 *
 *  Created on: Dec 14, 2022
 *      Author: 047759
 */

#include <stdio.h>
#include "DA9231.h"
#include "ndp_thread.h"

#define RESET_VALUE         0x00

static int i2c_write(uint8_t *data, uint32_t bytes, bool const restart);
static int i2c_read(uint8_t *buf, uint32_t size, bool const restart);
static fsp_err_t validate_i2c_event(void);

int DA9231_open(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    /* Open I2C master */
    err = R_IIC_MASTER_Open(&g_i2c0_master_ctrl, &g_i2c0_master_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return 0;
}

void DA9231_close(void)
{
    R_IIC_MASTER_Close (&g_i2c0_master_ctrl);
    return ;
}

static int DA9231_reg_read(uint8_t addr, uint8_t * val)
{
    fsp_err_t err     = FSP_SUCCESS;
    uint8_t  temp;

    temp = addr;
    err = i2c_write(&temp, 1, true);
    if (FSP_SUCCESS != err) {printf("%s-%d", __func__, __LINE__); return err; }

    err = i2c_read(&temp, 1, false);
    if (FSP_SUCCESS != err) {printf("%s-%d", __func__, __LINE__); return err; }

    *val = temp;

    return FSP_SUCCESS;
}

static int DA9231_reg_write(uint8_t addr, uint8_t val)
{
    fsp_err_t err     = FSP_SUCCESS;
    uint8_t  temp[2];

    temp[0] = addr;
    temp[1] = val;
    err = i2c_write(temp, 2, false);
    if (FSP_SUCCESS != err) {printf("%s-%d", __func__, __LINE__); return err; }

    return FSP_SUCCESS;
}

int DA9231_dump_regs(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t i;
    uint8_t  temp;

    for(i=0; i<=0xA; i++)
    {
        temp = 0;
        err = DA9231_reg_read(i, &temp);
        printf("REG%x=%x\r\n", i, temp);
    }

#if 1
    // read SYS_DEVICE_ID    @ 0x80
    // read SYS_VARIANT_ID   @ 0x81
    // read SYS_CONFIG_ID    @ 0x82
    for(i=0x80; i<=0x82; i++)
    {
        temp = 0;
        err = DA9231_reg_read(i, &temp);
        printf("REG%x=%x\r\n", i, temp);
    }
#endif

    return err;
}


int  DA9231_enable(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    // Software reset
    err = DA9231_reg_write(DA9231_REG_SYS_SRST, SRST);

    // Clear reset flag
    // RESET event flag. After Reset, this bit is set. When I2C write '1' to this bit, this event flag is cleared.
    err = DA9231_reg_write(DA9231_REG_SYS_RST_EVENT, RESET_EVENT);

    // Set BUCK, 1.8V
    err = DA9231_reg_write(DA9231_REG_BUCK, BUCK_VOLT);
    // Enable BUCK
    err = DA9231_reg_write(DA9231_REG_BUCK, BUCK_EN | VOUT_RANGE_HI | BUCK_VOLT);

    // Set LDO 1.1V
    err = DA9231_reg_write(DA9231_REG_LS_LDO, LS_LDO);
    // Enable LDO
    err = DA9231_reg_write(DA9231_REG_LS_LDO, EN_LS_LDO | LS_LDO);

    return err;
}

int  DA9231_disable(void)
{
    fsp_err_t err     = FSP_SUCCESS;

    // Disable LDO 1.1
    err = DA9231_reg_write(DA9231_REG_LS_LDO, LS_LDO);

    // Disable BUCK 1.8
    err = DA9231_reg_write(DA9231_REG_BUCK, VOUT_RANGE_HI | BUCK_VOLT);

    return err;
}

/* Reading I2C call back event through i2c_Master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

void i2c0_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* capture callback event for validating the i2c transfer event*/
        i2c_event = p_args->event;
    }
}
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    i2c_event = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_event == RESET_VALUE);

    if(i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        i2c_event = (i2c_master_event_t)RESET_VALUE;  // Make sure this is always Reset before return
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t)RESET_VALUE; // Make sure this is always Reset before return
    return FSP_ERR_TRANSFER_ABORTED;
}

static int i2c_write(uint8_t *data, uint32_t bytes, bool const restart)
{
    fsp_err_t err     = FSP_SUCCESS;

    if( !data || bytes<=0 )
        return FSP_ERR_INVALID_ARGUMENT;

    err = R_IIC_MASTER_Write(&g_i2c0_master_ctrl, data, bytes, restart);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = validate_i2c_event();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}

static int i2c_read(uint8_t *buf, uint32_t size, bool const restart)
{
    fsp_err_t err     = FSP_SUCCESS;

    if( !buf || size<=0 )
        return FSP_ERR_INVALID_ARGUMENT;

    err = R_IIC_MASTER_Read(&g_i2c0_master_ctrl, buf, size, restart);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = validate_i2c_event();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return FSP_SUCCESS;
}

