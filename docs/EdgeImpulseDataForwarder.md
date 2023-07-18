# Using Edge Impulse Data Forwarder

The RASynBoard can be used to collect IMU data and directly feed to to Edge Impulse for training or testing.

## Prerequisites

In order to use the data forwarder, you must have NodeJS with npm installed. Please refer to the NodeJS website for installation: https://nodejs.org/en/download/package-manager

Once you have installed NodeJS and npm, install the Edge Impulse CLI tools with the following command:

MacOS/Linux:

```bash
sudo npm install -g edge-impulse-cli
```

Windows:

```Powershell
npm install -g edge-impulse-cli
```

## Setting Up RASynBoard

Clone or download the contents of the [RASynBoard OOB Demo repo](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo) and copy all the files from `ndp120/synpkg_files` to the root of a `FAT 32` formatted microSD card.

Modify the config.ini to contain the following:

```
[NDP Firmware]
Mode=1      # select function mode:  1->Function_1;  2->Function_2;  3->Function_3; ...

[Function_1]
Description="Single Mic"
MCU=mcu_fw_120_notify.synpkg
DSP=dsp_firmware.synpkg
DNN=menu_demo_512_general_newph_v100_rasyn_pdm0_ext_icm.synpkg
Button_shift=imu  # Stream IMU data when the user button is pressed

[Function_2]
Description="Dual Mic"
MCU=mcu_fw_120_notify.synpkg
DSP=dsp_firmware_noaec_ff.synpkg
DNN=menu_demo_512_noaec_newph_v100_rasyn_icm.synpkg

[Function_3]
Description="Circle Motion"
MCU=mcu_fw_120_notify.synpkg
DSP=dsp_firmware.synpkg
DNN=circular_motion_NDP120B0_icm42670.synpkg
Button_shift=imu      # redefine the short press button to capture the IMU data

[Led]
# set led response color for each voice command, choose from "red","green","blue","yellow","cyan" and "magenta".
IDX0=yellow     # ok-syntiant
IDX1=cyan       # up
IDX2=magenta    # down
IDX3=red        # back
IDX4=green      # next

[Debug Print]
Port=1          # select debug port:  1->by UART;  2->by USB-VCOM

[Recording Period] 
# Defines the duration of voice or IMU data recording when the user button is pressed.
Recording_Period=3600  # in seconds, stream IMU data for 60min to the debug uart

[IMU data stream]
# Select the feature:  0->Disable;  1->Enable
Write_to_file=0        # write data to a file in sdcard
Print_to_terminal=1    # output data to the serial debug terminal

[Low Power Mode]
# 0->Enter Low Power Mode with "down . . down" command; 1->Enter Low Power mode automatically after each feature detection 
Power_Mode=0

```
Remove the microSD card and insert into the RAsynBoard. Connect a USB Serial adapter to the RASynBoard as shown below:

![](../images/UartConnection.jpg "")

Power on the RASynBoard.

## Capturing Data

Create a free account with Edge Impulse and create a new project if one isn't already created: https://studio.edgeimpulse.com/studio/profile/projects

Press the user button to start printing IMU data to the serial port:
[Insert picture of user button]

From a terminal window, run the following command:

```bash
edge-impulse-data-forwarder
```

At this point, you should be prompted with your Edge Impulse credentials:

```
Edge Impulse data forwarder v1.20.0
? What is your user name or e-mail address (edgeimpulse.com)? evan.stoddard@avnet.com
? What is your password? [hidden]
```

You should see that a serial device was connected and the sample rate of the incoming data stream is detected:

```
[SER] Connecting to /dev/ttyUSB0
[SER] Could not read serial number for device, defaulting to 000000000000
[SER] Serial is connected (00:00:00:00:00:00)
[WS ] Connecting to wss://remote-mgmt.edgeimpulse.com
[WS ] Connected to wss://remote-mgmt.edgeimpulse.com
[SER] Could not read serial number for device, defaulting to 000000000000

[SER] Detecting data frequency...
[SER] Detected data frequency: 96Hz
```

The utility should detect that 6 sensor axes were detected and will prompt you to provide a title for each axes:

```
? 6 sensor axes detected (example values: [-466,-6006,-14900,-44,-54,36]). What do you want to call them? Separate the names with ',':
```

You can paste the following headers into your terminal:

```
Acc_x,Acc_y,Acc_z,Gyro_x,Gyro_y,Gyro_z
```

You will no be prompted to provide a name for the device:

```
? What name do you want to give this device? RASynBoard
```

If you only have one project in your Edge Impulse account, the tool will automatically select that project to import data into. If you have more than one, you will be presented with a menu of projects to select from. Once the tool is associated with a project, you should see the following:

```
[WS ] Device "RASynBoard" is now connected to project "RASynBoard-Forwarder". To connect to another project, run `edge-impulse-data-forwarder --clean`.
[WS ] Go to https://studio.edgeimpulse.com/studio/251679/acquisition/training to build your machine learning model!
```

Navigate to the acquisition link that the tool provides. You should be presented with the following screen:
![Edge Impulse Acquisition Screen](assets/acq_landing_screen.png "Edge Impulse Acquisition Screen")

On the right side of the screen, there's a `Collect Data` pane. This is where you specify the label for the data being collected, along with the sample size. In this case, `circle` has been entered for the label and a sample size of 5000 ms will be collected.
![Collect Data Pane](assets/acq_collect_data.png "Collect Data Pane")

Click the `Start sampling` button and press the user button on the RASynBoard. At this point, the forwarder will start sampling the sensors on the device.

Once the sample period has completed, the data should be uploaded to Edge Impulse and you should see the samples appear:
![Collected Samples](assets/acq_sampled_data.png "Collected Samples")

More information on the data forwarder can be found at the following link: https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-data-forwarder