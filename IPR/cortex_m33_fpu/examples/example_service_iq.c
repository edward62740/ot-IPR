// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_iq.h"
#include "acc_version.h"


/** \example example_service_iq.c
 * @brief This is an example on how the IQ service can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an IQ service configuration
 *   - Create an IQ service using the previously created configuration
 *   - Destroy the IQ service configuration
 *   - Activate the IQ service
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the IQ service
 *   - Deactivate Radar System Software (RSS)
 */


static void update_configuration(acc_service_configuration_t iq_configuration);


static void print_data(float complex *data, uint16_t data_length);


int acc_example_service_iq(int argc, char *argv[]);


int acc_example_service_iq(int argc, char *argv[])
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

	acc_service_configuration_t iq_configuration = acc_service_iq_configuration_create();

	if (iq_configuration == NULL)
	{
		printf("acc_service_iq_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	update_configuration(iq_configuration);

	acc_service_handle_t handle = acc_service_create(iq_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create() failed\n");
		acc_service_iq_configuration_destroy(&iq_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_iq_configuration_destroy(&iq_configuration);

	acc_service_iq_metadata_t iq_metadata = { 0 };
	acc_service_iq_get_metadata(handle, &iq_metadata);

	printf("Start: %d mm\n", (int)(iq_metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(iq_metadata.length_m * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(iq_metadata.data_length));

	if (!acc_service_activate(handle))
	{
		printf("acc_service_activate() failed\n");
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                         success    = true;
	const int                    iterations = 5;
	float complex                data[iq_metadata.data_length];
	acc_service_iq_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_service_iq_get_next(handle, data, iq_metadata.data_length, &result_info);

		if (!success)
		{
			printf("acc_service_iq_get_next() failed\n");
			break;
		}

		print_data(data, iq_metadata.data_length);
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


void update_configuration(acc_service_configuration_t iq_configuration)
{
	acc_service_iq_output_format_set(iq_configuration, ACC_SERVICE_IQ_OUTPUT_FORMAT_FLOAT_COMPLEX);

	float start_m  = 0.2f;
	float length_m = 0.5f;

	acc_service_requested_start_set(iq_configuration, start_m);
	acc_service_requested_length_set(iq_configuration, length_m);
}


void print_data(float complex *data, uint16_t data_length)
{
	printf("IQ data in polar coordinates (r, phi) (multiplied by 1000):\n");

	for (uint16_t i = 0; i < data_length; i++)
	{
		if ((i > 0) && ((i % 8) == 0))
		{
			printf("\n");
		}

		printf("(%d, %d)\t", (int)(cabsf(data[i]) * 1000), (int)(cargf(data[i]) * 1000));
	}

	printf("\n");
}
