// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_SERVICE_IQ_H_
#define ACC_SERVICE_IQ_H_

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "acc_service.h"

/**
 * @defgroup IQ IQ Service
 * @ingroup Services
 *
 * @brief IQ Service API description
 *
 * @{
 */


/**
 * @brief Output format
 *
 */
typedef enum
{
	ACC_SERVICE_IQ_OUTPUT_FORMAT_FLOAT_COMPLEX, // The output format is float complex
	ACC_SERVICE_IQ_OUTPUT_FORMAT_INT16_COMPLEX  // The output format is acc_int16_complex_t
} acc_service_iq_output_format_enum_t;
typedef uint32_t acc_service_iq_output_format_t;


/**
 * @brief Metadata for the iq service
 */
typedef struct
{
	/** Start of sweep, derived from request set by @ref acc_service_requested_start_set */
	float start_m;
	/** Length of sweep, derived from request set by @ref acc_service_requested_length_set */
	float length_m;
	/** Number of elements in the iq data array */
	uint16_t data_length;
	/** Number of stitches in the data */
	uint16_t stitch_count;
	/** Distance between adjacent data points */
	float step_length_m;
	/** The depth domain lowpass cutoff frequency ratio */
	float depth_lowpass_cutoff_ratio;
} acc_service_iq_metadata_t;


/**
 * @brief Metadata for each result provided by the iq service
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
	/** Power bin with information close to the sensor. Experimental. */
	uint16_t proximity_power;
} acc_service_iq_result_info_t;


/**
 * @brief Create a configuration for an iq service
 *
 * A configuration is created for the iq service and populated
 * with default values.
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_iq_configuration_create(void);


/**
 * @brief Destroy an iq configuration
 *
 * Destroy an iq configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_iq_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the distance domain lowpass filter cutoff frequency ratio override parameters
 *
 * The cutoff for the distance domain lowpass filter is specified as the ratio between the spatial
 * frequency cutoff and the sample frequency. A ratio close to zero damps everything except the lowest
 * frequencies. Increasing ratios output a wider band of spatial frequencies, and a ratio of 0.5 means
 * that the filter is deactivated.
 *
 * If unset, i.e., if override is false, the ratio will be chosen automatically. This ratio is
 * returned in the metadata upon creation of the service.
 *
 * @param[in] service_configuration The configuration to get the parameters from
 * @param[out] override If true, the specified cutoff ratio is used
 * @param[out] cutoff_ratio The cutoff ratio to use if override is true
 */
void acc_service_iq_depth_lowpass_cutoff_ratio_get(acc_service_configuration_t service_configuration, bool *override, float *cutoff_ratio);


/**
 * @brief Set the distance domain lowpass filter cutoff frequency ratio override parameters
 *
 * The cutoff for the distance domain lowpass filter is specified as the ratio between the spatial
 * frequency cutoff and the sample frequency. An input value of zero for the cutoff ratio will
 * configure the smoothest allowed filter. A cutoff ratio of 0.5 turns the filter off.
 *
 * The set of available cutoff frequencies is limited due to internal properties of the filter
 * implementation.
 *
 * If unset, i.e., if override is false, the ratio will be chosen automatically. This ratio is
 * returned in the metadata upon creation of the service.
 *
 * @param[in] service_configuration The configuration to set the parameters for
 * @param[in] override If true, the specified cutoff ratio is used
 * @param[in] cutoff_ratio The cutoff ratio to use if override is true
 */
void acc_service_iq_depth_lowpass_cutoff_ratio_set(acc_service_configuration_t service_configuration,
                                                   bool override,
                                                   float cutoff_ratio);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the IQ service, the base step length is ~0.5mm. Thus, for example setting downsampling factor to 4 makes
 * the distance between two points in the measured range ~2mm.
 *
 * @param[in] service_configuration The configuration to get downsampling factor from
 * @return downsampling_factor The downsampling factor
 */
uint16_t acc_service_iq_downsampling_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Sets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the IQ service, the base step length is ~0.5mm. Thus, for example setting downsampling factor to 4 makes
 * the distance between two points in the measured range ~2mm.
 *
 * The IQ service supports a downsampling factor of 1, 2, or 4.
 *
 * @param[in] service_configuration The configuration to set downsampling factor in
 * @param[in] downsampling_factor The downsampling factor
 */
void acc_service_iq_downsampling_factor_set(acc_service_configuration_t service_configuration, uint16_t downsampling_factor);


/**
 * @brief Get if noise level normalization will be done or not
 *
 * The purpose of the noise level normalization is to scale the signal according to the
 * sensor noise level to decrease the signal amplitude variation between individual sensors.
 *
 * @param[in] service_configuration The configuration to get the noise level normalization setting for
 * @return The noise level normalization flag
 */
bool acc_service_iq_noise_level_normalization_get(acc_service_configuration_t service_configuration);


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
void acc_service_iq_noise_level_normalization_set(acc_service_configuration_t service_configuration, bool noise_level_normalization);


/**
 * @brief Get if power bin with information close to the sensor is enabled
 *
 * Experimental.
 *
 * This function controls enabling of a power bin with information close to the sensor
 *
 * @param[in] service_configuration The configuration to get the noise level normalization setting for
 * @return True if proximity power output is enabled
 */
bool acc_service_iq_proximity_power_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set to enable of a power bin with information close to the sensor
 *
 * Experimental.
 *
 * This function controls enabling of a power bin with information close to the sensor
 *
 * @param[in] service_configuration The configuration to enable or disable the noise level normalization for
 * @param[in] enable True to enable proximity power output
 */
void acc_service_iq_proximity_power_set(acc_service_configuration_t service_configuration, bool enable);


/**
 * @brief Configure the output format of the IQ service
 *
 * @param[in,out] service_configuration The configuration to set the format on
 * @param[in] format The format that should be used
 */
void acc_service_iq_output_format_set(acc_service_configuration_t    service_configuration,
                                      acc_service_iq_output_format_t format);


/**
 * @brief Get the configured output format
 *
 * @param[in] service_configuration The configuration to get the format from
 *
 * @returns The configured output format
 */
acc_service_iq_output_format_t acc_service_iq_output_format_get(acc_service_configuration_t service_configuration);


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
void acc_service_iq_get_metadata(acc_service_handle_t handle, acc_service_iq_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready. The service must be configured for floating point output.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_iq_get_next(acc_service_handle_t handle, void *data, uint16_t data_length,
                             acc_service_iq_result_info_t *result_info);


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
 * The length of the resulting data is provided in @ref acc_service_iq_get_metadata
 *
 * Note that this function is only compatible with the ACC_SERVICE_IQ_OUTPUT_FORMAT_INT16_COMPLEX
 * option for @ref acc_service_iq_output_format_set
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Reference to IQ result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_iq_get_next_by_reference(acc_service_handle_t handle, acc_int16_complex_t **data,
                                          acc_service_iq_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. May fail if the service is already
 * active. The format of the data is configured by calling acc_service_iq_output_format_set().
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_iq_execute_once(acc_service_handle_t handle, void *data, uint16_t data_length,
                                 acc_service_iq_result_info_t *result_info);


/**
 * @}
 */

#endif
