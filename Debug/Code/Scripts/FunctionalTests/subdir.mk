################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Code/Scripts/FunctionalTests/distance.c \
../Code/Scripts/FunctionalTests/fdr_tests.c \
../Code/Scripts/FunctionalTests/receiver.c \
../Code/Scripts/FunctionalTests/transmitter.c 

OBJS += \
./Code/Scripts/FunctionalTests/distance.o \
./Code/Scripts/FunctionalTests/fdr_tests.o \
./Code/Scripts/FunctionalTests/receiver.o \
./Code/Scripts/FunctionalTests/transmitter.o 

C_DEPS += \
./Code/Scripts/FunctionalTests/distance.d \
./Code/Scripts/FunctionalTests/fdr_tests.d \
./Code/Scripts/FunctionalTests/receiver.d \
./Code/Scripts/FunctionalTests/transmitter.d 


# Each subdirectory must supply rules for building sources it contributes
Code/Scripts/FunctionalTests/distance.o: ../Code/Scripts/FunctionalTests/distance.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/FunctionalTests/distance.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/FunctionalTests/fdr_tests.o: ../Code/Scripts/FunctionalTests/fdr_tests.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/FunctionalTests/fdr_tests.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/FunctionalTests/receiver.o: ../Code/Scripts/FunctionalTests/receiver.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/FunctionalTests/receiver.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Code/Scripts/FunctionalTests/transmitter.o: ../Code/Scripts/FunctionalTests/transmitter.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DDEBUG -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Inc -I../Drivers/CMSIS/Include -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Components" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Sensors" -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code" -I"C:/Users/damia/Desktop/CanSat/Satellite20/Code/Scripts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Code/Scripts/FunctionalTests/transmitter.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

