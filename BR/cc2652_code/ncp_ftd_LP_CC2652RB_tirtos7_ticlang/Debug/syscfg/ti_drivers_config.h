/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the LP_CC2652RB
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_LP_CC2652RB
#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== CCFG ========
 */
#include DeviceFamily_constructPath(driverlib/osc.h)


/*
 *  ======== AESECB ========
 */

extern const uint_least8_t                  CONFIG_AESECB_0_CONST;
#define CONFIG_AESECB_0                     0
#define CONFIG_TI_DRIVERS_AESECB_COUNT      1


/*
 *  ======== ECDH ========
 */

extern const uint_least8_t              CONFIG_ECDH_0_CONST;
#define CONFIG_ECDH_0                   0
#define CONFIG_TI_DRIVERS_ECDH_COUNT    1


/*
 *  ======== ECDSA ========
 */

extern const uint_least8_t                  CONFIG_ECDSA_0_CONST;
#define CONFIG_ECDSA_0                      0
#define CONFIG_TI_DRIVERS_ECDSA_COUNT       1


/*
 *  ======== ECJPAKE ========
 */

extern const uint_least8_t                  CONFIG_ECJPAKE_0_CONST;
#define CONFIG_ECJPAKE_0                    0
#define CONFIG_TI_DRIVERS_ECJPAKE_COUNT     1


/*
 *  ======== GPIO ========
 */
extern const uint_least8_t CONFIG_GPIO_BTN1_CONST;
#define CONFIG_GPIO_BTN1 13

extern const uint_least8_t CONFIG_GPIO_BTN2_CONST;
#define CONFIG_GPIO_BTN2 14

extern const uint_least8_t CONFIG_GPIO_GLED_CONST;
#define CONFIG_GPIO_GLED 7

extern const uint_least8_t CONFIG_GPIO_RLED_CONST;
#define CONFIG_GPIO_RLED 6

extern const uint_least8_t CONFIG_SPINEL_INT_CONST;
#define CONFIG_SPINEL_INT 16

/* Owned by CONFIG_SPI_1 as  */
extern const uint_least8_t CONFIG_PIN_SPI1_SCLK_CONST;
#define CONFIG_PIN_SPI1_SCLK 26

/* Owned by CONFIG_SPI_1 as  */
extern const uint_least8_t CONFIG_PIN_SPI1_MISO_CONST;
#define CONFIG_PIN_SPI1_MISO 24

/* Owned by CONFIG_SPI_1 as  */
extern const uint_least8_t CONFIG_PIN_SPI1_MOSI_CONST;
#define CONFIG_PIN_SPI1_MOSI 25

/* Owned by CONFIG_DISPLAY_UART as  */
extern const uint_least8_t CONFIG_PIN_UART_TX_CONST;
#define CONFIG_PIN_UART_TX 3

/* Owned by CONFIG_DISPLAY_UART as  */
extern const uint_least8_t CONFIG_PIN_UART_RX_CONST;
#define CONFIG_PIN_UART_RX 2

/* Owned by CONFIG_SPI_1 as  */
extern const uint_least8_t CONFIG_PIN_SPI1_CSN_CONST;
#define CONFIG_PIN_SPI1_CSN 27

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== NVS ========
 */

extern const uint_least8_t              CONFIG_NVSINTERNAL_CONST;
#define CONFIG_NVSINTERNAL              0
#define CONFIG_TI_DRIVERS_NVS_COUNT     1




/*
 *  ======== SHA2 ========
 */

extern const uint_least8_t              CONFIG_SHA2_0_CONST;
#define CONFIG_SHA2_0                   0
#define CONFIG_TI_DRIVERS_SHA2_COUNT    1


/*
 *  ======== SPI ========
 */

/*
 *  MOSI: DIO25
 *  MISO: DIO24
 *  SCLK: DIO26
 *  SS: DIO27
 */
extern const uint_least8_t              CONFIG_SPI_1_CONST;
#define CONFIG_SPI_1                    0
#define CONFIG_TI_DRIVERS_SPI_COUNT     1


/*
 *  ======== TRNG ========
 */

extern const uint_least8_t              CONFIG_TRNG_0_CONST;
#define CONFIG_TRNG_0                   0
#define CONFIG_TI_DRIVERS_TRNG_COUNT    1


/*
 *  ======== UART2 ========
 */

/*
 *  TX: DIO3
 *  RX: DIO2
 *  XDS110 UART
 */
extern const uint_least8_t                  CONFIG_DISPLAY_UART_CONST;
#define CONFIG_DISPLAY_UART                 0
#define CONFIG_TI_DRIVERS_UART2_COUNT       1


/*
 *  ======== Watchdog ========
 */

extern const uint_least8_t                  CONFIG_WATCHDOG0_CONST;
#define CONFIG_WATCHDOG0                    0
#define CONFIG_TI_DRIVERS_WATCHDOG_COUNT    1


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
