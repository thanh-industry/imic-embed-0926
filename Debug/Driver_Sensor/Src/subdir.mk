################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver_Sensor/Src/LCD.c 

OBJS += \
./Driver_Sensor/Src/LCD.o 

C_DEPS += \
./Driver_Sensor/Src/LCD.d 


# Each subdirectory must supply rules for building sources it contributes
Driver_Sensor/Src/%.o Driver_Sensor/Src/%.su Driver_Sensor/Src/%.cyclo: ../Driver_Sensor/Src/%.c Driver_Sensor/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/84899/Desktop/STM32-HCM/Project_IMIC/Driver_Sensor/Src" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver_Sensor-2f-Src

clean-Driver_Sensor-2f-Src:
	-$(RM) ./Driver_Sensor/Src/LCD.cyclo ./Driver_Sensor/Src/LCD.d ./Driver_Sensor/Src/LCD.o ./Driver_Sensor/Src/LCD.su

.PHONY: clean-Driver_Sensor-2f-Src

