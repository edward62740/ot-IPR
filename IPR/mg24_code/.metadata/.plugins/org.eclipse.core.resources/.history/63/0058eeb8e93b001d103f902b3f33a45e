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

#include "main.h"

#include "acc_definitions_common.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_integration.h"
#include "acc_integration_log.h"


/* spi handle */
extern SPI_HandleTypeDef A111_SPI_HANDLE;

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

/**
 * @brief The reference frequency used by this board
 *
 * This assumes 26 MHz on the Sparkfun A111 Board
 */
#define ACC_BOARD_REF_FREQ 26000000


static inline void disable_interrupts(void)
{
	__disable_irq();
}


static inline void enable_interrupts(void)
{
	__enable_irq();
	__ISB();
}


#ifdef A111_USE_SPI_DMA
static volatile bool spi_transfer_complete;


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *h_spi)
{
	(void)h_spi;
	spi_transfer_complete = true;
}


#endif

//----------------------------------------
// Implementation of RSS HAL handlers
//----------------------------------------


static void acc_hal_integration_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_size)
{
	(void)sensor_id;  // Ignore parameter sensor_id

	const uint32_t SPI_TRANSMIT_RECEIVE_TIMEOUT = 5000;

	HAL_GPIO_WritePin(A111_CS_N_GPIO_Port, A111_CS_N_Pin, GPIO_PIN_RESET);

#ifdef A111_USE_SPI_DMA
	spi_transfer_complete = false;
	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(&A111_SPI_HANDLE, buffer, buffer, buffer_size);

	if (status != HAL_OK)
	{
		return;
	}

	uint32_t start = HAL_GetTick();

	while (!spi_transfer_complete && (HAL_GetTick() - start) < SPI_TRANSMIT_RECEIVE_TIMEOUT)
	{
		// Turn off interrupts
		disable_interrupts();
		// Check once more so that the interrupt have not occurred
		if (!spi_transfer_complete)
		{
			__WFI();
		}

		// Enable interrupt again, the ISR will execute directly after this
		enable_interrupts();
	}
#else
	HAL_SPI_TransmitReceive(&A111_SPI_HANDLE, buffer, buffer, buffer_size, SPI_TRANSMIT_RECEIVE_TIMEOUT);
#endif

	HAL_GPIO_WritePin(A111_CS_N_GPIO_Port, A111_CS_N_Pin, GPIO_PIN_SET);
}


static void acc_hal_integration_sensor_power_on(acc_sensor_id_t sensor_id)
{
	(void)sensor_id;  // Ignore parameter sensor_id

	HAL_GPIO_WritePin(A111_ENABLE_GPIO_Port, A111_ENABLE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(A111_CS_N_GPIO_Port, A111_CS_N_Pin, GPIO_PIN_SET);

	// Wait 2 ms to make sure that the sensor crystal have time to stabilize
	HAL_Delay(2);
}


static void acc_hal_integration_sensor_power_off(acc_sensor_id_t sensor_id)
{
	(void)sensor_id;  // Ignore parameter sensor_id

	HAL_GPIO_WritePin(A111_CS_N_GPIO_Port, A111_CS_N_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A111_ENABLE_GPIO_Port, A111_ENABLE_Pin, GPIO_PIN_RESET);

	// Wait after power off to leave the sensor in a known state
	// in case the application intends to enable the sensor directly
	HAL_Delay(2);
}


static bool acc_hal_integration_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms)
{
	(void)sensor_id; // Ignore parameter sensor_id

	const uint32_t wait_begin_ms = HAL_GetTick();
	while ((HAL_GPIO_ReadPin(A111_SENSOR_INTERRUPT_GPIO_Port, A111_SENSOR_INTERRUPT_Pin) != GPIO_PIN_SET) &&
	       (HAL_GetTick() - wait_begin_ms < timeout_ms))
	{
		// Wait for the GPIO interrupt
		disable_interrupts();
		// Check again so that IRQ did not occur
		if (HAL_GPIO_ReadPin(A111_SENSOR_INTERRUPT_GPIO_Port, A111_SENSOR_INTERRUPT_Pin) != GPIO_PIN_SET)
		{
			__WFI();
		}

		// Enable interrupts again to allow pending interrupt to be handled
		enable_interrupts();
	}

	return HAL_GPIO_ReadPin(A111_SENSOR_INTERRUPT_GPIO_Port, A111_SENSOR_INTERRUPT_Pin) == GPIO_PIN_SET;
}


static float acc_hal_integration_get_reference_frequency(void)
{
	return ACC_BOARD_REF_FREQ;
}


static const acc_hal_t hal =
{
	.properties.sensor_count          = SENSOR_COUNT,
	.properties.max_spi_transfer_size = A111_SPI_MAX_TRANSFER_SIZE,

	.sensor_device.power_on                = acc_hal_integration_sensor_power_on,
	.sensor_device.power_off               = acc_hal_integration_sensor_power_off,
	.sensor_device.wait_for_interrupt      = acc_hal_integration_wait_for_sensor_interrupt,
	.sensor_device.transfer                = acc_hal_integration_sensor_transfer,
	.sensor_device.get_reference_frequency = acc_hal_integration_get_reference_frequency,

	.os.mem_alloc = malloc,
	.os.mem_free  = free,
	.os.gettime   = acc_integration_get_time,

	.log.log_level = ACC_LOG_LEVEL_INFO,
	.log.log       = acc_integration_log,

	.optimization.transfer16 = NULL,
};


const acc_hal_t *acc_hal_integration_get_implementation(void)
{
	return &hal;
}
