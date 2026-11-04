################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Test/test_hardware.c 

OBJS += \
./Test/test_hardware.o 

C_DEPS += \
./Test/test_hardware.d 


# Each subdirectory must supply rules for building sources it contributes
Test/%.o Test/%.su Test/%.cyclo: ../Test/%.c Test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Devices/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Test" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Test

clean-Test:
	-$(RM) ./Test/test_hardware.cyclo ./Test/test_hardware.d ./Test/test_hardware.o ./Test/test_hardware.su

.PHONY: clean-Test

