################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Configuration.c \
../src/Deadlock.c \
../src/Nivel.c \
../src/pantalla.c 

OBJS += \
./src/Configuration.o \
./src/Deadlock.o \
./src/Nivel.o \
./src/pantalla.o 

C_DEPS += \
./src/Configuration.d \
./src/Deadlock.d \
./src/Nivel.d \
./src/pantalla.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/callende/tp-20131c-sonic-el-puercoespin/Librerias/ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


