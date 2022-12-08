// Copyright (c) Acconeer AB, 2018-2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.


#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"

#include "app.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_integration_log.h"
#include "sl_spidrv_instances.h"

/**
 * @brief The number of sensors available on the board
 */
#define SENSOR_COUNT 1

/**
 * @brief Size of SPI transfer buffer
 */
#ifndef A111_SPI_MAX_TRANSFER_SIZE
#define A111_SPI_MAX_TRANSFER_SIZE 65535
#endif

#define ACC_BOARD_REF_FREQ 26000000

static inline void disable_interrupts(void) {
	__disable_irq();
}

static inline void enable_interrupts(void) {
	__enable_irq();
	__ISB();
}

//----------------------------------------
// Implementation of RSS HAL handlers
//----------------------------------------

static void acc_hal_integration_sensor_transfer(acc_sensor_id_t sensor_id,
		uint8_t *buffer, size_t buffer_size) {
	(void) sensor_id;  // Ignore parameter sensor_id

	GPIO_PinOutClear(A111_CS_PORT, A111_CS_PIN);

	__asm__("nop"); // no-op, CS set-up time

	/* Polling method appears to consume less current over time than interrupt */
	for(size_t i=0; i<buffer_size; i++)
	    buffer[i] = EUSART_Spi_TxRx(EUSART0, buffer[i]);

	// De-assert chip select upon transfer completion (drive high)
	GPIO_PinOutSet(A111_CS_PORT, A111_CS_PIN);
}

static void acc_hal_integration_sensor_power_on(acc_sensor_id_t sensor_id) {
	(void) sensor_id;  // Ignore parameter sensor_id

	GPIO_PinOutSet(A111_EN_PORT, A111_EN_PIN);
	GPIO_PinOutSet(A111_CS_PORT, A111_CS_PIN);

	// Wait 3 ms to make sure that the sensor crystal have time to stabilize
	acc_integration_sleep_ms(5);
}

static void acc_hal_integration_sensor_power_off(acc_sensor_id_t sensor_id) {
	(void) sensor_id;  // Ignore parameter sensor_id

	GPIO_PinOutClear(A111_EN_PORT, A111_EN_PIN);
	GPIO_PinOutClear(A111_CS_PORT, A111_CS_PIN);
	acc_integration_sleep_ms(5);
}

static bool acc_hal_integration_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms) {
    (void) sensor_id; // Ignore parameter sensor_id

    const uint32_t wait_begin_ms = sl_sleeptimer_get_tick_count();
    while ((GPIO_PinInGet(A111_INT_PORT, A111_INT_PIN) != 1)
            && ((sl_sleeptimer_get_tick_count() - wait_begin_ms) < timeout_ms))
    {
        // Wait for the GPIO interrupt
        disable_interrupts();
        // Check again so that IRQ did not occur
        if (GPIO_PinInGet(A111_INT_PORT, A111_INT_PIN) != 1)
        {
            __asm__("wfi"); // wait for interrupt
        }

        // Enable interrupts again to allow pending interrupt to be handled
        enable_interrupts();
    }

    return GPIO_PinInGet(A111_INT_PORT, A111_INT_PIN) == 1;
}

static float acc_hal_integration_get_reference_frequency(void) {
    return ACC_BOARD_REF_FREQ;
}

static const acc_hal_t hal = { .properties.sensor_count = SENSOR_COUNT,
	.properties.max_spi_transfer_size =
	A111_SPI_MAX_TRANSFER_SIZE,

	.sensor_device.power_on = acc_hal_integration_sensor_power_on,
	.sensor_device.power_off = acc_hal_integration_sensor_power_off,
	.sensor_device.wait_for_interrupt =
			acc_hal_integration_wait_for_sensor_interrupt,
	.sensor_device.transfer = acc_hal_integration_sensor_transfer,
	.sensor_device.get_reference_frequency =
			acc_hal_integration_get_reference_frequency,

	.os.mem_alloc = malloc, .os.mem_free = free, .os.gettime =
			acc_integration_get_time,

	.log.log_level = ACC_LOG_LEVEL_INFO, .log.log = acc_integration_log,

	.optimization.transfer16 = NULL, };

const acc_hal_t* acc_hal_integration_get_implementation(void) {
    return &hal;
}
