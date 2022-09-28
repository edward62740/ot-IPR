// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_DETECTOR_PRESENCE_H_
#define ACC_DETECTOR_PRESENCE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_definitions_a111.h"
#include "acc_definitions_common.h"

/**
 * @defgroup Presence Presence Detector
 * @ingroup Detectors
 *
 * @brief Presence detector API description
 *
 * @{
 */


/**
 * @brief Presence detector handle
 */
struct acc_detector_presence_handle;

typedef struct acc_detector_presence_handle *acc_detector_presence_handle_t;


/**
 * @brief Presence detector configuration container
 */
struct acc_detector_presence_configuration;

typedef struct acc_detector_presence_configuration *acc_detector_presence_configuration_t;


/**
 * @brief Parameter structure for data filtering settings
 */
typedef struct
{
	/** Time constant in s for inter-frame signal smoothing after the bandpass filtering */
	float inter_frame_deviation_time_const;
	/** Low pass cutoff frequency in Hz for inter-frame signal bandpass filtering */
	float inter_frame_fast_cutoff;
	/** High pass cutoff frequency in Hz for inter-frame signal bandpass filtering */
	float inter_frame_slow_cutoff;
	/** Time constant in s for inter-frame signal smoothing */
	float intra_frame_time_const;
	/** Weight between 0 and 1 for presence score interpolation between the inter-frame and intra-frame signals */
	float intra_frame_weight;
	/** Time constant in s for smoothing of the presence score */
	float output_time_const;
} acc_detector_presence_configuration_filter_parameters_t;


/**
 * @brief Presence detector results container
 */
typedef struct
{
	/** True if presence was detected, False otherwise */
	bool presence_detected;
	/** A measure of the amount of motion detected */
	float presence_score;
	/** The distance, in meters, to the detected object */
	float presence_distance;

	/** Indication of a sensor communication error, detector probably needs to be restarted */
	bool sensor_communication_error;
	/** Indication of sensor data being saturated, can cause result instability */
	bool data_saturated;
} acc_detector_presence_result_t;


/**
 * @brief A callback for retrieving the service data buffer that the detector is based on
 *
 * @param[in] data A pointer to the buffer with sparse data
 * @param[in] data_size Size of the data buffer in bytes
 * @param[in] client_reference Pointer to a client reference
 */
typedef void (*acc_detector_presence_service_data_callback_t)(const uint16_t *data, size_t data_size, void *client_reference);


/**
 * @brief Create a configuration for a presence detector
 *
 * @return Presence detector configuration, NULL if creation was not possible
 */
acc_detector_presence_configuration_t acc_detector_presence_configuration_create(void);


/**
 * @brief Destroy a presence detector configuration
 *
 * @param[in] presence_configuration The configuration to destroy, set to NULL
 */
void acc_detector_presence_configuration_destroy(acc_detector_presence_configuration_t *presence_configuration);


/**
 * @brief Create a presence detector with the provided configuration
 *
 * Only one presence detector may exist for a specific sensor at any given time and
 * invalid configurations will not allow for presence detector creation.
 *
 * @param[in] presence_configuration The presence detector configuration to create a presence detector with
 * @return Presence detector handle, NULL if presence detector was not possible to create
 */
acc_detector_presence_handle_t acc_detector_presence_create(acc_detector_presence_configuration_t presence_configuration);


/**
 * @brief Destroy a presence detector identified with the provided handle
 *
 * Destroy the context of a presence detector allowing another presence detector to be created using the
 * same resources. The presence detector handle reference is set to NULL after destruction.
 * If NULL is sent in, nothing happens.
 *
 * @param[in] presence_handle A reference to the presence detector handle to destroy
 */
void acc_detector_presence_destroy(acc_detector_presence_handle_t *presence_handle);


/**
 * @brief Reconfigure a presence detector with the provided configuration
 *
 * Only one presence detector may exist for a specific sensor at any given time and
 * invalid reconfigurations will not allow for presence detector creation.
 *
 * @param[in] presence_handle A reference to the presence detector handle to reconfigure
 * @param[in] presence_configuration The presence detector configuration to reconfigure a presence detector with
 * @return True if possible to reconfigure
 */
bool acc_detector_presence_reconfigure(acc_detector_presence_handle_t        *presence_handle,
                                       acc_detector_presence_configuration_t presence_configuration);


/**
 * @brief Activate the presence detector associated with the provided handle
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to activate
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_activate(acc_detector_presence_handle_t presence_handle);


/**
 * @brief Deactivate the presence detector associated with the provided handle
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to deactivate
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_deactivate(acc_detector_presence_handle_t presence_handle);


/**
 * @brief Retrieve the next result from the presence detector
 *
 * May only be called after a presence detector has been activated, blocks
 * the application until a result is ready. Can still be called after vector
 * output mode has been selected.
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to get the next result for
 * @param[out] result Presence detector results, can be NULL if no result is wanted.
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_get_next(acc_detector_presence_handle_t presence_handle, acc_detector_presence_result_t *result);


/**
 * @brief Retrieve the next distance point vector from the presence detector
 *
 * May only be called after a presence detector has been activated, blocks
 * the application until a result is ready. Vector output mode has to be set
 * to true with the function @ref acc_detector_presence_configuration_vector_output_mode_set,
 * otherwise returns with an error. Memory is owned by the detector and the client receives a pointer.
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to get the next result for
 * @param[out] distance_point_vector_length The number of elements in the distance_point_vector, can be NULL if no result is wanted
 * @param[out] distance_point_vector The distance point vector, can be NULL if no result is wanted
 * @param[out] result Presence detector results, same as result from acc_detector_presence_get_next
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_distance_point_vector_get_next(acc_detector_presence_handle_t presence_handle,
                                                          uint16_t                       *distance_point_vector_length,
                                                          float                          **distance_point_vector,
                                                          acc_detector_presence_result_t *result);


/**
 * @brief Get start of sweep in m
 *
 * @param[in] configuration The configuration to get the sweep start for
 * @return requested sweep start in m
 */
float acc_detector_presence_configuration_start_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set start of sweep in m
 *
 * @param[in] configuration The configuration to set the sweep start for
 * @param[in] start The requested sweep start in m
 */
void acc_detector_presence_configuration_start_set(acc_detector_presence_configuration_t configuration, float start);


/**
 * @brief Get length of sweep in m
 *
 * @param[in] configuration The configuration to get the sweep length for
 * @return requested sweep length in m
 */
float acc_detector_presence_configuration_length_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set length of sweep in m
 *
 * @param[in] configuration The configuration to set the requested sweep length for
 * @param[in] length The requested sweep length in m
 */
void acc_detector_presence_configuration_length_set(acc_detector_presence_configuration_t configuration, float length);


/**
 * @brief Get sensor ID
 *
 * @param[in] configuration The configuration to get the sensor ID for
 * @return sensor ID
 */
acc_sensor_id_t acc_detector_presence_configuration_sensor_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sensor ID
 *
 * @param[in] configuration The configuration to set the sensor ID for
 * @param[in] sensor_id The sensor ID
 */
void acc_detector_presence_configuration_sensor_set(acc_detector_presence_configuration_t configuration, acc_sensor_id_t sensor_id);


/**
 * @brief Get detection threshold
 *
 * @param[in] configuration The configuration to get the detection threshold for
 * @return detection threshold
 */
float acc_detector_presence_configuration_detection_threshold_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set detection threshold
 *
 * @param[in] configuration The configuration to set the detection threshold for
 * @param[in] detection_threshold The threshold
 */
void acc_detector_presence_configuration_detection_threshold_set(acc_detector_presence_configuration_t configuration,
                                                                 float                                 detection_threshold);


/**
 * @brief Get detection update rate
 *
 * Set the update rate of which the client call the detector to produce data. It's the clients responsibility
 * to keep the configured timing.
 *
 * @param[in] configuration The configuration to get the detection update rate for
 * @return detection update rate
 */
float acc_detector_presence_configuration_update_rate_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set detection update rate
 *
 * Set the update rate of which the client call the detector to produce data. It's the clients responsibility
 * to keep the configured timing.
 *
 * @param[in] configuration The configuration to set the detection update rate for
 * @param[in] update_rate
 */
void acc_detector_presence_configuration_update_rate_set(acc_detector_presence_configuration_t configuration,
                                                         float                                 update_rate);


/**
 * @brief Get the number of sweeps per frame
 *
 * @param[in] configuration The configuration to get the number of sweeps per frame for
 * @return The number of sweeps per frame
 */
uint16_t acc_detector_presence_configuration_sweeps_per_frame_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set the number of sweeps per frame
 *
 * @param[in] configuration The configuration to set the number of sweeps per frame for
 * @param[in] sweeps_per_frame The requested number of sweeps per frame
 */
void acc_detector_presence_configuration_sweeps_per_frame_set(acc_detector_presence_configuration_t configuration,
                                                              uint16_t                              sweeps_per_frame);


/**
 * @brief Get sensor data filtering parameters
 *
 * See @ref acc_detector_presence_configuration_filter_parameters_set
 *
 * @param[in] configuration The configuration to get the filter parameters for
 * @return The filter parameters, see @ref acc_detector_presence_configuration_filter_parameters_t
 */
acc_detector_presence_configuration_filter_parameters_t acc_detector_presence_configuration_filter_parameters_get(
	acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sensor data filtering parameters
 *
 * Set filter parameters for the sensor data filtering that is performed before presence detection thresholding.
 *
 * @param[in] configuration The configuration to set the filter parameters for
 * @param[in] filter_parameters The filter parameter structure, see @ref acc_detector_presence_configuration_filter_parameters_t
 */
void acc_detector_presence_configuration_filter_parameters_set(acc_detector_presence_configuration_t                         configuration,
                                                               const acc_detector_presence_configuration_filter_parameters_t *filter_parameters);



/**
 * @brief Get the number of principal components removed in the PCA based noise reduction
 *
 * @param[in] configuration The configuration to get the number of principal components of noise to remove for
 * @return The number of principal components of noise to remove, between 0 and 2
 */
uint8_t acc_detector_presence_configuration_nbr_removed_pc_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set the number of principal components removed in the PCA based noise reduction
 *
 * Sets the number of principal components removed in the PCA based noise reduction.
 * Filters out static reflections.
 * Setting to 0 (default) disables the PCA based noise reduction completely.
 * For more information on the PCA based noise reduction algorithm, see
 * <a href="https://acconeer-python-exploration.readthedocs.io/en/latest/processing/
 * presence_detection_sparse.html#pca-based-noise-reduction">Read-the-Docs</a>
 *
 * @param[in] configuration The configuration to set the number of principal components of noise to remove for
 * @param[in] nbr_removed_pc The number of principal components of noise to remove, between 0 and 2
 */
void acc_detector_presence_configuration_nbr_removed_pc_set(acc_detector_presence_configuration_t configuration, uint8_t nbr_removed_pc);


/**
 * @brief Get power save mode
 *
 * @param[in] configuration The configuration to get the power save mode for
 * @return power save mode
 */
acc_power_save_mode_t acc_detector_presence_configuration_power_save_mode_get(
	acc_detector_presence_configuration_t configuration);


/**
 * @brief Set power save mode
 *
 * @param[in] configuration The configuration to set the power save mode for
 * @param[in] power_save_mode The power save mode
 */
void acc_detector_presence_configuration_power_save_mode_set(acc_detector_presence_configuration_t configuration,
                                                             acc_power_save_mode_t                 power_save_mode);


/**
 * @brief Get the current service profile used by the detector
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] configuration The configuration to get a profile from
 * @return The current profile, 0 if configuration is invalid
 */
acc_service_profile_t acc_detector_presence_configuration_service_profile_get(
	acc_detector_presence_configuration_t configuration);


/**
 * @brief Set a service profile to be used by the detector
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] configuration   The configuration to set a profile for
 * @param[in] service_profile The profile to set
 */
void acc_detector_presence_configuration_service_profile_set(acc_detector_presence_configuration_t configuration,
                                                             acc_service_profile_t                 service_profile);


/**
 * @brief Get the current receiver gain used by the detector
 *
 * @param[in] configuration The configuration to get gain from
 * @return The current gain
 */
float acc_detector_presence_configuration_receiver_gain_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set a receiver gain to be used by the detector
 *
 * @param[in] configuration   The configuration to set gain for
 * @param[in] gain            The gain to set
 */
void acc_detector_presence_configuration_receiver_gain_set(acc_detector_presence_configuration_t configuration,
                                                           float                                 gain);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting downsampling factor to 3 makes
 * the distance between two points in the measured range ~18cm.
 *
 * @param[in] configuration The configuration to get downsampling factor from
 * @return The downsampling factor
 */
uint16_t acc_detector_presence_configuration_downsampling_factor_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Sets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting downsampling factor to 3 makes
 * the distance between two points in the measured range ~18cm.
 *
 * The sparse service supports setting an arbitrary downsampling factor of at least 1.
 *
 * @param[in] configuration         The configuration to set downsampling factor in
 * @param[in] downsampling_factor   The downsampling factor
 */
void acc_detector_presence_configuration_downsampling_factor_set(acc_detector_presence_configuration_t configuration, uint16_t downsampling_factor);


/**
 * @brief Get the hardware accelerated average samples
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration The configuration to get hardware accelerated average samples from
 * @return Hardware accelerated average samples
 */
uint8_t acc_detector_presence_configuration_hw_accelerated_average_samples_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set the hardware accelerated average samples
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] configuration   The configuration to set hardware accelerated average samples in
 * @param[in] samples         Hardware accelerated average samples
 */
void acc_detector_presence_configuration_hw_accelerated_average_samples_set(acc_detector_presence_configuration_t configuration, uint8_t samples);


/**
 * @brief Get asynchronous measurement mode
 *
 * If set to true, asynchronous measurement is enabled.
 * In synchronous mode the sensor will measure while the host is waiting.
 * In asynchronous mode the sensor will measure while the host is working.
 *
 * This means that if in synchronous mode, the sensor will only measure during
 * a get_next call, while in asynchronous mode the sensor can measure outside
 * of the get_next call.
 *
 * @param[in] configuration The configuration to get asynchronous_measurement mode from
 * @return Asynchronous measurement mode
 */
bool acc_detector_presence_configuration_asynchronous_measurement_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Set asynchronous measurement mode
 *
 * If set to true, asynchronous measurement is enabled.
 * In synchronous mode the sensor will measure while the host is waiting.
 * In asynchronous mode the sensor will measure while the host is working.
 *
 * This means that if in synchronous mode, the sensor will only measure during
 * a get_next call, while in asynchronous mode the sensor can measure outside
 * of the get_next call.
 *
 * @param[in] configuration The configuration to set asynchronous_measurement mode in
 * @param[in] asynchronous_measurement asynchronous measurement mode, true or false
 */
void acc_detector_presence_configuration_asynchronous_measurement_set(acc_detector_presence_configuration_t configuration, bool asynchronous_measurement);


/**
 * @brief Gets vector output mode
 *
 * vector_output_mode decides whether or not the presence detector should output a distance point vector
 * from the function @ref acc_detector_presence_distance_point_vector_get_next.
 *
 * @param[in] configuration The configuration to get vector_output_mode from
 * @return vector_output_mode
 */
bool acc_detector_presence_configuration_vector_output_mode_get(acc_detector_presence_configuration_t configuration);


/**
 * @brief Sets vector output mode
 *
 * vector_output_mode decides whether or not the presence detector should output a distance point vector.
 *
 * @param[in] configuration        The configuration to set vector_output_mode in
 * @param[in] vector_output_mode   The vector_output_mode
 */
void acc_detector_presence_configuration_vector_output_mode_set(acc_detector_presence_configuration_t configuration, bool vector_output_mode);


/**
 * @brief Set a callback function to get the service data
 *
 * A callback can be used to get the sparse service buffer. The data is the sparse data that is fed into the presence algorithm
 *
 * @param[in] configuration         The configuration to set vector_output_mode in
 * @param[in] service_data_callback The callback to get service data
 * @param[in] client_reference      Pointer to a client reference
 */
void acc_detector_presence_configuration_set_service_data_callback(acc_detector_presence_configuration_t         configuration,
                                                                   acc_detector_presence_service_data_callback_t service_data_callback,
                                                                   void                                          *client_reference);


/**
 * @}
 */

#endif
