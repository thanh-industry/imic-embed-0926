################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Services/Src/cli_parser.c \
../Services/Src/log.c 

OBJS += \
./Services/Src/cli_parser.o \
./Services/Src/log.o 

C_DEPS += \
./Services/Src/cli_parser.d \
./Services/Src/log.d 


# Each subdirectory must supply rules for building sources it contributes
Services/Src/%.o Services/Src/%.su Services/Src/%.cyclo: ../Services/Src/%.c Services/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Devices/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Test" -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Services/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Services-2f-Src

clean-Services-2f-Src:
	-$(RM) ./Services/Src/cli_parser.cyclo ./Services/Src/cli_parser.d ./Services/Src/cli_parser.o ./Services/Src/cli_parser.su ./Services/Src/log.cyclo ./Services/Src/log.d ./Services/Src/log.o ./Services/Src/log.su

.PHONY: clean-Services-2f-Src

