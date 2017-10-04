################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
app/communication_drivers/parameters/communication/communication_par.obj: ../app/communication_drivers/parameters/communication/communication_par.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/board_drivers" --include_path="C:/ti/controlSUITE/device_support/f28m36x/v206/MWare" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/communication_drivers/ethernet" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/communication_drivers/ethernet/server_net/includes" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/communication_drivers/ethernet/server_net/uip-1.0" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/communication_drivers/ethernet/server_net/uip-1.0/uip" --include_path="R:/LNLS/Grupos/ELP/Doctec/Sirius/Projetos/Controlador Digital/Prototipos/V2.1/UDC/Jiga/Docs Ali/JIGA_GIT/ARM/app/communication_drivers/ethernet/server_net/uip-1.0/apps" -g --gcc --define=ccs --define="_STANDALONE" --diag_wrap=off --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="app/communication_drivers/parameters/communication/communication_par.d" --obj_directory="app/communication_drivers/parameters/communication" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


