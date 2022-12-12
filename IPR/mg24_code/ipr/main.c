/***************************************************************************//**
 * @file
 * @brief main() function.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <app_main.h>
#include "sl_component_catalog.h"
#include "sl_system_init.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "em_burtc.h"
#include "sl_power_manager.h"
#include "sl_system_process_action.h"

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_version.h"
#include <stdio.h>
#include "acc_detector_presence.h"
#include "app_util.h"
#include "app_coap.h"
#include "opt3001.h"

/* Radar configuration params */
#define DEFAULT_START_M             (0.2f)
#define DEFAULT_LENGTH_M            (1.6f)
#define DEFAULT_UPDATE_RATE         (1)
#define DEFAULT_POWER_SAVE_MODE     ACC_POWER_SAVE_MODE_OFF
#define DEFAULT_DETECTION_THRESHOLD (2.0f)
#define DEFAULT_NBR_REMOVED_PC      (0)
#define DEFAULT_SERVICE_PROFILE     (5)


static void update_configuration(acc_detector_presence_configuration_t presence_configuration);
acc_detector_presence_handle_t handle = NULL;
acc_detector_presence_result_t result;


struct
{
    uint8_t th;
    uint32_t delay_ms;
    uint8_t ctr;
    uint32_t prev;
    bool active;
    bool meas;
    float dx;
} radar_trig;
bool coap_notify_act_flag = false;
bool coap_notify_noact_flag = false;
bool coap_sent = false;

void BURTC_IRQHandler(void)
{
    BURTC_IntClear(BURTC_IF_COMP); // compare match
    if (result.presence_detected && radar_trig.ctr == 10) { radar_trig.dx = (radar_trig.dx + 0) / 2.0;}
    else if (result.presence_detected && radar_trig.ctr < 10)
    {
        if (radar_trig.ctr >= 8 && radar_trig.dx > 0)
        {
            coap_notify_act_flag = true;
            radar_trig.active = true;
        }
        radar_trig.ctr+=2;
        BURTC_CounterReset();
        uint32_t delay = radar_trig.delay_ms / radar_trig.ctr;
        radar_trig.dx = (radar_trig.dx + delay) / 2.0;
        BURTC_CompareSet(0, delay > 500 ? delay : 500);
        otCliOutputFormat("Interval %d\n", delay);
    }
    else
    {
        if (radar_trig.ctr > 1)
        {
            if(radar_trig.ctr == 2 && radar_trig.dx < 0) {
                if(coap_sent) coap_notify_noact_flag = true;
            }
            radar_trig.ctr--;
            BURTC_CounterReset();
            uint32_t delay = radar_trig.delay_ms / radar_trig.ctr;
            radar_trig.dx = (radar_trig.dx - delay) / 2.0;
            BURTC_CompareSet(0, delay > 500 ? delay : 500);
            otCliOutputFormat("Interval %d\n", delay);
        }

        if (radar_trig.ctr == 1)
        {
            radar_trig.dx = (radar_trig.dx + 0) / 2.0;
            radar_trig.active = false;
        }
    }
    otCliOutputFormat("**** DELTA X: %d ****\n ", (int)radar_trig.dx);
    BURTC_IntEnable(BURTC_IEN_COMP);      // compare match
    BURTC_IntClear (BURTC_IntGet ());
    NVIC_EnableIRQ(BURTC_IRQn);
    BURTC_Enable(true);
    radar_trig.meas = true;
}


void initBURTC(void)
{
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = true; // reset counter when counter reaches compare value
  burtcInit.em4comp = true;     // BURTC compare interrupt wakes from EM4 (causes reset)
  BURTC_Init(&burtcInit);

  BURTC_CounterReset();
  BURTC_CompareSet(0, radar_trig.delay_ms/radar_trig.ctr);

  BURTC_IntEnable(BURTC_IEN_COMP);      // compare match
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
}

void initRadar(void)
{
    otCliOutputFormat("Acconeer software version %s\n", acc_version_get());

    const acc_hal_t *hal = acc_hal_integration_get_implementation();

    if (!acc_rss_activate(hal))
    {
        otCliOutputFormat("Failed to activate RSS\n");
    }

    acc_detector_presence_configuration_t presence_configuration =
            acc_detector_presence_configuration_create();
    if (presence_configuration == NULL)
    {
        otCliOutputFormat("Failed to create configuration\n");
        acc_rss_deactivate();
    }

    update_configuration(presence_configuration);

    handle = acc_detector_presence_create(presence_configuration);
    if (handle == NULL)
    {
        otCliOutputFormat("Failed to create detector\n");
        acc_detector_presence_configuration_destroy(&presence_configuration);
        acc_rss_deactivate();
    }

    acc_detector_presence_configuration_destroy(&presence_configuration);

    if (!acc_detector_presence_activate(handle))
    {
        otCliOutputFormat("Failed to activate detector\n");
        acc_detector_presence_destroy(&handle);
        acc_rss_deactivate();
    }
}


void initGPIO(void) {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(A111_CS_PORT, A111_CS_PIN, gpioModePushPull, 1);

    GPIO_PinModeSet(A111_INT_PORT, A111_INT_PIN, gpioModeInputPull, 0);
    GPIO_ExtIntConfig(A111_INT_PORT,
                      A111_INT_PIN,
                      A111_INT_PIN,
                        true,
                        false,
                        true);
    GPIO_PinModeSet(A111_EN_PORT, A111_EN_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet(IP_LED_PORT, IP_LED_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(ACT_LED_PORT, ACT_LED_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(ERR_LED_PORT, ERR_LED_PIN, gpioModePushPull, 0);
}


void radarAppAlgo(void)
{
    if (radar_trig.meas)
    {
        //float tmp;
        //tmp = opt3001_conv(opt3001_read());
        //otCliOutputFormat("Output sensor: %d\n", (int)tmp);
        bool success = true;
        GPIO_PinOutSet(ACT_LED_PORT, ACT_LED_PIN);
        success = acc_detector_presence_get_next(handle, &result);
        GPIO_PinOutClear(ACT_LED_PORT, ACT_LED_PIN);
        if (!success)
        {
            otCliOutputFormat("acc_detector_presence_get_next() failed\n");
        }
        print_result(result, radar_trig.ctr);
        radar_trig.meas = false;
    }
    if (remote_res_fix && ((coap_notify_act_flag && !coap_sent) | coap_notify_noact_flag))
    {
        if (coap_notify_noact_flag)
        {
            coap_sent = false;
            coap_notify_act_flag = false;
            coap_notify_noact_flag = false;
            appCoapRadarSender("CLEAR");
        }
        else
        {
            coap_sent = true;
            coap_notify_act_flag = false;
            appCoapRadarSender("TRIG");
        }
    }
}

int main(void) {
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();
    initGPIO();
    opt3001_init();
    otCliOutputFormat("READ REG: %x", opt3001_read_reg(0));
    otCliOutputFormat("READ REG: %x", opt3001_read_reg(1));
    otCliOutputFormat("READ REG: %x", opt3001_read_reg(2));

    /* Default radar measurement conditions */
    radar_trig.th = 10;
    radar_trig.delay_ms = 2500;
    radar_trig.ctr = 1;
    radar_trig.prev = sl_sleeptimer_get_tick_count();
    radar_trig.active = false;
    radar_trig.meas = false;
    radar_trig.dx = 1;

    initBURTC();
    app_init();
    initRadar();
    while (1) {
        // Do not remove this call: Silicon Labs components process action routine
        // must be called from the super loop.
        sl_system_process_action();

        app_process_action();
        radarAppAlgo();

        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();

    }

}




void update_configuration(acc_detector_presence_configuration_t presence_configuration)
{
    acc_detector_presence_configuration_update_rate_set(presence_configuration, DEFAULT_UPDATE_RATE);
    acc_detector_presence_configuration_detection_threshold_set(presence_configuration, DEFAULT_DETECTION_THRESHOLD);
    acc_detector_presence_configuration_start_set(presence_configuration, DEFAULT_START_M);
    acc_detector_presence_configuration_length_set(presence_configuration, DEFAULT_LENGTH_M);
    acc_detector_presence_configuration_power_save_mode_set(presence_configuration, DEFAULT_POWER_SAVE_MODE);
    acc_detector_presence_configuration_nbr_removed_pc_set(presence_configuration, DEFAULT_NBR_REMOVED_PC);
    acc_detector_presence_configuration_service_profile_set(presence_configuration, DEFAULT_SERVICE_PROFILE);
}
