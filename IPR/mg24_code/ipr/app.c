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
#include "app.h"

#include "sl_component_catalog.h"

#include <openthread/coap.h>
#include "utils/code_utils.h"

#include "stdio.h"
#include "string.h"

void sleepyInit(void);
void setNetworkConfiguration(void);
void initUdp(void);
void applicationTick(void);

char str_buf[64];
char val[2] = {1, 27};
#define PERMISSIONS_URI "permissions"
otCoapResource mResource_PERMISSIONS;
otIp6Address brAddr;
const char mPERMISSIONSUriPath[] = PERMISSIONS_URI;
bool done = false;
inline uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}

void printIPv6Addr(const otIp6Address *addr)
{
    if (addr == NULL)
        return;
    char str[256];
    int str_idx;
    str_idx = 0;
    memset(str, 0x0, 256);
    str_idx = strlen(str);

    str_idx = strlen(str);
    sprintf(&str[str_idx], "DEBUG PRINT IPv6 ->%x:%x:%x:%x:%x:%x:%x:%x\r\n",
            Swap16(addr->mFields.m16[0]), Swap16(addr->mFields.m16[1]),
            Swap16(addr->mFields.m16[2]), Swap16(addr->mFields.m16[3]),
            Swap16(addr->mFields.m16[4]), Swap16(addr->mFields.m16[5]),
            Swap16(addr->mFields.m16[6]), Swap16(addr->mFields.m16[7]));

    otCliOutputFormat("%s", str);
}


void permissions_coapHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    printIPv6Addr(&aMessageInfo->mPeerAddr);
    brAddr = aMessageInfo->mPeerAddr;
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);
    otCoapType messageType = otCoapMessageGetType(aMessage);

    responseMessage = otCoapNewMessage((otInstance*) aContext, NULL);
    otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
    otCoapMessageInitResponse(responseMessage, aMessage,
                              OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                          otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(responseMessage);

    if (OT_COAP_CODE_GET == messageCode)
    {
        if (!done)
        {
            error = otMessageAppend(responseMessage, val, strlen((const char*) val));
            done = true;
        }
        otEXPECT(OT_ERROR_NONE == error);
        error = otCoapSendResponse((otInstance*) aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }

    exit:
    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
    done = true;
}


/**
 * This function initializes the CLI app.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
extern void otAppCliInit(otInstance *aInstance);

static otInstance* sInstance = NULL;

otInstance *otGetInstance(void)
{
    return sInstance;
}

void radar_coapSender(char *buf)
{
    otError error = OT_ERROR_NONE;
    otMessage *message = NULL;
    otMessageInfo messageInfo;
    uint16_t payloadLength = 0;

    // Default parameters
    char coapUri[32] = "radar";
    otCoapType coapType = OT_COAP_TYPE_CONFIRMABLE;
    otIp6Address coapDestinationIp = brAddr;
    message = otCoapNewMessage(otGetInstance(), NULL);

    otCoapMessageInit(message, coapType, OT_COAP_CODE_PUT);
    otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);
    error = otCoapMessageAppendUriPathOptions(message, coapUri);

    payloadLength = strlen(buf);

    if (payloadLength > 0)
    {
        error = otCoapMessageSetPayloadMarker(message);
    }

    // Embed content into message if given
    if (payloadLength > 0)
    {
        error = otMessageAppend(message, buf, payloadLength);
    }

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = coapDestinationIp;
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
    error = otCoapSendRequestWithParameters(otGetInstance(), message,
                                            &messageInfo, NULL, NULL,
                                            NULL);

    if ((error != OT_ERROR_NONE) && (message != NULL))
    {
        otMessageFree(message);
    }
    otCliOutputFormat("Sent message: %d\n", error);

}

/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif

void sl_ot_create_instance(void)
{
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    size_t   otInstanceBufferLength = 0;
    uint8_t *otInstanceBuffer       = NULL;

    // Call to query the buffer size
    (void)otInstanceInit(NULL, &otInstanceBufferLength);

    // Call to allocate the buffer
    otInstanceBuffer = (uint8_t *)malloc(otInstanceBufferLength);
    assert(otInstanceBuffer);

    // Initialize OpenThread with the buffer
    sInstance = otInstanceInit(otInstanceBuffer, &otInstanceBufferLength);
#else
    sInstance = otInstanceInitSingle();
#endif
    assert(sInstance);
}

void sl_ot_cli_init(void)
{
    otAppCliInit(sInstance);
}

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/

void app_init(void)
{
    sleepyInit();
    setNetworkConfiguration();
    initUdp();
    assert(otIp6SetEnabled(sInstance, true) == OT_ERROR_NONE);
    assert(otThreadSetEnabled(sInstance, true) == OT_ERROR_NONE);

    otCoapStart(otGetInstance(),OT_DEFAULT_COAP_PORT);

    mResource_PERMISSIONS.mUriPath = mPERMISSIONSUriPath;

    mResource_PERMISSIONS.mContext = otGetInstance();

    mResource_PERMISSIONS.mHandler = &permissions_coapHandler;

    strncpy(mPERMISSIONSUriPath, PERMISSIONS_URI, sizeof(PERMISSIONS_URI));

    otCoapAddResource(otGetInstance(),&mResource_PERMISSIONS);

}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
    otTaskletsProcess(sInstance);
    otSysProcessDrivers(sInstance);
    applicationTick();

}

/**************************************************************************//**
 * Application Exit.
 *****************************************************************************/
void app_exit(void)
{
    otInstanceFinalize(sInstance);
}
