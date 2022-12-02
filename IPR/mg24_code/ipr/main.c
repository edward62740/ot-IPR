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
#include "sl_component_catalog.h"
#include "sl_system_init.h"
#include "app.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else // !SL_CATALOG_KERNEL_PRESENT
#include "sl_system_process_action.h"
#endif // SL_CATALOG_KERNEL_PRESENT

#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_rss_assembly_test.h"
#include "acc_version.h"
#include <stdio.h>
#include "acc_detector_presence.h"
#define DEFAULT_SENSOR_ID 1

static bool run_test(acc_rss_assembly_test_configuration_t configuration);
#define DEFAULT_START_M             (0.2f)
#define DEFAULT_LENGTH_M            (1.4f)
#define DEFAULT_UPDATE_RATE         (1)
#define DEFAULT_POWER_SAVE_MODE     ACC_POWER_SAVE_MODE_SLEEP
#define DEFAULT_DETECTION_THRESHOLD (2.0f)
#define DEFAULT_NBR_REMOVED_PC      (0)


static void update_configuration(acc_detector_presence_configuration_t presence_configuration);


static void print_result(acc_detector_presence_result_t result);



void initGPIO(void);
int main(void) {
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
     sl_system_init();
    initGPIO();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.`
    USTIMER_Init();

    GPIO_PinOutSet(gpioPortC, 8);
    GPIO_PinOutSet(A111_EN_PORT, A111_EN_PIN);
    otCliOutputFormat("Acconeer software version %s\n", acc_version_get());

        const acc_hal_t *hal = acc_hal_integration_get_implementation();

        if (!acc_rss_activate(hal))
        {
            otCliOutputFormat("Failed to activate RSS\n");

        }

        acc_detector_presence_configuration_t presence_configuration = acc_detector_presence_configuration_create();
        if (presence_configuration == NULL)
        {
            otCliOutputFormat("Failed to create configuration\n");
            acc_rss_deactivate();

        }

        update_configuration(presence_configuration);

        acc_detector_presence_handle_t handle = acc_detector_presence_create(presence_configuration);
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

        bool                           success    = true;
        const int                      iterations = 200;
        acc_detector_presence_result_t result;

       while(1)
        {
            success = acc_detector_presence_get_next(handle, &result);
            if (!success)
            {
                otCliOutputFormat("acc_detector_presence_get_next() failed\n");
                break;
            }

            print_result(result);

            acc_integration_sleep_ms(250 / DEFAULT_UPDATE_RATE);
        }

        bool deactivated = acc_detector_presence_deactivate(handle);

        acc_detector_presence_destroy(&handle);

        acc_rss_deactivate();

        if (deactivated && success)
        {
            otCliOutputFormat("Application finished OK\n");
        }



#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Start the kernel. Task(s) created in app_init() will start running.
  sl_system_kernel_start();
#else // SL_CATALOG_KERNEL_PRESENT
    while (1) {
        // Do not remove this call: Silicon Labs components process action routine
        // must be called from the super loop.
        sl_system_process_action();

        // Application process.
        app_process_action();
        GPIO_PinOutToggle(gpioPortC, 7);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();
#endif
    }
    // Clean-up when exiting the application.
   // app_exit();
#endif // SL_CATALOG_KERNEL_PRESENT
}


static bool run_test(acc_rss_assembly_test_configuration_t configuration)
{
    acc_rss_assembly_test_result_t test_results[ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS];
    uint16_t                       nr_of_test_results = ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS;

    bool success = acc_rss_assembly_test(configuration, test_results, &nr_of_test_results);

    if (success)
    {
        for (uint16_t i = 0; i < nr_of_test_results; i++)
        {
            const bool passed = test_results[i].test_passed;
            printf("Name: %s, result: %s\n", test_results[i].test_name, passed ? "Pass" : "Fail");
        }
    }
    else
    {
        printf("Bring up test: Failed to complete\n");
        return success;
    }

    return success;
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
   /* GPIO_ExtIntConfig(A111_EN_PORT,
                      A111_EN_PIN,
                      A111_EN_PIN,
                        true,
                        false,
                        true);*/
    GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);
    GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 0);
    GPIO_PinModeSet(gpioPortC, 7, gpioModePushPull, 0);
}

void update_configuration(acc_detector_presence_configuration_t presence_configuration)
{
    acc_detector_presence_configuration_update_rate_set(presence_configuration, DEFAULT_UPDATE_RATE);
    acc_detector_presence_configuration_detection_threshold_set(presence_configuration, DEFAULT_DETECTION_THRESHOLD);
    acc_detector_presence_configuration_start_set(presence_configuration, DEFAULT_START_M);
    acc_detector_presence_configuration_length_set(presence_configuration, DEFAULT_LENGTH_M);
    acc_detector_presence_configuration_power_save_mode_set(presence_configuration, DEFAULT_POWER_SAVE_MODE);
    acc_detector_presence_configuration_nbr_removed_pc_set(presence_configuration, DEFAULT_NBR_REMOVED_PC);
}


void print_result(acc_detector_presence_result_t result)
{
    if (result.presence_detected)
    {
        otCliOutputFormat("Motion\n");
        GPIO_PinOutSet(gpioPortC, 9);
    }
    else
    {
        otCliOutputFormat("No motion\n");
        GPIO_PinOutClear(gpioPortC, 9);
    }

    otCliOutputFormat("Presence score: %d, Distance: %d\n", (int)(result.presence_score * 1000.0f), (int)(result.presence_distance * 1000.0f));
}
