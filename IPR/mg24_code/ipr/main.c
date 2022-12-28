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
#include "em_prs.h"
#include "em_iadc.h"
#include "em_system.h"
#include "sl_power_manager.h"
#include "sl_system_process_action.h"
#include "sl_i2cspm.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_version.h"
#include <stdio.h>
#include <string.h>
#include "acc_detector_presence.h"
#include "app_util.h"
#include "app_coap.h"
#include "app_main.h"
#include "opt3001.h"

/* Radar configuration params */
#define DEFAULT_START_M             (0.2f)
#define DEFAULT_LENGTH_M            (1.4f)
#define DEFAULT_UPDATE_RATE         (1)
#define DEFAULT_POWER_SAVE_MODE     ACC_POWER_SAVE_MODE_OFF
#define DEFAULT_DETECTION_THRESHOLD (2.0f)
#define DEFAULT_NBR_REMOVED_PC      (0)
#define DEFAULT_SERVICE_PROFILE     (4)

char tx_buffer[255];
volatile uint32_t vdd_meas;

static void update_configuration(acc_detector_presence_configuration_t presence_configuration);
acc_detector_presence_handle_t handle = NULL;
acc_detector_presence_result_t result;

#define ALIVE_SLEEPTIMER_INTERVAL_MS 60000
sl_sleeptimer_timer_handle_t alive_timer;

#define RADAR_APP_DEFAULT_TH               10
#define RADAR_APP_DEFAULT_FRAME_SPACING_MS 3000

volatile struct
{
    uint8_t threshold;
    uint32_t frameSpacingMs;
    uint8_t detectConf;
    uint32_t prev; //unused
    bool hystTrigFlag;
    bool clearToMeasure;
    float dx;
} radarAppVars;

volatile bool radarCoapSendActive = false;
volatile bool radarCoapSendInactive = false;
volatile bool radarCoapRequireInactivation = false;
volatile bool appCoapSendAlive = false;
volatile uint32_t appCoapSendTxCtr = 0;

static void alive_cb(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    appCoapSendAlive = true;
}

void IADC_IRQHandler(void){
  static volatile IADC_Result_t sample;
  sample = IADC_pullSingleFifoResult(IADC0);
  vdd_meas = (sample.data * 1200)/1000;
  IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);
}

void BURTC_IRQHandler(void)
{
    BURTC_IntClear(BURTC_IF_COMP); // compare match
    if (result.presence_detected && radarAppVars.detectConf == 10) { radarAppVars.dx = (radarAppVars.dx + 0) / 2.0;}
    else if (result.presence_detected && radarAppVars.detectConf < 10)
    {
        if (radarAppVars.detectConf >= 8 && radarAppVars.dx > 0)
        {
            radarCoapSendActive = true;
            radarAppVars.hystTrigFlag = true;
        }
        radarAppVars.detectConf+=2;
        BURTC_CounterReset();
        uint32_t delay = radarAppVars.frameSpacingMs / radarAppVars.detectConf;
        radarAppVars.dx = (radarAppVars.dx + delay) / 2.0;
        BURTC_CompareSet(0, delay > 500 ? delay : 500);
    }
    else
    {
        if (radarAppVars.detectConf > 1)
        {
            if(radarAppVars.detectConf == 2 && radarAppVars.dx < 0) {
                if(radarCoapRequireInactivation) radarCoapSendInactive = true;
            }
            radarAppVars.detectConf--;
            BURTC_CounterReset();
            uint32_t delay = radarAppVars.frameSpacingMs / radarAppVars.detectConf;
            radarAppVars.dx = (radarAppVars.dx - delay) / 2.0;
            BURTC_CompareSet(0, delay > 500 ? delay : 500);
        }

        if (radarAppVars.detectConf == 1)
        {
            radarAppVars.dx = (radarAppVars.dx + 0) / 2.0;
            radarAppVars.hystTrigFlag = false;
        }
    }
    BURTC_IntEnable(BURTC_IEN_COMP);      // compare match
    BURTC_IntClear (BURTC_IntGet ());
    NVIC_EnableIRQ(BURTC_IRQn);
    BURTC_Enable(true);
    radarAppVars.clearToMeasure = true;
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
  BURTC_CompareSet(0, radarAppVars.frameSpacingMs/radarAppVars.detectConf);

  BURTC_IntEnable(BURTC_IEN_COMP);      // compare match
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
}

void initRadar(void)
{

    const acc_hal_t *hal = acc_hal_integration_get_implementation();

    if (!acc_rss_activate(hal))
    {
    }

    acc_detector_presence_configuration_t presence_configuration =
            acc_detector_presence_configuration_create();
    if (presence_configuration == NULL)
    {
        acc_rss_deactivate();
    }

    update_configuration(presence_configuration);

    handle = acc_detector_presence_create(presence_configuration);
    if (handle == NULL)
    {
        acc_detector_presence_configuration_destroy(&presence_configuration);
        acc_rss_deactivate();
    }

    acc_detector_presence_configuration_destroy(&presence_configuration);

    if (!acc_detector_presence_activate(handle))
    {
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


void initVddMonitor(void)
{
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;

  CMU_ClockEnable (cmuClock_PRS, true);
  PRS_SourceAsyncSignalSet (0,
  PRS_ASYNC_CH_CTRL_SOURCESEL_MODEM,
                            PRS_MODEMH_PRESENT);
  PRS_ConnectConsumer (0, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
  CMU_ClockEnable (cmuClock_IADC0, true);
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1200;
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;

  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale (
      IADC0, 1000000, 0, iadcCfgModeNormal, init.srcClkPrescale);
  initSingle.triggerSelect = iadcTriggerSelPrs0PosEdge;
  initSingle.dataValidLevel = iadcFifoCfgDvl4;
  initSingle.start = true;
  singleInput.posInput = iadcPosInputAvdd;
  singleInput.negInput = iadcNegInputGnd;
  IADC_init (IADC0, &init, &initAllConfigs);
  IADC_initSingle (IADC0, &initSingle, &singleInput);
  IADC_clearInt (IADC0, _IADC_IF_MASK);
  IADC_enableInt (IADC0, IADC_IEN_SINGLEDONE);
  NVIC_ClearPendingIRQ (IADC_IRQn);
  NVIC_SetPriority(GPIO_ODD_IRQn, 7);
  NVIC_EnableIRQ (IADC_IRQn);
}

/* Application logic to take measurements and send coap packets */
void radarAppAlgo(void)
{
    if (radarAppVars.clearToMeasure)
    {

        GPIO_PinOutSet(ACT_LED_PORT, ACT_LED_PIN);
        acc_detector_presence_get_next(handle, &result);
        GPIO_PinOutClear(ACT_LED_PORT, ACT_LED_PIN);

        //print_result(result, radar_trig.ctr);
        radarAppVars.clearToMeasure = false;
        if(!remote_res_fix) GPIO_PinOutToggle(IP_LED_PORT, IP_LED_PIN);
    }

    /* Trigger condition logic in BURTC handler */
    if (remote_res_fix && ((radarCoapSendActive && !radarCoapRequireInactivation) || radarCoapSendInactive))
    {
        float opt_buf = opt3001_conv(opt3001_read());
        memset(tx_buffer, 0, 254);
        int8_t rssi;
        otThreadGetParentLastRssi(otGetInstance(), &rssi);
        snprintf(tx_buffer, 254, "%d,%lu,%lu,%lu,%lu,%d,%lu",
                 (uint8_t) !radarCoapSendInactive,
                 (uint32_t) (result.presence_score * 1000.0f),
                 (uint32_t) (result.presence_distance * 1000.0f),
                 (uint32_t) opt_buf, vdd_meas,
                 rssi,
                 ++appCoapSendTxCtr);
        if (radarCoapSendInactive)
        {
            radarCoapRequireInactivation = false;
            radarCoapSendActive = false;
            radarCoapSendInactive = false;
            appCoapRadarSender(tx_buffer, true);
        }
        else
        {
            radarCoapRequireInactivation = true;
            radarCoapSendActive = false;
            appCoapRadarSender(tx_buffer, true);
        }
    }
    else if(remote_res_fix && appCoapSendAlive) // Specifically ELSE to give alive packet lower priority and to prevent successive tx
    {
        appCoapSendAlive = false;
        float opt_buf = opt3001_conv(opt3001_read());
        memset(tx_buffer, 0, 254);
        int8_t rssi;
        otThreadGetParentLastRssi(otGetInstance(), &rssi);
        snprintf(tx_buffer, 254, "%d,%lu,%lu,%lu,%lu,%d,%lu", -1,
                 (uint32_t) (result.presence_score * 1000.0f),
                 (uint32_t) (result.presence_distance * 1000.0f),
                 (uint32_t) opt_buf, vdd_meas,
                 rssi,
                 ++appCoapSendTxCtr);
        appCoapRadarSender(tx_buffer, false);
    }
}


int main(void) {
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();
    initGPIO();

    opt3001_init();

    /* Default radar measurement conditions */
    radarAppVars.threshold = RADAR_APP_DEFAULT_TH;
    radarAppVars.frameSpacingMs = RADAR_APP_DEFAULT_FRAME_SPACING_MS;
    radarAppVars.detectConf = 1;
    radarAppVars.prev = sl_sleeptimer_get_tick_count();
    radarAppVars.hystTrigFlag = false;
    radarAppVars.clearToMeasure = false;
    radarAppVars.dx = 1;

    initBURTC();
    app_init();
    initRadar();

    GPIO_PinOutSet(IP_LED_PORT, IP_LED_PIN);
    initVddMonitor();
    sl_sleeptimer_start_periodic_timer_ms(&alive_timer, ALIVE_SLEEPTIMER_INTERVAL_MS, alive_cb, NULL, 0, 0);
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
    acc_detector_presence_configuration_hw_accelerated_average_samples_set(presence_configuration,32);
}
