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
#include "acc_rss_assembly_test.h"
#include "acc_version.h"


/** \example example_bring_up.c
 * @brief This is an example on how the assembly test can be used to ease bring-up
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an assembly test configuration, disabling every test
 *   - Enable one assembly test at a time and run it
 *   - Print the assembly test results
 *   - Destroy the assembly test configuration
 *   - Deactivate Radar System Software (RSS)
 */


#define DEFAULT_SENSOR_ID 1

static bool run_test(acc_rss_assembly_test_configuration_t configuration);


int acc_example_bring_up(int argc, char *argv[]);


int acc_example_bring_up(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_t configuration = acc_rss_assembly_test_configuration_create();
	acc_rss_assembly_test_configuration_sensor_set(configuration, DEFAULT_SENSOR_ID);

	// Disable all tests (they are enabled by default)
	acc_rss_assembly_test_configuration_all_tests_disable(configuration);

	// Enable and run: Read Test
	acc_rss_assembly_test_configuration_communication_read_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Read Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_communication_read_test_disable(configuration);

	// Enable and run: Write Read Test
	acc_rss_assembly_test_configuration_communication_write_read_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Write Read Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_communication_write_read_test_disable(configuration);

	// Enable and run: Interrupt Test
	acc_rss_assembly_test_configuration_communication_interrupt_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Interrupt Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_communication_interrupt_test_disable(configuration);

	// Enable and run: Hibernate Test
	acc_rss_assembly_test_configuration_communication_hibernate_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Hibernate Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_communication_hibernate_test_disable(configuration);

	// Enable and run: Supply Test
	acc_rss_assembly_test_configuration_supply_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Supply Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_supply_test_disable(configuration);

	// Enable and run: Clock Test
	acc_rss_assembly_test_configuration_clock_test_enable(configuration);
	if (!run_test(configuration))
	{
		printf("Bring up test: Clock Test failed\n");
		acc_rss_assembly_test_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_clock_test_disable(configuration);

	printf("Bring up test: All tests passed\n");

	acc_rss_assembly_test_configuration_destroy(&configuration);
	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


static bool run_test(acc_rss_assembly_test_configuration_t configuration)
{
	acc_rss_assembly_test_result_t test_results[ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS];
	uint16_t                       nr_of_test_results = ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS;

	bool success = acc_rss_assembly_test(configuration, test_results, &nr_of_test_results);

	if (success)
	{
		for (uint16_t i = 0; i < nr_of_test_results; i++)
		{
			const bool passed = test_results[i].test_passed;
			printf("Name: %s, result: %s\n", test_results[i].test_name, passed ? "Pass" : "Fail");
		}
	}
	else
	{
		printf("Bring up test: Failed to complete\n");
		return success;
	}

	return success;
}
