/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>

#include "openthread-system.h"
#include "app_coap.h"
#include "sl_component_catalog.h"

#include <openthread/coap.h>
#include "utils/code_utils.h"

#include "stdio.h"
#include "string.h"
#include "app_main.h"


static otInstance* sInstance = NULL;
#define SLEEPY_POLL_PERIOD_MS 2000


otInstance *otGetInstance(void)
{
    return sInstance;
}


void sl_ot_create_instance(void)
{

    sInstance = otInstanceInitSingle();
    assert(sInstance);
}

void sl_ot_cli_init(void)
{
    //otAppCliInit(sInstance);
}

void setNetworkConfiguration(void)
{
    otPlatRadioSetTransmitPower(otGetInstance(), 5);

    static char          aNetworkName[] = "OpenThread X-1";
    otError              error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    //aDataset.mActiveTimestamp.mSeconds             = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    aDataset.mChannel                      = 15;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId                      = (otPanId)0xDEAD;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC0, 0xDE, 0x1A, 0xB5, 0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set network key to 1234C0DE1AB51234C0DE1AB51234C0DE */
    uint8_t key[OT_NETWORK_KEY_SIZE] = {0x12, 0x34, 0xC0, 0xDE, 0x1A, 0xB5, 0x12, 0x34,
                                        0xC0, 0xDE, 0x1A, 0xB5, 0x12, 0x34, 0xC0, 0xDE};
    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    /* Set Network Name */
    size_t length = strlen(aNetworkName);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;
    /* Set the Active Operational Dataset to this dataset */
    error = otDatasetSetActive(otGetInstance(), &aDataset);

    if (error != OT_ERROR_NONE)
    {
        //otCliOutputFormat("otDatasetSetActive failed with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }
}

void sleepyInit(void)
{
    otError error;

    otLinkModeConfig config;
    error = otLinkSetPollPeriod(otGetInstance(), SLEEPY_POLL_PERIOD_MS);

    config.mRxOnWhenIdle = false;
    config.mDeviceType   = 0;
    config.mNetworkData  = 0;
    error = otThreadSetLinkMode(otGetInstance(), config);

    if (error != OT_ERROR_NONE)
    {
        //otCliOutputFormat("Initialization failed with: %d, %s\r\n", error, otThreadErrorToString(error));
    }

}


/**************************************************************************//**
 * Application Init.
 *****************************************************************************/

void app_init(void)
{
    sleepyInit();
    setNetworkConfiguration();
    assert(otIp6SetEnabled(sInstance, true) == OT_ERROR_NONE);
    assert(otThreadSetEnabled(sInstance, true) == OT_ERROR_NONE);
    appCoapInit();

}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
    otTaskletsProcess(sInstance);
    otSysProcessDrivers(sInstance);
}

bool efr32AllowSleepCallback(void)
{
    return true;
}


/**************************************************************************//**
 * Application Exit.
 *****************************************************************************/
void app_exit(void)
{
    otInstanceFinalize(sInstance);
}
