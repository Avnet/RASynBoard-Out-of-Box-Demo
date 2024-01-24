# RASynBoard Video Series

This document provides links to short videos designed to help users get the most out of the Avnet RASynBoard Out of Box example application.

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

## Changing ML models
This video shows the user how to select different ML models that are included with the OOB releases.  Additionally, the details shown here can help users define custom deployment using their own custom ML models designed in [Edge Impulse Studio](http://edgeimpulse.com).
- [Video Link (3 minutes)](http://avnet.me/RASynChangingMlModelsVideo)
- [Documentation Link](./ApplicationUserGuide.md/#ndp120-configuration)

## Record Training Data 
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
- [Video Link (25 minutes)](http://avnet.me/IoTConnectOnAWS)
    - 2:10: Review the IoTConnect device template
    - 5:10: Create a new device
    - 6:48: Download device certificate and keys 
    - 7:58: Configure the OOB application to connect to our new Thing
    - 10:14: Setting up a WiFi connection 
    - 11:30: Fix the certificate configuration 
    - 13:20: Exercise the application and confirm Device to Cloud (D2C) messages
    - 16:19: **Bonus Materials** Create a custom  IoTConnect dashboard in under 10 minutes!
- [Documentation Link](./IoTConnect.md)