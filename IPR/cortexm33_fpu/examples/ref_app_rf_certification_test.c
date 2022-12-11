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
#include "acc_rf_certification_test.h"
#include "acc_rss.h"
#include "acc_version.h"

int acc_ref_app_rf_certification_test(int argc, char *argv[]);


int acc_ref_app_rf_certification_test(int argc, char *argv[])
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

	bool     tx_disable = false;
	uint32_t iterations = 0; // 0 means 'run forever'

	bool success = acc_rf_certification_test(tx_disable, iterations);

	acc_rss_deactivate();

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
