################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./src/lib/hdlc/hdlc.o" \
"./src/lib/spinel/spinel.o" \
"./src/lib/spinel/spinel_buffer.o" \
"./src/lib/spinel/spinel_decoder.o" \
"./src/lib/spinel/spinel_encoder.o" \
"./src/ncp/changed_props_set.o" \
"./src/ncp/example_vendor_hook.o" \
"./src/ncp/ncp_base.o" \
"./src/ncp/ncp_base_dispatcher.o" \
"./src/ncp/ncp_base_ftd.o" \
"./src/ncp/ncp_base_mtd.o" \
"./src/ncp/ncp_base_radio.o" \
"./src/ncp/ncp_hdlc.o" \
"./src/ncp/ncp_spi.o" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include src/lib/hdlc/subdir_vars.mk
-include src/lib/spinel/subdir_vars.mk
-include src/ncp/subdir_vars.mk
-include src/lib/hdlc/subdir_rules.mk
-include src/lib/spinel/subdir_rules.mk
-include src/ncp/subdir_rules.mk
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
libopenthread_rcp.a \

LIB_OUTPUTS__QUOTED += \
"libopenthread_rcp.a" \


# All Target
all: libopenthread_rcp.a

# Tool invocations
libopenthread_rcp.a: $(OBJS)
ifneq ($(strip $(ORDERED_OBJS)),)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Archiver'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-armllvm_2.1.0.LTS/bin/tiarmar.exe" rc "libopenthread_rcp.a" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
endif

# Other Targets
clean:
	-$(RM) $(LIB_OUTPUTS__QUOTED)
	-$(RM) "src\lib\hdlc\hdlc.o" "src\lib\spinel\spinel.o" "src\lib\spinel\spinel_buffer.o" "src\lib\spinel\spinel_decoder.o" "src\lib\spinel\spinel_encoder.o" "src\ncp\changed_props_set.o" "src\ncp\example_vendor_hook.o" "src\ncp\ncp_base.o" "src\ncp\ncp_base_dispatcher.o" "src\ncp\ncp_base_ftd.o" "src\ncp\ncp_base_mtd.o" "src\ncp\ncp_base_radio.o" "src\ncp\ncp_hdlc.o" "src\ncp\ncp_spi.o" 
	-$(RM) "src\lib\spinel\spinel.d" 
	-$(RM) "src\lib\hdlc\hdlc.d" "src\lib\spinel\spinel_buffer.d" "src\lib\spinel\spinel_decoder.d" "src\lib\spinel\spinel_encoder.d" "src\ncp\changed_props_set.d" "src\ncp\example_vendor_hook.d" "src\ncp\ncp_base.d" "src\ncp\ncp_base_dispatcher.d" "src\ncp\ncp_base_ftd.d" "src\ncp\ncp_base_mtd.d" "src\ncp\ncp_base_radio.d" "src\ncp\ncp_hdlc.d" "src\ncp\ncp_spi.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

