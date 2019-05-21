################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include str_safe/subdir.mk
-include shared/subdir.mk
-include reg/subdir.mk
-include io/subdir.mk
-include devmgr/subdir.mk
-include devices/tracere/subdir.mk
-include devices/stx104/subdir.mk
-include devices/subdir.mk
-include demo_shared/subdir.mk
-include components/indirect/subdir.mk
-include components/i8255m0/subdir.mk
-include components/i8254/subdir.mk
-include components/egs/subdir.mk
-include components/subdir.mk
-include cmpmgr/subdir.mk
-include aul_lib_dll/subdir.mk
-include aul/subdir.mk
-include acgr/subdir.mk
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
all: libaul.a

# Tool invocations
libaul.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r  "libaul.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(ARCHIVES)$(OBJS)$(C_DEPS) libaul.a
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
