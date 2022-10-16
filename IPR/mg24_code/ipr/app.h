/***************************************************************************//**
 * @file
 * @brief Application interface provided to main().
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_H
#define APP_H

#include "sl_spidrv_eusart_spi0_config.h"


#define A111_MOSI_PORT   SL_SPIDRV_EUSART_SPI0_TX_PORT
#define A111_MOSI_PIN    SL_SPIDRV_EUSART_SPI0_TX_PIN
#define A111_MISO_PORT   SL_SPIDRV_EUSART_SPI0_RX_PORT
#define A111_MISO_PIN    SL_SPIDRV_EUSART_SPI0_RX_PIN
#define A111_SCLK_PORT   SL_SPIDRV_EUSART_SPI0_SCLK_PORT
#define A111_SCLK_PIN    SL_SPIDRV_EUSART_SPI0_SCLK_PIN
#define A111_CS_PORT     SL_SPIDRV_EUSART_SPI0_CS_PORT
#define A111_CS_PIN      SL_SPIDRV_EUSART_SPI0_CS_PIN
#define A111_EN_PORT     gpioPortA
#define A111_EN_PIN      4
#define A111_INT_PORT    gpioPortA
#define A111_INT_PIN     5

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void);

/**************************************************************************//**
 * Application Exit.
 *****************************************************************************/
void app_exit(void);

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void);

#endif
