################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver_Plus/Src/ADC_PIN.c \
../Driver_Plus/Src/BMP280.c \
../Driver_Plus/Src/LCD.c 

OBJS += \
./Driver_Plus/Src/ADC_PIN.o \
./Driver_Plus/Src/BMP280.o \
./Driver_Plus/Src/LCD.o 

C_DEPS += \
./Driver_Plus/Src/ADC_PIN.d \
./Driver_Plus/Src/BMP280.d \
./Driver_Plus/Src/LCD.d 


# Each subdirectory must supply rules for building sources it contributes
Driver_Plus/Src/%.o Driver_Plus/Src/%.su Driver_Plus/Src/%.cyclo: ../Driver_Plus/Src/%.c Driver_Plus/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I"C:/Users/84899/Desktop/STM32-HCM/Project_IMIC/Driver_Plus/Inc" -I"C:/Users/84899/Desktop/STM32-HCM/Project_IMIC/Driver_Plus/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Driver_Plus-2f-Src

clean-Driver_Plus-2f-Src:
	-$(RM) ./Driver_Plus/Src/ADC_PIN.cyclo ./Driver_Plus/Src/ADC_PIN.d ./Driver_Plus/Src/ADC_PIN.o ./Driver_Plus/Src/ADC_PIN.su ./Driver_Plus/Src/BMP280.cyclo ./Driver_Plus/Src/BMP280.d ./Driver_Plus/Src/BMP280.o ./Driver_Plus/Src/BMP280.su ./Driver_Plus/Src/LCD.cyclo ./Driver_Plus/Src/LCD.d ./Driver_Plus/Src/LCD.o ./Driver_Plus/Src/LCD.su

.PHONY: clean-Driver_Plus-2f-Src

