################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Scripts/duplex.c \
../Code/Scripts/loging.c \
../Code/Scripts/sensing.c \
../Code/Scripts/steering.c 

OBJS += \
./Code/Scripts/duplex.o \
./Code/Scripts/loging.o \
./Code/Scripts/sensing.o \
./Code/Scripts/steering.o 

C_DEPS += \
./Code/Scripts/duplex.d \
./Code/Scripts/loging.d \
./Code/Scripts/sensing.d \
./Code/Scripts/steering.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Scripts/duplex.o: ../Code/Scripts/duplex.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/duplex.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/loging.o: ../Code/Scripts/loging.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/loging.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/sensing.o: ../Code/Scripts/sensing.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/sensing.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/steering.o: ../Code/Scripts/steering.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/steering.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

