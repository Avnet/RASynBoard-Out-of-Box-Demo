# Using Edge Impulse RASynBoard application

The Edge Impulse team has developed a custom RASynBoard application that can capture audio or IMU data from the RASynBoard's onboard sensors and transmit the captured data directly into your Edge Impulse project using the Edge Impulse Daemon utility running on your development PC.

The Avnet Out-of-Box application has the ability to send IMU data to your Edge Impulse project using the [Edge Impulse Data Forwarder](./EdgeImpulseDataForwarder.md) utility, but if you are capturing audio data with the Avnet Out-of-Box application you need to record the data to the microSD card and then [upload the audio file to Edge Impulse](./EdgeImpulseUploader.md). 

If you use the Edge Impulse RASynBoard application, you can capture IMU or audio data directly from your RASynBoard's sensors directly into your Edge Impulse project.  This workflow is optimized to help developers capture training or testing data directly into your Edge Impulse project.

- [Video Link (28 minutes)](http://avnet.me/RASynDataIngestionVideo)
    -Video walking through this document

[Edge Impulse RASynBoard Documentation](https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/avnet-rasynboard)

# Table of Contents

- [Pros and cons to using the Edge Impulse RASynBoard Application](#pros-to-using-the-edge-impulse-rasynboard-application)
- [Prerequisites to using this tutorial](#prerequisites)
- [Configure the Application](#configure-the-application)
- [Checkpoint](#checkpoint)
- [Collect Audio Data](#log-into-your-edge-impulse-account)
- [Collect IMU Data](#capture-6-axis-imu-data)
- [Conclusion](#conclusion)
- [Troubleshooting Tips](#troubleshooting-tips)

## Pros to using the Edge Impulse RASynBoard application:
- Capture audio or IMU data directly to your Edge Impulse project
- Drive the data capture workflow directly from the Edge Impulse project
    - Review the data and segment data into multiple files as you capture the data
    - Label the data as it's captured
- Test your ML models without having to load the Avnet Out-of-Box application

## Cons to using the Edge Impulse RASynBoard application:
- You must flash the Edge Impulse RASynBoard application to your RASynBoard
- The Avnet Out-of-Box features like cloud connectivity are not implemented in the Edge Impulse RASynBoard application

## Prerequisites

### Install NodeJS with npm
In order to use the data forwarder, you must have NodeJS with npm installed. Please refer to the NodeJS website for installation: https://nodejs.org/en/download/package-manager

### Install the Edge Impulse CLI tools

Once you have installed NodeJS and npm, install the Edge Impulse CLI tools with the following command:

MacOS/Linux:

```bash
sudo npm install -g edge-impulse-cli
```

Windows:

```Powershell
npm install -g edge-impulse-cli
```
### Load the Edge Impulse RASynBoard Application onto your device

### Where to find the application

Every time your create a RASynBoard deployment in Edge Impulse Studio the Edge Impulse RASynBoard application is built and delivered as part of the deployment.

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion15.jpg width="500">
<br />

Starting with the [Avnet Out-of-Box v.1.5.0 release](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/releases/tag/v1.5.0), we include this application in the Release Assets section. 

**Note** the *.srec image file is contained inside the *.zip file and must be extracted before loading onto the RASynBoard.

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion01.jpg width="700">
<br />

### Load the latest microSD card files to your RASynBoard's microSD card

You should have the latest RASynBoard Out-of-Box release microSD card files on your microSD card.  Unzip the compressed file and copy all the files to the root directory of your microSD card.

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion03.jpg width="700">
<br />


#### Flash the Edge Impulse RASynBoard application to your RASynBoard

The [UsingRASynBoardReleases.md](./UsingRASynbBoardReleases.md#how-to-load-a-release-onto-the-rasynboard-evk) documentation includes details on . . . 

- Installing the Renesas Flash Programmer (RFP)
- Using the RFP to flash a *.srec file to your board
    - Be sure to flash the ```firmware-avnet-rasyn*.srec``` file

## Prepare the RASynBoard to auto run

Install the J3 Jumper and connect your USB-C cable to the core board as shown 

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion09.jpg width="500">
<br />

## Configure the application

Similar to the Avnet RASynBoard Avnet Out-of-Box application you can configure the Edge Impulse RASynBoard application by editing the ```config.ini``` file in the root directory of the microSD card.

### Define the data output port ```[Debug Print]-->Port=x```

The ```edge-impulse-daemon``` will connect to your RASynBoard using a COM port on your development PC.  Just like we can define where debug data will be directed in the Avnet Out-of-Box application, we can define where the Edge Impulse RASynBoard application will output the sensor data.  This is done using the ```[Debug Print]-->Port=x``` configuration item. 

**Note**: You'll see higher data transfer rates if you use the VCOM port ```Port=2```

- ```Port=1``` Outputs sensor to the PMOD connector signals
- ```Port=2``` (Preferred) Outputs sensor to the UART associated with the core board USB-C connector

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion04.jpg width="600">
<br />

### Define the ML model to correspond with the data type to capture ```Mode=x```

We need to tell the Edge Impulse RASynBoard application which ML model to load. If we want to collect IMU data, we must use an IMU model, and likewise, if we want to capture audio data we must load an audio ML model.

#### Capture audio data

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion05.jpg width="600">
<br />

#### Capture IMU data

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion06.jpg width="600">
<br />

## Checkpoint

At this point we have . . . 
- Loaded the Edge Impulse RASynBoard application onto our board
- Copied the microSD card files from the most recent Avnet Out-of-Box release to our microSD card
- Configured the output port
    - ```[Debug Print]-->Port=2```
- Configured the ML model to correspond to the type of data we want to capture
    - ```Mode=4``` to capture Audio data
- Configured our board to auto run the application (installed the J3 jumper across pins 1-2)
- Connected our USB-C cable to the core board's USB-C connector

Now we can collect some data!

### Log into your Edge Impulse account

[Edge Impulse](https://studio.edgeimpulse.com)

### Create a new project

- Select the **+ Create new project** link

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion07.jpg width="700">
<br />

The **Create a new project** pop-up opens

- Give your project a name
- Select the **Personal** project type
- Select the **Public** project setting
- Click the **Create new project button**

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion08.jpg width="500">
<br />

### Connect your board to the project

This is one of the coolest features of Edge Impulse.  They provide all the tools I need to connect my device directly to my project.  Once the connection is established, I can define the data I want to collect including how I want to label the data.  Let's give it a shot . . .

For this example lets collect some audio data

#### Start the ```edge-impulse-daemon```

The ```edge-impulse-daemon``` utility was installed with the Edge Impulse CLI tools.  

1. Open a terminal on your development PC
    1. Windows PowerShell is supported however my work laptop restricts me from running the ```edge-impulse-daemon```
    1. I use the Windows command shell ```cmd```
2. The ```edge-impulse-daemon``` runs and prompts for . . . 
    - Your Edge Impulse login credentials
    - Which COM port is connected to your board
        - If you have multiple COM ports you'll be prompted to select the correct port
        - If you only have one COM port it will automatically use that one
    - Which Edge Impulse project to connect to
        - Select from the list
    - Name the device
        - Enter a name for your board, this will show up in Edge Impulse Studio
3. A URL is output that will take you directly to your Edge Impulse project and the Data acquisition tab!

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion0A.jpg width="900">
<br />

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion0B.jpg width="800">
<br />

## Capture Audio Data

At this point we can capture data using the **Collect data** controls.  In the **Collect data** window . . .

1. Define a label for your data
    - Use a name that describes your data
    - Labels are case sensitive
1. Select **Microphone** for the Sensor
1. Leave the Sample length (10 seconds), and Frequency at the defaults
1. Click the **Start sampling** link

This is where the magic happens!

- Edge Impulse will send a message down to the daemon telling it to collect 10 seconds of audio data
- The daemon will send a message to the RASynBoard app, telling it to collect 10 seconds of audio data
- The RASynBoard app starts to flash the Blue LED indicating that it's recording
- Start to repeat the keyword until the Blue LED stops flashing "Avnet  Avnet   Avnet . . ."
- The RASynBoard collects the data and transfers it to the daemon
- The daemon sends the data to your Edge Impulse project where it's displayed!

Once the data is in my project I can . . .
- Play back the audio using the audio control
- View what my data looks like

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion0C.jpg width="700">
<br />

I can also drill down into the data and split my sample into multiple files, each containing one instance of my keyword.

- In the **RAW DATA** audio control, click on the vertical ellipsis (upper right hand corner)
- Select the **Split sample** option

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion0D.jpg width="350">
<br />

The interface opens a new window and tries to identify unique segments and allows you to . . . 

- Play each segment 
    - Select a segment to play it
- Drag the segment windows around
- Remove segment windows
- Add segment windows
1. Click on the **Split** link

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion0F.jpg width="700">
<br />

The Dataset list updates.  Notice that the single 10 second sample is now replaced with multiple 1 second samples.

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion14.jpg width="700">
<br />

## Capture 6-Axis IMU Data

To capture IMU data from the RASynBoard we need to change the ML model that gets loaded 

1. Open the config.ini file on the microSD card
1. Change ```Mode=5``` to load the IMU model

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion06.jpg width="500">
<br />

1. Power cycle your RASynBoard to take the new configuration
1. Start the ```edge-impulse-daemon```
1. If you want to connect the ```edge-impulse-daemon``` to a different Edge Impulse project use the ```--clean``` argument
    - ```edge-impulse-daemon --clean```
1. In Edge Impulse define a new label **SideToSide**
1. Set the sensor to **Inertial**
1. Click the **Start sampling** link

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion10.jpg width="350">
<br />

1. The magic happens again!
1. Move your device side to side while the blue LED flashes

The data is captured and sent up to your Edge Impulse Project!

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion11.jpg width="600">
<br />

# Conclusion

I hope you were able to experience how the Edge Impulse RASynBoard application can optimize your data collection workflow.  If you have any issues or questions, please open a [new issue](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues) in the GitHub Repo.

## Next Steps

Now that you have your training data in Edge Impulse Studio, you can follow the Edge Impulse Tutorials detailing how to create ML models targeting the RASynBoard's Syntiant NDP120 Neural Decision Processor.

- [Create a keyword spotting model](http://avnet.me/RASynKeyWordSpottingModelCreation)
- [Create a motion detection model](http://avnet.me/RASynMotionModelCreation)

# Troubleshooting Tips

I've tried to anticipate issues that developers may encounter with this tutorial and captured some troubleshooting tips.

## ```edge-impulse-daemon``` does not connect to the board

- Verify that the microSD card is inserted into the I/O board
    - Since the config.ini file in on the microSD card, it must be present
- Verify you've selected the correct COM port when starting the ```edge-impulse-daemon```
    - Verify that the COM port selected is connected to the RASynBoard on the port indicated by the ```[Debug Print]-->Port=x``` setting
        - ```[Debug Print]-->Port=2``` the data will be on the USB-C connector on the core board (the smaller board)
        - ```[Debug Print]-->Port=1``` the data will be on the PMOD connector and you must use a [USB to TTL serial cable](https://www.adafruit.com/product/954?gad_source=1&gclid=Cj0KCQiAkKqsBhC3ARIsAEEjuJhpUsBOc-uVFZactNus_zYppARCn-KheovcQzORDKxqxlDabnSwBTYaAtw7EALw_wcB), connect it to the PMOD connector as shown below

<p align="center">
    <br />
    <img src=./assets/images/image-6.png width="250">
<br />


- Try rebooting your PC to clean up any COM port errors

## Sampling Failed Error 

### Trying to capture audio data with an IMU model loaded
If you try to capture audio data when an IMU model is loaded, you'll see this error

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion12.jpg width="600">
<br />

- Edit the config.ini file and set ```Mode=4``` to load the audio model

### Trying to capture IMU data with an audio model loaded
If you try to capture IMU data when an audio model is loaded, you'll see this error

<p align="center">
    <br />
    <img src=./assets/images/EIDataIngestion13.jpg width="600">
<br />


- Edit the config.ini file and set ```Mode=5``` to load the IMU model
