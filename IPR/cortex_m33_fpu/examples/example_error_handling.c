// Copyright (c) Acconeer AB, 2021
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


/** \example example_error_handling.c
 * @brief This is an example on how result_info can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an envelope service configuration
 *   - Create an envelope service using the previously created configuration
 *   - Activate the envelope service
 *   - Get the result and print it 5 times
 * 	 - Detect and appropriately handle indications from acc_service_envelope_get_next
 *   - Destroy envelope configuration
 *   - Deactivate and destroy the envelope service
 *   - Deactivate Radar System Software (RSS)
 */


static void print_data(uint16_t *data, uint16_t data_length);

static bool handle_indication(acc_service_envelope_result_info_t *result_info, acc_service_handle_t *handle,
                              acc_service_configuration_t envelope_configuration, bool *success);

static bool restart_service(acc_service_handle_t *handle, acc_service_configuration_t envelope_configuration);


int acc_example_error_handling(int argc, char *argv[]);


int acc_example_error_handling(int argc, char *argv[])
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

	acc_service_handle_t handle = acc_service_create(envelope_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create() failed\n");
		acc_service_envelope_configuration_destroy(&envelope_configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_service_envelope_metadata_t envelope_metadata = { 0 };
	acc_service_envelope_get_metadata(handle, &envelope_metadata);

	printf("Start: %d mm\n", (int)(envelope_metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(envelope_metadata.length_m * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(envelope_metadata.data_length));

	if (!acc_service_activate(handle))
	{
		printf("acc_service_activate() failed\n");
		acc_service_envelope_configuration_destroy(&envelope_configuration);
		acc_service_destroy(&handle);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	bool                               try_again;
	bool                               success       = true;
	const int                          iterations    = 5;
	const int                          attempt_limit = 10;
	uint16_t                           data[envelope_metadata.data_length];
	acc_service_envelope_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		int attempt = 0;

		do
		{
			attempt++;

			if (attempt > attempt_limit)
			{
				printf("An error could not be mitigated after %d attempts\n", attempt_limit);

				success = false;
				break;
			}

			/**
			 * Return value == false is assumed to be due to a sensor_communication_error, which is handled in handle_indication.
			 * Assume that the same data length can be used regardless of indication.
			 */
			acc_service_envelope_get_next(handle, data, envelope_metadata.data_length, &result_info);

			try_again = handle_indication(&result_info, &handle, envelope_configuration, &success);
		} while (try_again);

		if (!success)
		{
			printf("An error was encountered that could not be handled\n");
			printf("Exiting...\n");

			break;
		}

		print_data(data, envelope_metadata.data_length);
	}

	acc_service_envelope_configuration_destroy(&envelope_configuration);

	bool deactivated = acc_service_deactivate(handle);

	acc_service_destroy(&handle);

	acc_rss_deactivate();

	if (deactivated)
	{
		printf("Application finished OK\n");
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


void print_data(uint16_t *data, uint16_t data_length)
{
	printf("Envelope data:\n");
	for (uint16_t i = 0; i < data_length; i++)
	{
		if ((i > 0) && ((i % 10) == 0))
		{
			printf("\n");
		}

		printf("%6u", (unsigned int)(data[i]));
	}

	printf("\n");
}


/**
 * This function will look at the result_info field after a call to acc_service_envelope_get_next().
 * Returns true if an error was detected, after trying to correct the error.
 */
bool handle_indication(acc_service_envelope_result_info_t *result_info, acc_service_handle_t *handle,
                       acc_service_configuration_t envelope_configuration,
                       bool *success)
{
	if (result_info->missed_data)
	{
		/**
		 * Indication of missed data from the sensor.
		 * Only neccecary to handle this error if running streaming mode.
		 * Possible actions to mitigate this error are:
		 *  - Change the update rate using: acc_service_repetition_mode_streaming_set
		 *  - Add a downsampling factor using: acc_service_envelope_downsampling_factor_set
		 *  - Change the sweep length using: acc_service_requested_length_set
		 */

		printf("Missing data was reported\n");
		printf("Retrying...\n");

		return true;
	}

	if (result_info->sensor_communication_error)
	{
		/**
		 * Indication of a sensor communication error.
		 * The service probably needs to be restarted.
		 */

		printf("There was a sensor communication error\n");
		printf("Attempting to restart service...\n");

		*success = restart_service(handle, envelope_configuration);

		return *success;
	}

	if (result_info->data_saturated)
	{
		/**
		 * Indication of sensor data being saturated.
		 * This can cause result instability.
		 * Error can be mitigated by lowering the gain of the receiver.
		 */

		printf("Data was saturated\n");
		printf("Retrying with lower gain...\n");

		float gain              = acc_service_receiver_gain_get(envelope_configuration);
		float conversion_scalar = 0.9;
		float new_gain          = gain * conversion_scalar;

		acc_service_receiver_gain_set(envelope_configuration, new_gain);

		*success = restart_service(handle, envelope_configuration);

		return *success;
	}

	if (result_info->data_quality_warning)
	{
		/**
		 * Indication of bad data quality.
		 * This may be addressed by restarting the service to recalibrate the sensor.
		 */

		printf("The data may be of poor quality\n");
		printf("Attempting to restart service to recalibrate the sensor...\n");

		*success = restart_service(handle, envelope_configuration);

		return *success;
	}

	// No indications received, so no need to retry
	return EXIT_FAILURE;
}


bool restart_service(acc_service_handle_t *handle, acc_service_configuration_t envelope_configuration)
{
	acc_service_deactivate(*handle);
	acc_service_destroy(handle);

	*handle = acc_service_create(envelope_configuration);

	if (*handle == NULL)
	{
		printf("acc_service_create() failed\n");
		return false;
	}

	if (!acc_service_activate(*handle))
	{
		printf("acc_service_activate() failed\n");
		return false;
	}

	return true;
}
