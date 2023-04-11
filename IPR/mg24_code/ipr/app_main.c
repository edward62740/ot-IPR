/*
 * app_main.c
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
#include <openthread/srp_client.h>
#include <openthread/srp_client_buffers.h>

#include "openthread-system.h"
#include "app_coap.h"
#include "sl_component_catalog.h"

#include <openthread/coap.h>
#include "utils/code_utils.h"
#include "em_system.h"
#include "stdio.h"
#include "string.h"
#include "app_main.h"

static otInstance* sInstance = NULL;
#define SLEEPY_POLL_PERIOD_MS 5000

static bool srpDone = false;



otInstance *otGetInstance(void)
{
    return sInstance;
}


void sl_ot_create_instance(void)
{

    sInstance = otInstanceInitSingle();
    assert(sInstance);
}


void setNetworkConfiguration(void)
{
    otPlatRadioSetTransmitPower(otGetInstance(), 10);

    static char          aNetworkName[] = "";
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
    aDataset.mChannel                      = ;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId                      = (otPanId);
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to  */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {};
    memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set network key to  */
    uint8_t key[OT_NETWORK_KEY_SIZE] = {};
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

void appSrpInit(void)
{
    if(srpDone) return;
    srpDone = true;

    otError error = OT_ERROR_NONE;

    char *hostName;
    char *HOST_NAME = "OT-IPR-0";
    uint16_t size;
    hostName = otSrpClientBuffersGetHostNameString(sInstance, &size);
    error |= otSrpClientSetHostName(sInstance, HOST_NAME);
    memcpy(hostName, HOST_NAME, sizeof(HOST_NAME) + 1);


    otSrpClientEnableAutoHostAddress(sInstance);


    otSrpClientBuffersServiceEntry *entry = NULL;
    char *string;

    entry = otSrpClientBuffersAllocateService(sInstance);

    entry->mService.mPort = 33434;
    char INST_NAME[32];
    snprintf(INST_NAME, 32, "ipv6bc%d", (uint8_t)(SYSTEM_GetUnique() & 0xFF));
    char *SERV_NAME = "_ot._udp";
    string = otSrpClientBuffersGetServiceEntryInstanceNameString(entry, &size);
    memcpy(string, INST_NAME, size);


    string = otSrpClientBuffersGetServiceEntryServiceNameString(entry, &size);
    memcpy(string, SERV_NAME, size);

    error |= otSrpClientAddService(sInstance, &entry->mService);

    entry = NULL;

    otSrpClientEnableAutoStartMode(sInstance, /* aCallback */ NULL, /* aContext */ NULL);
    if(error != OT_ERROR_NONE) GPIO_PinOutSet(ERR_LED_PORT, ERR_LED_PIN);
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
    appSrpInit();
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
