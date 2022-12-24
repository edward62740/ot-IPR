/*
 * app_util.c
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include "openthread-system.h"

#include "sl_component_catalog.h"
#include <openthread/coap.h>
#include "utils/code_utils.h"

#include "stdio.h"
#include "string.h"

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_detector_presence.h"
#include "em_gpio.h"
#include "app_util.h"

static inline uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}

void printIPv6Addr(const otIp6Address *addr)
{
    if (addr == NULL)
        return;
    char str[256];
    int str_idx;
    memset(str, 0x0, 256);
    str_idx = strlen(str);
    sprintf(&str[str_idx], "DEBUG PRINT IPv6 ->%x:%x:%x:%x:%x:%x:%x:%x\r\n",
            Swap16(addr->mFields.m16[0]), Swap16(addr->mFields.m16[1]),
            Swap16(addr->mFields.m16[2]), Swap16(addr->mFields.m16[3]),
            Swap16(addr->mFields.m16[4]), Swap16(addr->mFields.m16[5]),
            Swap16(addr->mFields.m16[6]), Swap16(addr->mFields.m16[7]));

    //otCliOutputFormat("%s", str);
}


void print_result(acc_detector_presence_result_t result, uint8_t ctr)
{
    if (result.presence_detected)
    {
        //otCliOutputFormat("Motion %d \n", ctr);
    }
    else
    {
        //otCliOutputFormat("No motion  %d \n", ctr);
    }

    //otCliOutputFormat("Presence score: %d, Distance: %d\n", (int)(result.presence_score * 1000.0f), (int)(result.presence_distance * 1000.0f));
}

