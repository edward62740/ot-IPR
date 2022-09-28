// Copyright (c) Acconeer AB, 2019-2021
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
#include "acc_service_sparse.h"

#include "acc_version.h"


/** \example example_service_sparse.c
 * @brief This is an example on how the sparse service can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a sparse service configuration
 *   - Create a sparse service using the previously created configuration
 *   - Destroy the sparse service configuration
 *   - Activate the sparse service
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the sparse service
 *   - Deactivate Radar System Software (RSS)
 */


static void update_configuration(acc_service_configuration_t sparse_configuration);


static void print_data(uint16_t *data, uint16_t data_length, uint16_t sweeps_per_frame);


int acc_example_service_sparse(int argc, char *argv[]);


int acc_example_service_sparse(int argc, char *argv[])
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

	acc_service_configuration_t sparse_configuration = acc_service_sparse_configuration_create();

	if (sparse_configuration == NULL)
	{
		printf("acc_service_sparse_configuration_create() failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	update_configuration(sparse_configuration);

	acc_service_handle_t handle = acc_service_create(sparse_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create() failed\n");
		acc_service_sparse_configuration_destroy(&sparse_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_sparse_metadata_t sparse_metadata = { 0 };
	acc_service_sparse_get_metadata(handle, &sparse_metadata);

	uint16_t sweeps_per_frame = acc_service_sparse_configuration_sweeps_per_frame_get(sparse_configuration);

	printf("Start: %d mm\n", (int)(sparse_metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(sparse_metadata.length_m * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(sparse_metadata.data_length));
	printf("Sweeps per frame: %u\n", (unsigned int)sweeps_per_frame);

	acc_service_sparse_configuration_destroy(&sparse_configuration);

	if (!acc_service_activate(handle))
	{
		printf("acc_service_activate() failed\n");
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                             success    = true;
	const int                        iterations = 5;
	uint16_t                         data[sparse_metadata.data_length];
	acc_service_sparse_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_service_sparse_get_next(handle, data, sparse_metadata.data_length, &result_info);

		if (!success)
		{
			printf("acc_service_sparse_get_next() failed\n");
			break;
		}

		print_data(data, sparse_metadata.data_length, sweeps_per_frame);
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


void update_configuration(acc_service_configuration_t sparse_configuration)
{
	uint16_t sweeps_per_frame = 16;
	float    start_m          = 0.18f;
	float    length_m         = 0.36f;

	acc_service_sparse_configuration_sweeps_per_frame_set(sparse_configuration, sweeps_per_frame);

	acc_service_requested_start_set(sparse_configuration, start_m);
	acc_service_requested_length_set(sparse_configuration, length_m);
}


void print_data(uint16_t *data, uint16_t data_length, uint16_t sweeps_per_frame)
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
