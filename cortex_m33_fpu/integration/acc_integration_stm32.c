// Copyright (c) Acconeer AB, 2019-2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdint.h>
#include <stdio.h>

#include "main.h"

#include "acc_integration.h"


void acc_integration_sleep_ms(uint32_t time_msec)
{
	HAL_Delay(time_msec);
}


void acc_integration_sleep_us(uint32_t time_usec)
{
	uint32_t time_msec = (time_usec / 1000) + 1;

	HAL_Delay(time_msec);
}


uint32_t acc_integration_get_time(void)
{
	return HAL_GetTick();
}


void *acc_integration_mem_alloc(size_t size)
{
	return malloc(size);
}


void acc_integration_mem_free(void *ptr)
{
	free(ptr);
}
