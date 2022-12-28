/*
 * app_coap.h
 *
 *  Created on: Dec 12, 2022
 *      Author: edward62740
 */

#ifndef APP_COAP_H_
#define APP_COAP_H_

extern bool remote_res_fix;

void appCoapInit();
void appCoapPermissionsHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
void appCoapRadarSender(char *buf, bool require_ack);

#endif /* APP_COAP_H_ */
