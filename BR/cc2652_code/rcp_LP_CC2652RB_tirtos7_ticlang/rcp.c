/******************************************************************************

 @file rcp.c

 @brief Implements the main thread of the rcp example application.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread public API Header files */
#include <openthread/ncp.h>
#include <openthread/instance.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Example/Board Header files */
#include "task_config.h"
#include "ti_drivers_config.h"

/**
 * This function initializes the RCP app.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
extern void otAppNcpInit(otInstance *aInstance);

/* Application thread */
void *rcp_task(void *arg0);

/* Application thread call stack */
static char rcp_stack[TASK_CONFIG_RCP_TASK_STACK_SIZE];

/**
 * Create the task for the rcp application.
 */
void rcp_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_RCP_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)rcp_stack,
                                 TASK_CONFIG_RCP_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, rcp_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void) retc;

    GPIO_setConfig(CONFIG_GPIO_BTN1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_BTN2, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_RLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
}

/**
 * Main thread starting the RCP example within OpenThread.
 */
void *rcp_task(void *arg0)
{
    GPIO_write(CONFIG_GPIO_RLED, 1);

    OtStack_taskCreate();

    OtRtosApi_lock();
    otAppNcpInit(OtInstance_get());
    OtRtosApi_unlock();

    while (1)
    {
        usleep(200000);
        /* ignoring unslept return value */
        GPIO_toggle(CONFIG_GPIO_RLED);
    }
}
