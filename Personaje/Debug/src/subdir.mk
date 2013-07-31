################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/crearPersonaje.c \
../src/entrarEnNivel.c \
../src/handleConnectionError.c \
../src/jugar.c \
../src/morir.c \
../src/personaje.c \
../src/signalListeners.c \
../src/terminarNivel.c 

OBJS += \
./src/crearPersonaje.o \
./src/entrarEnNivel.o \
./src/handleConnectionError.o \
./src/jugar.o \
./src/morir.o \
./src/personaje.o \
./src/signalListeners.o \
./src/terminarNivel.o 

C_DEPS += \
./src/crearPersonaje.d \
./src/entrarEnNivel.d \
./src/handleConnectionError.d \
./src/jugar.d \
./src/morir.d \
./src/personaje.d \
./src/signalListeners.d \
./src/terminarNivel.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/callende/tp-20131c-sonic-el-puercoespin/Librerias/ -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


