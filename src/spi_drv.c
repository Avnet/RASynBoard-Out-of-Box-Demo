/*
 * spi_drv.c
 *
 *  Created on: Dec 18, 2022
 *      Author: david
 */

#include <spi_drv.h>
#include <stdio.h>
#include "r_spi_api.h"
#include "hal_data.h"


#define RESET_VALUE         0x00

static volatile spi_event_t spi_event;    // Master Transfer Event completion flag

void spi_deassert_sd(void){
    R_BSP_PinWrite(CS_NDP, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(CS_SD, BSP_IO_LEVEL_HIGH);
}

void spi_deassert_ndp(void)
{
    R_BSP_PinWrite(CS_SD, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(CS_NDP, BSP_IO_LEVEL_HIGH);
}

void spi_assert_sd(void){
    R_BSP_PinWrite(CS_NDP, BSP_IO_LEVEL_HIGH);
    R_BSP_PinWrite(CS_SD, BSP_IO_LEVEL_LOW);
}

void spi_assert_ndp(void)
{
    R_BSP_PinWrite(CS_SD, BSP_IO_LEVEL_HIGH);
    //R_BSP_PinWrite(CS_SD, BSP_IO_LEVEL_LOW);// see by logical analyzer
    R_BSP_PinWrite(CS_NDP, BSP_IO_LEVEL_LOW);
}


int spi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_SPI_Open (&g_spi1_master_ctrl, &g_spi1_master_cfg);
    if (FSP_SUCCESS != err)
    {
        printf("** R_SPI_Open API for SPI Master failed ** \r\n");
        return err;
    }

    spi_deassert_sd();
    spi_deassert_ndp();

    return err;
}

void spi1_master_callback(spi_callback_args_t * p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        spi_event = SPI_EVENT_TRANSFER_COMPLETE;
    }
    else
    {
        spi_event = SPI_EVENT_TRANSFER_ABORTED;
    }
}

static inline fsp_err_t validate_spi_event(void)
{
    uint32_t local_time_out = UINT32_MAX;

    while( SPI_EVENT_TRANSFER_COMPLETE != spi_event )
    {
        --local_time_out;
        if(RESET_VALUE == local_time_out)
        {
            spi_event = (spi_event_t)RESET_VALUE;
            return FSP_ERR_TIMEOUT;
        }
    }

    if(spi_event == SPI_EVENT_TRANSFER_COMPLETE)
    {
        spi_event = (spi_event_t)RESET_VALUE;
        return FSP_SUCCESS;
    }

    spi_event = (spi_event_t)RESET_VALUE;
    return FSP_ERR_TRANSFER_ABORTED;
}

int spi_write(uint8_t *data, uint32_t bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    /* resetting call back event capture variable */
    spi_event = (spi_event_t)RESET_VALUE;

    err = R_SPI_Write(&g_spi1_master_ctrl, data, bytes, SPI_BIT_WIDTH_8_BITS);
    if(FSP_SUCCESS != err)
    {
        printf("Master R_SPI_Write() failed\r\n");
        return err;
    }

    err = validate_spi_event();
    if (FSP_SUCCESS != err)
    {
        printf("** SPI write validate failed, err=%d ** \r\n", err);
        return err;
    }

    return FSP_SUCCESS;
}

int spi_read(uint8_t *buf, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* resetting call back event capture variable */
    spi_event = (spi_event_t)RESET_VALUE;

    err = R_SPI_Read(&g_spi1_master_ctrl, buf, size, SPI_BIT_WIDTH_8_BITS);
    if(FSP_SUCCESS != err)
    {
        printf("Master R_SPI_Write() failed\r\n");
        return err;
    }

    err = validate_spi_event();
    if (FSP_SUCCESS != err)
    {
        printf("** SPI write validate failed, err=%d ** \r\n", err);
        return err;
    }

    return FSP_SUCCESS;
}

int spi_write_read(uint8_t *out, uint8_t *in, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;

    /* resetting call back event capture variable */
    spi_event = (spi_event_t)RESET_VALUE;

    err = R_SPI_WriteRead(&g_spi1_master_ctrl, out, in, size, SPI_BIT_WIDTH_8_BITS);
    if(FSP_SUCCESS != err)
    {
        printf("Master R_SPI_Write() failed\r\n");
        return err;
    }

    err = validate_spi_event();
    if (FSP_SUCCESS != err)
    {
        printf("** SPI write validate failed, err=%d ** \r\n", err);
        return err;
    }

    return FSP_SUCCESS;
}

int sdmmc_exist_check(void)
{
    uint8_t cmd_arg[6];
    uint8_t idx;
    uint8_t dumy, data;
    int flag=0, timer = 0;

    spi_assert_sd();
    dumy  = 0xFF;
    for(idx = 0; idx < 16; idx++)
    {
        spi_write_read(&dumy, NULL, 1);
    }

    cmd_arg[0] = (0 | 0x40); //CMD0
    cmd_arg[1] = 0;
    cmd_arg[2] = 0;
    cmd_arg[3] = 0;
    cmd_arg[4] = 0;
    cmd_arg[5] = 0x95;

    while (timer < 2000)
    {
        timer ++;
        for (idx= 0; idx < 6; idx++)
        {
            spi_write_read(&cmd_arg[idx], NULL, 1);
        }

        for (idx= 0; idx < 10; idx++)
        {
            spi_write_read(&dumy, &data, 1);
            if ((data != 0xff) && (data != 0))
            {
                flag = 1;
                break;
            }
        }

        if (flag == 1)
            break;
    }

    /* Card is in socket */
   if (flag == 1)
        return 1;

    return 0; /* Socket empty */
}
