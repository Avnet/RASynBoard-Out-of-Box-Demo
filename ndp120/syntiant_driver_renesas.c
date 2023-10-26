
#include <stdio.h>

#include "hal_data.h"
#include "spi_drv.h"
#include "fat_load.h"

#include <syntiant_ilib/syntiant_ndp120_tiny.h>
#include "syntiant_driver.h"


int g_spi_read_delay;

int syntiant_ndp_spi_init(void *d, int spi_default_speed, int spi_read_delay)
{
    int s = 0;

    g_spi_read_delay = spi_read_delay;
    (void)spi_default_speed;// fix to 1Mhz now
    (void)d;

// add RZ SPI init
// init in hal_entry.c

    return s;
}

static int syntiant_ndp_spi_read(uint8_t spi_cmd, uint8_t *read_buff, int count)
{
    int s = 0;

    spi_assert_ndp();
    spi_write_read(&spi_cmd, NULL, 1);
    spi_write_read(NULL, read_buff, (uint32_t)count);
    spi_deassert_ndp();

    return s;
}

static int syntiant_ndp_spi_write(uint8_t spi_cmd, uint8_t *write_buff, int count)
{
    int s = 0;

    spi_assert_ndp();
    spi_write_read(&spi_cmd, NULL, 1);
    spi_write_read(write_buff, NULL, (uint32_t)count);
    spi_deassert_ndp();

    return s;
}

#ifdef SPI_TRANS_DEBUG
int ndp_core2_platform_io_transfer_debug = 0;
#endif

int syntiant_ndp_transfer(void *d, int mcu, uint32_t addr,
        void *out, void *in, unsigned int count)
{
    (void)d;
    int s = SYNTIANT_NDP120_ERROR_NONE;
    uint8_t spi_cmd;

    if (in && out) {
        s = SYNTIANT_NDP120_ERROR_ARG;
        return s;
    }

#ifdef SPI_TRANS_DEBUG  
    if (ndp_core2_platform_io_transfer_debug){
        if(mcu) {
            SYNTIANT_TRACE("transfer mcu %d, count %d, addr 0x%08x, out %p, in %p\r\n", 
                mcu, count, addr, out, in);
        }
        else {
            SYNTIANT_TRACE("transfer mcu %d, count %d, addr 0x%02x, out %p, in %p\r\n", 
                mcu, count, addr, out, in);
        }
    }
#endif

    if (mcu) { /* 32 bits address rd&wr */
        uint32_t addr_u32 = addr;

        if ((count & 0x3) != 0) {
            s = SYNTIANT_NDP120_ERROR_ARG;
            return s;
        }
        
        spi_cmd = NDP120_SPI_MADDR(0);
        //NDP120_SPI_MADDR(0) in MCU_MEM keeps the address for MCU access
        if (in) {
            uint8_t in_buffer[count+g_spi_read_delay];

            //send the address for MCU access to MCU_MEM, 
            //write size will be 5 bytes, 0x44 + 4 bytes address
            s = syntiant_ndp_spi_write(spi_cmd, (uint8_t*)&addr_u32, sizeof(addr_u32));
            if(s) return s;
            
            spi_cmd = 0x80 | (NDP120_SPI_MDATA(0) - g_spi_read_delay);
            //write command to get read buffer, the buffer size 
            //will be read_data + read_delay_bytes
            //write command will be 1 byte
            s = syntiant_ndp_spi_read(spi_cmd, in_buffer, count+g_spi_read_delay);
            if(s) return s;

            /* skip the read_delay dummy bytes */
            memcpy(in, &in_buffer[g_spi_read_delay], count);
        } else {
            uint8_t out_buffer[count+sizeof(addr)];

            //write size will be 5 + send_len bytes;
            //0x44 + 4 bytes address + data_send
            memcpy(&out_buffer[0], (uint8_t*)&addr_u32, sizeof(uint32_t));
            memcpy(&out_buffer[sizeof(uint32_t)], out, count);
            s = syntiant_ndp_spi_write(spi_cmd, out_buffer, 
                SYNTIANT_MAX_BLOCK_SIZE+sizeof(addr));
        }
    }
    else {
        if (0xff < addr) {
            s = SYNTIANT_NDP120_ERROR_ARG;
            return s;
        }

        if(in) {
            //write command to get read buffer, the buffer size will be 1 byte
            spi_cmd = 0x80 | addr;
            s = syntiant_ndp_spi_read(spi_cmd, in, count);
        }
        else {
            //write size will be 1 + send_len bytes;
            spi_cmd = addr;
            s = syntiant_ndp_spi_write(spi_cmd, out, count);
        }
    }

    return s;
}

int syntiant_flash_spi_read(uint32_t addr, uint8_t *read_buff, int read_len)
{
    //TBD
    (void) addr;
    (void) read_buff;
    (void) read_len;
    return 0;
}

int syntiant_file_host_len(char *file_name)
{
    return get_synpkg_size(file_name);
}

int syntiant_file_host_read(char *file_name, uint32_t offset, 
        uint8_t *read_buffer, int read_size)
{
    return read_synpkg_block(file_name, offset, read_buffer, read_size);
}

void syntiant_ndp_delay_us(int us_time)
{
    R_BSP_SoftwareDelay((uint32_t)us_time, BSP_DELAY_UNITS_MICROSECONDS);
}


#define NDP_SPI_SPACE 0
#define NDP_MCU_SPACE 1

void printf_array(uint8_t * array, int cnt)
{
    int i;

    printf("%s\r\n",__func__);

    for(i=0; i< cnt; i++)
        printf("0x%2x\r\n", array[i]);
}

void test_ndp_spi(void)
{
    uint32_t addr;
    uint8_t rx[12];
    uint8_t tx[4];

#if 0
    // SPI space
    // read id0@0 id1@1
    addr = 0;
    sytiant_ndp_transfer(NULL, NDP_SPI_SPACE, addr, NULL, rx, 2);
    // write 0xab to 0x20
    addr = 0x20;
    tx[0] = 0xab;
    sytiant_ndp_transfer(NULL, NDP_SPI_SPACE, addr, tx, NULL, 1);

    // read 0xab from 0x20
    addr = 0x20;
    sytiant_ndp_transfer(NULL, NDP_SPI_SPACE, addr, NULL, rx, 1);
#endif
    // MCU space
    // read 0x20000000
    addr = 0x20000000;
    syntiant_ndp_transfer(NULL, NDP_MCU_SPACE, addr, NULL, rx, 4);
    printf("First read\r\n");
    printf_array(rx, 4);

    // write 0xDEADBEEF  to  0x20000000
    addr = 0x20000000;
    tx[0] = 0xAB;
    tx[1] = 0xCD;
    tx[2] = 0xEF;
    tx[3] = 0x5A;
    syntiant_ndp_transfer(NULL, NDP_MCU_SPACE, addr, tx, NULL, 4);

    // MCU space
    // read 0x20000000
    addr = 0x20000000;
    syntiant_ndp_transfer(NULL, NDP_MCU_SPACE, addr, NULL, rx, 4);
    printf("Second read\r\n");
    printf_array(rx, 4);
}
