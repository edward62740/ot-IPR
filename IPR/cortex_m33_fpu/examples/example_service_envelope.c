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
#include "acc_service_envelope.h"
#include "acc_version.h"


/** \example example_service_envelope.c
 * @brief This is an example on how the envelope service can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an envelope service configuration
 *   - Create an envelope service using the previously created configuration
 *   - Destroy the envelope service configuration
 *   - Activate the envelope service
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the envelope service
 *   - Deactivate Radar System Software (RSS)
 */


static void update_configuration(acc_service_configuration_t envelope_configuration);


static void print_data(uint16_t *data, uint16_t data_length);


int acc_example_service_envelope(int argc, char *argv[]);


int acc_example_service_envelope(int argc, char *argv[])
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

	acc_service_configuration_t envelope_configuration = acc_service_envelope_configuration_create();

	if (envelope_configuration == NULL)
	{
		printf("acc_service_envelope_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	update_configuration(envelope_configuration);

	acc_service_handle_t handle = acc_service_create(envelope_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create() failed\n");
		acc_service_envelope_configuration_destroy(&envelope_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_envelope_configuration_destroy(&envelope_configuration);

	acc_service_envelope_metadata_t envelope_metadata = { 0 };
	acc_service_envelope_get_metadata(handle, &envelope_metadata);

	printf("Start: %d mm\n", (int)(envelope_metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(envelope_metadata.length_m * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(envelope_metadata.data_length));

	if (!acc_service_activate(handle))
	{
		printf("acc_service_activate() failed\n");
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                               success    = true;
	const int                          iterations = 5;
	uint16_t                           data[envelope_metadata.data_length];
	acc_service_envelope_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_service_envelope_get_next(handle, data, envelope_metadata.data_length, &result_info);

		if (!success)
		{
			printf("acc_service_envelope_get_next() failed\n");
			break;
		}

		print_data(data, envelope_metadata.data_length);
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


void update_configuration(acc_service_configuration_t envelope_configuration)
{
	float start_m  = 0.2f;
	float length_m = 0.5f;

	acc_service_requested_start_set(envelope_configuration, start_m);
	acc_service_requested_length_set(envelope_configuration, length_m);
}


void print_data(uint16_t *data, uint16_t data_length)
{
	printf("Envelope data:\n");
	for (uint16_t i = 0; i < data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		printf("%6u", (unsigned int)(data[i]));
	}

	printf("\n");
}
