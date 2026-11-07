################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/app_main.c \
../App/Src/task_cli.c \
../App/Src/task_display.c \
../App/Src/task_sensor.c \
../App/Src/task_storage.c 

OBJS += \
./App/Src/app_main.o \
./App/Src/task_cli.o \
./App/Src/task_display.o \
./App/Src/task_sensor.o \
./App/Src/task_storage.o 

C_DEPS += \
./App/Src/app_main.d \
./App/Src/task_cli.d \
./App/Src/task_display.d \
./App/Src/task_sensor.d \
./App/Src/task_storage.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su App/Src/%.cyclo: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/App/Inc" -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Devices/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Test" -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Services/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/app_main.cyclo ./App/Src/app_main.d ./App/Src/app_main.o ./App/Src/app_main.su ./App/Src/task_cli.cyclo ./App/Src/task_cli.d ./App/Src/task_cli.o ./App/Src/task_cli.su ./App/Src/task_display.cyclo ./App/Src/task_display.d ./App/Src/task_display.o ./App/Src/task_display.su ./App/Src/task_sensor.cyclo ./App/Src/task_sensor.d ./App/Src/task_sensor.o ./App/Src/task_sensor.su ./App/Src/task_storage.cyclo ./App/Src/task_storage.d ./App/Src/task_storage.o ./App/Src/task_storage.su

.PHONY: clean-App-2f-Src

