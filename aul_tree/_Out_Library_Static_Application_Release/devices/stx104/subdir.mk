################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../devices/stx104/dev_stx104.c \
../devices/stx104/dev_stx104_ai.c \
../devices/stx104/dev_stx104_ao.c \
../devices/stx104/dev_stx104_di.c \
../devices/stx104/dev_stx104_do.c \
../devices/stx104/dev_stx104_gen.c \
../devices/stx104/dev_stx104_index.c \
../devices/stx104/dev_stx104_int.c \
../devices/stx104/dev_stx104_spi.c \
../devices/stx104/dev_stx104_trg.c 

OBJS += \
./devices/stx104/dev_stx104.o \
./devices/stx104/dev_stx104_ai.o \
./devices/stx104/dev_stx104_ao.o \
./devices/stx104/dev_stx104_di.o \
./devices/stx104/dev_stx104_do.o \
./devices/stx104/dev_stx104_gen.o \
./devices/stx104/dev_stx104_index.o \
./devices/stx104/dev_stx104_int.o \
./devices/stx104/dev_stx104_spi.o \
./devices/stx104/dev_stx104_trg.o 

C_DEPS += \
./devices/stx104/dev_stx104.d \
./devices/stx104/dev_stx104_ai.d \
./devices/stx104/dev_stx104_ao.d \
./devices/stx104/dev_stx104_di.d \
./devices/stx104/dev_stx104_do.d \
./devices/stx104/dev_stx104_gen.d \
./devices/stx104/dev_stx104_index.d \
./devices/stx104/dev_stx104_int.d \
./devices/stx104/dev_stx104_spi.d \
./devices/stx104/dev_stx104_trg.d 


# Each subdirectory must supply rules for building sources it contributes
devices/stx104/%.o: ../devices/stx104/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DAUL_LIBRARY_STATIC_RELEASE -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


