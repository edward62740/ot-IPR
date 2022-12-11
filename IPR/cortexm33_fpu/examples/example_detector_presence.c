// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdio.h>

#include "acc_definitions_common.h"
#include "acc_detector_presence.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_integration.h"
#include "acc_rss.h"
#include "acc_version.h"

/** \example example_detector_presence.c
 * @brief This is an example on how the presence detector can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a presence detector configuration
 *   - Create a presence detector using the previously created configuration
 *   - Destroy the presence detector configuration
 *   - Activate the presence detector
 *   - Get the result and print it 200 times
 *   - Deactivate and destroy the presence detector
 *   - Deactivate Radar System Software (RSS)
 */


#define DEFAULT_START_M             (0.2f)
#define DEFAULT_LENGTH_M            (1.4f)
#define DEFAULT_UPDATE_RATE         (10)
#define DEFAULT_POWER_SAVE_MODE     ACC_POWER_SAVE_MODE_SLEEP
#define DEFAULT_DETECTION_THRESHOLD (2.0f)
#define DEFAULT_NBR_REMOVED_PC      (0)


static void update_configuration(acc_detector_presence_configuration_t presence_configuration);


static void print_result(acc_detector_presence_result_t result);


int acc_example_detector_presence(int argc, char *argv[]);


int acc_example_detector_presence(int argc, char *argv[])
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

	acc_detector_presence_configuration_t presence_configuration = acc_detector_presence_configuration_create();
	if (presence_configuration == NULL)
	{
		printf("Failed to create configuration\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	update_configuration(presence_configuration);

	acc_detector_presence_handle_t handle = acc_detector_presence_create(presence_configuration);
	if (handle == NULL)
	{
		printf("Failed to create detector\n");
		acc_detector_presence_configuration_destroy(&presence_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_presence_configuration_destroy(&presence_configuration);

	if (!acc_detector_presence_activate(handle))
	{
		printf("Failed to activate detector\n");
		acc_detector_presence_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                           success    = true;
	const int                      iterations = 200;
	acc_detector_presence_result_t result;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_detector_presence_get_next(handle, &result);
		if (!success)
		{
			printf("acc_detector_presence_get_next() failed\n");
			break;
		}

		print_result(result);

		acc_integration_sleep_ms(1000 / DEFAULT_UPDATE_RATE);
	}

	bool deactivated = acc_detector_presence_deactivate(handle);

	acc_detector_presence_destroy(&handle);

	acc_rss_deactivate();

	if (deactivated && success)
	{
		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


void update_configuration(acc_detector_presence_configuration_t presence_configuration)
{
	acc_detector_presence_configuration_update_rate_set(presence_configuration, DEFAULT_UPDATE_RATE);
	acc_detector_presence_configuration_detection_threshold_set(presence_configuration, DEFAULT_DETECTION_THRESHOLD);
	acc_detector_presence_configuration_start_set(presence_configuration, DEFAULT_START_M);
	acc_detector_presence_configuration_length_set(presence_configuration, DEFAULT_LENGTH_M);
	acc_detector_presence_configuration_power_save_mode_set(presence_configuration, DEFAULT_POWER_SAVE_MODE);
	acc_detector_presence_configuration_nbr_removed_pc_set(presence_configuration, DEFAULT_NBR_REMOVED_PC);
}


void print_result(acc_detector_presence_result_t result)
{
	if (result.presence_detected)
	{
		printf("Motion\n");
	}
	else
	{
		printf("No motion\n");
	}

	printf("Presence score: %d, Distance: %d\n", (int)(result.presence_score * 1000.0f), (int)(result.presence_distance * 1000.0f));
}
