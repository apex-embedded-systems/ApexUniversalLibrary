################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aul/aul.c \
../aul/aul_general.c \
../aul/aul_hwdef.c 

OBJS += \
./aul/aul.o \
./aul/aul_general.o \
./aul/aul_hwdef.o 

C_DEPS += \
./aul/aul.d \
./aul/aul_general.d \
./aul/aul_hwdef.d 


# Each subdirectory must supply rules for building sources it contributes
aul/%.o: ../aul/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fPIC -D_DEBUG -DAUL_LIBRARY_SHARED_DRIVER_DEBUG -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


