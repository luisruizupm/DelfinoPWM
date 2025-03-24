################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd/device" --include_path="C:/ti/C2000Ware_5_04_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=_LAUNCHXL_F28379D --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-258691217: ../lab_sci_launchpad.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2002/ccs/utils/sysconfig_1.22.0/sysconfig_cli.bat" --script "C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd/lab_sci_launchpad.syscfg" -o "syscfg" -s "C:/ti/C2000Ware_5_04_00_00/.metadata/sdk.json" -b "/boards/LAUNCHXL_F28379D" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-258691217 ../lab_sci_launchpad.syscfg
syscfg/board.h: build-258691217
syscfg/board.cmd.genlibs: build-258691217
syscfg/board.opt: build-258691217
syscfg/board.json: build-258691217
syscfg/pinmux.csv: build-258691217
syscfg/c2000ware_libraries.cmd.genlibs: build-258691217
syscfg/c2000ware_libraries.opt: build-258691217
syscfg/c2000ware_libraries.c: build-258691217
syscfg/c2000ware_libraries.h: build-258691217
syscfg/clocktree.h: build-258691217
syscfg: build-258691217

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd/device" --include_path="C:/ti/C2000Ware_5_04_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=_LAUNCHXL_F28379D --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/sci_lp_f2837xd/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


