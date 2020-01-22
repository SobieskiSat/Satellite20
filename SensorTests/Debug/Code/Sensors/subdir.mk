################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Sensors/bmp280.c 

OBJS += \
./Code/Sensors/bmp280.o 

C_DEPS += \
./Code/Sensors/bmp280.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Sensors/bmp280.o: ../Code/Sensors/bmp280.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Core/Inc -I../USB_DEVICE/App -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../FATFS/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/soft/SensorTests/Code" -I"C:/Users/damia/Desktop/CanSat/soft/SensorTests/Code/Sensors" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Sensors/bmp280.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

