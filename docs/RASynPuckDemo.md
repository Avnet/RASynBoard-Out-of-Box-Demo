# Avnet RASynBoard RASynPuck Demo

This page contains details for creating a copy of the RASynPuck demo.

<p align="center">
    <br />
    <img src=./assets/images/puckDemo2.jpg width="250">
<br />

<p align="center">
    <br />
    <img src=./assets/images/puckDemo3.jpg width="1250">
<br />

# Demo Overview

The RASynPuck demo is a pocket friendly, wireless, battery operated demo fixture with a supporting cloud dashboard hosted on Avnet's IoTConnect Cloud Platform.  The demo can be used to demonstrate ML models running on the Syntiant NDP120 Neural Decision Processor.  Additionally all ML model data and ML inference events can be viewed in a cloud hosted dashboard.

> **Note:** If you don't care to run the demo on just the core board, you can work through the [IoTConnect instructions](./IoTConnect.md) and end up with the same demo using the RASynBoard EVK (core board + I/O board).

The demo uses the core board only and requires managing the following items without the microSD card.  The Out-of-Box application implementation includes ways to manage these items without the microSD card.

- Cloud X.509 certificates
- ML model
- Cloud configuration items from the config.ini file

# Table of Contents

1. [Overview](#demo-overview)
1. [Required Hardware](#required-hardware)
1. [Required Software](#required-software)
1. [Setup the Demo System](#setup-the-demo-system)
    1. [Flash the latest OOB release to the RASynBoard](#flash-the-oob-v151-image-to-the-renesas-ra6m4-mcu)
    1. [Copy the release microSD Card files to the microSD card](#copy-the-release-microsd-card-file-to-the-microsd-card)
    1. [Select the ML model to run](#select-the-ml-model-to-run)
    1. [Connect the RASynbBoard to IoTConnect](#connect-the-rasynboard-to-iotconnect)
    1. [Update the WiFi configuration](#update-the-wifi-configuration)
    1. [Update the X.509 certificate location](#change-the-certificates-configuration-to-dont-load-any-certificates-the-certificates-are-already-loaded-onto-the-da16600)
    1. [Flash the ML model and configuration to SPI Flash](#flash-the-ml-models-and-configuration-to-spi-flash-memory-on-the-core-board)
    1. [Separate the boards and test](#separate-the-core-board-from-the-io-board-and-test)
1. [Conclusion](#conclusion)

# Required Hardware

- 1 [RASynBoard EVK](http://avnet.me/RASynBoard)
- 1 microSD card
- 1 [RASynPuck Enclosure](http://avnet.me/RASynEnclosures)
    - Optional, this demo can be run without the RASynPuck fixture if desired
    - For best results print the RASynPuck with a resin 3D printer
- 1 [500mAH battery](https://tinycircuits.com/products/lithium-ion-polymer-battery-3-7v-500mah)
    - Optional, this demo can be powered by the USB-C connector if desired

# Required Software

- Renesas Flash Programmer (to flash the image to the RA6M4)
- Renesas WiFi Provisioning app (optional but desirable to change/manage the wifi network when just running the core board alone)

# Setup the Demo System

Follow the steps here to get your RASynBoard demo up and running.  The tasks we need to complete are . . . 

1. Flash the Avnet RASynBoard Out-of-Box application (V1.5.1 or greater) onto the RASynBoard's Renesas RA6M4 MCU
    1. Flash the image
    1. Update the microSD card with the release microSD card files 
1. Select the ML model that you want to run on the demo
1. Connect the board to IoTConnect and create the dashboard from a template
1. Update the WiFi configuration
1. Change the certificates configuration to ```Don't load any certificates, the certificates are already loaded onto the DA16600```
1. Flash the ML models and configuration to SPI Flash memory on the Core board
1. Separate the core board from the I/O board and test

## Flash the OOB V1.5.1+ image to the Renesas RA6M4 MCU

1. Follow the [How to use RASynBoard Releases](./UsingRASynbBoardReleases.md) instructions to load the latest release
    1. Be sure to use the latest release >= 1.5.1
    1. A video walkthrough of the process is available [here](http://avnet.me/RASynUsingReleasesVideo)

### Copy the release microSD card file to the microSD card
Each release contains a zip file **microSD-Files-\<version string\>.zip** that contains all the files that should be loaded onto the microSD card.  
1. Copy the files from the release **microSD-Files-\<version string\>.zip** file into the root directory of your microSD card

## Select the ML model to run

The default model configuration is the 5-keyword model "up," "down," "back," "next," "ok-syntiant."  If you want to run a different model . . .
1. Use the instructions [here](./ApplicationUserGuide.md#ndp120-configuration) to change the model running on the board.
1. A video walkthrough of the process is available [here](http://avnet.me/RASynChangingMlModelsVideo)

> **Note:** the IoTConnect dashboard is generic and will work with any ML model

## Connect the RASynBoard to IoTConnect

In order to use the IoTconnect dashboard, you must first setup the IoTConnect account/device/dashboard.  All the details to accomplish these tasks is documented in the [IoTConnect Documentation](./IoTConnect.md).

1. A video walkthrough of the process is available [here](http://avnet.me/IoTConnectOnAWS)

## Update the WiFi configuration
There are two ways to configure the WiFi network, both are documented [here](./ApplicationUserGuide.md/#wifi-access-point-credentials).  Which configuration you use depends on the WiFi network where you plan to show the demo.
- If you have a WiFi Hotspot or dedicated Access Point then it’s recommended to use the static configuration where you add the SSID and Password to the config.ini file
- If you must use the WiFi at the location, use the Renesas WiFi Configuration mobile application.  Use caution here as some public WiFi networks require the user acknowledge an agreement and since the RASynBoard is headless, there will be no way to complete the WiFi registration.
    - Don't forget to enable BLE in the config.ini file

- [Video Link (9 minutes)](http://avnet.me/RASynWiFi)

## Change the certificates configuration to "Don't load any certificates, the certificates are already loaded onto the DA16600"
When you setup the device to connect to IoTConnect, you copied the X.509 certificates to the microSD card.  Since we'll be separating the core board from the I/O board, we need to reconfigure the application and tell it how to find the X.509 certificates.

1. Verify that the current configuration connects to IoTConnect
    - If you completed the IoTConnect configuration this should be true
    - This step verifies that the certificates are correctly loaded into the DA16600 non-volatile memory
1. Edit the config.ini file on the micro SD card
    - Change ```Cert_Location``` to **2**
    - This tells the application not to delete the certificates loaded on the DA16600 and to assume that they are there and good to go. 
1. Power cycle the board to read the new configuration
1. Again verify that the application connects to IoTConnect
1. Proceed to the next step

<p align="center">
    <br />
    <img src=./assets/images/puckDemo4.jpg width="600">
<br />

## Flash the ML models and configuration to SPI Flash memory on the Core board
We're just about finished.  At this point we have . . . 
1. Flashed the latest OOB release onto the RASynBoard EVK
1. Updated the microSD card with the files matching the release
1. Selected the ML model we want to demo
1. Connected the board to IoTConnect and created the dashboard from a template
1. Configured the WiFi for our target demo location(s)
1. Verified that the X.509 certificates are loaded in the DA16600 non-volatile memory
1. Configured the application to use the certificates loaded on the DA16600
1. Power cycled the device so that all the configuration changes are loaded

Next we need to flash the ML model files and the current configuration to the SPI memory on the core board
1. With the application still running, press the user button (button closest to the corner) for 3+ seconds 
    - The application flashes all the NDP120 model files to SPI flash
    - The application writes the current configuration to SPI flash

<p align="center">
    <br />
    <img src=./assets/images/puckDemo5.jpg width="500">
<br />

## Separate the core board from the I/O board and test
Now we can separate the boards!
1. Power down the device
1. Carefully remove the core board from the I/O board
1. Power the core board using a USB-C cable to your development PC
1. Note that a COM port will enumerate on your PC that can be used to see debug from your core board
    1. COM settings are 115200, 8, N, 1
1. Exercise your ML model, you should see debug

> **Note:** early debug is not displayed since the application must bring up the Virtual COM port

1. To see early debug type "log" in your terminal

<p align="center">
    <br />
    <img src=./assets/images/puckDemo6.jpg width="800">
<br />

## Mount the RASynBoard core board into the RASynPuck fixture
1. Mount the core board onto the board mount piece
1. Place the battery in the bottom void
1. Pass the power connector up through the hole in the board mount piece
1. Screw the bottom onto the board mount capturing the battery
1. Connect the battery connector to the RASynBoard
1. Screw the top onto the fixture

> **Note:** The core board contains a charging circuit.  Simply connect the battery to the core board and connect a USB-C cable between a USB-A port and the core board.  A small LED will light RED while charging.

# Conclusion

I hope you were successful in setting up your demo.  If you run into any issues or have suggestion on how to improve the OOB application, please open a [new issue](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues) in the GitHub repo. 