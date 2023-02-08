/*
 * opt3001.h
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */

#ifndef OPT3001_H_
#define OPT3001_H_

#include <stdint.h>


#define DEFAULT_CONFIG_800    0b1100110000010000 // 800ms
#define DEFAULT_CONFIG_800_OS 0b1100101000010000 // 800ms, one shot

#define DEFAULT_CONFIG_100    0b1100010000010000 // 100ms
#define DEFAULT_CONFIG_100_OS 0b1100001000010000 // 100ms, one shot

#define DEFAULT_CONFIG_SHDWN  0xc810 // Shut down converter

/*  CONFIG REGISTER BITS: RN3 RN2 RN1 RN0 CT M1 M0 OVF CRF FH FL L Pol ME FC1 FC0
    RN3 to RN0 = Range select:
                    1100 by default, enables auto-range
    CT = Conversion time bit
                    0 = 100ms conversion time
                    1 = 800ms conversion time (default)
    M1 to M0 = Mode bits
                    00 = Shutdown mode
                    01 = Single shot mode
                    10 = Continuous conversion (default)
                    11 = Continuous conversion
    OVF (Bit 8)     Overflow flag. When set the conversion result is overflown.
    CRF (Bit 7)     Conversion ready flag. Sets at end of conversion. Clears by read or write of the Configuration register.
    FH (Bit 6)      Flag high bit. Read only. Sets when result is higher that TH register. Clears when Config register is
                    read or when Latch bit is 0 and the result goes bellow TH register.
    FL (Bit 5)      Flag low bit. Read only. Sets when result is lower that TL register. Clears when Config register is read
                    or when Latch bit is 0 and the result goes above TL register.
    L (Bit 4)       Latch bit. Read/write bit. Default 1, Controls Latch/transparent functionality of FH and FL bits. When
                    L = 1 the Alert pin works in window comparator mode with Latched functionality When L = 0 the Alert pin
                    works in transparent mode and the two limit registers provide the hysteresis.
    Pol (Bit 3)     Polarity. Read/write bit. Default 0, Controls the active state of the Alert pin. Pol = 0 means Alert
                    active low.
    ME (Bit 2)      Exponent mask. In fixed range modes masks the exponent bits in the result register to 0000.
    FC1 to FC0  -   Fault count bits. Read/write bits. Default 00 - the first fault will trigger the alert pin.
*/

uint16_t opt3001_read_reg(uint8_t reg);
void opt3001_write_reg(uint8_t reg, uint8_t dataL, uint8_t dataH);
void opt3001_init();
uint16_t opt3001_read(void);
float opt3001_conv(uint16_t raw);
void opt3001_deinit(void);
int opt3001_is_measuring(void);



#define OPT3001_CFG_FL          (1 << 5)
#define OPT3001_CFG_FH          (1 << 6)
#define OPT3001_CFG_CRF         (1 << 7)
#define OPT3001_CFG_OVF         (1 << 8)
#define OPT3001_CFG_L           (1 << 4)
#define OPT3001_CFG_POL         (1 << 3)
#define OPT3001_CFG_ME          (1 << 2)
#define OPT3001_CFG_CT          (1 << 11)
#define OPT3001_CFG_FC_SHIFT    0
#define OPT3001_CFG_FC_MASK     (0x03 << OPT3001_CFG_FC_SHIFT)
#define OPT3001_CFG_M_SHIFT     9
#define OPT3001_CFG_M_MASK      (0x03 << OPT3001_CFG_M_SHIFT)
#define OPT3001_CFG_RN_SHIFT    12
#define OPT3001_CFG_RN_MASK     (0xf << OPT3001_CFG_RN_SHIFT)



#endif /* OPT3001_H_ */
