################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/lib/drivers/hardware_drivers/encoder.cpp \
../Core/lib/drivers/hardware_drivers/motor.cpp \
../Core/lib/drivers/hardware_drivers/sensors.cpp 

OBJS += \
./Core/lib/drivers/hardware_drivers/encoder.o \
./Core/lib/drivers/hardware_drivers/motor.o \
./Core/lib/drivers/hardware_drivers/sensors.o 

CPP_DEPS += \
./Core/lib/drivers/hardware_drivers/encoder.d \
./Core/lib/drivers/hardware_drivers/motor.d \
./Core/lib/drivers/hardware_drivers/sensors.d 


# Each subdirectory must supply rules for building sources it contributes
Core/lib/drivers/hardware_drivers/%.o Core/lib/drivers/hardware_drivers/%.su Core/lib/drivers/hardware_drivers/%.cyclo: ../Core/lib/drivers/hardware_drivers/%.cpp Core/lib/drivers/hardware_drivers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/Project/RETOCAR_Junior/RETOCAR_Junior_Main_STM/RETOCAR_Junior_Main_STM32/Core/lib/drivers/communication" -I"D:/Project/RETOCAR_Junior/RETOCAR_Junior_Main_STM/RETOCAR_Junior_Main_STM32/Core/lib/drivers/hardware_drivers" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-lib-2f-drivers-2f-hardware_drivers

clean-Core-2f-lib-2f-drivers-2f-hardware_drivers:
	-$(RM) ./Core/lib/drivers/hardware_drivers/encoder.cyclo ./Core/lib/drivers/hardware_drivers/encoder.d ./Core/lib/drivers/hardware_drivers/encoder.o ./Core/lib/drivers/hardware_drivers/encoder.su ./Core/lib/drivers/hardware_drivers/motor.cyclo ./Core/lib/drivers/hardware_drivers/motor.d ./Core/lib/drivers/hardware_drivers/motor.o ./Core/lib/drivers/hardware_drivers/motor.su ./Core/lib/drivers/hardware_drivers/sensors.cyclo ./Core/lib/drivers/hardware_drivers/sensors.d ./Core/lib/drivers/hardware_drivers/sensors.o ./Core/lib/drivers/hardware_drivers/sensors.su

.PHONY: clean-Core-2f-lib-2f-drivers-2f-hardware_drivers

