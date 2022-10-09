// Copyright (c) Acconeer AB, 2018-2022
// All rights reserved

#ifndef ACC_HAL_DEFINITIONS_H_
#define ACC_HAL_DEFINITIONS_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "acc_definitions_common.h"


/**
 * @brief Defines the largest allowed value of a sensor ID
 */
#define ACC_SENSOR_ID_MAX 42U

/**
 * @brief Specifies the minimal size in bytes that SPI transfers must be able to handle
 */
#define ACC_SPI_TRANSFER_SIZE_REQUIRED 16

/**
 * @brief Specifies the number of clock cycles needed for sensor to enter hibernate mode
 */
#define ACC_NBR_CLOCK_CYCLES_REQUIRED_HIBERNATE_ENTER 10

/**
 * @brief Specifies the number of clock cycles needed for the first part of the sensor exiting hibernate mode
 */
#define ACC_NBR_CLOCK_CYCLES_REQUIRED_STEP_1_HIBERNATE_EXIT 3

/**
 * @brief Specifies the number of clock cycles needed for the second part of the sensor exiting hibernate mode
 */
#define ACC_NBR_CLOCK_CYCLES_REQUIRED_STEP_2_HIBERNATE_EXIT (13 - ACC_NBR_CLOCK_CYCLES_REQUIRED_STEP_1_HIBERNATE_EXIT)

/**
 * @brief Specifies the number of millisonds of delay needed for the Oscillator to stabilize when exiting hibernate mode
 */
#define ACC_WAIT_TIME_HIBERNATE_EXIT_MS 2

/**
 * @defgroup OS OS Integration
 * @ingroup Integration
 *
 * @brief Integration OS primitives
 *
 * @{
 */


/**
 * @brief Definition of a memory allocation function
 *
 * Allocated memory should be suitably aligned for any built-in type. Returning NULL is seen as failure.
 */
typedef void *(*acc_os_mem_alloc_function_t)(size_t);


/**
 * @brief Definition of a memory free function
 *
 * Free memory which is previously allocated.
 */
typedef void (*acc_os_mem_free_function_t)(void *);


/**
 * @brief Definition of a time retrieval function
 *
 * The time returned must be in milliseconds.
 *
 * Must be implemented by integration.
 */
typedef uint32_t (*acc_os_get_time_function_t)(void);


/**
 * @brief Struct that contains the implementation of os integration primitives
 */
typedef struct
{
	acc_os_mem_alloc_function_t mem_alloc;
	acc_os_mem_free_function_t  mem_free;
	acc_os_get_time_function_t  gettime;
} acc_rss_integration_os_primitives_t;

/**
 * @}
 */


/**
 * @defgroup HAL Hardware Integration
 * @ingroup Integration
 *
 * @brief Integration of Hardware Abstraction Layer for the radar sensor
 *
 * @{
 */


/**
 * @brief Definition of a sensor power function
 *
 * These functions control the power of the sensor. It typically control PS_ENABLE
 * and PMU_ENABLE. The hibernate functions should also toggle the CTRL pin.
 *
 * In the case of the power_on function:
 * Any pending sensor interrupts should be cleared before returning from function.
 */
typedef void (*acc_hal_sensor_power_function_t)(acc_sensor_id_t sensor_id);


/**
 * @brief Definition of a hal get frequency function
 *
 * This function shall return the frequency of the reference clock connected to the sensor.
 *
 */
typedef float (*acc_hal_get_frequency_function_t)(void);


/**
 * @brief Definition of a wait for sensor interrupt function
 *
 * This function shall wait at most timeout_ms for the interrupt to become active and
 * then return true. It may return true immediately if an interrupt have
 * occurred since last call to this function.
 *
 * If waited more than timeout_ms for the interrupt to become active it shall
 * return false.
 *
 * Note that this function can be called with a timeout_ms = 0.
 *
 */
typedef bool (*acc_hal_sensor_wait_for_interrupt_function_t)(acc_sensor_id_t sensor_id, uint32_t timeout_ms);


/**
 * @brief Definition of a sensor transfer function
 *
 * This function shall transfer data to and from the sensor over spi. It's beneficial from a performance perspective
 * to use dma if available.
 * The buffer is naturally aligned to a maximum of 4 bytes.
 *
 */
typedef void (*acc_hal_sensor_transfer_function_t)(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_size);


/**
 * @brief This struct contains function pointers that point to
 *        functions needed for communication with the radar sensor.
 */
typedef struct
{
	acc_hal_sensor_power_function_t              power_on;
	acc_hal_sensor_power_function_t              power_off;
	acc_hal_sensor_power_function_t              hibernate_enter;
	acc_hal_sensor_power_function_t              hibernate_exit;
	acc_hal_sensor_wait_for_interrupt_function_t wait_for_interrupt;
	acc_hal_sensor_transfer_function_t           transfer;
	acc_hal_get_frequency_function_t             get_reference_frequency;
} acc_rss_integration_sensor_device_t;


/**
 * @}
 */


/**
 * @defgroup Properties Integration Properties
 * @ingroup Integration
 *
 * @brief Integration specific properties that is used for configuration of RSS.
 *
 * Number of sensors connected to each board and the maximum buffer size that the
 * spi driver can handle can be different between applications. These values shall
 * be configured by the client.
 *
 * @{
 */


/**
 * @brief This struct contains information about board properties that
 *        are needed by RSS.
 *
 * @ref sensor_count is the maximal sensor ID that the integration layer supports.
 * This value must not exceed @ref ACC_SENSOR_ID_MAX.
 *
 * @ref max_spi_transfer_size is the maximal buffer size that is supported
 * by the implementation of @ref acc_hal_sensor_transfer_function_t.
 * This value must not be smaller than @ref ACC_SPI_TRANSFER_SIZE_REQUIRED.
 */
typedef struct
{
	uint32_t sensor_count;
	size_t   max_spi_transfer_size;
} acc_rss_integration_properties_t;

/**
 * @}
 */


/**
 * @defgroup Opimization Optional optimizations
 * @ingroup Integration
 *
 * @brief Support for different optimizations
 *
 * @{
 */


/**
 * @brief Definition of an optimized 16-bit sensor transfer function
 *
 * This function shall transfer data to and from the sensor over spi with 16 bits data size.
 * It's beneficial from a performance perspective to use dma if available.
 * The buffer is naturally aligned to a minimum of 4 bytes.
 *
 * If defined it will be used instead of the (8-bit) transfer function @ref acc_hal_sensor_transfer_function_t
 *
 */
typedef void (*acc_sensor_transfer16_function_t)(acc_sensor_id_t sensor_id, uint16_t *buffer, size_t buffer_length);


/**
 * @brief This struct contains function pointers that are optional to support different optimizations
 *
 * Optional
 *
 * This struct contains function pointers to support different optimizations.
 * These optimizations can be utilized for some integrations.
 * If they are defined, they will override the corresponding non-optimized function.
 *
 * For example, if the transfer16 function is implemented, it will be used instead of the transfer function.
 */
typedef struct
{
	acc_sensor_transfer16_function_t transfer16;
} acc_optimization_t;

/**
 * @}
 */


/**
 * @defgroup Log Log Integration
 * @ingroup Integration
 *
 * @brief Integration for log functionality
 *
 * @{
 */


/**
 * @brief Definition of a log function
 */
typedef void (*acc_log_function_t)(acc_log_level_t level, const char *module, const char *format, ...);


/**
 * @brief This struct contains information about log properties and functions
 *        needed by RSS
 */
typedef struct
{
	acc_log_level_t    log_level;
	acc_log_function_t log;
} acc_rss_integration_log_t;


/**
 * @}
 */


/**
 * @defgroup Integration Integration
 * @brief Driver and OS Integration
 *
 * @{
 */


/**
 * @brief This struct contains the information about the sensor
 *        integration that RSS needs.
 */

typedef struct
{
	acc_rss_integration_properties_t    properties;
	acc_rss_integration_os_primitives_t os;
	acc_rss_integration_sensor_device_t sensor_device;
	acc_rss_integration_log_t           log;
	acc_optimization_t                  optimization;
} acc_hal_t;

/**
 * @}
 */
#endif
