################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../demo_shared/cfgrprt.c \
../demo_shared/config.c \
../demo_shared/demo_stx104_shared.c \
../demo_shared/demo_tracere_config.c \
../demo_shared/demo_tracere_reports.c \
../demo_shared/demo_tracere_shared.c \
../demo_shared/menu.c \
../demo_shared/stopwatch.c 

OBJS += \
./demo_shared/cfgrprt.o \
./demo_shared/config.o \
./demo_shared/demo_stx104_shared.o \
./demo_shared/demo_tracere_config.o \
./demo_shared/demo_tracere_reports.o \
./demo_shared/demo_tracere_shared.o \
./demo_shared/menu.o \
./demo_shared/stopwatch.o 

C_DEPS += \
./demo_shared/cfgrprt.d \
./demo_shared/config.d \
./demo_shared/demo_stx104_shared.d \
./demo_shared/demo_tracere_config.d \
./demo_shared/demo_tracere_reports.d \
./demo_shared/demo_tracere_shared.d \
./demo_shared/menu.d \
./demo_shared/stopwatch.d 


# Each subdirectory must supply rules for building sources it contributes
demo_shared/%.o: ../demo_shared/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


