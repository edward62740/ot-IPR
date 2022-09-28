// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_RSS_ASSEMBLY_TEST_H_
#define ACC_RSS_ASSEMBLY_TEST_H_

#include "acc_definitions_common.h"
#include <stdbool.h>


#define ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS (20U)


/**
 * @defgroup Assembly_test Assembly test
 * @ingroup RSS
 *
 * @brief RSS Assembly test
 *
 * @{
 */


/**
 * @brief The result of an assembly test run
 */
typedef struct
{
	const char *test_name;
	bool       test_passed;
} acc_rss_assembly_test_result_t;


/**
 * @brief Assembly test configuration container
 */
struct acc_rss_assembly_test_configuration;

typedef struct acc_rss_assembly_test_configuration *acc_rss_assembly_test_configuration_t;


/**
 * @brief Create a configuration for an assembly test run
 *
 * @return An assembly test configuration, NULL if creation was not possible
 */
acc_rss_assembly_test_configuration_t acc_rss_assembly_test_configuration_create(void);


/**
 * @brief Destroy an assembly test configuration
 *
 * The assembly test configuration reference is set to NULL after destruction.
 *
 * @param[in] configuration The configuration to destroy, set to NULL
 */
void acc_rss_assembly_test_configuration_destroy(acc_rss_assembly_test_configuration_t *configuration);


/**
 * @brief Set the sensor id
 *
 * @param[in] configuration An assembly test configuration
 * @param[in] sensor_id The sensor id to set
 */
void acc_rss_assembly_test_configuration_sensor_set(acc_rss_assembly_test_configuration_t configuration, acc_sensor_id_t sensor_id);


/**
 * @brief Get the sensor id that is configured
 *
 * @param[in] configuration An assembly test configuration
 * @return Sensor id
 */
acc_sensor_id_t acc_rss_assembly_test_configuration_sensor_get(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enable the sensor communication read test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_read_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the sensor communication read test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_read_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enable the sensor communication write and read test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_write_read_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the sensor communication write and read test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_write_read_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enalbe the sensor communication interrupt test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_interrupt_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the sensor communication interrupt test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_interrupt_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enable the sensor communication hibernate test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_hibernate_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the sensor communication hibernate test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_communication_hibernate_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enable the supply test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_supply_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the supply test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_supply_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Enable the clock test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_clock_test_enable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable the clock test
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_clock_test_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Disable all tests
 *
 * @param[in] configuration An assembly test configuration
 */
void acc_rss_assembly_test_configuration_all_tests_disable(acc_rss_assembly_test_configuration_t configuration);


/**
 * @brief Run assembly test
 *
 * This function executes a suite of tests suitable for PCB assembly testing.
 *
 * @details The caller need to allocate an array of assembly test results of at least the size
 *          of ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS. The array size is to be provided in the
 *          nr_of_test_results parameter. If the size is not sufficiently large the test will
 *          fail.
 *
 *          If a test fails to execute the return value will be set to false.
 *          If a test cannot pass its test limits the test_passed member of test_results will be
 *          set to false but the return value of this function will still be true.
 *
 * @param[in] test_configuration The test configuration
 * @param[out] test_results Reference to struct where the test results will be stored
 * @param[in,out] nr_of_test_results Input is the maximum number of items in the results array.
 *                                   Output is the actual number of test results available after
 *                                   the execution of the tests.
 * @return True if successfully run, false otherwise
 */
bool acc_rss_assembly_test(acc_rss_assembly_test_configuration_t test_configuration, acc_rss_assembly_test_result_t *test_results,
                           uint16_t *nr_of_test_results);


/**
 * @}
 */

#endif
