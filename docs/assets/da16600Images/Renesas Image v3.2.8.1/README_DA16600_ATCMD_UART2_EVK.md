# Readme - DA16600 AT Command UART2
DA16600 firmware images for QFN package that includes AT commands over UART2

## Contents
- DA16600_FBOOT-*.img:
	DA16600 bootloader image
- DA16600_FRTOS-*.img:
	DA16600 RTOS image including AT Command feature
- da16600.map:
	Map file created by the SDK code built
- da16600_da14531_1_download.ttl:
	Teraterm script file to download the images into serial flash memory including BLE peripheral
- da16600_da14531_2_download.ttl:
	Teraterm script file to download the images into serial flash memory including BLE Central
- mrom_otp_lock_check.ttl:
	Teraterm script file to lock the OTP during programming

## UART1 Pin out
- GPIOC6  (UART2 TXD)
- GPIOC7  (UART2 RXD)

## References
- Getting Started Guide:
	https://www.renesas.com/us/en/document/qsg/da16200-da16600-freertos-getting-started-guide?r=1600096
- AT Command User Manual:
	https://www.renesas.com/us/en/document/mas/um-wi-003-da16200-da16600-host-interface-and-command-user-manual?r=1601951
- Further available documents in:
	https://www.renesas.com/us/en/products/wireless-connectivity/wi-fi/low-power-wi-fi