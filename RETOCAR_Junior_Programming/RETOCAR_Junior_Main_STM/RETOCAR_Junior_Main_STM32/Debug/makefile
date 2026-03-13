################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/lib/drivers/hardware_drivers/subdir.mk
-include Core/lib/drivers/communication/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
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


BUILD_ARTIFACT_NAME := RETOCAR_Junior_Main_STM32
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
RETOCAR_Junior_Main_STM32.elf \

MAP_FILES += \
RETOCAR_Junior_Main_STM32.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
RETOCAR_Junior_Main_STM32.list \

OBJCOPY_HEX += \
RETOCAR_Junior_Main_STM32.hex \

OBJCOPY_BIN += \
RETOCAR_Junior_Main_STM32.bin \


# All Target
all: main-build

# Main-build Target
main-build: RETOCAR_Junior_Main_STM32.elf secondary-outputs

# Tool invocations
RETOCAR_Junior_Main_STM32.elf RETOCAR_Junior_Main_STM32.map: $(OBJS) $(USER_OBJS) D:\Project\RETOCAR_Junior\RETOCAR_Junior_Main_STM\RETOCAR_Junior_Main_STM32\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "RETOCAR_Junior_Main_STM32.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\Project\RETOCAR_Junior\RETOCAR_Junior_Main_STM\RETOCAR_Junior_Main_STM32\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="RETOCAR_Junior_Main_STM32.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

RETOCAR_Junior_Main_STM32.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "RETOCAR_Junior_Main_STM32.list"
	@echo 'Finished building: $@'
	@echo ' '

RETOCAR_Junior_Main_STM32.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "RETOCAR_Junior_Main_STM32.hex"
	@echo 'Finished building: $@'
	@echo ' '

RETOCAR_Junior_Main_STM32.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "RETOCAR_Junior_Main_STM32.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) RETOCAR_Junior_Main_STM32.bin RETOCAR_Junior_Main_STM32.elf RETOCAR_Junior_Main_STM32.hex RETOCAR_Junior_Main_STM32.list RETOCAR_Junior_Main_STM32.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
