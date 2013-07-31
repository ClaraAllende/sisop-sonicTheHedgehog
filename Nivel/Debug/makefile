################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: Nivel

dependents:
	-cd /home/callende/tp-20131c-sonic-el-puercoespin/Librerias/Debug && $(MAKE) all

# Tool invocations
Nivel: $(OBJS) $(USER_OBJS) /home/callende/tp-20131c-sonic-el-puercoespin/Librerias/Debug/libLibrerias.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L/home/callende/tp-20131c-sonic-el-puercoespin/Librerias/Debug/ -L/home/callende/tp-20131c-sonic-el-puercoespin/Librerias/gui/ -o "Nivel" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Nivel
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/callende/tp-20131c-sonic-el-puercoespin/Librerias/Debug/libLibrerias.so:

-include ../makefile.targets
