################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../2837xD_RAM_lnk_cpu1.cmd \
../F2837xD_Headers_nonBIOS_cpu1.cmd 

LIB_SRCS += \
../driverlib.lib 

ASM_SRCS += \
../DCL_PID_C4.asm \
../F2837xD_CodeStartBranch.asm \
../F2837xD_usDelay.asm 

C_SRCS += \
../F2837xD_CpuTimers.c \
../F2837xD_DefaultISR.c \
../F2837xD_Dma.c \
../F2837xD_EPwm.c \
../F2837xD_GlobalVariableDefs.c \
../F2837xD_Gpio.c \
../F2837xD_PieCtrl.c \
../F2837xD_PieVect.c \
../F2837xD_Spi.c \
../F2837xD_SysCtrl.c \
../_adc.c \
../_angle_loop.c \
../_current_loop.c \
../_dac.c \
../_epwm.c \
../_force_loop.c \
../_globals.c \
../_led.c \
../_main_DC_motor.c \
../_spi.c \
../_timer.c \
../_velocity_loop.c 

C_DEPS += \
./F2837xD_CpuTimers.d \
./F2837xD_DefaultISR.d \
./F2837xD_Dma.d \
./F2837xD_EPwm.d \
./F2837xD_GlobalVariableDefs.d \
./F2837xD_Gpio.d \
./F2837xD_PieCtrl.d \
./F2837xD_PieVect.d \
./F2837xD_Spi.d \
./F2837xD_SysCtrl.d \
./_adc.d \
./_angle_loop.d \
./_current_loop.d \
./_dac.d \
./_epwm.d \
./_force_loop.d \
./_globals.d \
./_led.d \
./_main_DC_motor.d \
./_spi.d \
./_timer.d \
./_velocity_loop.d 

OBJS += \
./DCL_PID_C4.obj \
./F2837xD_CodeStartBranch.obj \
./F2837xD_CpuTimers.obj \
./F2837xD_DefaultISR.obj \
./F2837xD_Dma.obj \
./F2837xD_EPwm.obj \
./F2837xD_GlobalVariableDefs.obj \
./F2837xD_Gpio.obj \
./F2837xD_PieCtrl.obj \
./F2837xD_PieVect.obj \
./F2837xD_Spi.obj \
./F2837xD_SysCtrl.obj \
./F2837xD_usDelay.obj \
./_adc.obj \
./_angle_loop.obj \
./_current_loop.obj \
./_dac.obj \
./_epwm.obj \
./_force_loop.obj \
./_globals.obj \
./_led.obj \
./_main_DC_motor.obj \
./_spi.obj \
./_timer.obj \
./_velocity_loop.obj 

ASM_DEPS += \
./DCL_PID_C4.d \
./F2837xD_CodeStartBranch.d \
./F2837xD_usDelay.d 

OBJS__QUOTED += \
"DCL_PID_C4.obj" \
"F2837xD_CodeStartBranch.obj" \
"F2837xD_CpuTimers.obj" \
"F2837xD_DefaultISR.obj" \
"F2837xD_Dma.obj" \
"F2837xD_EPwm.obj" \
"F2837xD_GlobalVariableDefs.obj" \
"F2837xD_Gpio.obj" \
"F2837xD_PieCtrl.obj" \
"F2837xD_PieVect.obj" \
"F2837xD_Spi.obj" \
"F2837xD_SysCtrl.obj" \
"F2837xD_usDelay.obj" \
"_adc.obj" \
"_angle_loop.obj" \
"_current_loop.obj" \
"_dac.obj" \
"_epwm.obj" \
"_force_loop.obj" \
"_globals.obj" \
"_led.obj" \
"_main_DC_motor.obj" \
"_spi.obj" \
"_timer.obj" \
"_velocity_loop.obj" 

C_DEPS__QUOTED += \
"F2837xD_CpuTimers.d" \
"F2837xD_DefaultISR.d" \
"F2837xD_Dma.d" \
"F2837xD_EPwm.d" \
"F2837xD_GlobalVariableDefs.d" \
"F2837xD_Gpio.d" \
"F2837xD_PieCtrl.d" \
"F2837xD_PieVect.d" \
"F2837xD_Spi.d" \
"F2837xD_SysCtrl.d" \
"_adc.d" \
"_angle_loop.d" \
"_current_loop.d" \
"_dac.d" \
"_epwm.d" \
"_force_loop.d" \
"_globals.d" \
"_led.d" \
"_main_DC_motor.d" \
"_spi.d" \
"_timer.d" \
"_velocity_loop.d" 

ASM_DEPS__QUOTED += \
"DCL_PID_C4.d" \
"F2837xD_CodeStartBranch.d" \
"F2837xD_usDelay.d" 

ASM_SRCS__QUOTED += \
"../DCL_PID_C4.asm" \
"../F2837xD_CodeStartBranch.asm" \
"../F2837xD_usDelay.asm" 

C_SRCS__QUOTED += \
"../F2837xD_CpuTimers.c" \
"../F2837xD_DefaultISR.c" \
"../F2837xD_Dma.c" \
"../F2837xD_EPwm.c" \
"../F2837xD_GlobalVariableDefs.c" \
"../F2837xD_Gpio.c" \
"../F2837xD_PieCtrl.c" \
"../F2837xD_PieVect.c" \
"../F2837xD_Spi.c" \
"../F2837xD_SysCtrl.c" \
"../_adc.c" \
"../_angle_loop.c" \
"../_current_loop.c" \
"../_dac.c" \
"../_epwm.c" \
"../_force_loop.c" \
"../_globals.c" \
"../_led.c" \
"../_main_DC_motor.c" \
"../_spi.c" \
"../_timer.c" \
"../_velocity_loop.c" 


