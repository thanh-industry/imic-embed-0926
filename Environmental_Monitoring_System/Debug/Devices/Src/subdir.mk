################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Devices/Src/adc_sensor.c \
../Devices/Src/bme280.c \
../Devices/Src/rtc_time.c \
../Devices/Src/ssd1306.c \
../Devices/Src/ssd1306_fonts.c \
../Devices/Src/w25q32.c 

OBJS += \
./Devices/Src/adc_sensor.o \
./Devices/Src/bme280.o \
./Devices/Src/rtc_time.o \
./Devices/Src/ssd1306.o \
./Devices/Src/ssd1306_fonts.o \
./Devices/Src/w25q32.o 

C_DEPS += \
./Devices/Src/adc_sensor.d \
./Devices/Src/bme280.d \
./Devices/Src/rtc_time.d \
./Devices/Src/ssd1306.d \
./Devices/Src/ssd1306_fonts.d \
./Devices/Src/w25q32.d 


# Each subdirectory must supply rules for building sources it contributes
Devices/Src/%.o Devices/Src/%.su Devices/Src/%.cyclo: ../Devices/Src/%.c Devices/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Devices/Inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/LENOVO/Desktop/imic-embed-0926/Environmental_Monitoring_System/Test" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Devices-2f-Src

clean-Devices-2f-Src:
	-$(RM) ./Devices/Src/adc_sensor.cyclo ./Devices/Src/adc_sensor.d ./Devices/Src/adc_sensor.o ./Devices/Src/adc_sensor.su ./Devices/Src/bme280.cyclo ./Devices/Src/bme280.d ./Devices/Src/bme280.o ./Devices/Src/bme280.su ./Devices/Src/rtc_time.cyclo ./Devices/Src/rtc_time.d ./Devices/Src/rtc_time.o ./Devices/Src/rtc_time.su ./Devices/Src/ssd1306.cyclo ./Devices/Src/ssd1306.d ./Devices/Src/ssd1306.o ./Devices/Src/ssd1306.su ./Devices/Src/ssd1306_fonts.cyclo ./Devices/Src/ssd1306_fonts.d ./Devices/Src/ssd1306_fonts.o ./Devices/Src/ssd1306_fonts.su ./Devices/Src/w25q32.cyclo ./Devices/Src/w25q32.d ./Devices/Src/w25q32.o ./Devices/Src/w25q32.su

.PHONY: clean-Devices-2f-Src

