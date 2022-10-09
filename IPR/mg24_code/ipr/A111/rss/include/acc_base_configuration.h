// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_BASE_CONFIGURATION_H_
#define ACC_BASE_CONFIGURATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions_a111.h"
#include "acc_definitions_common.h"


/**
 * @defgroup Base Service Base Configuration
 *
 * @brief Service base configuration API description
 *
 * @{
 */


#define ACC_BASE_CONFIGURATION_POWER_SAVE_MODE_OFF       ACC_POWER_SAVE_MODE_OFF
#define ACC_BASE_CONFIGURATION_POWER_SAVE_MODE_SLEEP     ACC_POWER_SAVE_MODE_SLEEP
#define ACC_BASE_CONFIGURATION_POWER_SAVE_MODE_READY     ACC_POWER_SAVE_MODE_READY
#define ACC_BASE_CONFIGURATION_POWER_SAVE_MODE_ACTIVE    ACC_POWER_SAVE_MODE_ACTIVE
#define ACC_BASE_CONFIGURATION_POWER_SAVE_MODE_HIBERNATE ACC_POWER_SAVE_MODE_HIBERNATE


/**
 * @brief Power save mode data type, see @ref acc_power_save_mode_enum_t
 */
typedef acc_power_save_mode_t acc_base_configuration_power_save_mode_t;


/**
 * @brief Service base configuration container
 */
struct acc_base_configuration;

typedef struct acc_base_configuration *acc_base_configuration_t;


/**
 * @brief Get the sensor ID for the sensor to be configured
 *
 * @param[in] configuration The base configuration to get the sensor id from
 * @return Sensor Id
 */
acc_sensor_id_t acc_base_configuration_sensor_get(acc_base_configuration_t configuration);


/**
 * @brief Set the sensor ID for the sensor to be configured
 *
 * @param[in] configuration The base configuration to set the sensor id in
 * @param[in] sensor_id The sensor id to set
 */
void acc_base_configuration_sensor_set(acc_base_configuration_t configuration, acc_sensor_id_t sensor_id);


/**
 * @brief Get the requested start of the sweep
 *
 * @param[in] configuration The base configuration to get the requested start from
 * @return Requested start
 */
float acc_base_configuration_requested_start_get(acc_base_configuration_t configuration);


/**
 * @brief Set the requested start of the sweep
 *
 * @param[in] configuration The base configuration to set the requested start in
 * @param[in] start_m The requested start in meters
 */
void acc_base_configuration_requested_start_set(acc_base_configuration_t configuration, float start_m);


/**
 * @brief Get the requested length of the sweep
 *
 * @param[in] configuration The base configuration to get the requested length from
 * @return Requested length
 */
float acc_base_configuration_requested_length_get(acc_base_configuration_t configuration);


/**
 * @brief Set the requested length of the sweep
 *
 * @param[in] configuration The base configuration to set the requested length in
 * @param[in] length_m The requested length in meters
 */
void acc_base_configuration_requested_length_set(acc_base_configuration_t configuration, float length_m);


/**
 * @brief Set the service repetition mode to on demand
 *
 * In on demand mode, the sensor produces data when requested by the application.
 * The application is also responsible for the timing between updates.
 *
 * This mode must be used if the configured length requires stitching in the service.
 *
 * @param[in] configuration The base configuration to set on demand mode in
 */
void acc_base_configuration_repetition_mode_on_demand_set(acc_base_configuration_t configuration);


/**
 * @brief Set the service repetition mode to streaming mode
 *
 * The sensor produces data according to the configured update rate using sensor
 * hardware timing which is very accurate.
 *
 * @param[in] configuration The base configuration to set streaming mode in
 * @param[in] update_rate The output data rate from the service in hertz
 */
void acc_base_configuration_repetition_mode_streaming_set(acc_base_configuration_t configuration, float update_rate);


/**
 * @brief Get power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data frame aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] configuration The base configuration to get power save mode for
 * @return Power save mode
 */
acc_power_save_mode_t acc_base_configuration_power_save_mode_get(acc_base_configuration_t configuration);


/**
 * @brief Set power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data frame aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] configuration The base configuration to set power save mode in
 * @param[in] power_save_mode The power save mode to use
 */
void acc_base_configuration_power_save_mode_set(acc_base_configuration_t configuration,
                                                acc_power_save_mode_t    power_save_mode);


/**
 * @brief Get receiver gain setting
 *
 * Will be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] configuration The base configuration to get receiver gain setting for
 * @return Receiver gain setting
 */
float acc_base_configuration_receiver_gain_get(acc_base_configuration_t configuration);


/**
 * @brief Set receiver gain setting
 *
 * Must be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] configuration The base configuration to set receiver gain setting in
 * @param[in] gain Receiver gain setting, must be between 0.0 and 1.0
 */
void acc_base_configuration_receiver_gain_set(acc_base_configuration_t configuration, float gain);


/**
 * @brief Get TX disable mode
 *
 * Will be true if TX is disabled, false otherwise.
 *
 * @param[in] configuration The base configuration to set TX disable mode in
 * @return TX disable mode
 */
bool acc_base_configuration_tx_disable_get(acc_base_configuration_t configuration);


/**
 * @brief Set TX disable mode
 *
 * If set to true, TX disable mode is enabled. This will disable the radio transmitter.
 * To measure RX noise floor, it is recommended to also switch off internal
 * noise level normalization (see each service if applicable).
 *
 * @param[in] configuration The base configuration to set TX disable mode in
 * @param[in] tx_disable TX disable mode, true or false
 */
void acc_base_configuration_tx_disable_set(acc_base_configuration_t configuration, bool tx_disable);


/**
 * @brief Get the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration The base configuration to get hardware accelerated average samples from
 * @return Hardware accelerated average samples
 */
uint8_t acc_base_configuration_hw_accelerated_average_samples_get(acc_base_configuration_t configuration);


/**
 * @brief Set the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration The base configuration to set hardware accelerated average samples in
 * @param[in] samples Hardware accelerated average samples
 */
void acc_base_configuration_hw_accelerated_average_samples_set(acc_base_configuration_t configuration, uint8_t samples);


/**
 * @}
 */

#endif
