################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/cmp_avail.c 

OBJS += \
./components/cmp_avail.o 

C_DEPS += \
./components/cmp_avail.d 


# Each subdirectory must supply rules for building sources it contributes
components/%.o: ../components/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fPIC -D_DEBUG -DAUL_LIBRARY_SHARED_DRIVER_DEBUG -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


