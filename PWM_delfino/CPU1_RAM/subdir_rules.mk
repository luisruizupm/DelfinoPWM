################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-857742945: ../PWM_delfino.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2002/ccs/utils/sysconfig_1.22.0/sysconfig_cli.bat" --script "C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino/PWM_delfino.syscfg" -o "syscfg" -s "C:/ti/C2000Ware_5_04_00_00/.metadata/sdk.json" -b "/boards/LAUNCHXL_F28379D" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-857742945 ../PWM_delfino.syscfg
syscfg/board.h: build-857742945
syscfg/board.cmd.genlibs: build-857742945
syscfg/board.opt: build-857742945
syscfg/board.json: build-857742945
syscfg/pinmux.csv: build-857742945
syscfg/epwm.dot: build-857742945
syscfg/c2000ware_libraries.cmd.genlibs: build-857742945
syscfg/c2000ware_libraries.opt: build-857742945
syscfg/c2000ware_libraries.c: build-857742945
syscfg/c2000ware_libraries.h: build-857742945
syscfg/clocktree.h: build-857742945
syscfg: build-857742945

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino/device" --include_path="C:/ti/C2000Ware_5_04_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=_LAUNCHXL_F28379D --define=NO_SELECT_ECAP_INPUT --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --ramfunc=off --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino/device" --include_path="C:/ti/C2000Ware_5_04_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2002/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=_LAUNCHXL_F28379D --define=NO_SELECT_ECAP_INPUT --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --ramfunc=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Luis/Documents/Doctorado/Controladores/TI_delfino/PWM_openloop/PWM_delfino/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


