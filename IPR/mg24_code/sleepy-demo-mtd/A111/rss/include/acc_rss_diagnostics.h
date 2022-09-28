// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_RSS_DIAGNOSTICS_H_
#define ACC_RSS_DIAGNOSTICS_H_

#include "acc_definitions_common.h"
#include <stdbool.h>


/**
 * @defgroup Diagnostic_test Diagnostic test
 * @ingroup RSS
 *
 * @brief RSS Diagnostic test
 *
 * @{
 */


/**
 * @brief Run diagnostic test
 *
 * This function executes a suite of tests for diagnostic testing of the A111 sensor.
 *
 * @details
 *   - This function must be called after #acc_rss_activate.
 *   - This function must be called before any service is created.
 *   - If a test fails to execute the return value will be set to false.
 *
 * @param[in] sensor_id The sensor to run diagnostic test on
 * @return True if successfully run, false otherwise
 */
bool acc_rss_diagnostic_test(acc_sensor_id_t sensor_id);


/**
 * @}
 */

#endif
