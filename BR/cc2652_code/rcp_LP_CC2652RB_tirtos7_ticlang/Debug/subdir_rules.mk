################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/Debug" -I"C:/Users/Edward/workspace_v12/libmbedcrypto_cc13x2_26x2_ticlang/config" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/examples/platforms" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/include" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/src/core" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/src" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/third_party/mbedtls/repo/include" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/platform/crypto" -I"C:/Users/Edward/workspace_v12/libopenthread_radio_cc13x2_26x2_ticlang/config" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/ti/devices/cc13x2_cc26x2" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/ti/posix/ticlang" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/kernel/tirtos7/packages" -DHAVE_CONFIG_H -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNVOCMP_POSIX_MUTEX -DNVOCMP_NVPAGES=2 -DNDEBUG -DBOARD_DISPLAY_USE_UART=1 -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' -DOPENTHREAD_RADIO=1 -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-rcp.h"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' -DPLATFORM_SPI_CRC_SUPPORT -DDeviceFamily_CC13X2_CC26X2 -gstrict-dwarf -gdwarf-3 -Wall -Wno-ti-macros -march=armv7e-m -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-475890479: ../rcp.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1200/ccs/utils/sysconfig_1.13.0/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/.metadata/product.json" -s "C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/.metadata/product.json" --script "C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/rcp.syscfg" -o "syscfg" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/tiop_config.h: build-475890479 ../rcp.syscfg
syscfg/tiop_config.c: build-475890479
syscfg/ti_devices_config.c: build-475890479
syscfg/ti_radio_config.c: build-475890479
syscfg/ti_radio_config.h: build-475890479
syscfg/ti_drivers_config.c: build-475890479
syscfg/ti_drivers_config.h: build-475890479
syscfg/ti_utils_build_linker.cmd.genlibs: build-475890479
syscfg/syscfg_c.rov.xs: build-475890479
syscfg/ti_utils_runtime_model.gv: build-475890479
syscfg/ti_utils_runtime_Makefile: build-475890479
syscfg/ti_sysbios_config.h: build-475890479
syscfg/ti_sysbios_config.c: build-475890479
syscfg/: build-475890479

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS/bin/tiarmclang.exe" -c -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/Debug" -I"C:/Users/Edward/workspace_v12/libmbedcrypto_cc13x2_26x2_ticlang/config" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/examples/platforms" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/include" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/src/core" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/src" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/third_party/openthread/third_party/mbedtls/repo/include" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/platform/crypto" -I"C:/Users/Edward/workspace_v12/libopenthread_radio_cc13x2_26x2_ticlang/config" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/ti/devices/cc13x2_cc26x2" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/source/ti/posix/ticlang" -I"C:/ti/simplelink_cc13xx_cc26xx_sdk_6_20_00_29/kernel/tirtos7/packages" -DHAVE_CONFIG_H -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNVOCMP_POSIX_MUTEX -DNVOCMP_NVPAGES=2 -DNDEBUG -DBOARD_DISPLAY_USE_UART=1 -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc13x2_26x2.h"' -DOPENTHREAD_RADIO=1 -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc13x2_26x2-rcp.h"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc13x2_26x2-config.h"' -DPLATFORM_SPI_CRC_SUPPORT -DDeviceFamily_CC13X2_CC26X2 -gstrict-dwarf -gdwarf-3 -Wall -Wno-ti-macros -march=armv7e-m -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/Users/Edward/workspace_v12/rcp_LP_CC2652RB_tirtos7_ticlang/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


