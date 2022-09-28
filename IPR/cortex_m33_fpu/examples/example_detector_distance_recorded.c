// Copyright (c) Acconeer AB, 2020-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_detector_distance.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_version.h"

/** \example example_detector_distance_recorded.c
 *
 * @brief This is an example on how the distance detector can be used when using recorded threshold type.
 *
 * @n
 * It shows both how to record a background as well as setting a previously recorded background.
 *
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a distance detector configuration
 *   - Create a distance detector using the previously created configuration
 *   - Record a background
 *   - Deactivate and destroy the distance detector
 *   - Create a distance detector using the previously created configuration
 *   - Set the previously recorded background
 *   - Activate the distance detector
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the distance detector
 *   - Destroy the distance detector configuration
 *   - Deactivate Radar System Software (RSS)
 */


#define MAX_BACKGROUND_LENGTH 1200


static uint16_t background[MAX_BACKGROUND_LENGTH];
static uint16_t background_length;

static bool record_background(acc_detector_distance_configuration_t distance_configuration);


static bool measure_with_recorded_background(acc_detector_distance_configuration_t distance_configuration);


static void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count);


int acc_example_detector_distance_recorded(int argc, char *argv[]);


int acc_example_detector_distance_recorded(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		printf("acc_rss_activate() failed\n");
		return EXIT_FAILURE;
	}

	acc_detector_distance_configuration_t distance_configuration = acc_detector_distance_configuration_create();

	if (distance_configuration == NULL)
	{
		printf("acc_detector_distance_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_distance_configuration_threshold_type_set(distance_configuration, ACC_DETECTOR_DISTANCE_THRESHOLD_TYPE_RECORDED);
	acc_detector_distance_configuration_record_background_sweeps_set(distance_configuration, 10);
	acc_detector_distance_configuration_requested_start_set(distance_configuration, 0.2f);
	acc_detector_distance_configuration_requested_length_set(distance_configuration, 0.5f);

	if (!record_background(distance_configuration))
	{
		acc_detector_distance_configuration_destroy(&distance_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	if (!measure_with_recorded_background(distance_configuration))
	{
		acc_detector_distance_configuration_destroy(&distance_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_distance_configuration_destroy(&distance_configuration);

	acc_rss_deactivate();

	printf("Application finished OK\n");
	return EXIT_SUCCESS;
}


bool record_background(acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_handle_t distance_handle = acc_detector_distance_create(distance_configuration);

	if (distance_handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		return false;
	}

	acc_detector_distance_metadata_t metadata;
	acc_detector_distance_metadata_get(distance_handle, &metadata);

	background_length = metadata.background_length;

	if (background_length > MAX_BACKGROUND_LENGTH)
	{
		printf("background_length larger than MAX_BACKGROUND_LENGTH\n");
		acc_detector_distance_destroy(&distance_handle);
		return false;
	}

	if (!acc_detector_distance_record_background(distance_handle, background, background_length, NULL))
	{
		printf("acc_detector_distance_record_background() failed\n");
		acc_detector_distance_destroy(&distance_handle);
		return false;
	}

	acc_detector_distance_destroy(&distance_handle);

	return true;
}


bool measure_with_recorded_background(acc_detector_distance_configuration_t distance_configuration)
{
	acc_detector_distance_handle_t distance_handle = acc_detector_distance_create(distance_configuration);

	if (distance_handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		return false;
	}

	if (!acc_detector_distance_set_background(distance_handle, background, background_length))
	{
		printf("acc_detector_distance_set_background() failed\n");
		acc_detector_distance_destroy(&distance_handle);
		return false;
	}

	if (!acc_detector_distance_activate(distance_handle))
	{
		printf("acc_detector_distance_activate() failed\n");
		acc_detector_distance_destroy(&distance_handle);
		return false;
	}

	bool                                success         = true;
	const int                           iterations      = 5;
	uint16_t                            number_of_peaks = 5;
	acc_detector_distance_result_t      result[number_of_peaks];
	acc_detector_distance_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_detector_distance_get_next(distance_handle, result, number_of_peaks, &result_info);

		if (!success)
		{
			printf("acc_detector_distance_get_next() failed\n");
			break;
		}

		print_distances(result, result_info.number_of_peaks);
	}

	bool deactivated = acc_detector_distance_deactivate(distance_handle);

	acc_detector_distance_destroy(&distance_handle);

	return deactivated && success;
}


void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count)
{
	printf("Found %u peaks:\n", (unsigned int)reflection_count);

	for (uint16_t i = 0; i < reflection_count; i++)
	{
		printf("Amplitude %u at %u mm\n", (unsigned int)result[i].amplitude,
		       (unsigned int)(result[i].distance_m * 1000));
	}
}
