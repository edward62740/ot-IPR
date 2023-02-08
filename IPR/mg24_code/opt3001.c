/*
 * opt3001.c
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */
#include "sl_i2cspm.h"
#include "sl_sleeptimer.h"
#include "opt3001.h"
#include "math.h"

#define REG_RESULT                      0x00
#define REG_CONFIGURATION               0x01
#define REG_LOWLIMIT                    0x02


static const uint8_t address = 0x44;

uint16_t opt3001_read_reg(uint8_t reg)
{
    uint8_t data[2];
    memset(data, 0, sizeof(data));
    I2C_TransferSeq_TypeDef i2cTransfer;
    I2C_TransferReturn_TypeDef result;

    // Initialize I2C transfer
    i2cTransfer.addr          = address << 1;
    i2cTransfer.flags         = I2C_FLAG_WRITE_READ; // must write target address before reading
    i2cTransfer.buf[0].data   = &reg;
    i2cTransfer.buf[0].len    = 1;
    i2cTransfer.buf[1].data   = data;
    i2cTransfer.buf[1].len    = 2;

    result = I2CSPM_Transfer(I2C0, &i2cTransfer);

   return ((uint16_t) data[0] << 8) | data[1];
}

void opt3001_write_reg(uint8_t reg, uint8_t dataL, uint8_t dataH)
{

    uint8_t txBuffer[3];
    I2C_TransferSeq_TypeDef i2cTransfer;
    I2C_TransferReturn_TypeDef result;

    txBuffer[0] = reg;
    txBuffer[1] = dataL;
    txBuffer[2] = dataH;
    // Initialize I2C transfer
    i2cTransfer.addr = address << 1;
    i2cTransfer.flags = I2C_FLAG_WRITE;
    i2cTransfer.buf[0].data = txBuffer;
    i2cTransfer.buf[0].len = 3;
    i2cTransfer.buf[1].data = NULL;
    i2cTransfer.buf[1].len = 0;

    result = I2CSPM_Transfer(I2C0, &i2cTransfer);
}
void opt3001_init()
{
    opt3001_write_reg(REG_CONFIGURATION, 0b11000110, 0b00000000); //0b1100001000010000
    opt3001_write_reg(REG_LOWLIMIT, 0b11000000, 0b00000000);
}

void opt3001_deinit(void)
{

    opt3001_write_reg(REG_CONFIGURATION, 0b11000000, 0x10); //0b1100001000010000

}

uint16_t opt3001_read(void)
{
    uint8_t count = 0;
    const uint8_t max_wait = 255;
    int is_measuring;
    do
    {
      is_measuring = opt3001_is_measuring();
      count++;
    }while( (is_measuring == 1) && (count < max_wait) );
    if(count == max_wait)
    {
       return opt3001_read_reg(REG_CONFIGURATION);
    }
    else
    {
       uint16_t raw = opt3001_read_reg(REG_RESULT);
       return raw;
    }

}

int opt3001_is_measuring(void)
{
    uint16_t s;
    s = opt3001_read_reg(REG_CONFIGURATION);
    if (s & (1 << 7))
    {
      return 0;
    }
    else
    {
      return 1;
    }
}

float opt3001_conv(uint16_t raw)
{
    uint16_t e, m;

    m = raw & 0x0FFF;
    e = (raw & 0xF000) >> 12;

    return m * (10 * exp2(e));
}


