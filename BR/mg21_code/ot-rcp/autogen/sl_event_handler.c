#include "sl_event_handler.h"

#include "em_chip.h"
#include "sl_device_init_nvic.h"
#include "sl_device_init_hfrco.h"
#include "sl_device_init_hfxo.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_emu.h"
#include "pa_conversions_efr32.h"
#include "sl_rail_util_pti.h"
#include "sl_rail_util_rf_path.h"
#include "sl_rail_util_rssi.h"
#include "platform-efr32.h"
#include "sl_sleeptimer.h"
#include "gpiointerrupt.h"
#include "sl_mbedtls.h"
#include "sl_mpu.h"
#include "nvm3_default.h"
#include "sl_ot_init.h"
#include "sl_uartdrv_instances.h"
#include "sli_protocol_crypto.h"

void sl_platform_init(void)
{
  CHIP_Init();
  sl_device_init_nvic();
  sl_device_init_hfrco();
  sl_device_init_hfxo();
  sl_device_init_clocks();
  sl_device_init_emu();
  nvm3_initDefault();
}

void sl_driver_init(void)
{
  GPIOINT_Init();
  sl_uartdrv_init_instances();
}

void sl_service_init(void)
{
  sl_sleeptimer_init();
  sl_mbedtls_init();
  sl_mpu_disable_execute_from_ram();
  sli_aes_seed_mask();
}

void sl_stack_init(void)
{
  sl_rail_util_pa_init();
  sl_rail_util_pti_init();
  sl_rail_util_rf_path_init();
  sl_rail_util_rssi_init();
  sl_ot_sys_init();
}

void sl_internal_app_init(void)
{
  sl_ot_init();
}

void sl_platform_process_action(void)
{
}

void sl_service_process_action(void)
{
}

void sl_stack_process_action(void)
{
}

void sl_internal_app_process_action(void)
{
}

