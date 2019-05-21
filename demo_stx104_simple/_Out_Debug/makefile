################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: demo_stx104_simple

dependents:
	-cd E:/aes_eclipse/aul_tree/_Out_Library_Static_Application_Debug && $(MAKE) all

# Tool invocations
demo_stx104_simple: $(OBJS) $(USER_OBJS) E:/aes_eclipse/aul_tree/_Out_Library_Static_Application_Debug/libauldebug.a
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"../../aul_tree/_Out_Library_Static_Application_Debug" -o "demo_stx104_simple" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) demo_stx104_simple
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
E:/aes_eclipse/aul_tree/_Out_Library_Static_Application_Debug/libauldebug.a:

-include ../makefile.targets
