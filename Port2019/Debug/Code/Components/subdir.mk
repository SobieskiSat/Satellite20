################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Code/Components/CanSatKitRadio.cpp \
../Code/Components/Logger.cpp 

OBJS += \
./Code/Components/CanSatKitRadio.o \
./Code/Components/Logger.o 

CPP_DEPS += \
./Code/Components/CanSatKitRadio.d \
./Code/Components/Logger.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Components/CanSatKitRadio.o: ../Code/Components/CanSatKitRadio.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/soft/Port2019/Code" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Code/Components/CanSatKitRadio.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Components/Logger.o: ../Code/Components/Logger.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../FATFS/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../USB_DEVICE/App -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/soft/Port2019/Code" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Code/Components/Logger.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

