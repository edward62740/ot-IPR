// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_RSS_H_
#define ACC_RSS_H_

#include <stdbool.h>

#include "acc_definitions_a111.h"
#include "acc_definitions_common.h"
#include "acc_hal_definitions.h"


/**
 * @defgroup RSS Radar System Software, RSS
 *
 * @brief Acconeer Radar System Software, RSS
 *
 * @{
 */


/**
 * @brief Activate the Acconeer Radar System Software, RSS
 *
 * A HAL struct containing integration functions (such as 'wait_for_interrupt', 'mem_alloc' and 'log')
 * needed by RSS must first be populated and then sent in. See 'acc_definitions_common.h' for a full list
 * of functions needed.
 *
 * This function must be called before any other RSS function. If it is not, or it failed,
 * calling any other RSS function is undefined behaviour.
 *
 * @param[in] hal Reference to a HAL struct containing integration functions that is needed by RSS
 * @return True if RSS activated successfully
 */
bool acc_rss_activate(const acc_hal_t *hal);


/**
 * @brief Deactivate the Acconeer Radar System Software, RSS
 */
void acc_rss_deactivate(void);


/**
 * @brief Get the sensor calibration context
 *
 * Must be called after RSS has been activated.
 * A calibration will be done for the specific sensor.
 * A successful call to this function will also trigger context reset.
 *
 * @param[in] sensor_id The sensor to get the context for
 * @param[out] calibration_context Reference to struct where the context will be stored
 * @return True if successful, false otherwise
 */
bool acc_rss_calibration_context_get(acc_sensor_id_t sensor_id, acc_calibration_context_t *calibration_context);


/**
 * @brief Set a previously saved sensor calibration context and verify that the sensor calibration context is valid
 *
 * Must be called after RSS has been activated.
 * No active service is allowed on the sensor when setting the context.
 * If this function is successfully called, a new sensor calibration will not be done during service creation step.
 *
 * @param[in] sensor_id The sensor to set the context on
 * @param[in] calibration_context The calibration context to set
 *
 * @return True if successful, false otherwise
 */
bool acc_rss_calibration_context_set(acc_sensor_id_t sensor_id, acc_calibration_context_t *calibration_context);


/**
 * @brief Set a previously saved sensor calibration context, ignore the result from calibration context validation
 *
 * Must be called after RSS has been activated. Must only be used with a fresh calibration context
 * immediately after a successfull call to acc_rss_calibration_context_get.
 * No active service is allowed on the sensor when setting the context.
 * If this function is successfully called, a new sensor calibration will not be done during service creation step.
 *
 * @param[in] sensor_id The sensor to set the context on
 * @param[in] calibration_context The calibration context to set
 *
 * @return True if successful, false otherwise
 */
bool acc_rss_calibration_context_forced_set(acc_sensor_id_t sensor_id, acc_calibration_context_t *calibration_context);


/**
 * @brief Reset a calibration done on the specific sensor (or remove a previously set calibration context)
 *
 * No active service is allowed on the sensor when resetting the calibration
 * If this function is successfully called, a new sensor calibration will be done during service creation step.
 *
 * @param[in] sensor_id The sensor to reset the calibration on
 *
 * @return True if successful, false otherwise
 */
bool acc_rss_calibration_reset(acc_sensor_id_t sensor_id);


/**
 * @brief Enable or disable check of sensor id when creating a service or detector
 *
 * Must be called after RSS has been activated and before creating several services or detectors for the same sensor id.
 * After this function is called, with enable override set to true, it is possible to create several services or
 * detectors for the same sensor id. It is still only possible to have one service or detector per sensor id active
 * at the same time.
 *
 * @param[in] enable_override True if the sensor id check override should be enabled
 */
void acc_rss_override_sensor_id_check_at_creation(bool enable_override);


/**
 * @brief Set the log level that determines when the integration HAL logger function is called
 *
 * This function enables adjustment of the log level after RSS has been activated. Shall be called when
 * RSS is active as it has no effect otherwise. The severity of the logged messages is selected in the
 * same way as for log.log_level in @ref acc_hal_t.
 *
 * @param[in] level The severity level for selection of log output via @ref acc_log_function_t.
 */
void acc_rss_log_level_set(acc_log_level_t level);


/**
 * @}
 */

#endif
