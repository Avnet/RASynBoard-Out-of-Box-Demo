# RASynBoard Video Series

This document provides links to videos designed to help users get the most out of the Avnet RASynBoard Out of Box example application.

## Using RASynBoard Out-of-Box Releases
This video walks through the process of using the Renesas Flash Programmer to load one of the tested releases onto your RASynBoard.
- [Video Link (11:34)][http://avnet.me/RASynUsingReleasesVideo]
    - 0:45: Review of releases
    - 1:45: Review of the documentation
    - 2:50: Review of the prerequisites
    - 4:15: Configure the hardware for the Renesas Flash Programmer (RFP)
    - 5:10: Download the latest OOB release
    - 5:30: Run the RFP
    - 6:00: Configure the RFP to connect to our board
    - 7:20: Flash the image to the RASynBoard 
    - 8:15: Configure the hardware to auto-boot
    - 9:10: Update the microSD card files
- [Documentation](./UsingRASynbBoardReleases.md)

## Getting Started
This video walks through the [Getting Started Guide](RASyBoardGettingStarted.md) and reviews the process of cloning the OOB repo, building the application, loading it onto the RASynBoard and running the application using the Renesas e^2 studio debugger.  Common issues are reviewed along with details on how to overcome them.
- [Video Link (23 minutes)](http://avnet.me/RASynGettingStartedVideo)
    - 1:50: Clone the Repo
    - 2:50: Import the project into the e^2 studio IDE
    - 4:30: Configure and Build the application
    - 7:15: Setup the RASynBoard Hardware
    - 8:35: Setup the debugger
    - 10:20: Run the application in the debugger (including failed debug sessions and troubleshooting)
    - 13:20: Copy the required files onto the microSD card 
    - 16:00: Exercise the 5-keyword model
    - 19:20: Configure the hardware to automatically start the application on power-up
    - 20:45: Capture audio data on the microSD card
- [Documentation](RASyBoardGettingStarted.md)

## Understanding Debug Output
This video reviews the different ways the user can view application debug information.  
- [Video Link (5 minutes)](http://avnet.me/RASynUnderstandingDebugVideo)
- [Documentation Link](./ApplicationUserGuide.md/#debug-print)

## Troubleshooting issues with the e^2 debugger
This video captures common issues the developer may encounter when trying to use the Renesas e^2 debugger with the RASynBoard OOB application
- [Video Link (7 min)](http://avnet.me/RASynDebuggerIssuesVideo)
- [Documentation Link - Setting up the debugger](./RASyBoardGettingStarted.md#debug-the-project)
- [Documentation Link - Troubleshooting debugger issues](./RASynTroubleshootingGuide.md#debugger-fails-to-load-the-device-r7fa2a1ab-set-in-the-launch-configuration-does-not-match)
 
## Changing ML models
This video shows the user how to select different ML models that are included with the OOB releases.  Additionally, the details shown here can help users define custom deployment using their own custom ML models designed in [Edge Impulse Studio](http://edgeimpulse.com).
- [Video Link (3 minutes)](http://avnet.me/RASynChangingMlModelsVideo)
- [Documentation Link](./ApplicationUserGuide.md/#ndp120-configuration)

## Send Audio or IMU Sensor Data directly into your Edge Impulse ML Project
This video shows how to use the ```edge-impulse-daemon``` utility and Edge Impulse Studio to send RASynBoard sensor data directly into your Edge Impulse Project.
- [Video Link (28 minutes)](http://avnet.me/RASynDataIngestionVideo)
    - 0:00: Introduction
    - 1:26: Pros and Cons to using the Edge Impulse RASynbBoard application to capture training data
    - 3:03: Prerequisites
    - 3:45: Where to find the Edge Impulse RASynBoard application 
    - 6:57: Update the microSD card files
    - 9:00: Flash the Edge Impulse RASynBoard application onto our board
    - 12:35: Configure the application to capture audio data
    - 15:57: Checkpoint
    - 16:30: Create a new Edge Impulse Project
    - 17:10: Connect the RASynbBoard to your Edge Impulse project
    - 20:36: Capture audio data 
    - 24:30: Capture 6-Axis IMU data
- [Documentation Link](./docs/RASynEdgeImpulseDataIngestion.md):

## Record Training Data to the microSD card
This video shows how to use the Avnet RASynBoard OOB example application to capture audio and IMU data to the microSD card.  Once the data has been captured, it can be uploaded to [Edge Impulse Studio](http://edgeimpulse.com) to train a new ML model or to test an existing model
- [Video Link (11 minutes)](http://avnet.me/RASynRecordingTrainingDataVideo)
    - 1:10: Capture audio data to the microSD card
    - 4:50: Capture IMU data to the microSD card
    - 9:30: Stream IMU data to the debug UART
- [Documentation Link](./ApplicationUserGuide.md/#recording-period)

## Changing Inference LED colors
One of the OOB features is the ability to define different LED colors corresponding with the inference index.  
- [Video Link (3 minutes)](http://avnet.me/RASynChangeLedColorsVideo)
- [Documentation Link](./ApplicationUserGuide.md/#led-control)

## AWS IoT Core
In this video we walk through adding an AWS IoT Core Thing.  We'll download device certificates/keys, configuring the OOB application and exercise the data connection to send and receive MQTT date to/from AWS.
- [Video Link (18 minutes)](http://avnet.me/RASynAwsIotCore)
    - 1:50: Add a new AWS IoT Core Thing 
    - 4:53: Download device certificate and keys 
    - 7:30: Verify the hardware configuration is set to autoboot and microSD card access is available 
    - 8:36: Configure the OOB application to connect to our new Thing
    - 11:30: Loading X.509 certificates to the device 
    - 12:36: Setting up a WiFi connection 
    - 13:30: Exercise the application and confirm Device to Cloud (D2C) and Cloud to Device (C2D) data transfer
- [Documentation Link](./awsIoTCore.md)

## Configuring WiFi Network on the RASynBoard
This video shows the two different ways to configure the WiFi network on your RASynBoard
- [Video Link (9 minutes)](http://avnet.me/RASynWiFi)
- [Documentation Link](./ApplicationUserGuide.md/#wifi-access-point-configuration-source)

## Managing X.509 Certificates
This video shows two different ways to load X.509 certificates onto the RASynBoard's DA16600 device and three ways to configure the OOB application to use the certificates
- [Video Link (10 minutes)](http://avnet.me/RASynCertificates)

## Avnet's IoTConnect on AWS
This video walks through adding the RASynBoard to IoTConnect on AWS.  We'll download the device certificate/keys, configure the OOB application, an exercise the data connection by sending MQTT telemetry up to IoTConnect.  The video includes bonus content not currently documented showing the user how to create a custom dashboard to showcase the telemetry data in near real-time and how to share the dashboard for others to utilize.
- [Video Link (31 minutes)](http://avnet.me/IoTConnectOnAWS)
    - 2:30: Prerequisites
    - 3:32: Cloud account
    - 4:15: Create a new device
        - 4:45: Import a device template
        - 6:29: Review the template
        - 9:15: Create the device
        - 10:10: Download the device certificates
    - 11:00: Checkpoint
    - 11:15: Collect the IoTConnect details required to connect our board to IoTConnect
    - 10:50: Configure the OOB application to connect to IoTConnect
    - 18:30: Run the demo
    - 22:30: IoTConnect Dashboards
        - 26:40: Exercise the Dashboard
        - 27:50: Run a different ML Model
    - 30:22: Conclusion
- [Documentation Link](./IoTConnect.md)

# Avnet ML Workshop recordings

## 1/17/24 ML Workshop full recording
On 1/17/2024 Avnet hosted a webinar on [Element14.com](http://element14.com)
- [Link to full recorded webinar (60 min)](https://community.element14.com/learn/events/c/e/1695)
    - Click on the "Watch On-Demand" link to see the full video

## 1/17/24 ML Workshop Lab1
This video was recorded for an Avnet hosted ML webinar shows how to create a new ML model for the RASynBoard in [Edge Impulse](https://edgeimpulse.com)
- [Video Link (17 minutes)](http://avnet.me/RASynMlWorkshopLab1Video)
    - 0:40: Connect the RASynBoard to Edge Impulse using the Edge Impulse Daemon
    - 2:40: Capture audio data to Edge Impulse
    - 4:00: Break 5 seconds of audio into 4 1-second clips
    - 5:20: Review the Impulse (model processing and learning blocks)
    - 6:45: Review the Syntiant processing block
    - 8:00: Review the classifier learning block/Train the model
    - 10:42: Run the test dataset through the model in Edge Impulse
    - 11:50: Deploy the model
    - 14:00: Move the model to the RASynBoard and test

## 1/17/24 ML Workshop Lab2
This video was recorded for an Avnet hosted ML webinar shows how to exercise a RASynBoard OOB release
- [Video Link (26 minutes)](http://avnet.me/RASynMlWorkshopLab2Video)
    - 1:40: Flash the Avnet Out-of-Box application onto the RASynBoard
    - 4:15: Reconfigure the device to auto-boot and review the config.ini file changes to load the 5-keyword ML model
    - 6:00: Exercise the 5-keyword model
    - 7:00 Use the "down" "down" command to put the device into low-power mode
    - 8:35: Record and playback audio to/from the microSD card
    - 10:40: Change the model and exercise the circular motion model
    - 12:02: Configure the device to connect to AWS
    - 18:19: Configure the device to connect to Avnet's IoT Connect
    - 20:50: Flash the NDP120 images and the configuration to SPI flash
    - 24:24: Run the model on the core board only powered from a battery
