################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../devices/tracere/dev_tracere.c 

OBJS += \
./devices/tracere/dev_tracere.o 

C_DEPS += \
./devices/tracere/dev_tracere.d 


# Each subdirectory must supply rules for building sources it contributes
devices/tracere/%.o: ../devices/tracere/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


