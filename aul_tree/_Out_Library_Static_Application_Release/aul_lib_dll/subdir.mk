################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aul_lib_dll/auldll.c 

OBJS += \
./aul_lib_dll/auldll.o 

C_DEPS += \
./aul_lib_dll/auldll.d 


# Each subdirectory must supply rules for building sources it contributes
aul_lib_dll/%.o: ../aul_lib_dll/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


