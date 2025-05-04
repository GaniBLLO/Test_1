################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Applications/SD/src/SD_card.c 

OBJS += \
./Applications/SD/src/SD_card.o 

C_DEPS += \
./Applications/SD/src/SD_card.d 


# Each subdirectory must supply rules for building sources it contributes
Applications/SD/src/%.o Applications/SD/src/%.su Applications/SD/src/%.cyclo: ../Applications/SD/src/%.c Applications/SD/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F407VETx -c -I"/home/ganibllo/STM_/TEST/Test_1/Core/inc" -I"/home/ganibllo/STM_/TEST/Test_1/CMSIS/inc" -I"/home/ganibllo/STM_/TEST/Test_1/Applications/dp83848/inc" -I"/home/ganibllo/STM_/TEST/Test_1/Applications/SD/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Applications-2f-SD-2f-src

clean-Applications-2f-SD-2f-src:
	-$(RM) ./Applications/SD/src/SD_card.cyclo ./Applications/SD/src/SD_card.d ./Applications/SD/src/SD_card.o ./Applications/SD/src/SD_card.su

.PHONY: clean-Applications-2f-SD-2f-src

