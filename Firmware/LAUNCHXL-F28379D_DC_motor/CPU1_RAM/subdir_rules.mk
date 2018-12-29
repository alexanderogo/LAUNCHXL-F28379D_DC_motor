################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=4 --fp_mode=relaxed --include_path="D:/Workspace/Projects/LAUNCHXL-F28379D_DC_motor/Firmware/LAUNCHXL-F28379D_DC_motor" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/control/DCL/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/dsp/FixedPoint/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/dsp/FPU/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/math/FPUfastRTS/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="D:/Workspace/Projects/LAUNCHXL-F28379D_DC_motor/Firmware/LAUNCHXL-F28379D_DC_motor/device" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/include" --advice:performance=all --define=_LAUNCHXL_F28379D --define=DEBUG --define=_DUAL_HEADERS --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=4 --fp_mode=relaxed --include_path="D:/Workspace/Projects/LAUNCHXL-F28379D_DC_motor/Firmware/LAUNCHXL-F28379D_DC_motor" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/control/DCL/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/dsp/FixedPoint/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/dsp/FPU/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/libraries/math/FPUfastRTS/c28/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/common/include" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/device_support/f2837xd/headers/include" --include_path="D:/Workspace/Projects/LAUNCHXL-F28379D_DC_motor/Firmware/LAUNCHXL-F28379D_DC_motor/device" --include_path="C:/ti/c2000/C2000Ware_1_00_06_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.3.LTS/include" --advice:performance=all --define=_LAUNCHXL_F28379D --define=DEBUG --define=_DUAL_HEADERS --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


