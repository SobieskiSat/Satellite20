################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Sensors/gps_base/gps.c 

OBJS += \
./Code/Sensors/gps_base/gps.o 

C_DEPS += \
./Code/Sensors/gps_base/gps.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Sensors/gps_base/gps.o: ../Code/Sensors/gps_base/gps.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Sensors" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Core/Inc -I../USB_DEVICE/App -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Components" -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Sensors/gps_base/gps.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
