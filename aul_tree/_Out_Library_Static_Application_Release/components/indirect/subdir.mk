################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/indirect/cmp_indirect.c 

OBJS += \
./components/indirect/cmp_indirect.o 

C_DEPS += \
./components/indirect/cmp_indirect.d 


# Each subdirectory must supply rules for building sources it contributes
components/indirect/%.o: ../components/indirect/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


