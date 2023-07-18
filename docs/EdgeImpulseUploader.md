# Using Edge Impulse Uploader

The RASynBoard can be used to collect IMU data to upload to Edge Impulse for training or testing.

## Prerequisites

In order to use the data uploader, you must have NodeJS with npm installed. Please refer to the NodeJS website for installation: https://nodejs.org/en/download/package-manager

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
Recording_Period=30    # in seconds

[IMU data stream]
# Select the feature:  0->Disable;  1->Enable
Write_to_file=1        # write data to a file in sdcard
Print_to_terminal=0    # output data to the serial debug terminal

[Low Power Mode]
# 0->Enter Low Power Mode with "down . . down" command; 1->Enter Low Power mode automatically after each feature detection 
Power_Mode=0

```
Remove the microSD card and insert into the RAsynBoard.

Power on the RASynBoard.

## Capturing Data

### Run CSV Wizard

Create a free account with Edge Impulse and create a new project if one isn't already created: https://studio.edgeimpulse.com/studio/profile/projects

Press the user button to start recording IMU data.

Navigate back to the projects page and select your project. In the project, select the `Data Acqusition page. Startup the CSV Wizard by clicking CSV Wizard:
![Edge Impulse Acquisition Screen](assets/acq_upload_button.png "Edge Impulse Acquisition Screen")

Remove the microSD card from the RASynBoard and mount it on your computer. Click select file and select the IMU csv file:
![Select File](assets/acq_select_file.png "Select File")

You should be presented with a preview of the uploaded data.

Remove the microSD card from the RASynBoard and mount it on your computer. Click select file and select the IMU csv file:
![Preview Data](assets/acq_preview.png "Preview Data")

Click the next button.

You'll be presented with a pane to specify the type of data. Ensure that the time series option is slected and the option stating each column contains a reading and fill row contains a sample:
![Step 3](assets/acq_upload_step_3.png "Step 3")

Click the next button.

On the pane, select no for the first option. For first column, select `Acc_x` and last column select `Gyro_z`.

For the list of sensors, enter:

```
accel,gyro
```

For data frequency, enter 100Hz
![Step 4](assets/acq_upload_step_4.png "Step 4")

Finally, select finish wizard.

# Setup and Run Uploader

Run the following command in a terminal window:

```bash
edge-impulse-uploader --label <LABEL_NAME> --category <training|testing> <PATH TO CSV FILE>
```

Replace `<LABEL_NAME>` with the desired label name. After `--category` specify whether the data is used for trainging or testing. Finally, specify the path of the file you're uploading. 

Example:

```bash
edge-impulse-uploader --label circle --category trainging ./ndp_imu_record_0000.csv
```

At this point, you should be prompted with your Edge Impulse credentials:

```
Edge Impulse uploader v1.20.0
? What is your user name or e-mail address (edgeimpulse.com)? evan.stoddard@avnet.com
? What is your password? [hidden]
```

If you have more than one project, the following menu will be presented:

```
Upload configuration:
    Label:       circle
    Category:    training

? To which project do you want to upload the data? 
  Evan Stoddard / RASynBoard-Forwarder 
❯ Evan Stoddard / RASynBoard-Uploader 
```

Select the desired project. If there's only one project on your account, it will automatically be selected.

At this point the file should be uploaded:

```
[1/1] Uploading /ndp_imu_record_0000.csv OK (1626 ms)

Done. Files uploaded successful: 1. Files that failed to upload: 0.
```

The data should appear on the edge impulse data aqcuisition page:

More information on the data uploader can be found at the following link: https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-uploader