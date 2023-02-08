// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_SERVICE_POWER_BINS_H_
#define ACC_SERVICE_POWER_BINS_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_service.h"


/**
 * @defgroup Power Power Bins Service
 * @ingroup Services
 *
 * @brief Power Bins service API description
 *
 * @{
 */


/**
 * @brief Metadata for the power bins service
 */
typedef struct
{
	/** Start of sweep, derived from request set by @ref acc_service_requested_start_set */
	float    start_m;
	/** Length of sweep, derived from request set by @ref acc_service_requested_length_set */
	float    length_m;
	/** Number of elements in the power bins data array */
	uint16_t bin_count;
	/** Number of stitches in the data */
	uint16_t stitch_count;
	/** Distance between adjacent data points */
	float    step_length_m;
} acc_service_power_bins_metadata_t;


/**
 * @brief Metadata for each result provided by the power bins service
 */
typedef struct
{
	/** Indication of missed data from the sensor */
	bool missed_data;
	/** Indication of a sensor communication error, service probably needs to be restarted */
	bool sensor_communication_error;
	/** Indication of sensor data being saturated, can cause result instability */
	bool data_saturated;
	/** Indication of bad data quality that may be addressed by restarting the service to recalibrate the sensor */
	bool data_quality_warning;
} acc_service_power_bins_result_info_t;


/**
 * @brief Create a configuration for a power bins service
 *
 * A configuration is created for the power bins service and populated
 * with default values.
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_power_bins_configuration_create(void);


/**
 * @brief Destroy a power bins configuration
 *
 * Destroy a power bins configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_power_bins_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * @param[in] service_configuration The configuration to get downsampling factor from
 * @return The downsampling factor
 */
uint16_t acc_service_power_bins_downsampling_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Sets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * The power bins service supports a downsampling factor of 1, 2, or 4.
 *
 * In power bins, the downsampling factor does not affect the resulting data length.
 *
 * @param[in] service_configuration The configuration to set downsampling factor in
 * @param[in] downsampling_factor The downsampling factor
 */
void acc_service_power_bins_downsampling_factor_set(acc_service_configuration_t service_configuration, uint16_t downsampling_factor);


/**
 * @brief Get the requested bin count
 *
 * @param[in] service_configuration The service configuration to get the requested bin count from
 * @return Requested bin count
 */
uint16_t acc_service_power_bins_requested_bin_count_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the requested bin count
 *
 * @param[in] service_configuration The service configuration to set the requested bin count in
 * @param[in] requested_bin_count The requested bin count
 */
void acc_service_power_bins_requested_bin_count_set(acc_service_configuration_t service_configuration,
                                                    uint16_t                    requested_bin_count);


/**
 * @brief Get if noise level normalization will be done or not
 *
 * The purpose of the noise level normalization is to scale the signal according to the
 * sensor noise level to decrease the signal amplitude variation between individual sensors.
 *
 * @param[in] service_configuration The configuration to get the noise level normalization setting for
 * @return The noise level normalization flag
 */
bool acc_service_power_bins_noise_level_normalization_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set if noise level normalization should be done or not
 *
 * This function controls if a noise level normalization will be done at the beginning
 * of the processing.
 *
 * The purpose of the noise level normalization is to scale the signal according to the
 * sensor noise level to decrease the signal amplitude variation between individual sensors.
 *
 * @param[in] service_configuration The configuration to enable or disable the noise level normalization for
 * @param[in] noise_level_normalization Flag to determine if noise level normalization should be done or not
 */
void acc_service_power_bins_noise_level_normalization_set(acc_service_configuration_t service_configuration, bool noise_level_normalization);


/**
 * @brief Get service metadata
 *
 * Each service provide metadata after being created with information that could be relevant for an application.
 * The metadata contain information such as data length and actual start and length.
 *
 * May only be called after a service has been created.
 *
 * @param[in] handle The service handle for the service to get metadata for
 * @param[out] metadata Metadata results are provided in this parameter
 */
void acc_service_power_bins_get_metadata(acc_service_handle_t handle, acc_service_power_bins_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Power bins result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Power Bins result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_power_bins_get_next(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                     acc_service_power_bins_result_info_t *result_info);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 *
 * The provided memory is only valid until the next call to get_next for the
 * specified handle. The memory is specific for the handle and cannot be shared
 * between handles/services.
 *
 * The length of the resulting data is provided in @ref acc_service_power_bins_get_metadata
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Reference to Power Bins result
 * @param[out] result_info Power Bins result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_power_bins_get_next_by_reference(acc_service_handle_t handle, uint16_t **data,
                                                  acc_service_power_bins_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. May fail if the service is already active.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data Power bins result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Power Bins result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_power_bins_execute_once(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                         acc_service_power_bins_result_info_t *result_info);


/**
 * @}
 */


#endif
