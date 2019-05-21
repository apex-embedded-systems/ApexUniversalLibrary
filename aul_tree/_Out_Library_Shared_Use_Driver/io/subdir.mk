################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../io/io.o \
../io/io_linux_drv_gen_x86.o 

C_SRCS += \
../io/io.c \
../io/io_linux_lib_gen.c 

OBJS += \
./io/io.o \
./io/io_linux_lib_gen.o 

C_DEPS += \
./io/io.d \
./io/io_linux_lib_gen.d 


# Each subdirectory must supply rules for building sources it contributes
io/%.o: ../io/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -fPIC  -D_DEBUG -DAUL_LIBRARY_SHARED_DRIVER_DEBUG -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


