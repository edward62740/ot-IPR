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
#include "acc_rss_assembly_test.h"
#include "acc_version.h"


/** \example example_assembly_test.c
 * @brief --Deprecated-- This is an example on how the assembly test can be used
 * @n
 * This example was replaced with example_bring_up
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an assembly test configuration and configure it
 *   - Run the assembly test
 *   - Print the assembly test results
 *   - Destroy the assembly test configuration
 *   - Deactivate Radar System Software (RSS)
 */

int acc_example_assembly_test(int argc, char *argv[]);


#define DEFAULT_SENSOR_ID 1


int acc_example_assembly_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf("Deprecated: This example is replaced with example_bring_up.c");

	printf("Acconeer software version %s\n", acc_version_get());

	acc_rss_assembly_test_result_t test_results[ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS];
	uint16_t                       nr_of_test_results = ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS;
	bool                           success;

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_t configuration = acc_rss_assembly_test_configuration_create();

	acc_rss_assembly_test_configuration_sensor_set(configuration, DEFAULT_SENSOR_ID);

	success = acc_rss_assembly_test(configuration, test_results, &nr_of_test_results);

	if (success)
	{
		bool all_passed = true;

		for (uint16_t i = 0; i < nr_of_test_results; i++)
		{
			const bool test_passed = test_results[i].test_passed;
			printf("Name: %s, result: %s\n", test_results[i].test_name, test_passed ? "Pass" : "Fail");
			all_passed = test_passed ? all_passed : false;
		}

		if (all_passed)
		{
			printf("Assembly test: All tests passed\n");
		}
		else
		{
			printf("Assembly test: There are failed tests\n");
			success = false;
		}
	}
	else
	{
		printf("Assembly test: Failed to complete\n");
	}

	acc_rss_assembly_test_configuration_destroy(&configuration);

	acc_rss_deactivate();

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
