################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/i8255m0/cmp_i8255m0.c 

OBJS += \
./components/i8255m0/cmp_i8255m0.o 

C_DEPS += \
./components/i8255m0/cmp_i8255m0.d 


# Each subdirectory must supply rules for building sources it contributes
components/i8255m0/%.o: ../components/i8255m0/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


