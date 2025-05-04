################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/src/SPI_.c \
../Core/src/main.c \
../Core/src/rcc.c \
../Core/src/usart_dbg.c 

OBJS += \
./Core/src/SPI_.o \
./Core/src/main.o \
./Core/src/rcc.o \
./Core/src/usart_dbg.o 

C_DEPS += \
./Core/src/SPI_.d \
./Core/src/main.d \
./Core/src/rcc.d \
./Core/src/usart_dbg.d 


# Each subdirectory must supply rules for building sources it contributes
Core/src/%.o Core/src/%.su Core/src/%.cyclo: ../Core/src/%.c Core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I"/home/ganibllo/STM_/TEST/Test_1/Core/inc" -I"/home/ganibllo/STM_/TEST/Test_1/CMSIS/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-src

clean-Core-2f-src:
	-$(RM) ./Core/src/SPI_.cyclo ./Core/src/SPI_.d ./Core/src/SPI_.o ./Core/src/SPI_.su ./Core/src/main.cyclo ./Core/src/main.d ./Core/src/main.o ./Core/src/main.su ./Core/src/rcc.cyclo ./Core/src/rcc.d ./Core/src/rcc.o ./Core/src/rcc.su ./Core/src/usart_dbg.cyclo ./Core/src/usart_dbg.d ./Core/src/usart_dbg.o ./Core/src/usart_dbg.su

.PHONY: clean-Core-2f-src

