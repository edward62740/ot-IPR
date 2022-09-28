// Copyright (c) Acconeer AB, 2019-2022
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdint.h>
#include <stdio.h>

#include "acc_integration.h"
#include "sl_sleeptimer.h"

void acc_integration_sleep_ms(uint32_t time_msec)
{
  sl_sleeptimer_delay_millisecond(time_msec);
}


void acc_integration_sleep_us(uint32_t time_usec)
{
  sl_udelay_wait(time_usec);
}


uint32_t acc_integration_get_time(void)
{
	return sl_sleeptimer_get_tick_count();
}


void *acc_integration_mem_alloc(size_t size)
{
	return malloc(size);
}


void acc_integration_mem_free(void *ptr)
{
	free(ptr);
}
