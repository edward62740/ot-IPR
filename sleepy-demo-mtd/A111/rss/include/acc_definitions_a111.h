// Copyright (c) Acconeer AB, 2018-2021
// All rights reserved

#ifndef ACC_DEFINITIONS_A111_H_
#define ACC_DEFINITIONS_A111_H_

#include <stdint.h>


/**
 * @brief Power save mode
 *
 * Each power save mode corresponds to how much of the sensor hardware is shutdown
 * between data frame aquisition. Mode 'OFF' means that the whole sensor is shutdown.
 * Mode 'ACTIVE' means that the sensor is in its active state all the time.
 * Mode 'HIBERNATE' means that the sensor is still powered but the internal oscillator is
 * turned off and the application needs to clock the sensor by toggling a GPIO a pre-defined
 * number of times to enter and exit this mode. Mode 'HIBERNATE' is currently only supported
 * by the Sparse service
 *
 * For each power save mode there will be a limit in the achievable update rate. Mode 'OFF'
 * can achieve the lowest power consumption but also has the lowest update rate limit
 */
typedef enum
{
	ACC_POWER_SAVE_MODE_OFF,
	ACC_POWER_SAVE_MODE_SLEEP,
	ACC_POWER_SAVE_MODE_READY,
	ACC_POWER_SAVE_MODE_ACTIVE,
	ACC_POWER_SAVE_MODE_HIBERNATE,
} acc_power_save_mode_enum_t;
typedef uint32_t acc_power_save_mode_t;


/**
 * @brief Service profile
 *
 * Each profile consists of a number of settings for the sensor that configures
 * the RX and TX paths. Profile 1 maximizes on the depth resolution and
 * profile 5 maximizes on radar loop gain with a sliding scale in between.
 */
typedef enum
{
	ACC_SERVICE_PROFILE_1 = 1,
	ACC_SERVICE_PROFILE_2,
	ACC_SERVICE_PROFILE_3,
	ACC_SERVICE_PROFILE_4,
	ACC_SERVICE_PROFILE_5,
} acc_service_profile_t;


/**
 * @brief Maximum Unambiguous Range
 */
typedef enum
{
	/** Maximum unambiguous range 11.5 m, maximum measurable distance 7.0 m, pulse repetition frequency 13.0 MHz */
	ACC_SERVICE_MUR_6 = 6,
	/** Maximum unambiguous range 17.3 m, maximum measurable distance 12.7 m , pulse repetition frequency 8.7 MHz */
	ACC_SERVICE_MUR_9 = 9,

	/** Default maximum unambiguous range */
	ACC_SERVICE_MUR_DEFAULT = ACC_SERVICE_MUR_6,
} acc_service_mur_t;


/**
 * Type used when retrieving and setting a sensor calibration context
 */
typedef struct
{
	uint8_t data[64];
} acc_calibration_context_t;


#endif
