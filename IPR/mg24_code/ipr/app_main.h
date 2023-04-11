/*
 * app_main.h
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */


#ifndef APP_H
#define APP_H

#include "stdbool.h"
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include "em_gpio.h"
#include "openthread-system.h"
#include "em_gpio.h"

#define A111_MOSI_PORT   gpioPortB
#define A111_MOSI_PIN    2
#define A111_MISO_PORT   gpioPortB
#define A111_MISO_PIN    1
#define A111_SCLK_PORT   gpioPortB
#define A111_SCLK_PIN    0
#define A111_CS_PORT     gpioPortB
#define A111_CS_PIN      3
#define A111_EN_PORT     gpioPortA
#define A111_EN_PIN      4
#define A111_INT_PORT    gpioPortA
#define A111_INT_PIN     5

#define IP_LED_PORT      gpioPortC
#define IP_LED_PIN       7
#define ACT_LED_PORT     gpioPortC
#define ACT_LED_PIN      8
#define ERR_LED_PORT     gpioPortC
#define ERR_LED_PIN      9

#define OPT_SCL_PORT     gpioPortA
#define OPT_SCL_PIN      9
#define OPT_SDA_PORT     gpioPortA
#define OPT_SDA_PIN      8
#define OPT_INT_PORT     gpioPortB
#define OPT_INT_PIN      4




void app_init(void);

void app_exit(void);
void app_process_action(void);
otInstance *otGetInstance(void);
void setNetworkConfiguration(void);
void sleepyInit(void);
void appSrpInit(void);
#endif
