// Copyright (c) Acconeer AB, 2018-2022
// All rights reserved

#ifndef ACC_SERVICE_H_
#define ACC_SERVICE_H_

#include <stdbool.h>

#include "acc_base_configuration.h"
#include "acc_definitions_a111.h"
#include "acc_definitions_common.h"

/**
 * @defgroup Services Services
 *
 * @brief Radar services provided by Acconeer
 *
 * @defgroup Experimental Experimental
 * @brief Features in an early version
 *
 *          In our code you might encounter features tagged “Experimental”.
 *          This means that the feature in question is an early version that has a
 *          limited test scope, and the API and/or functionality might change in
 *          upcoming releases.
 *
 *          The intention is to let users try these features out and we appreciate
 *          feedback.
 *
 * @defgroup Generic Generic Service API
 * @ingroup Services
 *
 * @brief Generic service API description
 *
 * @{
 */


/**
 * @brief Generic service configuration container
 */
struct acc_service_configuration;

typedef struct acc_service_configuration *acc_service_configuration_t;


/**
 * @brief Generic service handle
 */
struct acc_service_handle;

typedef struct acc_service_handle *acc_service_handle_t;


/**
 * @brief Create a service with the provided configuration
 *
 * Only one service may exist for a specific sensor at any given time,
 * unless @ref acc_rss_override_sensor_id_check_at_creation has been invoked.
 * Invalid configurations will not allow for service creation.
 *
 * @param[in] configuration The service configuration to create a service with
 * @return Service handle, NULL if service was not possible to create
 */
acc_service_handle_t acc_service_create(acc_service_configuration_t configuration);


/**
 * @brief Activate the service associated with the provided handle
 *
 * When activated, the application can get data from the service with the
 * associated handle.
 *
 * @param[in] service_handle The service handle for the service to activate
 * @return True if successful, false otherwise
 */
bool acc_service_activate(acc_service_handle_t service_handle);


/**
 * @brief Deactivate the service associated with the provided handle
 *
 * @param[in] service_handle The service handle for the service to deactivate
 * @return True if successful, false otherwise
 */
bool acc_service_deactivate(acc_service_handle_t service_handle);


/**
 * @brief Destroy a service identified with the provided service handle
 *
 * Destroy the context of a service allowing another service to be created using the
 * same resources. The service handle reference is set to NULL after destruction.
 *
 * @param[in] service_handle A reference to the service handle to destroy
 */
void acc_service_destroy(acc_service_handle_t *service_handle);


/**
 * @brief Retrieve a base configuration from a service configuration
 *
 * The base configuration can be used to configure the service for different use cases.
 * See @ref acc_base_configuration.h for configuration parameters.
 *
 * @param[in] service_configuration The service configuration to get a base configuration from
 * @return Base configuration, NULL if the service configuration does not contain a base configuration
 */
acc_base_configuration_t acc_service_get_base_configuration(acc_service_configuration_t service_configuration);


/**
 * @brief Get the sensor ID for the sensor to be configured
 *
 * @param[in] configuration The service configuration to get the sensor id from
 * @return Sensor Id
 */
acc_sensor_id_t acc_service_sensor_get(acc_service_configuration_t configuration);


/**
 * @brief Set the sensor ID for the sensor to be configured
 *
 * @param[in] configuration The service configuration to set the sensor id in
 * @param[in] sensor_id The sensor id to set
 */
void acc_service_sensor_set(acc_service_configuration_t configuration, acc_sensor_id_t sensor_id);


/**
 * @brief Get the requested start of the sweep
 *
 * @param[in] configuration The service configuration to get the requested start from
 * @return Requested start
 */
float acc_service_requested_start_get(acc_service_configuration_t configuration);


/**
 * @brief Set the requested start of the sweep
 *
 * @param[in] configuration The service configuration to set the requested start in
 * @param[in] start_m The requested start in meters
 */
void acc_service_requested_start_set(acc_service_configuration_t configuration, float start_m);


/**
 * @brief Get the requested length of the sweep
 *
 * @param[in] configuration The service configuration to get the requested length from
 * @return Requested length
 */
float acc_service_requested_length_get(acc_service_configuration_t configuration);


/**
 * @brief Set the requested length of the sweep
 *
 * @param[in] configuration The service configuration to set the requested length in
 * @param[in] length_m The requested length in meters
 */
void acc_service_requested_length_set(acc_service_configuration_t configuration, float length_m);


/**
 * @brief Set the service repetition mode to on demand
 *
 * In on demand mode, the sensor produces data when requested by the application.
 * The application is also responsible for the timing between updates.
 *
 * This mode must be used if the configured length requires stitching in the service.
 *
 * @param[in] configuration The service configuration to set on demand mode in
 */
void acc_service_repetition_mode_on_demand_set(acc_service_configuration_t configuration);


/**
 * @brief Set the service repetition mode to streaming mode
 *
 * The sensor produces data according to the configured update rate using sensor
 * hardware timing which is very accurate.
 *
 * @param[in] configuration The service configuration to set streaming mode in
 * @param[in] update_rate The output data rate from the service in hertz
 */
void acc_service_repetition_mode_streaming_set(acc_service_configuration_t configuration, float update_rate);


/**
 * @brief Get power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data frame aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] configuration The service configuration to get power save mode for
 * @return Power save mode
 */
acc_power_save_mode_t acc_service_power_save_mode_get(acc_service_configuration_t configuration);


/**
 * @brief Set power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data frame aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] configuration The service configuration to set power save mode in
 * @param[in] power_save_mode The power save mode to use
 */
void acc_service_power_save_mode_set(acc_service_configuration_t configuration,
                                     acc_power_save_mode_t       power_save_mode);


/**
 * @brief Get receiver gain setting
 *
 * Will be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] configuration The service configuration to get receiver gain setting for
 * @return Receiver gain setting
 */
float acc_service_receiver_gain_get(acc_service_configuration_t configuration);


/**
 * @brief Set receiver gain setting
 *
 * Must be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] configuration The service configuration to set receiver gain setting in
 * @param[in] gain Receiver gain setting, must be between 0.0 and 1.0
 */
void acc_service_receiver_gain_set(acc_service_configuration_t configuration, float gain);


/**
 * @brief Get TX disable mode
 *
 * Will be true if TX is disabled, false otherwise.
 *
 * @param[in] configuration The service configuration to get TX disable mode for
 * @return TX disable mode
 */
bool acc_service_tx_disable_get(acc_service_configuration_t configuration);


/**
 * @brief Set TX disable mode
 *
 * If set to true, TX disable mode is enabled. This will disable the radio transmitter.
 * To measure RX noise floor, it is recommended to also switch off internal
 * noise level normalization (see each service if applicable).
 *
 * @param[in] configuration The service configuration to set TX disable mode in
 * @param[in] tx_disable TX disable mode, true or false
 */
void acc_service_tx_disable_set(acc_service_configuration_t configuration, bool tx_disable);


/**
 * @brief Get the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration The service configuration to get hardware accelerated average samples from
 * @return Hardware accelerated average samples
 */
uint8_t acc_service_hw_accelerated_average_samples_get(acc_service_configuration_t configuration);


/**
 * @brief Set the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration The service configuration to set hardware accelerated average samples in
 * @param[in] samples Hardware accelerated average samples
 */
void acc_service_hw_accelerated_average_samples_set(acc_service_configuration_t configuration, uint8_t samples);


/**
 * @brief Get asynchronous measurement mode
 *
 * If set to true, asynchronous measurement is enabled.
 * In synchronous mode the sensor will generate sweep data while the host is waiting.
 * In asynchronous mode the sensor will generate sweep data while the host is working.
 *
 * This means that if in synchronous mode, the sensor will only measure during
 * a get_next call, while in asynchronous mode the sensor can measure outside
 * of the get_next call.
 *
 * Setting asynchronous measurement to false (i.e using synchronous mode) is incompatible
 * with repetition mode streaming where the sensor is in control of the update rate timing.
 *
 * @param[in] configuration The service configuration to get asynchronous_measurement mode from
 * @return asynchronous measurement mode
 */
bool acc_service_asynchronous_measurement_get(acc_service_configuration_t configuration);


/**
 * @brief Set asynchronous measurement mode
 *
 * If set to true, asynchronous measurement is enabled.
 * In synchronous mode the sensor will generate sweep data while the host is waiting.
 * In asynchronous mode the sensor will generate sweep data while the host is working.
 *
 * This means that if in synchronous mode, the sensor will only measure during
 * a get_next call, while in asynchronous mode the sensor can measure outside
 * of the get_next call.
 *
 * Setting asynchronous measurement to false (i.e using synchronous mode) is incompatible
 * with repetition mode streaming where the sensor is in control of the update rate timing.
 *
 * @param[in] configuration The service configuration to set asynchronous_measurement mode in
 * @param[in] asynchronous_measurement asynchronous measurement mode, true or false
 */
void acc_service_asynchronous_measurement_set(acc_service_configuration_t configuration, bool asynchronous_measurement);


/**
 * @brief Get the currently used service profile
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] service_configuration The configuration to get a profile for
 * @return The current profile, 0 if configuration is invalid
 */
acc_service_profile_t acc_service_profile_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set a service profile
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] service_configuration The configuration to set a profile for
 * @param[in] profile                The profile to set
 */
void acc_service_profile_set(acc_service_configuration_t service_configuration,
                             acc_service_profile_t       profile);


/**
 * @brief Get Maximize signal attenuation mode
 *
 * Will be true if Maximize signal attenuation mode is enabled, false otherwise
 *
 * @param[in] service_configuration The configuration to get Maximize signal attenuation mode for
 * @return Maximize signal attenuation mode
 */
bool acc_service_maximize_signal_attenuation_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set Maximize signal attenuation mode
 *
 * Enable or disable Maximize signal attenuation mode to measure the direct leakage
 *
 * @param[in] service_configuration        The configuration to set Maximize signal attenuation mode in
 * @param[in] maximize_signal_attenuation  Maximize signal attenuation mode, true or false
 */
void acc_service_maximize_signal_attenuation_set(acc_service_configuration_t service_configuration,
                                                 bool                        maximize_signal_attenuation);


/**
 * @brief Set the maximum unambiguous range
 *
 * Experimental.
 *
 * Sets the maximum unambiguous range (MUR), which in turn sets the maximum measurable
 * distance (MMD).
 *
 * The MMD is the maximum value for the range end, i.e., the range start + length. The MMD
 * is smaller than the MUR due to hardware limitations.
 *
 * The MUR is the maximum distance at which an object can be located to guarantee that its
 * reflection corresponds to the most recent transmitted pulse. Objects farther away than
 * the MUR may fold into the measured range. For example, with a MUR of 10 m, an object at
 * 12 m could become visible at 2 m.
 *
 * A higher setting gives a larger MUR/MMD, but comes at a cost of increasing the
 * measurement time for a sweep. The measurement time is approximately proportional to the
 * MUR.
 *
 * This setting changes the pulse repetition frequency (PRF) of the radar system. The
 * relation between PRF and MUR is
 * MUR = c / (2 * PRF)
 * where c is the speed of light.
 *
 * | Setting           |    MUR |    MMD |      PRF |
 * |------------------:|-------:|-------:|---------:|
 * | ACC_SERVICE_MUR_6 | 11.5 m |  7.0 m | 13.0 MHz |
 * | ACC_SERVICE_MUR_9 | 17.3 m | 12.7 m |  8.7 MHz |
 *
 * It is not possible to change MUR for the IQ service.
 *
 * @param[in] service_configuration The configuration
 * @param[in] max_unambiguous_range The desired maximum unambiguous range
 */
void acc_service_mur_set(acc_service_configuration_t service_configuration,
                         acc_service_mur_t           max_unambiguous_range);


/**
 * @brief Get the maximum unambiguous range
 *
 * Experimental.
 *
 * This gets the maximum unambiguous range. For more information see acc_service_mur_set().
 *
 * @param[in] service_configuration The configuration
 * @return Maximum unambiguous range
 */
acc_service_mur_t acc_service_mur_get(acc_service_configuration_t service_configuration);


/**
 * @}
 */

#endif
