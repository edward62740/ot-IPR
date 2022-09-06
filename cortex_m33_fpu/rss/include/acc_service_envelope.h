// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_SERVICE_ENVELOPE_H_
#define ACC_SERVICE_ENVELOPE_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_service.h"

/**
 * @defgroup Envelope Envelope Service
 * @ingroup Services
 *
 * @brief Envelope service API description
 *
 * @{
 */


/**
 * @brief Metadata for the envelope service
 */
typedef struct
{
	/** Start of sweep, derived from request set by @ref acc_service_requested_start_set */
	float    start_m;
	/** Length of sweep, derived from request set by @ref acc_service_requested_length_set */
	float    length_m;
	/** Number of elements in the envelope data array */
	uint16_t data_length;
	/** Number of stitches in the data */
	uint16_t stitch_count;
	/** Distance between adjacent data points */
	float    step_length_m;
} acc_service_envelope_metadata_t;


/**
 * @brief Metadata for each result provided by the envelope service
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
} acc_service_envelope_result_info_t;


/**
 * @brief Create a configuration for an envelope service
 *
 * A configuration is created for the envelope service and populated
 * with default values.
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_envelope_configuration_create(void);


/**
 * @brief Destroy an envelope configuration
 *
 * Destroy an envelope configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_envelope_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the envelope service, the base step length is ~0.5mm. Thus, for example setting downsampling factor to 4
 * makes the distance between two points in the measured range ~2mm.
 *
 * @param[in] service_configuration The configuration to get downsampling factor from
 * @return The downsampling factor
 */
uint16_t acc_service_envelope_downsampling_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Sets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the envelope service, the base step length is ~0.5mm. Thus, for example setting downsampling factor to 4
 * makes the distance between two points in the measured range ~2mm.
 *
 * The envelope service supports a downsampling factor of 1, 2, or 4.
 *
 * @param[in] service_configuration The configuration to set downsampling factor in
 * @param[in] downsampling_factor The downsampling factor
 */
void acc_service_envelope_downsampling_factor_set(acc_service_configuration_t service_configuration, uint16_t downsampling_factor);


/**
 * @brief Get running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 * The filtering is coherent and is done on complex valued IQ data before conversion to envelope data.
 *
 * @param[in] service_configuration The configuration to get the running average factor for
 * @return Running average factor
 */
float acc_service_envelope_running_average_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 * The filtering is coherent and is done on complex valued IQ data before conversion to envelope data.
 *
 * @param[in] service_configuration The configuration to set the running average factor for
 * @param[in] factor The running average factor to set
 */
void acc_service_envelope_running_average_factor_set(acc_service_configuration_t service_configuration, float factor);


/**
 * @brief Get if noise level normalization will be done or not
 *
 * The purpose of the noise level normalization is to scale the signal according to the
 * sensor noise level to decrease the signal amplitude variation between individual sensors.
 *
 * @param[in] service_configuration The configuration to get the noise level normalization setting for
 * @return The noise level normalization flag
 */
bool acc_service_envelope_noise_level_normalization_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set if noise level normalization should be done or not
 *
 * This function controls if a noise level normalization will be done at the beginning
 * of the envelope processing.
 *
 * The purpose of the noise level normalization is to scale the signal according to the
 * sensor noise level to decrease the signal amplitude variation between individual sensors.
 *
 * @param[in] service_configuration The configuration to enable or disable the noise level normalization for
 * @param[in] noise_level_normalization Flag to determine if noise level normalization should be done or not
 */
void acc_service_envelope_noise_level_normalization_set(acc_service_configuration_t service_configuration, bool noise_level_normalization);


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
void acc_service_envelope_get_metadata(acc_service_handle_t handle, acc_service_envelope_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Envelope result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Envelope result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_envelope_get_next(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                   acc_service_envelope_result_info_t *result_info);


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
 * The length of the resulting data is provided in @ref acc_service_envelope_get_metadata
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Reference to Envelope result
 * @param[out] result_info Envelope result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_envelope_get_next_by_reference(acc_service_handle_t handle, uint16_t **data,
                                                acc_service_envelope_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. May fail if the service is already active.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data Envelope result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Envelope result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_envelope_execute_once(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                       acc_service_envelope_result_info_t *result_info);


/**
 * @}
 */

#endif
