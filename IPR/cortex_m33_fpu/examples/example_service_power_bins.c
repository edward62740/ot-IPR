// Copyright (c) Acconeer AB, 2015-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_power_bins.h"
#include "acc_version.h"


/** \example example_service_power_bins.c
 * @brief This is an example on how the Power Bins service can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a Power Bins service configuration
 *   - Create a Power Bins service using the previously created configuration
 *   - Destroy the Power Bins service configuration
 *   - Activate the Power Bins service
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the Power Bins service
 *   - Deactivate Radar System Software (RSS)
 */


static void update_configuration(acc_service_configuration_t power_bins_configuration);


static void print_data(uint16_t *data, uint16_t data_length);


int acc_example_service_power_bins(int argc, char *argv[]);


int acc_example_service_power_bins(int argc, char *argv[])
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

	acc_service_configuration_t power_bins_configuration = acc_service_power_bins_configuration_create();

	if (power_bins_configuration == NULL)
	{
		printf("acc_service_power_bins_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	update_configuration(power_bins_configuration);

	acc_service_handle_t handle = acc_service_create(power_bins_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create() failed\n");
		acc_service_power_bins_configuration_destroy(&power_bins_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_power_bins_configuration_destroy(&power_bins_configuration);

	acc_service_power_bins_metadata_t power_bins_metadata = { 0 };
	acc_service_power_bins_get_metadata(handle, &power_bins_metadata);

	printf("Start: %d mm\n", (int)(power_bins_metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(power_bins_metadata.length_m * 1000.0f));
	printf("Bin count: %u\n", (unsigned int)(power_bins_metadata.bin_count));

	if (!acc_service_activate(handle))
	{
		printf("acc_service_activate() failed\n");
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                                 success    = true;
	const int                            iterations = 5;
	uint16_t                             data[power_bins_metadata.bin_count];
	acc_service_power_bins_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_service_power_bins_get_next(handle, data, power_bins_metadata.bin_count, &result_info);

		if (!success)
		{
			printf("acc_service_power_bins_get_next() failed\n");
			break;
		}

		print_data(data, power_bins_metadata.bin_count);
	}

	bool deactivated = acc_service_deactivate(handle);

	acc_service_destroy(&handle);

	acc_rss_deactivate();

	if (deactivated && success)
	{
		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


void update_configuration(acc_service_configuration_t power_bins_configuration)
{
	float start_m  = 0.2f;
	float length_m = 0.5f;

	acc_service_requested_start_set(power_bins_configuration, start_m);
	acc_service_requested_length_set(power_bins_configuration, length_m);
}


void print_data(uint16_t *data, uint16_t data_length)
{
	printf("Power Bins data:\n");

	for (uint16_t i = 0; i < data_length; i++)
	{
		printf("%u\t", (unsigned int)(data[i]));
	}

	printf("\n");
}
