/*
 * app_coap.c
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */

#include <app_main.h>
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
#include "app_coap.h"


char resource_name[32];

char *ack = "ack";
char *nack = "nack";

#define PERMISSIONS_URI "permissions"
otCoapResource mResource_PERMISSIONS;
otIp6Address brAddr;
const char mPERMISSIONSUriPath[] = PERMISSIONS_URI;

bool appCoapConnectionEstablished = false;
uint32_t appCoapFailCtr = 0;

void appCoapInit()
{
    GPIO_PinOutSet(IP_LED_PORT, IP_LED_PIN);

    otCoapStart(otGetInstance(),OT_DEFAULT_COAP_PORT);

    mResource_PERMISSIONS.mUriPath = mPERMISSIONSUriPath;
    mResource_PERMISSIONS.mContext = otGetInstance();
    mResource_PERMISSIONS.mHandler = &appCoapPermissionsHandler;

    strncpy((char *)mPERMISSIONSUriPath, PERMISSIONS_URI, sizeof(PERMISSIONS_URI));
    otCoapAddResource(otGetInstance(),&mResource_PERMISSIONS);


    GPIO_PinOutClear(IP_LED_PORT, IP_LED_PIN);
}


void appCoapPermissionsHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    GPIO_PinOutSet(IP_LED_PORT, IP_LED_PIN);
    //printIPv6Addr(&aMessageInfo->mPeerAddr);
    brAddr = aMessageInfo->mPeerAddr;
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);

    responseMessage = otCoapNewMessage((otInstance*) aContext, NULL);
    otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);
    otCoapMessageInitResponse(responseMessage, aMessage,
                              OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                          otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(responseMessage);


    uint16_t offset = otMessageGetOffset(aMessage);
    otMessageRead(aMessage, offset, resource_name, sizeof(resource_name)-1);
    //otCliOutputFormat("Unique resource ID: %s\n", resource_name);

    if (OT_COAP_CODE_GET == messageCode)
    {

        error = otMessageAppend(responseMessage, ack, strlen((const char*) ack));
        otEXPECT(OT_ERROR_NONE == error);
        error = otCoapSendResponse((otInstance*) aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }
    else
    {
        error = otMessageAppend(responseMessage, nack, strlen((const char*) nack));
        otCoapMessageSetCode(responseMessage, OT_COAP_CODE_METHOD_NOT_ALLOWED);
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
    appCoapConnectionEstablished = true;
    GPIO_PinOutClear(IP_LED_PORT, IP_LED_PIN);
}


void appCoapRadarSender(char *buf, bool require_ack)
{
    appCoapCheckConnection();
    GPIO_PinOutSet(IP_LED_PORT, IP_LED_PIN);
    otError error = OT_ERROR_NONE;
    otMessage *message = NULL;
    otMessageInfo messageInfo;
    uint16_t payloadLength = 0;

    // Default parameters
    otCoapType coapType = require_ack ? OT_COAP_TYPE_CONFIRMABLE: OT_COAP_TYPE_NON_CONFIRMABLE;
    otIp6Address coapDestinationIp = brAddr;
    message = otCoapNewMessage(otGetInstance(), NULL);

    otCoapMessageInit(message, coapType, OT_COAP_CODE_PUT);
    otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);
    error = otCoapMessageAppendUriPathOptions(message, resource_name);
    otEXPECT(OT_ERROR_NONE == error);

    payloadLength = strlen(buf);

    if (payloadLength > 0)
    {
        error = otCoapMessageSetPayloadMarker(message);
        otEXPECT(OT_ERROR_NONE == error);
    }

    // Embed content into message if given
    if (payloadLength > 0)
    {
        error = otMessageAppend(message, buf, payloadLength);
        otEXPECT(OT_ERROR_NONE == error);
    }

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = coapDestinationIp;
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
    error = otCoapSendRequestWithParameters(otGetInstance(), message,
                                            &messageInfo, NULL, NULL,
                                            NULL);
    otEXPECT(OT_ERROR_NONE == error);

    exit:
    if ((error != OT_ERROR_NONE) && (message != NULL))
    {
        appCoapFailCtr = 0;
        otMessageFree(message);
    }

    if(error != OT_ERROR_NONE) {
        appCoapFailCtr++;
        GPIO_PinOutSet(ERR_LED_PORT, ERR_LED_PIN);
    }
    else GPIO_PinOutClear(ERR_LED_PORT, ERR_LED_PIN);

    //otCliOutputFormat("Sent message: %d\n", error);
    GPIO_PinOutClear(IP_LED_PORT, IP_LED_PIN);
}

void appCoapCheckConnection(void)
{
    if(!appCoapConnectionEstablished) return;

    if(otThreadGetDeviceRole(otGetInstance()) != OT_DEVICE_ROLE_CHILD || appCoapFailCtr > 10)
    {
        otThreadBecomeDetached(otGetInstance());
        //otInstanceErasePersistentInfo();
        sleepyInit();
        setNetworkConfiguration();
        otIp6SetEnabled(otGetInstance(), true);
        otThreadSetEnabled(otGetInstance(), true);
        appCoapInit();
        GPIO_PinOutToggle(IP_LED_PORT, IP_LED_PIN);
        appCoapFailCtr = 0;
    }
}
