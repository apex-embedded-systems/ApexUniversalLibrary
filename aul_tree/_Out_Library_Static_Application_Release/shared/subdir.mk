################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../shared/cmd.c \
../shared/debug.c \
../shared/error.c \
../shared/mem.c \
../shared/module.c \
../shared/report.c \
../shared/status.c 

OBJS += \
./shared/cmd.o \
./shared/debug.o \
./shared/error.o \
./shared/mem.o \
./shared/module.o \
./shared/report.o \
./shared/status.o 

C_DEPS += \
./shared/cmd.d \
./shared/debug.d \
./shared/error.d \
./shared/mem.d \
./shared/module.d \
./shared/report.d \
./shared/status.d 


# Each subdirectory must supply rules for building sources it contributes
shared/%.o: ../shared/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


