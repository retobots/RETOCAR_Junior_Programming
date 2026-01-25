################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/lib/drivers/communication/uart_protocol.cpp 

OBJS += \
./Core/lib/drivers/communication/uart_protocol.o 

CPP_DEPS += \
./Core/lib/drivers/communication/uart_protocol.d 


# Each subdirectory must supply rules for building sources it contributes
Core/lib/drivers/communication/%.o Core/lib/drivers/communication/%.su Core/lib/drivers/communication/%.cyclo: ../Core/lib/drivers/communication/%.cpp Core/lib/drivers/communication/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/Project/RETOCAR_Junior/RETOCAR_Junior_Main_STM/RETOCAR_Junior_Main_STM32/Core/lib/drivers/communication" -I"D:/Project/RETOCAR_Junior/RETOCAR_Junior_Main_STM/RETOCAR_Junior_Main_STM32/Core/lib/drivers/hardware_drivers" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-lib-2f-drivers-2f-communication

clean-Core-2f-lib-2f-drivers-2f-communication:
	-$(RM) ./Core/lib/drivers/communication/uart_protocol.cyclo ./Core/lib/drivers/communication/uart_protocol.d ./Core/lib/drivers/communication/uart_protocol.o ./Core/lib/drivers/communication/uart_protocol.su

.PHONY: clean-Core-2f-lib-2f-drivers-2f-communication

