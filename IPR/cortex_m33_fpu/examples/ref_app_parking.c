// Copyright (c) Acconeer AB, 2020-2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_integration.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_envelope.h"
#include "acc_version.h"


// Default values for this reference application
// ---------------------------------------------

// Service configuration settings
#define SENSOR_ID              1
#define RANGE_START_M          0.12f
#define RANGE_LENGTH_M         0.50f
#define SERVICE_DOWNSAMPLING   2
#define SERVICE_HWAAS          20
#define RUNNING_AVERAGE_FACTOR 0.0f
#define POWER_SAVE_MODE        ACC_POWER_SAVE_MODE_OFF

// The expected background level for the envelope service
// This parameter is not expected to change as long as noise level normalization is active
#define ENVELOPE_BACKGROUND_LEVEL 100

// Parameters for direct leakage subtraction
// To be set from inspection of the worst case direct leakage
// LEAKAGE_SAMPLE_POSITION_M and LEAKAGE_END_POSITION_M must be within
// the measurement range [RANGE_START_M, RANGE_START_M + RANGE_LENGTH_M]
// and LEAKAGE_END_POSITION_M must be beyond LEAKAGE_SAMPLE_POSITION_M
#define LEAKAGE_SAMPLE_POSITION_M 0.15f
#define LEAKAGE_END_POSITION_M    0.30f
#define MAX_LEAK_AMPLITUDE        2000

// The number of observations in the parking detection queue
#define DETECTION_OBSERVATION_COUNT 3

// The minimal weight for each observation in the parking detection queue for
// detection of a parked car
#define DETECTION_WEIGHT_THRESHOLD 5.0f

// Parameters for exclusion of transient reflections from people and items that are near the
// sensor for short durations
#define DETECTION_WEIGHT_RATIO_LIMIT 3.0f
#define DETECTION_DISPLACEMENT_LIMIT 0.1f

// The time duration between two consecutive sweeps
#define DETECTOR_SWEEP_PERIOD_S 10.0f

// Minimal envelope service runtime before sensor recalibration due to a data quality warning.
// A sensor calibration is costly in terms of power consumption relative to the sweeps
#define SERVICE_RUNTIME_MIN_S 900.0f

// Maximal envelope service active time before the service is destroyed and recreated
// to renew the noise level normalization. The sensor output amplitude may change notably due
// to temperature changes before a data quality warning is triggered. This type of refresh
// is inexpensive with respect to power consumption.
#define SERVICE_UPTIME_MAX_S 900.0f


typedef struct
{
	float weight;
	float distance;
} sweep_observable_t;


/**
 * Recreate the service to renew the noise noise level normalization
 *
 * @param configuration The configuration to recreate
 * @param handle The handle of the service to recreate
 * @return True, if reactivation was successful
 */
static bool service_recreate(const acc_service_configuration_t configuration, acc_service_handle_t *handle);


/**
 * Calibrate the sensor
 *
 * @return True, if sensor calibration was successful
 */
static bool sensor_calibration(void);


/**
 * Configure the service to the specified configuration
 *
 * @param configuration The service configuration to configure with application settings
 */
static void configure_service(acc_service_configuration_t configuration);


/**
 * Exectute the parking detection
 *
 * @param metadata Service metadata
 * @param leak_sample_index Index where to sample leakage
 * @param leak_end_index Index where leakage is assumed to end
 * @param observations Observation history
 * @param observation_count The number of observations in the history
 * @param data Service data
 * @return True, if a car is detected
 */
static bool parking_detection(const acc_service_envelope_metadata_t *metadata, uint16_t leak_sample_index,
                              uint16_t leak_end_index, sweep_observable_t *observations, uint16_t *observation_count,
                              const uint16_t *data);


int acc_ref_app_parking(int argc, char *argv[]);


int acc_ref_app_parking(int argc, char *argv[])
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

	acc_service_configuration_t configuration = acc_service_envelope_configuration_create();

	if (configuration == NULL)
	{
		printf("Failed to create service configuration\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	if (!sensor_calibration())
	{
		printf("Failed to calibrate sensor\n");
		acc_service_envelope_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	configure_service(configuration);

	acc_service_handle_t handle = acc_service_create(configuration);

	if (handle == NULL)
	{
		printf("Failed to create service handle\n");
		acc_service_envelope_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_envelope_metadata_t metadata;
	acc_service_envelope_get_metadata(handle, &metadata);

	uint16_t leak_sample_index = (uint16_t)(((LEAKAGE_SAMPLE_POSITION_M - metadata.start_m) / metadata.step_length_m) + 0.5f);
	uint16_t leak_end_index    = (uint16_t)(((LEAKAGE_END_POSITION_M - metadata.start_m) / metadata.step_length_m) + 0.5f);
	bool     valid_leak_setup  = (leak_sample_index < leak_end_index && leak_sample_index < metadata.data_length);

	uint16_t                           *data = NULL;
	acc_service_envelope_result_info_t result_info;
	sweep_observable_t                 observations[DETECTION_OBSERVATION_COUNT];
	uint16_t                           observation_count   = 0;
	uint32_t                           last_update_ms      = 0;
	uint32_t                           last_activate_ms    = hal->os.gettime();
	uint32_t                           last_calibration_ms = hal->os.gettime();
	uint16_t                           sweep_index         = 0;

	bool status = true;

	if (!valid_leak_setup)
	{
		printf("Parameters are not valid\n");
		status = false;
	}
	else
	{
		status = acc_service_activate(handle);
	}

	while (status)
	{
		if (hal->os.gettime() - last_activate_ms > SERVICE_UPTIME_MAX_S * 1000)
		{
			status           = service_recreate(configuration, &handle);
			last_activate_ms = hal->os.gettime();
		}

		if (status)
		{
			while (last_update_ms != 0 && hal->os.gettime() - last_update_ms < DETECTOR_SWEEP_PERIOD_S * 1000)
			{
				acc_integration_sleep_ms((DETECTOR_SWEEP_PERIOD_S * 1000) - (hal->os.gettime() - last_update_ms));
			}

			status         = acc_service_envelope_get_next_by_reference(handle, &data, &result_info);
			last_update_ms = hal->os.gettime();
		}

		if (status && result_info.data_quality_warning &&
		    hal->os.gettime() - last_calibration_ms > SERVICE_RUNTIME_MIN_S * 1000)
		{
			status = acc_service_deactivate(handle);

			if (status)
			{
				acc_service_destroy(&handle);

				status = sensor_calibration();
			}

			if (status)
			{
				handle = acc_service_create(configuration);

				status = handle != NULL;
			}

			if (status)
			{
				status = acc_service_activate(handle);
			}

			last_calibration_ms = hal->os.gettime();

			if (status)
			{
				status         = acc_service_envelope_get_next_by_reference(handle, &data, &result_info);
				last_update_ms = hal->os.gettime();
			}
		}

		if (status)
		{
			bool detection = parking_detection(&metadata, leak_sample_index, leak_end_index, observations, &observation_count, data);

			if (sweep_index < DETECTION_OBSERVATION_COUNT - 1)
			{
				printf("%" PRIu16 ": No result\n", sweep_index);
			}
			else
			{
				printf("%" PRIu16 ": %s\n", sweep_index, detection ? "Car" : "No car");
			}

			sweep_index++;
		}
	}

	acc_service_envelope_configuration_destroy(&configuration);
	acc_service_deactivate(handle);
	acc_service_destroy(&handle);
	acc_rss_deactivate();

	return status ? EXIT_SUCCESS : EXIT_FAILURE;
}


bool service_recreate(const acc_service_configuration_t configuration, acc_service_handle_t *handle)
{
	if (!acc_service_deactivate(*handle))
	{
		return false;
	}

	acc_service_destroy(handle);

	*handle = acc_service_create(configuration);

	if (*handle == NULL)
	{
		return false;
	}

	if (!acc_service_activate(*handle))
	{
		return false;
	}

	return true;
}


bool sensor_calibration(void)
{
	acc_calibration_context_t calibration_context;

	if (!acc_rss_calibration_context_get(SENSOR_ID, &calibration_context))
	{
		return false;
	}

	if (!acc_rss_calibration_context_forced_set(SENSOR_ID, &calibration_context))
	{
		return false;
	}

	return true;
}


void configure_service(acc_service_configuration_t configuration)
{
	acc_service_sensor_set(configuration, SENSOR_ID);
	acc_service_requested_start_set(configuration, RANGE_START_M);
	acc_service_requested_length_set(configuration, RANGE_LENGTH_M);
	acc_service_envelope_downsampling_factor_set(configuration, SERVICE_DOWNSAMPLING);
	acc_service_hw_accelerated_average_samples_set(configuration, SERVICE_HWAAS);
	acc_service_envelope_running_average_factor_set(configuration, RUNNING_AVERAGE_FACTOR);
	acc_service_power_save_mode_set(configuration, POWER_SAVE_MODE);
}


bool parking_detection(const acc_service_envelope_metadata_t *metadata, uint16_t leak_sample_index,
                       uint16_t leak_end_index, sweep_observable_t *observations, uint16_t *observation_count,
                       const uint16_t *data)
{
	float    weight_sum     = 0.0f;
	float    weight_sum_r   = 0.0f;
	float    leak_start     = 0.0f;
	uint16_t leak_amplitude = MAX_LEAK_AMPLITUDE < data[leak_sample_index] ? MAX_LEAK_AMPLITUDE : data[leak_sample_index];
	uint16_t a_leak         = leak_amplitude < ENVELOPE_BACKGROUND_LEVEL ? 0 : leak_amplitude - ENVELOPE_BACKGROUND_LEVEL;
	float    leak_step      = ((float)(a_leak) / (leak_end_index - leak_sample_index));

	leak_start = leak_end_index * leak_step + ENVELOPE_BACKGROUND_LEVEL;

	for (uint16_t i = 0; i < metadata->data_length; i++)
	{
		float r  = metadata->start_m + i * metadata->step_length_m;
		float bg = 0.0f;
		if (i <= leak_end_index)
		{
			bg = leak_start - i * leak_step;
		}
		else
		{
			bg = ENVELOPE_BACKGROUND_LEVEL;
		}

		float sweep_above_bg = data[i] - bg;
		sweep_above_bg = sweep_above_bg > 0.0f ? sweep_above_bg : 0.0f;

		float weight = sweep_above_bg / ENVELOPE_BACKGROUND_LEVEL;
		weight = weight < 1.0f ? weight : 1.0f;
		weight = weight * sweep_above_bg * r;

		weight_sum   += weight;
		weight_sum_r += weight * r;
	}

	if (*observation_count == DETECTION_OBSERVATION_COUNT)
	{
		for (uint16_t i = 1; i < DETECTION_OBSERVATION_COUNT; i++)
		{
			observations[i - 1].weight   = observations[i].weight;
			observations[i - 1].distance = observations[i].distance;
		}
	}
	else
	{
		(*observation_count)++;
	}

	observations[*observation_count - 1].weight   = weight_sum / metadata->data_length;
	observations[*observation_count - 1].distance = weight_sum_r / weight_sum;

	float weight_min   = __FLT_MAX__;
	float weight_max   = 0.0f;
	float distance_min = __FLT_MAX__;
	float distance_max = 0.0f;

	for (uint16_t i = 0; i < *observation_count; i++)
	{
		if (weight_min > observations[i].weight)
		{
			weight_min = observations[i].weight;
		}

		if (weight_max < observations[i].weight)
		{
			weight_max = observations[i].weight;
		}

		if (distance_min > observations[i].distance)
		{
			distance_min = observations[i].distance;
		}

		if (distance_max < observations[i].distance)
		{
			distance_max = observations[i].distance;
		}
	}

	bool detection = *observation_count == DETECTION_OBSERVATION_COUNT &&
	                 weight_min >= DETECTION_WEIGHT_THRESHOLD &&
	                 weight_max / weight_min <= DETECTION_WEIGHT_RATIO_LIMIT &&
	                 distance_max - distance_min <= DETECTION_DISPLACEMENT_LIMIT;

	return detection;
}
