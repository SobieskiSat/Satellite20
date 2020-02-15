################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Sensors/bmp280.c \
../Code/Sensors/gps.c \
../Code/Sensors/mpu9250.c 

OBJS += \
./Code/Sensors/bmp280.o \
./Code/Sensors/gps.o \
./Code/Sensors/mpu9250.o 

C_DEPS += \
./Code/Sensors/bmp280.d \
./Code/Sensors/gps.d \
./Code/Sensors/mpu9250.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Sensors/bmp280.o: ../Code/Sensors/bmp280.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Sensors" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Core/Inc -I../USB_DEVICE/App -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Components" -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Sensors/bmp280.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Sensors/gps.o: ../Code/Sensors/gps.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Sensors" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Core/Inc -I../USB_DEVICE/App -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Components" -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Sensors/gps.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Sensors/mpu9250.o: ../Code/Sensors/mpu9250.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Sensors" -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Core/Inc -I../USB_DEVICE/App -I"C:/Users/damia/Desktop/CanSat/soft/Satellite2/Code/Components" -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Sensors/mpu9250.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

