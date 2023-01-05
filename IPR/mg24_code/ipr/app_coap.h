/*
 * app_coap.h
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */

#ifndef APP_COAP_H_
#define APP_COAP_H_

extern bool appCoapConnectionEstablished;

void appCoapInit();
void appCoapPermissionsHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
void appCoapRadarSender(char *buf, bool require_ack);
void appCoapCheckConnection(void);

#endif /* APP_COAP_H_ */
