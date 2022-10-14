################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./src/core/api/diags_api.o" \
"./src/core/api/error_api.o" \
"./src/core/api/instance_api.o" \
"./src/core/api/link_raw_api.o" \
"./src/core/api/logging_api.o" \
"./src/core/api/random_noncrypto_api.o" \
"./src/core/api/tasklet_api.o" \
"./src/core/common/instance.o" \
"./src/core/common/log.o" \
"./src/core/common/random.o" \
"./src/core/common/string.o" \
"./src/core/common/tasklet.o" \
"./src/core/common/timer.o" \
"./src/core/common/uptime.o" \
"./src/core/crypto/aes_ccm.o" \
"./src/core/crypto/aes_ecb.o" \
"./src/core/crypto/crypto_platform.o" \
"./src/core/crypto/storage.o" \
"./src/core/diags/factory_diags.o" \
"./src/core/mac/link_raw.o" \
"./src/core/mac/mac_frame.o" \
"./src/core/mac/mac_types.o" \
"./src/core/mac/sub_mac.o" \
"./src/core/mac/sub_mac_callbacks.o" \
"./src/core/radio/radio.o" \
"./src/core/radio/radio_callbacks.o" \
"./src/core/radio/radio_platform.o" \
"./src/core/thread/link_quality.o" \
"./src/core/utils/parse_cmdline.o" \
"./src/lib/platform/exit_code.o" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include src/core/api/subdir_vars.mk
-include src/core/common/subdir_vars.mk
-include src/core/crypto/subdir_vars.mk
-include src/core/diags/subdir_vars.mk
-include src/core/mac/subdir_vars.mk
-include src/core/radio/subdir_vars.mk
-include src/core/thread/subdir_vars.mk
-include src/core/utils/subdir_vars.mk
-include src/lib/platform/subdir_vars.mk
-include src/core/api/subdir_rules.mk
-include src/core/common/subdir_rules.mk
-include src/core/crypto/subdir_rules.mk
-include src/core/diags/subdir_rules.mk
-include src/core/mac/subdir_rules.mk
-include src/core/radio/subdir_rules.mk
-include src/core/thread/subdir_rules.mk
-include src/core/utils/subdir_rules.mk
-include src/lib/platform/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LIB_OUTPUTS += \
libopenthread_radio.a \

LIB_OUTPUTS__QUOTED += \
"libopenthread_radio.a" \


# All Target
all: libopenthread_radio.a

# Tool invocations
libopenthread_radio.a: $(OBJS)
ifneq ($(strip $(ORDERED_OBJS)),)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Archiver'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS/bin/tiarmar.exe" rc "libopenthread_radio.a" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
endif

# Other Targets
clean:
	-$(RM) $(LIB_OUTPUTS__QUOTED)
	-$(RM) "src\core\api\diags_api.o" "src\core\api\error_api.o" "src\core\api\instance_api.o" "src\core\api\link_raw_api.o" "src\core\api\logging_api.o" "src\core\api\random_noncrypto_api.o" "src\core\api\tasklet_api.o" "src\core\common\instance.o" "src\core\common\log.o" "src\core\common\random.o" "src\core\common\string.o" "src\core\common\tasklet.o" "src\core\common\timer.o" "src\core\common\uptime.o" "src\core\crypto\aes_ccm.o" "src\core\crypto\aes_ecb.o" "src\core\crypto\crypto_platform.o" "src\core\crypto\storage.o" "src\core\diags\factory_diags.o" "src\core\mac\link_raw.o" "src\core\mac\mac_frame.o" "src\core\mac\mac_types.o" "src\core\mac\sub_mac.o" "src\core\mac\sub_mac_callbacks.o" "src\core\radio\radio.o" "src\core\radio\radio_callbacks.o" "src\core\radio\radio_platform.o" "src\core\thread\link_quality.o" "src\core\utils\parse_cmdline.o" "src\lib\platform\exit_code.o" 
	-$(RM) "src\lib\platform\exit_code.d" 
	-$(RM) "src\core\api\diags_api.d" "src\core\api\error_api.d" "src\core\api\instance_api.d" "src\core\api\link_raw_api.d" "src\core\api\logging_api.d" "src\core\api\random_noncrypto_api.d" "src\core\api\tasklet_api.d" "src\core\common\instance.d" "src\core\common\log.d" "src\core\common\random.d" "src\core\common\string.d" "src\core\common\tasklet.d" "src\core\common\timer.d" "src\core\common\uptime.d" "src\core\crypto\aes_ccm.d" "src\core\crypto\aes_ecb.d" "src\core\crypto\crypto_platform.d" "src\core\crypto\storage.d" "src\core\diags\factory_diags.d" "src\core\mac\link_raw.d" "src\core\mac\mac_frame.d" "src\core\mac\mac_types.d" "src\core\mac\sub_mac.d" "src\core\mac\sub_mac_callbacks.d" "src\core\radio\radio.d" "src\core\radio\radio_callbacks.d" "src\core\radio\radio_platform.d" "src\core\thread\link_quality.d" "src\core\utils\parse_cmdline.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

