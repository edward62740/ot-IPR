// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_RF_CERTIFICATION_TEST_H_
#define ACC_RF_CERTIFICATION_TEST_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Execute the RF certification test
 *
 * @param tx_disable Set to true to disable TX, set to false to enable TX
 * @param iterations The number of iterations, set to 0 to run infinite number of iterations
 *
 * @return True if executed successfully, false if not
 */
bool acc_rf_certification_test(bool tx_disable, uint32_t iterations);


#endif
