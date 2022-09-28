// Copyright (c) Acconeer AB, 2020-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions_common.h"
#include "acc_detector_distance.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_version.h"


// Default values for this reference application
// Note that if a range longer than 7 m is used, an update of 'mur' is required.
// See API documentation for more information of respective parameter
#define DEFAULT_SENSOR 1

#define DEFAULT_CLOSE_RANGE_MUR                      ACC_SERVICE_MUR_6
#define DEFAULT_CLOSE_RANGE_START                    -0.11f
#define DEFAULT_CLOSE_RANGE_LENGTH                   0.23f
#define DEFAULT_CLOSE_RANGE_GAIN                     0.3182f
#define DEFAULT_CLOSE_MAXIMIZE_SIGNAL_ATTENUATION    true
#define DEFAULT_CLOSE_RANGE_SERVICE_PROFILE          ACC_SERVICE_PROFILE_1
#define DEFAULT_CLOSE_RANGE_DOWNSAMPLIG_FACTOR       1
#define DEFAULT_CLOSE_RANGE_SWEEP_AVERAGING          30
#define DEFAULT_CLOSE_RANGE_THRESHOLD_TYPE           ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED
#define DEFAULT_CLOSE_RANGE_RECORD_BACKGROUND_SWEEPS 30
#define DEFAULT_CLOSE_RANGE_THRESHOLD_SENSITIVITY    0.2f

#define DEFAULT_MID_RANGE_MUR                      ACC_SERVICE_MUR_6
#define DEFAULT_MID_RANGE_START                    0.1f
#define DEFAULT_MID_RANGE_LENGTH                   0.37f
#define DEFAULT_MID_RANGE_GAIN                     0.5f
#define DEFAULT_MID_MAXIMIZE_SIGNAL_ATTENUATION    false
#define DEFAULT_MID_RANGE_SERVICE_PROFILE          ACC_SERVICE_PROFILE_1
#define DEFAULT_MID_RANGE_DOWNSAMPLING_FACTOR      1
#define DEFAULT_MID_RANGE_SWEEP_AVERAGING          30
#define DEFAULT_MID_RANGE_THRESHOLD_TYPE           ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED
#define DEFAULT_MID_RANGE_RECORD_BACKGROUND_SWEEPS 30
#define DEFAULT_MID_RANGE_THRESHOLD_SENSITIVITY    0.2f

#define DEFAULT_FAR_RANGE_MUR                   ACC_SERVICE_MUR_6
#define DEFAULT_FAR_RANGE_START                 0.19f
#define DEFAULT_FAR_RANGE_LENGTH                1.3f
#define DEFAULT_FAR_RANGE_GAIN                  0.8182f
#define DEFAULT_FAR_MAXIMIZE_SIGNAL_ATTENUATION false
#define DEFAULT_FAR_RANGE_SERVICE_PROFILE       ACC_SERVICE_PROFILE_2
#define DEFAULT_FAR_RANGE_DOWNSAMPLING_FACTOR   4
#define DEFAULT_FAR_RANGE_SWEEP_AVERAGING       10
#define DEFAULT_FAR_RANGE_THRESHOLD_TYPE        ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_CFAR
#define DEFAULT_FAR_RANGE_THRESHOLD_SENSITIVITY 0.4f
#define DEFAULT_FAR_RANGE_CFAR_THRESHOLD_GUARD  0.12f
#define DEFAULT_FAR_RANGE_CFAR_THRESHOLD_WINDOW 0.03f

// Gain can be configured in 22 steps between 0.0 and 1.0
#define GAIN_STEP             (1.0f / 22.0f)
#define MAX_BACKGROUND_LENGTH 1200


static uint16_t close_background[MAX_BACKGROUND_LENGTH];
static uint16_t close_background_length;
static float    close_range_gain = DEFAULT_CLOSE_RANGE_GAIN;

static uint16_t mid_background[MAX_BACKGROUND_LENGTH];
static uint16_t mid_background_length;
static float    mid_range_gain = DEFAULT_MID_RANGE_GAIN;


/**
 * Calibrate the sensor
 *
 * @return True, if sensor calibration was successful
 */
static bool sensor_calibration(void);


/**
 * Configure distance detector to measure in the close range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @return True, if configuration was successful
 */
static bool configure_close_range(acc_detector_distance_handle_t        *distance_handle,
                                  acc_detector_distance_configuration_t distance_configuration);


/**
 * Configure distance detector to measure in the mid range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @return True, if configuration was successful
 */
static bool configure_mid_range(acc_detector_distance_handle_t        *distance_handle,
                                acc_detector_distance_configuration_t distance_configuration);


/**
 * Configure distance detector to measure in the far range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @return True, if configuration was successful
 */
static bool configure_far_range(acc_detector_distance_handle_t        *distance_handle,
                                acc_detector_distance_configuration_t distance_configuration);


/**
 * Record the background for specific configuration
 *
 * This function will set the requested gain, but the gain will be lowered in case of data saturation.
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @param range_gain The gain actually used
 * @param background Array to store background in
 * @param background_length Length of background array
 * @return True, if recording was successful
 */
static bool record_background(acc_detector_distance_handle_t *distance_handle,
                              acc_detector_distance_configuration_t distance_configuration,
                              float *range_gain, uint16_t *background, uint16_t background_length);


/**
 * Record background threshold for close and mid range sector
 *
 * Make sure no object is within these two sectors, i.e. no objects closer
 * to the sensor than DEFAULT_MID_RANGE_START + DEFAULT_MID_RANGE_LENGTH
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @return True, if recording was successful
 */
static bool record_backgrounds(acc_detector_distance_handle_t        *distance_handle,
                               acc_detector_distance_configuration_t distance_configuration);


/**
 * Perform one measurement
 *
 * This function will set the requested gain, but the gain will be lowered in case of data saturation.
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @param result Distance Detector result
 * @param result_info Distance Detector result info
 */
static bool measurement(acc_detector_distance_handle_t        *distance_handle,
                        acc_detector_distance_configuration_t distance_configuration,
                        acc_detector_distance_result_t        *result,
                        acc_detector_distance_result_info_t   *result_info);


/**
 * Measure in the close range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @param distance_detected True, if a distance was detected
 * @param distance Detected distance
 * @return True, if measurement was successful
 */
static bool measure_close_range(acc_detector_distance_handle_t *distance_handle,
                                acc_detector_distance_configuration_t distance_configuration,
                                bool *distance_detected, float *distance);


/**
 * Measure in the mid range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @param distance_detected True, if a distance was detected
 * @param distance Detected distance
 * @return True, if measurement was successful
 */
static bool measure_mid_range(acc_detector_distance_handle_t *distance_handle,
                              acc_detector_distance_configuration_t distance_configuration,
                              bool *distance_detected, float *distance);


/**
 * Measure in the far range sector
 *
 * @param distance_handle Distance Detector handle
 * @param distance_configuration Distance Detector configuration
 * @param distance_detected True, if a distance was detected
 * @param distance Detected distance
 * @return True, if measurement was successful
 */
static bool measure_far_range(acc_detector_distance_handle_t *distance_handle,
                              acc_detector_distance_configuration_t distance_configuration,
                              bool *distance_detected, float *distance);


int acc_ref_app_tank_level(int argc, char *argv[]);


int acc_ref_app_tank_level(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		printf("Failed to activate RSS\n");
		return EXIT_FAILURE;
	}

	acc_detector_distance_configuration_t distance_configuration = acc_detector_distance_configuration_create();

	if (distance_configuration == NULL)
	{
		printf("Failed to create detector configuration\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_distance_handle_t distance_handle = acc_detector_distance_create(distance_configuration);

	if (distance_handle == NULL)
	{
		printf("Failed to create detector\n");
		acc_detector_distance_configuration_destroy(&distance_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	if (!record_backgrounds(&distance_handle, distance_configuration))
	{
		printf("Failed to calibrate detector\n");
		acc_detector_distance_configuration_destroy(&distance_configuration);
		acc_detector_distance_destroy(&distance_handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	// If there is a long time between measurements it is good to calibrate the sensor
	// before each measurement
	acc_detector_distance_destroy(&distance_handle);

	if (!sensor_calibration())
	{
		printf("Failed to calibrate sensor\n");
		return EXIT_FAILURE;
	}

	distance_handle = acc_detector_distance_create(distance_configuration);

	bool status = true;

	while (status)
	{
		float distance          = 0.0f;
		bool  distance_detected = false;

		printf("Measure close range\n");
		status = measure_close_range(&distance_handle, distance_configuration, &distance_detected, &distance);
		if (status && !distance_detected)
		{
			printf("Measure mid range\n");
			status = measure_mid_range(&distance_handle, distance_configuration, &distance_detected, &distance);
		}

		if (status && !distance_detected)
		{
			printf("Measure far range\n");
			status = measure_far_range(&distance_handle, distance_configuration, &distance_detected, &distance);
		}

		if (!status)
		{
			printf("Measurement failed\n");
			break;
		}

		if (distance_detected)
		{
			printf("Peak at %u mm\n", (unsigned int)(distance * 1000));
		}
		else
		{
			printf("No peak found\n");
		}

		//Add a call to a sleep function here to limit measurement update rate
	}

	acc_detector_distance_configuration_destroy(&distance_configuration);
	acc_detector_distance_destroy(&distance_handle);
	acc_rss_deactivate();

	return status ? EXIT_SUCCESS : EXIT_FAILURE;
}


bool sensor_calibration(void)
{
	acc_calibration_context_t calibration_context;

	if (!acc_rss_calibration_context_get(DEFAULT_SENSOR, &calibration_context))
	{
		return false;
	}

	if (!acc_rss_calibration_context_forced_set(DEFAULT_SENSOR, &calibration_context))
	{
		return false;
	}

	return true;
}


bool configure_close_range(acc_detector_distance_handle_t        *distance_handle,
                           acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_configuration_sensor_set(distance_configuration, DEFAULT_SENSOR);
	acc_detector_distance_configuration_mur_set(distance_configuration, DEFAULT_CLOSE_RANGE_MUR);
	acc_detector_distance_configuration_requested_start_set(distance_configuration, DEFAULT_CLOSE_RANGE_START);
	acc_detector_distance_configuration_requested_length_set(distance_configuration, DEFAULT_CLOSE_RANGE_LENGTH);
	acc_detector_distance_configuration_receiver_gain_set(distance_configuration, close_range_gain);
	acc_detector_distance_configuration_maximize_signal_attenuation_set(distance_configuration,
	                                                                    DEFAULT_CLOSE_MAXIMIZE_SIGNAL_ATTENUATION);
	acc_detector_distance_configuration_service_profile_set(distance_configuration,
	                                                        DEFAULT_CLOSE_RANGE_SERVICE_PROFILE);
	acc_detector_distance_configuration_downsampling_factor_set(distance_configuration,
	                                                            DEFAULT_CLOSE_RANGE_DOWNSAMPLIG_FACTOR);
	acc_detector_distance_configuration_sweep_averaging_set(distance_configuration,
	                                                        DEFAULT_CLOSE_RANGE_SWEEP_AVERAGING);
	acc_detector_distance_configuration_threshold_type_set(distance_configuration,
	                                                       DEFAULT_CLOSE_RANGE_THRESHOLD_TYPE);
	acc_detector_distance_configuration_record_background_sweeps_set(distance_configuration,
	                                                                 DEFAULT_CLOSE_RANGE_RECORD_BACKGROUND_SWEEPS);
	acc_detector_distance_configuration_threshold_sensitivity_set(distance_configuration,
	                                                              DEFAULT_CLOSE_RANGE_THRESHOLD_SENSITIVITY);

	return acc_detector_distance_reconfigure(distance_handle, distance_configuration);
}


bool configure_mid_range(acc_detector_distance_handle_t        *distance_handle,
                         acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_configuration_mur_set(distance_configuration, DEFAULT_MID_RANGE_MUR);
	acc_detector_distance_configuration_requested_start_set(distance_configuration, DEFAULT_MID_RANGE_START);
	acc_detector_distance_configuration_requested_length_set(distance_configuration, DEFAULT_MID_RANGE_LENGTH);
	acc_detector_distance_configuration_receiver_gain_set(distance_configuration, mid_range_gain);
	acc_detector_distance_configuration_maximize_signal_attenuation_set(distance_configuration,
	                                                                    DEFAULT_MID_MAXIMIZE_SIGNAL_ATTENUATION);
	acc_detector_distance_configuration_service_profile_set(distance_configuration,
	                                                        DEFAULT_MID_RANGE_SERVICE_PROFILE);
	acc_detector_distance_configuration_downsampling_factor_set(distance_configuration,
	                                                            DEFAULT_MID_RANGE_DOWNSAMPLING_FACTOR);
	acc_detector_distance_configuration_sweep_averaging_set(distance_configuration,
	                                                        DEFAULT_MID_RANGE_SWEEP_AVERAGING);
	acc_detector_distance_configuration_threshold_type_set(distance_configuration,
	                                                       DEFAULT_MID_RANGE_THRESHOLD_TYPE);
	acc_detector_distance_configuration_record_background_sweeps_set(distance_configuration,
	                                                                 DEFAULT_MID_RANGE_RECORD_BACKGROUND_SWEEPS);
	acc_detector_distance_configuration_threshold_sensitivity_set(distance_configuration,
	                                                              DEFAULT_MID_RANGE_THRESHOLD_SENSITIVITY);

	return acc_detector_distance_reconfigure(distance_handle, distance_configuration);
}


bool configure_far_range(acc_detector_distance_handle_t        *distance_handle,
                         acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_configuration_mur_set(distance_configuration, DEFAULT_FAR_RANGE_MUR);
	acc_detector_distance_configuration_requested_start_set(distance_configuration, DEFAULT_FAR_RANGE_START);
	acc_detector_distance_configuration_requested_length_set(distance_configuration, DEFAULT_FAR_RANGE_LENGTH);
	acc_detector_distance_configuration_receiver_gain_set(distance_configuration, DEFAULT_FAR_RANGE_GAIN);
	acc_detector_distance_configuration_maximize_signal_attenuation_set(distance_configuration,
	                                                                    DEFAULT_FAR_MAXIMIZE_SIGNAL_ATTENUATION);
	acc_detector_distance_configuration_service_profile_set(distance_configuration,
	                                                        DEFAULT_FAR_RANGE_SERVICE_PROFILE);
	acc_detector_distance_configuration_downsampling_factor_set(distance_configuration,
	                                                            DEFAULT_FAR_RANGE_DOWNSAMPLING_FACTOR);
	acc_detector_distance_configuration_sweep_averaging_set(distance_configuration,
	                                                        DEFAULT_FAR_RANGE_SWEEP_AVERAGING);
	acc_detector_distance_configuration_threshold_type_set(distance_configuration,
	                                                       DEFAULT_FAR_RANGE_THRESHOLD_TYPE);
	acc_detector_distance_configuration_threshold_sensitivity_set(distance_configuration,
	                                                              DEFAULT_FAR_RANGE_THRESHOLD_SENSITIVITY);
	acc_detector_distance_configuration_cfar_threshold_guard_set(distance_configuration,
	                                                             DEFAULT_FAR_RANGE_CFAR_THRESHOLD_GUARD);
	acc_detector_distance_configuration_cfar_threshold_window_set(distance_configuration,
	                                                              DEFAULT_FAR_RANGE_CFAR_THRESHOLD_WINDOW);

	return acc_detector_distance_reconfigure(distance_handle, distance_configuration);
}


bool record_background(acc_detector_distance_handle_t *distance_handle,
                       acc_detector_distance_configuration_t distance_configuration,
                       float *range_gain, uint16_t *background, uint16_t background_length)
{
	acc_detector_distance_recorded_background_info_t recorded_background_info;
	bool                                             status                  = true;
	bool                                             previous_gain_saturated = true;

	float gain = acc_detector_distance_configuration_receiver_gain_get(distance_configuration);

	recorded_background_info.data_saturated = true;

	while (status && gain > 0)
	{
		acc_detector_distance_configuration_receiver_gain_set(distance_configuration, gain);

		status = acc_detector_distance_reconfigure(distance_handle, distance_configuration);

		if (status)
		{
			status = acc_detector_distance_record_background(*distance_handle, background, background_length,
			                                                 &recorded_background_info);

			if (!status || (!previous_gain_saturated && !recorded_background_info.data_saturated))
			{
				break;
			}

			previous_gain_saturated = recorded_background_info.data_saturated;

			gain = gain - GAIN_STEP;
		}
	}

	if (status && recorded_background_info.data_saturated)
	{
		printf("Unable to record background without data saturation\n");
		status = false;
	}

	*range_gain = gain;

	return status;
}


bool record_backgrounds(acc_detector_distance_handle_t        *distance_handle,
                        acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_metadata_t metadata;

	if (!configure_close_range(distance_handle, distance_configuration))
	{
		return false;
	}

	if (!acc_detector_distance_metadata_get(*distance_handle, &metadata))
	{
		return false;
	}

	close_background_length = metadata.background_length;
	printf("Record close range\n");

	if (!record_background(distance_handle, distance_configuration, &close_range_gain,
	                       close_background, close_background_length))
	{
		return false;
	}

	if (!configure_mid_range(distance_handle, distance_configuration))
	{
		return false;
	}

	if (!acc_detector_distance_metadata_get(*distance_handle, &metadata))
	{
		return false;
	}

	mid_background_length = metadata.background_length;
	printf("Record mid range\n");

	if (!record_background(distance_handle, distance_configuration, &mid_range_gain,
	                       mid_background, mid_background_length))
	{
		return false;
	}

	return true;
}


bool measurement(acc_detector_distance_handle_t        *distance_handle,
                 acc_detector_distance_configuration_t distance_configuration,
                 acc_detector_distance_result_t        *result,
                 acc_detector_distance_result_info_t   *result_info)
{
	float gain = acc_detector_distance_configuration_receiver_gain_get(distance_configuration);

	result_info->data_saturated = true;

	while (gain > 0)
	{
		if (!acc_detector_distance_activate(*distance_handle))
		{
			printf("Failed to activate detector\n");
			return false;
		}

		if (!acc_detector_distance_get_next(*distance_handle, result, 1, result_info))
		{
			printf("Failed to get next from detector\n");
			return false;
		}

		if (!acc_detector_distance_deactivate(*distance_handle))
		{
			printf("Failed to deactivate detector\n");
			return false;
		}

		if (!result_info->data_saturated || gain < GAIN_STEP)
		{
			break;
		}

		gain = gain - GAIN_STEP;

		acc_detector_distance_configuration_receiver_gain_set(distance_configuration, gain);

		if (!acc_detector_distance_reconfigure(distance_handle, distance_configuration))
		{
			return false;
		}
	}

	if (result_info->data_saturated)
	{
		printf("Unable to measure without data saturation\n");
		return false;
	}

	return true;
}


bool measure_close_range(acc_detector_distance_handle_t *distance_handle,
                         acc_detector_distance_configuration_t distance_configuration,
                         bool *distance_detected, float *distance)
{
	*distance_detected = false;
	acc_detector_distance_result_info_t result_info;
	acc_detector_distance_result_t      result;

	if (!configure_close_range(distance_handle, distance_configuration))
	{
		return false;
	}

	if (!acc_detector_distance_set_background(*distance_handle, close_background, close_background_length))
	{
		return false;
	}

	if (!measurement(distance_handle, distance_configuration, &result, &result_info))
	{
		return false;
	}

	if (result_info.measurement_sample_above_threshold)
	{
		*distance_detected = true;
		*distance          = result_info.closest_detection_m;
	}

	return true;
}


bool measure_mid_range(acc_detector_distance_handle_t *distance_handle,
                       acc_detector_distance_configuration_t distance_configuration,
                       bool *distance_detected, float *distance)
{
	*distance_detected = false;
	acc_detector_distance_result_info_t result_info;
	acc_detector_distance_result_t      result;

	if (!configure_mid_range(distance_handle, distance_configuration))
	{
		return false;
	}

	if (!acc_detector_distance_set_background(*distance_handle, mid_background, mid_background_length))
	{
		return false;
	}

	if (!measurement(distance_handle, distance_configuration, &result, &result_info))
	{
		return false;
	}

	if (result_info.number_of_peaks > 0)
	{
		*distance_detected = true;
		*distance          = result.distance_m;
	}

	return true;
}


bool measure_far_range(acc_detector_distance_handle_t *distance_handle,
                       acc_detector_distance_configuration_t distance_configuration,
                       bool *distance_detected, float *distance)
{
	*distance_detected = false;
	acc_detector_distance_result_info_t result_info;
	acc_detector_distance_result_t      result;

	if (!configure_far_range(distance_handle, distance_configuration))
	{
		return false;
	}

	if (!measurement(distance_handle, distance_configuration, &result, &result_info))
	{
		return false;
	}

	if (result_info.number_of_peaks > 0)
	{
		*distance_detected = true;
		*distance          = result.distance_m;
	}

	return true;
}
