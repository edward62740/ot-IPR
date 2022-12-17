################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gecko_sdk_4.1.3/util/third_party/segger/systemview/SEGGER/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/third_party/tcplp/lib/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/third_party/tcplp/bsdtcp/cc/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/third_party/tcplp/bsdtcp/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/utils/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/thread/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/radio/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/net/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/meshcop/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/mac/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/diags/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/crypto/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/coprocessor/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/common/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/coap/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/border_router/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/backbone_router/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/src/core/api/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/openthread/examples/platforms/utils/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/crypto/sl_component/sl_psa_driver/src/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/crypto/sl_component/sl_protocol_crypto/src/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/crypto/sl_component/sl_mbedtls_support/src/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/crypto/sl_component/se_manager/src/subdir.mk
-include gecko_sdk_4.1.3/util/third_party/crypto/mbedtls/library/subdir.mk
-include gecko_sdk_4.1.3/util/silicon_labs/silabs_core/memory_manager/subdir.mk
-include gecko_sdk_4.1.3/protocol/openthread/platform-abstraction/efr32/subdir.mk
-include gecko_sdk_4.1.3/platform/service/udelay/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/system/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/sleeptimer/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/power_manager/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/mpu/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/hfxo_manager/src/subdir.mk
-include gecko_sdk_4.1.3/platform/service/device_init/src/subdir.mk
-include gecko_sdk_4.1.3/platform/radio/rail_lib/plugin/rail_util_rssi/subdir.mk
-include gecko_sdk_4.1.3/platform/radio/rail_lib/plugin/rail_util_pti/subdir.mk
-include gecko_sdk_4.1.3/platform/radio/rail_lib/plugin/pa-conversions/subdir.mk
-include gecko_sdk_4.1.3/platform/peripheral/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emlib/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/ustimer/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/uartdrv/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/spidrv/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/nvm3/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/gpiointerrupt/src/subdir.mk
-include gecko_sdk_4.1.3/platform/emdrv/dmadrv/src/subdir.mk
-include gecko_sdk_4.1.3/platform/driver/i2cspm/src/subdir.mk
-include gecko_sdk_4.1.3/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_4.1.3/platform/common/src/subdir.mk
-include gecko_sdk_4.1.3/platform/Device/SiliconLabs/EFR32MG24/Source/subdir.mk
-include autogen/subdir.mk
-include A111/integration/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := ipr
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: ipr.axf

# Tool invocations
ipr.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C++ Linker'
	arm-none-eabi-g++ '@ipr.axf.rsp' -o ipr.axf 
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: ipr.hex'
	arm-none-eabi-objcopy -O ihex "ipr.axf" "ipr.hex"
	@echo ' '

	@echo 'Building bin file: ipr.bin'
	arm-none-eabi-objcopy -O binary "ipr.axf" "ipr.bin"
	@echo ' '

	@echo 'Building s37 file: ipr.s37'
	arm-none-eabi-objcopy -O srec "ipr.axf" "ipr.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "ipr.axf" -A
	@echo ' '

# Other Targets
clean:
	-@$(foreach FILE, $(CC_DEPS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C++_DEPS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(EXECUTABLES), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(OBJS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_UPPER_DEPS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(CXX_DEPS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(C_DEPS), $(shell $(RM) $(FILE)) )
	-@$(foreach FILE, $(CPP_DEPS), $(shell $(RM) $(FILE)) )
	-$(RM) ipr.axf
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
