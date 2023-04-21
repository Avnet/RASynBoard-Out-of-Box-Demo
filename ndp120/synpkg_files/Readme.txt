
1    Copy the firmwares and config.ini to sdcard
2    Use different settings in config.ini for single and dual microphones 

		For dual mic: [default]
		[NDP Firmware]
		MCU=mcu_fw_120.synpkg
		DSP=dsp_firmware_noaec_ff.synpkg
		DNN=menu_demo_512_noaec_newph_v96_rasyn.synpkg

		For single mic: []
		[NDP Firmware]
		MCU=mcu_fw_120.synpkg
		DSP=dsp_firmware.synpkg
		DNN=menu_demo_512_id0_newph_v96_rasyn.synpkg


