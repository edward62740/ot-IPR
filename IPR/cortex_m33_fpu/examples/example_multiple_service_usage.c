// Copyright (c) Acconeer AB, 2020-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_envelope.h"
#include "acc_service_sparse.h"
#include "acc_version.h"

/** \example example_multiple_service_usage.c
 * @brief This is an example on how the RSS configuration 'override sensor id check at creation' can be used for using multiple services
 *
 * It is mainly a tool to easier and faster switch between services and detectors
 * on the same sensor. This can also be used to switch between configurations for the
 * same service or detector.
 *
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Set 'override sensor id check at creation' to true
 *   - Create an envelope service configuration
 *   - Create a sparse service configuration
 *   - Create an envelope service using the envelope service configuration
 *   - Create a sparse service using the sparse service configuration
 *   - Destroy the configurations
 *   - In a loop:
 *     - Activate the envelope service
 *     - Get the result and print it
 *     - Deactivate the envelope service
 *     - Activate the sparse service
 *     - Get the result and print it
 *     - Deactivate the sparse service
 *   - End of loop
 *   - Destroy the services
 *   - Deactivate Radar System Software (RSS)
 */


static void print_envelope_data(uint16_t *data, uint16_t data_length);


static void print_sparse_data(uint16_t *data, uint16_t data_length, uint16_t sweeps_per_frame);


int acc_example_multiple_service_usage(int argc, char *argv[]);


int acc_example_multiple_service_usage(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	// Activate RSS and override sensor ID check
	if (!acc_rss_activate(hal))
	{
		printf("acc_rss_activate() failed\n");
		return EXIT_FAILURE;
	}

	acc_rss_override_sensor_id_check_at_creation(true);

	// Create service configurations
	acc_service_configuration_t envelope_configuration = acc_service_envelope_configuration_create();

	if (envelope_configuration == NULL)
	{
		printf("acc_service_envelope_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_configuration_t sparse_configuration = acc_service_sparse_configuration_create();

	if (sparse_configuration == NULL)
	{
		printf("acc_service_sparse_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	// Create envelope service and extract metadata
	acc_service_handle_t envelope_handle = acc_service_create(envelope_configuration);

	if (envelope_handle == NULL)
	{
		printf("acc_service_create() for envelope failed\n");
		acc_service_envelope_configuration_destroy(&envelope_configuration);
		acc_service_sparse_configuration_destroy(&sparse_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_envelope_metadata_t envelope_metadata = { 0 };
	acc_service_envelope_get_metadata(envelope_handle, &envelope_metadata);

	acc_service_envelope_configuration_destroy(&envelope_configuration);

	// Create sparse service and extract metadata
	acc_service_handle_t sparse_handle = acc_service_create(sparse_configuration);

	if (sparse_handle == NULL)
	{
		printf("acc_service_create() for sparse failed\n");
		acc_service_destroy(&envelope_handle);
		acc_service_sparse_configuration_destroy(&sparse_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_sparse_metadata_t sparse_metadata = { 0 };
	acc_service_sparse_get_metadata(sparse_handle, &sparse_metadata);

	uint16_t sweeps_per_frame = acc_service_sparse_configuration_sweeps_per_frame_get(sparse_configuration);

	acc_service_sparse_configuration_destroy(&sparse_configuration);

	// Activate, retrieve result and deactivate the services
	bool      success    = true;
	const int iterations = 2;

	uint16_t                           *envelope_data;
	acc_service_envelope_result_info_t envelope_result_info;

	uint16_t                         *sparse_data;
	acc_service_sparse_result_info_t sparse_result_info;

	for (int i = 0; i < iterations; i++)
	{
		// Envelope service
		if (!acc_service_activate(envelope_handle))
		{
			success = false;
			printf("acc_service_activate() for envelope failed\n");
			break;
		}

		if (!acc_service_envelope_get_next_by_reference(envelope_handle, &envelope_data, &envelope_result_info))
		{
			acc_service_deactivate(envelope_handle);
			success = false;
			printf("acc_service_envelope_get_next_by_reference() failed\n");
			break;
		}

		print_envelope_data(envelope_data, envelope_metadata.data_length);

		if (!acc_service_deactivate(envelope_handle))
		{
			success = false;
			printf("acc_service_deactivate() for envelope failed\n");
			break;
		}

		// Sparse service
		if (!acc_service_activate(sparse_handle))
		{
			success = false;
			printf("acc_service_activate() for sparse failed\n");
			break;
		}

		if (!acc_service_sparse_get_next_by_reference(sparse_handle, &sparse_data, &sparse_result_info))
		{
			acc_service_deactivate(sparse_handle);
			success = false;
			printf("acc_service_sparse_get_next_by_reference() failed\n");
			break;
		}

		print_sparse_data(sparse_data, sparse_metadata.data_length, sweeps_per_frame);

		if (!acc_service_deactivate(sparse_handle))
		{
			success = false;
			printf("acc_service_deactivate() for sparse failed\n");
			break;
		}
	}

	acc_service_destroy(&envelope_handle);
	acc_service_destroy(&sparse_handle);

	if (success)
	{
		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


void print_envelope_data(uint16_t *data, uint16_t data_length)
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

	printf("\n\n");
}


void print_sparse_data(uint16_t *data, uint16_t data_length, uint16_t sweeps_per_frame)
{
	uint16_t sweep_length = data_length / sweeps_per_frame;

	printf("Sparse data:\n");
	for (uint16_t sweep = 0; sweep < sweeps_per_frame; sweep++)
	{
		printf("Sweep %u:\t", (unsigned int)(sweep + 1));

		for (uint16_t sweep_idx = 0; sweep_idx < sweep_length; sweep_idx++)
		{
			printf("%6u", (unsigned int)data[(sweep * sweep_length) + sweep_idx]);
		}

		printf("\n");
	}

	printf("\n");
}
