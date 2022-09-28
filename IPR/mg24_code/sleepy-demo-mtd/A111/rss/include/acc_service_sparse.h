// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_SERVICE_SPARSE_H_
#define ACC_SERVICE_SPARSE_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_service.h"

/**
 * @defgroup Sparse Sparse Service
 * @ingroup Services
 *
 * @brief Sparse service API description
 *
 * @{
 */


/**
 * @brief Sampling mode
 *
 * The sampling mode changes how the hardware accelerated averaging is done. Mode A is optimized
 * for maximal independence of the depth points, giving a higher depth resolution than mode B.
 * Mode B is instead optimized for maximal radar loop gain per unit time spent on measuring. This
 * makes it more energy efficient and suitable for cases where small movements are to be detected
 * over long ranges. Mode A is more suitable for applications like gesture recognition, measuring
 * the distance to a movement, and speed measurements.
 *
 * Mode B typically gives roughly 3 dB better SNR per unit time than mode A. However, please note
 * that very short ranges of only one or a few points are suboptimal with mode B. In those cases,
 * always use mode A.
 */
typedef enum
{
	ACC_SERVICE_SPARSE_SAMPLING_MODE_A,
	ACC_SERVICE_SPARSE_SAMPLING_MODE_B
} acc_service_sparse_sampling_mode_enum_t;
typedef uint32_t acc_service_sparse_sampling_mode_t;


/**
 * @brief Metadata for the sparse service
 */
typedef struct
{
	/** Start of sweep, derived from request set by @ref acc_service_requested_start_set */
	float start_m;
	/** Length of sweep, derived from request set by @ref acc_service_requested_length_set */
	float length_m;
	/** Number of elements in the sparse data array */
	uint16_t data_length;
	/** Sweep rate used  */
	float sweep_rate;
	/** Distance between adjacent data points */
	float step_length_m;
} acc_service_sparse_metadata_t;


/**
 * @brief Metadata for each result provided by the sparse service
 */
typedef struct
{
	/** Indication of a sensor communication error, service probably needs to be restarted */
	bool sensor_communication_error;
	/** Indication of sensor data being saturated, can cause result instability */
	bool data_saturated;
	/** Indication of missed data from the sensor */
	bool missed_data;
} acc_service_sparse_result_info_t;


/**
 * @brief Create a configuration for a sparse service
 *
 * A configuration is created for the sparse service and populated
 * with default values.
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_sparse_configuration_create(void);


/**
 * @brief Destroy a sparse configuration
 *
 * Destroy a sparse configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_sparse_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the number of sweeps per service frame
 *
 * Gets the number of sweeps that will be returned in each frame from the service.
 *
 * @param[in] service_configuration The service configuration to get sweeps per result from
 * @return sweeps per frame
 */
uint16_t acc_service_sparse_configuration_sweeps_per_frame_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set sweeps per service frame
 *
 * Sets the number of sweeps that will be returned in each frame from the service.
 *
 * For sampling mode B, the number of sweeps per frame must be less than or equal to 64.
 *
 * @param[in] service_configuration The service configuration to set sweeps per results in
 * @param[in] sweeps Sweeps per frame
 */
void acc_service_sparse_configuration_sweeps_per_frame_set(acc_service_configuration_t service_configuration, uint16_t sweeps);


/**
 * @brief Get the sweep rate
 *
 * Gets the requested sweep rate. Values of zero of lower are treated as the maximum possible rate.
 *
 * @param[in] service_configuration The service configuration to get the sweep rate from
 * @return sweep_rate The sweep rate
 */
float acc_service_sparse_configuration_sweep_rate_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set sweep rate
 *
 * Sets the requested sweep rate. Values of zero of lower are treated as the maximum possible rate.
 *
 * @param[in] service_configuration The service configuration to set the sweep rate in
 * @param[in] sweep_rate The sweep rate
 */
void acc_service_sparse_configuration_sweep_rate_set(acc_service_configuration_t service_configuration, float sweep_rate);


/**
 * @brief Get sampling mode
 *
 * @param[in] service_configuration The configuration to get the sampling mode for
 * @return sampling mode
 */
acc_service_sparse_sampling_mode_t acc_service_sparse_sampling_mode_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set sampling mode
 *
 * @param[in] service_configuration The configuration to set the sampling mode for
 * @param[in] sampling_mode The sampling mode to use
 */
void acc_service_sparse_sampling_mode_set(acc_service_configuration_t service_configuration, acc_service_sparse_sampling_mode_t sampling_mode);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting downsampling factor to 4 makes
 * the distance between two points in the measured range ~24cm.
 *
 * @param[in] service_configuration The configuration to get downsampling factor from
 * @return The downsampling factor
 */
uint16_t acc_service_sparse_downsampling_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Sets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting downsampling factor to 4 makes
 * the distance between two points in the measured range ~24cm.
 *
 * The sparse service supports setting the downsampling factor to 1, 2, 4, or 8.
 *
 * @param[in] service_configuration The configuration to set downsampling factor in
 * @param[in] downsampling_factor The downsampling factor
 */
void acc_service_sparse_downsampling_factor_set(acc_service_configuration_t service_configuration, uint16_t downsampling_factor);


/**
 * @brief Get minimum service memory size
 *
 * The service memory size depends on several parameters and has a default
 * minimum size of around 4kByte. The service memory usage increases with increasing range.
 * This parameter changes the default minimum size from 4kByte to the used value.
 * With a small value there might be a time penalty for service creation and activation.
 *
 * @param[in] service_configuration The service configuration to get min service memory size from
 * @return The minimum service memory size
 */
uint16_t acc_service_sparse_min_service_memory_size_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set minimum service memory size
 *
 * The service memory size depends on several parameters and has a default
 * minimum size of around 4kByte. The service memory usage increases with increasing range.
 * This parameter changes the default minimum size from 4kByte to the used value.
 * With a small value there might be a time penalty for service creation and activation.
 *
 * @param[in] service_configuration The service configuration to set min service memory size in
 * @param[in] min_service_memory_size The minimum service memory size
 */
void acc_service_sparse_min_service_memory_size_set(acc_service_configuration_t service_configuration, uint16_t min_service_memory_size);


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
void acc_service_sparse_get_metadata(acc_service_handle_t handle, acc_service_sparse_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data sparse result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Sparse result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_sparse_get_next(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                 acc_service_sparse_result_info_t *result_info);


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
 * The length of the resulting data is provided in @ref acc_service_sparse_get_metadata
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Reference to Sparse result
 * @param[out] result_info Sparse result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_sparse_get_next_by_reference(acc_service_handle_t handle, uint16_t **data,
                                              acc_service_sparse_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. May fail if the service is already active.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data sparse result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Sparse result info, sending in NULL is ok
 * @return True if successful, false otherwise
 */
bool acc_service_sparse_execute_once(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                     acc_service_sparse_result_info_t *result_info);


/**
 * @}
 */

#endif
