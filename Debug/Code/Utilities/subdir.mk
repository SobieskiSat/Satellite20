################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Utilities/logger.c 

OBJS += \
./Code/Utilities/logger.o 

C_DEPS += \
./Code/Utilities/logger.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Utilities/logger.o: ../Code/Utilities/logger.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Utilities" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Utilities/logger.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
