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
#include "acc_detector_distance.h"
#define DEFAULT_SENSOR_ID 1

static bool run_test(acc_rss_assembly_test_configuration_t configuration);
static void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count);
void initEUSART1(void);
int main(void) {
    // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
    // Note that if the kernel is present, processing task(s) will be created by
    // this call.
    sl_system_init();

    // Initialize the application. For example, create periodic timer(s) or
    // task(s) if the kernel is present.
    app_init();
    initEUSART1();
    otCliOutputFormat("Acconeer software version %s\n", acc_version_get());

        const acc_hal_t *hal = acc_hal_integration_get_implementation();

        if (!acc_rss_activate(hal))
        {
            return EXIT_FAILURE;
        }
        acc_rss_assembly_test_configuration_t configuration = acc_rss_assembly_test_configuration_create();
        acc_rss_assembly_test_configuration_sensor_set(configuration, DEFAULT_SENSOR_ID);

        // Disable all tests (they are enabled by default)
        acc_rss_assembly_test_configuration_all_tests_disable(configuration);

        // Enable and run: Read Test
        acc_rss_assembly_test_configuration_communication_read_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Read Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }
        otCliOutputFormat("Bring up test: Read Test success\n");
        acc_rss_assembly_test_configuration_communication_read_test_disable(configuration);

        // Enable and run: Write Read Test
        acc_rss_assembly_test_configuration_communication_write_read_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Write Read Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }
        otCliOutputFormat("Bring up test: Write Read Test success\n");
        acc_rss_assembly_test_configuration_communication_write_read_test_disable(configuration);
/*
        // Enable and run: Interrupt Test
        acc_rss_assembly_test_configuration_communication_interrupt_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Interrupt Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }
*/
        acc_rss_assembly_test_configuration_communication_interrupt_test_disable(configuration);

        // Enable and run: Hibernate Test
        acc_rss_assembly_test_configuration_communication_hibernate_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Hibernate Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }
        otCliOutputFormat("Bring up test: Hibernate Test success\n");
        acc_rss_assembly_test_configuration_communication_hibernate_test_disable(configuration);
/*
        // Enable and run: Supply Test
        acc_rss_assembly_test_configuration_supply_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Supply Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }

        acc_rss_assembly_test_configuration_supply_test_disable(configuration);

        // Enable and run: Clock Test
        acc_rss_assembly_test_configuration_clock_test_enable(configuration);
        if (!run_test(configuration))
        {
            otCliOutputFormat("Bring up test: Clock Test failed\n");
            acc_rss_assembly_test_configuration_destroy(&configuration);
            acc_rss_deactivate();
            return EXIT_FAILURE;
        }

        acc_rss_assembly_test_configuration_clock_test_disable(configuration);
*/
        otCliOutputFormat("Bring up test: All tests passed\n");

        acc_rss_assembly_test_configuration_destroy(&configuration);
        acc_rss_deactivate();
        printf("Acconeer software version %s\n", acc_version_get());



            if (!acc_rss_activate(hal))
            {
                otCliOutputFormat("acc_rss_activate() failed\n");
                return EXIT_FAILURE;
            }

            acc_detector_distance_configuration_t distance_configuration = acc_detector_distance_configuration_create();

            if (distance_configuration == NULL)
            {
                otCliOutputFormat("acc_detector_distance_configuration_create() failed\n");
                acc_rss_deactivate();
                return EXIT_FAILURE;
            }

            acc_detector_distance_handle_t distance_handle = acc_detector_distance_create(distance_configuration);

            if (distance_handle == NULL)
            {
                otCliOutputFormat("acc_detector_distance_create() failed\n");
                acc_detector_distance_configuration_destroy(&distance_configuration);
                acc_rss_deactivate();
                return EXIT_FAILURE;
            }

            acc_detector_distance_configuration_destroy(&distance_configuration);

            if (!acc_detector_distance_activate(distance_handle))
            {
                otCliOutputFormat("acc_detector_distance_activate() failed\n");
                acc_detector_distance_destroy(&distance_handle);
                acc_rss_deactivate();
                return EXIT_FAILURE;
            }

            bool                                success         = true;
            const int                           iterations      = 5;
            uint16_t                            number_of_peaks = 5;
            acc_detector_distance_result_t      result[number_of_peaks];
            acc_detector_distance_result_info_t result_info;

            for (int i = 0; i < iterations; i++)
            {
                success = acc_detector_distance_get_next(distance_handle, result, number_of_peaks, &result_info);

                if (!success)
                {
                    otCliOutputFormat("acc_detector_distance_get_next() failed\n");
                    break;
                }

                print_distances(result, result_info.number_of_peaks);
            }

            bool deactivated = acc_detector_distance_deactivate(distance_handle);

            acc_detector_distance_destroy(&distance_handle);

            acc_rss_deactivate();

            if (deactivated && success)
            {
                otCliOutputFormat("Application finished OK\n");
                return EXIT_SUCCESS;
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

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();
#endif
    }
    // Clean-up when exiting the application.
    app_exit();
#endif // SL_CATALOG_KERNEL_PRESENT
}

static void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count)
{
    otCliOutputFormat("Found %u peaks:\n", (unsigned int)reflection_count);

    for (uint16_t i = 0; i < reflection_count; i++)
    {
        otCliOutputFormat("Amplitude %u at %u mm\n", (unsigned int)result[i].amplitude,
               (unsigned int)(result[i].distance_m * 1000));
    }
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



void initEUSART1(void) {
	CMU_ClockEnable(cmuClock_EUSART1, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	// SPI advanced configuration (part of the initializer)
	EUSART_SpiAdvancedInit_TypeDef adv = EUSART_SPI_ADVANCED_INIT_DEFAULT;

	adv.msbFirst = true;        // SPI standard MSB first

	// Default asynchronous initializer (main/master mode and 8-bit data)
	EUSART_SpiInit_TypeDef init = EUSART_SPI_MASTER_INIT_DEFAULT_HF;

	init.bitRate = 1000000;        // 1 MHz shift clock
	init.advancedSettings = &adv;  // Advanced settings structure

	/*
	 * Route EUSART1 MOSI, MISO, and SCLK to the specified pins.  CS is
	 * not controlled by EUSART1 so there is no write to the corresponding
	 * EUSARTROUTE register to do this.
	 */
	GPIO->EUSARTROUTE[1].TXROUTE = (A111_MOSI_PORT
			<< _GPIO_EUSART_TXROUTE_PORT_SHIFT)
			| (A111_MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
	GPIO->EUSARTROUTE[1].RXROUTE = (A111_MISO_PORT
			<< _GPIO_EUSART_RXROUTE_PORT_SHIFT)
			| (A111_MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
	GPIO->EUSARTROUTE[1].SCLKROUTE = (A111_SCLK_PORT
			<< _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
			| (A111_SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);

	// Enable EUSART interface pins
	GPIO->EUSARTROUTE[1].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN |    // MISO
			GPIO_EUSART_ROUTEEN_TXPEN |    // MOSI
			GPIO_EUSART_ROUTEEN_SCLKPEN;

	// Configure and enable EUSART1
	EUSART_SpiInit(EUSART1, &init);
	GPIO_PinModeSet(A111_INT_PORT, A111_INT_PIN, gpioModeInputPull, 0);
	GPIO_ExtIntConfig(A111_INT_PORT,
	                  A111_INT_PIN,
	                  A111_INT_PIN,
	                    true,
	                    false,
	                    true);
	GPIO_PinModeSet(A111_EN_PORT, A111_EN_PIN, gpioModeInputPull, 0);
    GPIO_ExtIntConfig(A111_EN_PORT,
                      A111_EN_PIN,
                      A111_EN_PIN,
                        true,
                        false,
                        true);
}
