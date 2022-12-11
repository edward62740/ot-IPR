// Copyright (c) Acconeer AB, 2020-2021
// All rights reserved

#ifndef ACC_DETECTOR_DISTANCE_H_
#define ACC_DETECTOR_DISTANCE_H_


#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions_a111.h"
#include "acc_definitions_common.h"


/**
 * @defgroup Distance Distance Detector
 * @ingroup Detectors
 *
 * @brief Distance detector API description
 *
 * @{
 */


/**
 * @brief Metadata for the distance detector
 */
typedef struct
{
	/** Start of measurement in meters, derived from request set by @ref acc_detector_distance_configuration_requested_start_set */
	float start_m;
	/** Length of measurement in meters, derived from request set by @ref acc_detector_distance_configuration_requested_length_set */
	float length_m;
	/** Length needed for potential background, used in @ref acc_detector_distance_record_background */
	uint16_t background_length;
} acc_detector_distance_metadata_t;


/**
 * @brief Metadata for the recorded background
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
} acc_detector_distance_recorded_background_info_t;


/**
 * @brief Metadata for each result provided by the distance detector
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
	/** Number of detected peaks returned to application. Maximum is number of requested peaks.*/
	uint16_t number_of_peaks;
	/** Indicating if any measurement samples are above the threshold.*/
	bool measurement_sample_above_threshold;
	/** The closest measurement distance above the threshold. Only valid if any measurement samples are above
	 * threshold. Note, it is valid even if no peaks are found.*/
	float closest_detection_m;
} acc_detector_distance_result_info_t;


/**
 * @brief Enum for threshold type
 */
typedef enum
{
	ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_FIXED,
	ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED,
	ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_CFAR
} acc_detector_distance_threshold_type_t;


/**
 * @brief Enum for peak sorting algorithms
 */
typedef enum
{
	/* Return peaks with the closest detection first. */
	ACC_DETECTOR_DISTANCE_PEAK_SORTING_CLOSEST_FIRST,
	/* Return peaks with the peak with the highest amplitude first. */
	ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST_FIRST,
	/* Return peaks with the peak from the strongest reflector first.
	   The decrease in amplitude over distance is accounted for.
	   Cannot be combined with negative start range. */
	ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST_REFLECTOR_FIRST,
	/* Return peaks with the peak from the strongest flat reflector first.
	   The decrease in amplitude over distance is accounted for.
	   Cannot be combined with negative start range. */
	ACC_DETECTOR_DISTANCE_PEAK_SORTING_STRONGEST_FLAT_REFLECTOR_FIRST
} acc_detector_distance_peak_sorting_t;


/**
 * @brief Distance detector handle
 */
struct acc_detector_distance_handle;

typedef struct acc_detector_distance_handle *acc_detector_distance_handle_t;


/**
 * @brief Distance detector configuration container
 */
struct acc_detector_distance_configuration;

typedef struct acc_detector_distance_configuration *acc_detector_distance_configuration_t;


/**
 * @brief Result type for distance detector
 *
 * @param[out] amplitude Absolute amplitude of peak
 * @param[out] distance Distance to peak in meters
 */
typedef struct
{
	uint16_t amplitude;
	float    distance_m;
} acc_detector_distance_result_t;


/**
 * @brief A callback for retrieving the service data buffer that the detector is based on
 *
 * @param[in] data A pointer to the buffer with envelope data
 * @param[in] data_length Length of the data buffer
 */
typedef void (*acc_detector_distance_service_data_callback_t)(const uint16_t *data, uint16_t data_length);


/**
 * @brief Create a configuration for a distance detector
 *
 * @return Distance detector configuration, NULL if creation was not possible
 */
acc_detector_distance_configuration_t acc_detector_distance_configuration_create(void);


/**
 * @brief Destroy a configuration for a distance detector
 *
 * @param[in] distance_configuration The configuration to destroy, set to NULL
 */
void acc_detector_distance_configuration_destroy(acc_detector_distance_configuration_t *distance_configuration);


/**
 * @brief Create a distance detector with the provided configuration
 *
 * Only one distance detector may exist for a specific sensor at any given time and
 * invalid configurations will not allow for a distance detector creation.
 *
 * @param[in] distance_configuration The distance detector configuration to create a distance detector with
 * @return Distance detector handle, NULL if distance detector was not possible to create
 */
acc_detector_distance_handle_t acc_detector_distance_create(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Destroy a distance detector identified with provided handle
 *
 * Destroy the context of a distance detector allowing another distance detector to be created using the
 * same resources. The distance detector handle reference is set to NULL after destruction.
 * If NULL is sent in, nothing happens
 *
 * @param[in] distance_handle A reference to the distance detector handle to destroy
 */
void acc_detector_distance_destroy(acc_detector_distance_handle_t *distance_handle);


/**
 * @brief Activate the distance detector associated with the provided handle
 *
 * @param[in] distance_handle The distance detector handle for the distance detector to activate
 * @return True if successful, otherwise false
 */
bool acc_detector_distance_activate(acc_detector_distance_handle_t distance_handle);


/**
 * @brief Deactivate the distance detector associated with the provided handle
 *
 * @param[in] distance_handle The distance detector handle for the distance detector to deactivate
 * @return True if successful, otherwise false
 */
bool acc_detector_distance_deactivate(acc_detector_distance_handle_t distance_handle);


/**
 * @brief Reconfigure a distance detector with the provided configuration
 *
 * Only one distance detector may exist for a specific sensor at any given time.
 * A distance detector may not be active when reconfiguring the detector.
 * Invalid configurations will not allow for distance detector reconfiguration and
 * the distance detector will be destroyed.
 *
 * NOTE! The old distance handle will be invalid after reconfigure.
 *
 * @param[in, out] distance_handle A reference to the distance detector handle to reconfigure
 * @param[in] distance_configuration The distance detector configuration to reconfigure a distance detector with
 * @return True if possible to reconfigure
 */
bool acc_detector_distance_reconfigure(acc_detector_distance_handle_t        *distance_handle,
                                       acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Get detector metadata
 *
 * The detector provides metadata after being created with information that could be relevant for an application.
 *
 * May only be called after a detector has been created.
 *
 * @param[in] distance_handle The distance detector handle to get metadata for
 * @param[out] metadata Metadata are provided in this parameter
 * @return True if successful
 */
bool acc_detector_distance_metadata_get(acc_detector_distance_handle_t   distance_handle,
                                        acc_detector_distance_metadata_t *metadata);


/**
 * @brief Record background
 *
 * Must be called after @ref acc_detector_distance_create and before @ref acc_detector_distance_activate
 * when ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED is set.
 *
 * Can be called again to make a new recording. Detector has to be deactivated to record a new background.
 *
 * The background length can be retrieved from @ref acc_detector_distance_metadata_get.
 *
 * The result should typically be set to the detector by calling @ref acc_detector_distance_set_background
 *
 * @param[in] distance_handle The distance detector handle for the distance detector to record background for
 * @param[out] background The recorded background will be stored here
 * @param[in] background_length The length of the background
 * @param[out] background_info Recorded background metadata, passing NULL is OK
 * @return True if successful
 */
bool acc_detector_distance_record_background(acc_detector_distance_handle_t                   distance_handle,
                                             uint16_t                                         *background,
                                             uint16_t                                         background_length,
                                             acc_detector_distance_recorded_background_info_t *background_info);


/**
 * @brief Set a background
 *
 * Set a previously recorded background. The background can typically be generated using
 * @ref acc_detector_distance_record_background. The background must have a length corresponding
 * to the configuration that was used when creating the detector. For example, if the background
 * was created using @ref acc_detector_distance_record_background, the configuration of the detector
 * should be the same both when generating the background as when later setting it.
 *
 * Must always be called for detector to work in ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED mode.
 *
 * Must be called after @ref acc_detector_distance_create and before @ref acc_detector_distance_activate
 *
 * The background length can be retrieved from @ref acc_detector_distance_metadata_get.
 *
 * @param[in] distance_handle The distance detector handle for the distance detector to set background for
 * @param[in] background The background to be set
 * @param[in] background_length The length of the background to be set
 * @return True if successful
 */
bool acc_detector_distance_set_background(acc_detector_distance_handle_t distance_handle, const uint16_t *background, uint16_t background_length);


/**
 * @brief Retrieve the next result from the distance detector
 *
 * May only be called after a distance detector has been activated, blocks
 * the application until result is ready.
 *
 * When using CFAR threshold, no detections will be made outside of range
 * [range_start + (guard / 2) + window, range_end - (guard / 2) - window], if only_lower_distances is not set.
 * If only_lower_distances is set, no detections will be made outside of range
 * [range_start + (guard / 2) + window, range_end].
 *
 * @param[in] distance_handle The distance detector handle for the distance detector to get next result for
 * @param[out] result Distance detector results, can be NULL if no result is wanted
 * @param[in] result_length Length of data array passed to function. Number of peaks returned <= result_length.
 * @param[out] result_info Detector result info
 * @return True if successful, otherwise false
 */
bool acc_detector_distance_get_next(acc_detector_distance_handle_t      distance_handle,
                                    acc_detector_distance_result_t      *result,
                                    uint16_t                            result_length,
                                    acc_detector_distance_result_info_t *result_info);


/**
 * @brief Get the sensor ID for the sensor to be configured
 *
 * @param[in] distance_configuration The detector distance configuration to get the sensor ID from
 * @return Sensor ID
 */
acc_sensor_id_t acc_detector_distance_configuration_sensor_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the sensor ID for the sensor to be configured
 *
 * @param[in] distance_configuration The detector distance configuration to set the sensor ID in
 * @param[in] sensor_id The sensor ID to set
 */
void acc_detector_distance_configuration_sensor_set(acc_detector_distance_configuration_t distance_configuration, acc_sensor_id_t sensor_id);


/**
 * @brief Get the requested start distance for measurement in meters
 *
 * @param[in] distance_configuration The detector distance configuration to get the requested start from
 * @return Requested start in meters
 */
float acc_detector_distance_configuration_requested_start_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the requested start distance for measurement in meters
 *
 * @param[in] distance_configuration The detector distance configuration to set the requested start in
 * @param[in] start_m The requested start in meters
 */
void acc_detector_distance_configuration_requested_start_set(acc_detector_distance_configuration_t distance_configuration, float start_m);


/**
 * @brief Get the requested length of the measurement in meters
 *
 * @param[in] distance_configuration The detector distance configuration to get the requested length from
 * @return Requested length in meters
 */
float acc_detector_distance_configuration_requested_length_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the requested length of the measurement in meters
 *
 * @param[in] distance_configuration The detector distance configuration to set the requested length in
 * @param[in] length_m The requested length in meters
 */
void acc_detector_distance_configuration_requested_length_set(acc_detector_distance_configuration_t distance_configuration, float length_m);


/**
 * @brief Get power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] distance_configuration The detector distance configuration to get power save mode from
 * @return Power save mode
 */
acc_power_save_mode_t acc_detector_distance_configuration_power_save_mode_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set power save mode
 *
 * The power save modes of the sensor correspond to how much of the sensor hardware is shutdown
 * between data aquisition. The supported power save modes are defined in
 * @ref acc_power_save_mode_enum_t.
 *
 * @param[in] distance_configuration The detector distance configuration to set power save mode in
 * @param[in] power_save_mode The power save mode to use
 */
void acc_detector_distance_configuration_power_save_mode_set(acc_detector_distance_configuration_t distance_configuration,
                                                             acc_power_save_mode_t                 power_save_mode);


/**
 * @brief Get the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on. Only 1, 2 and 4
 * are valid numbers.
 *
 * @param[in] distance_configuration The detector distance configuration to get downsampling factor for
 * @return Downsampling factor
 */
uint16_t acc_detector_distance_configuration_downsampling_factor_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the sensor downsampling factor
 *
 * Gets the downsampling factor - the number of steps taken between each data point. A downsampling factor of 1 samples
 * every possible point in the range. A downsampling factor of 2 samples every other point, and so on. Only 1, 2 and 4
 * are valid numbers.
 *
 * @param[in] distance_configuration The detector distance configuration to set downsampling factor for
 * @param[in] downsampling_factor Downsampling factor
 */
void acc_detector_distance_configuration_downsampling_factor_set(acc_detector_distance_configuration_t distance_configuration,
                                                                 uint16_t                              downsampling_factor);


/**
 * @brief Get the service profile
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] distance_configuration The detector distance configuration to get service profile for
 * @return The service profile
 */
acc_service_profile_t acc_detector_distance_configuration_service_profile_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the service profile
 *
 * See @ref acc_service_profile_t for details
 *
 * @param[in] distance_configuration The detector distance configuration to set service profile for
 * @param[in] service_profile
 */
void acc_detector_distance_configuration_service_profile_set(acc_detector_distance_configuration_t distance_configuration,
                                                             acc_service_profile_t                 service_profile);


/**
 * @brief Get Maximize signal attenuation mode
 *
 * Will be true if Maximize signal attenuation mode is enabled, false otherwise
 *
 * @param[in] distance_configuration The detector distance configuration to get Maximize signal attenuation mode for
 * @return Maximize signal attenuation mode
 */
bool acc_detector_distance_configuration_maximize_signal_attenuation_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set Maximize signal attenuation mode
 *
 * Enable or disable Maximize signal attenuation mode to measure the direct leakage
 *
 * @param[in] distance_configuration The detector distance configuration to set Maximize signal attenuation mode in
 * @param[in] maximize_signal_attenuation Maximize signal attenuation mode, true or false
 */
void acc_detector_distance_configuration_maximize_signal_attenuation_set(acc_detector_distance_configuration_t distance_configuration,
                                                                         bool                                  maximize_signal_attenuation);


/**
 * @brief Get receiver gain setting
 *
 * Will be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] distance_configuration The detector distance configuration to get gain setting for
 * @return Receiver gain setting
 */
float acc_detector_distance_configuration_receiver_gain_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set receiver gain setting
 *
 * Must be a value between 0.0 and 1.0, where 0.0 is the lowest gain and 1.0 is the highest gain.
 *
 * @param[in] distance_configuration The detector distance configuration to set gain setting for
 * @param[in] gain Receiver gain setting, must be between 0.0 and 1.0
 */
void acc_detector_distance_configuration_receiver_gain_set(acc_detector_distance_configuration_t distance_configuration, float gain);


/**
 * @brief Get the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] distance_configuration The distance configuration to get hardware accelerated average samples from
 * @return Hardware accelerated average samples
 */
uint8_t acc_detector_distance_configuration_hw_accelerated_average_samples_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set the hardware accelerated average samples (HWAAS)
 *
 * Each data point can be sampled between 1 and 63 times, inclusive, and the sensor hardware then
 * produces an average value of those samples. The time needed to measure a sweep is roughly proportional
 * to the number of averaged samples. Hence, if there is a need to obtain a higher update rate, HWAAS
 * could be decreased but this leads to lower SNR.
 *
 * @param[in] distance_configuration The distance configuration to set hardware accelerated average samples in
 * @param[in] samples Hardware accelerated average samples
 */
void acc_detector_distance_configuration_hw_accelerated_average_samples_set(
	acc_detector_distance_configuration_t distance_configuration, uint8_t samples);


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
 * @param[in] distance_configuration The configuration to get asynchronous_measurement mode from
 * @return Asynchronous measurement mode
 */
bool acc_detector_distance_configuration_asynchronous_measurement_get(acc_detector_distance_configuration_t distance_configuration);


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
 * @param[in] distance_configuration The configuration to set asynchronous_measurement mode in
 * @param[in] asynchronous_measurement asynchronous measurement mode, true or false
 */
void acc_detector_distance_configuration_asynchronous_measurement_set(acc_detector_distance_configuration_t distance_configuration,
                                                                      bool                                  asynchronous_measurement);


/**
 * @brief Get number of sweeps to calculate average for
 *
 * @param[in] distance_configuration The detector distance configuration to get sweep average for
 * @return Number of sweeps to calculate average for
 */
uint16_t acc_detector_distance_configuration_sweep_averaging_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set number of sweeps to calculate average for
 *
 * @param[in] distance_configuration The detector distance configuration to set sweep average for
 * @param[in] sweep_averaging Number of sweeps to calculate average for
 */
void acc_detector_distance_configuration_sweep_averaging_set(acc_detector_distance_configuration_t distance_configuration,
                                                             uint16_t                              sweep_averaging);


/**
 * @brief Get threshold type
 *
 * @param[in] distance_configuration The detector distance configuration to set threshold type for
 * @return Used threshold type
 */
acc_detector_distance_threshold_type_t acc_detector_distance_configuration_threshold_type_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set threshold type
 *
 * @param[in] distance_configuration The detector distance configuration to set threshold type for
 * @param[in] threshold Threshold type to be used
 */
void acc_detector_distance_configuration_threshold_type_set(acc_detector_distance_configuration_t  distance_configuration,
                                                            acc_detector_distance_threshold_type_t threshold);


/**
 * @brief Get fixed threshold
 *
 * @param[in] distance_configuration The detector distance configuration to get fixed threshold from
 * @return Fixed threshold
 */
uint16_t acc_detector_distance_configuration_fixed_threshold_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set fixed threshold
 *
 * @param[in] distance_configuration The detector distance configuration to set fixed threshold in
 * @param[in] threshold Fixed threshold to be used by the detector
 */
void acc_detector_distance_configuration_fixed_threshold_set(acc_detector_distance_configuration_t distance_configuration,
                                                             uint16_t                              threshold);


/**
 * @brief Get number of sweeps to record
 *
 * @param[in] distance_configuration The detector distance configuration to get number of sweeps to record for
 * @return Number of sweeps to record
 */
uint16_t acc_detector_distance_configuration_record_background_sweeps_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set number of sweeps to record
 *
 * @param[in] distance_configuration The detector distance configuration to set number of sweeps to record in
 * @param[in] record_sweeps Number of sweeps to record
 */
void acc_detector_distance_configuration_record_background_sweeps_set(
	acc_detector_distance_configuration_t distance_configuration, uint16_t record_sweeps);


/**
 * @brief Get threshold sensitivity
 *
 * Applicable when using recorded threshold or CFAR threshold
 *
 * @param[in] distance_configuration The detector distance configuration to get threshold sensitivity for
 * @return Threshold sensitivity
 */
float acc_detector_distance_configuration_threshold_sensitivity_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set threshold sensitivity
 *
 * Applicable when using recorded threshold or CFAR threshold
 *
 * @param[in] distance_configuration The detector distance configuration to set threshold sensitivity in
 * @param[in] sensitivity
 */
void acc_detector_distance_configuration_threshold_sensitivity_set(
	acc_detector_distance_configuration_t distance_configuration,
	float                                 sensitivity);


/**
 * @brief Get guard for CFAR threshold
 *
 * Range around the distance of interest that is omitted when calculating
 * CFAR threshold. Can be low, ~0.04 m, for Profile 1, and should be
 * increased for higher Profiles.
 *
 * @param[in] distance_configuration The detector distance configuration to get threshold guard for
 * @return Threshold guard in meters
 */
float acc_detector_distance_configuration_cfar_threshold_guard_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set guard for CFAR threshold
 *
 * Range around the distance of interest that is omitted when calculating
 * CFAR threshold. Can be low, ~0.04 cm, for Profile 1, and should be
 * increased for higher Profiles.
 *
 * @param[in] distance_configuration The detector distance configuration to set threshold guard in
 * @param[in] guard_m Threshold guard in meters
 */
void acc_detector_distance_configuration_cfar_threshold_guard_set(
	acc_detector_distance_configuration_t distance_configuration,
	float                                 guard_m);


/**
 * @brief Get window for CFAR threshold
 *
 * Range next to the CFAR guard from which the threshold level will be calculated.
 *
 * @param[in] distance_configuration The detector distance configuration to get threshold window for
 * @return Threshold window in meters
 */
float acc_detector_distance_configuration_cfar_threshold_window_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set window for CFAR threshold
 *
 * Range next to the CFAR guard from which the threshold level will be calculated.
 *
 * @param[in] distance_configuration The detector distance configuration to set threshold window in
 * @param[in] window_m Threshold window in meters
 */
void acc_detector_distance_configuration_cfar_threshold_window_set(
	acc_detector_distance_configuration_t distance_configuration,
	float                                 window_m);


/**
 * @brief Get if only lower distance is set
 *
 * Instead of determining the CFAR threshold from sweep amplitudes from
 * distances both closer and father away, use only closer. Helpful e.g. for
 * fluid level in small tanks, where many multipath signals can appear
 * just after the main peak.
 *
 * @param[in] distance_configuration The detector distance configuration to get setting for
 * @return True, if only lower distance is set
 */
bool acc_detector_distance_configuration_cfar_threshold_only_lower_distance_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set only lower distance
 *
 * Instead of determining the CFAR threshold from sweep amplitudes from
 * distances both closer and father away, use only closer. Helpful e.g. for
 * fluid level in small tanks, where many multipath signals can appear
 * just after the main peak.
 *
 * @param[in] distance_configuration The detector distance configuration to set setting for
 * @param[in] only_lower_distance True if only lower distances should be used
 */
void acc_detector_distance_configuration_cfar_threshold_only_lower_distance_set(
	acc_detector_distance_configuration_t distance_configuration,
	bool                                  only_lower_distance);


/**
 * @brief Get peak sorting algorithm
 *
 * Peak sorting algoritm specifies in what order peaks should be reported back to the application.
 *
 * @param[in] distance_configuration The detector distance configuration to get peak sorting algorithm from
 * @return Peak sorting algorithm
 */
acc_detector_distance_peak_sorting_t acc_detector_distance_configuration_peak_sorting_get(
	acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set peak sorting algorithm to be used
 *
 * Peak sorting algoritm specifies in what order peaks should be reported back to the application.
 *
 * @param[in] distance_configuration The detector distance configuration to set peak sorting algorithm in
 * @param[in] peak_sorting Peak sorting algorithm to be used
 */
void acc_detector_distance_configuration_peak_sorting_set(acc_detector_distance_configuration_t distance_configuration,
                                                          acc_detector_distance_peak_sorting_t  peak_sorting);


/**
 * @brief Get peak merge limit in meters
 *
 * Defining minimum distance between peaks to be considered individual peaks
 *
 * @param[in] distance_configuration The detector distance configuration to get peak merge limit from
 * @return Peak merge limit in meters
 */
float acc_detector_distance_configuration_peak_merge_limit_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @brief Set peak merge limit in meters
 *
 * Defining minimum distance between peaks to be considered individual peaks
 *
 * @param[in] distance_configuration The detector distance configuration to set peak merge limit in
 * @param[in] peak_merge_limit_m Peak merge limit in meters
 */
void acc_detector_distance_configuration_peak_merge_limit_set(acc_detector_distance_configuration_t distance_configuration,
                                                              float                                 peak_merge_limit_m);


/**
 * @brief Set a callback function to get the service data
 *
 * A callback can be used to get the envelope service buffer that the detector is based on.
 * The data is the unprocessed envelope data that is fed into the distance algorithm.
 * Set service_data_callback to NULL to disable callback.
 *
 * @param[in] distance_configuration The configuration to set the service data callback for
 * @param[in] service_data_callback The callback to get service data
 */
void acc_detector_distance_configuration_service_data_callback_set(
	acc_detector_distance_configuration_t         distance_configuration,
	acc_detector_distance_service_data_callback_t service_data_callback);


/**
 * @brief Set the maximum unambiguous range
 *
 * Experimental.
 *
 * See acc_service_mur_set() for more detailed information.
 *
 * @param[in] distance_configuration The configuration
 * @param[in] max_unambiguous_range The desired maximum unambiguous range
 */
void acc_detector_distance_configuration_mur_set(acc_detector_distance_configuration_t distance_configuration,
                                                 acc_service_mur_t                     max_unambiguous_range);


/**
 * @brief Get the maximum unambiguous range
 *
 * Experimental.
 *
 * See acc_service_mur_get() for more detailed information.
 *
 * @param[in] distance_configuration The configuration
 * @return Maximum unambiguous range
 */
acc_service_mur_t acc_detector_distance_configuration_mur_get(acc_detector_distance_configuration_t distance_configuration);


/**
 * @}
 */

#endif
