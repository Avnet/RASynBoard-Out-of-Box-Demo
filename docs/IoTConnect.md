# IoTConnect Instructions

# Introduction

The RASynBoard is architected for ML at the Edge.  We can use ML inference events/detections to drive local hardware such as a GPIO to shut down a motor that is about to harm itself.  But how does a service team know that there's a problem with the motor?  Without cloud connectivity, the service team must physically put eyes on the motor or the motor control panel to see that there is a problem.  This is where cloud connectivity and the Internet of Things (IoT) can help.  We can connect our board to a cloud solution and not only periodically send data detailing the health of the motor (or whatever we're monitoring), but we can also send up critical events.  On the cloud side we can monitor for critical events and when they occur, we can programmatically generate a service ticket, or shoot off an email or text to the service technician informing them of the issue.

This Avnet RASynBoard OOB application supports connecting your device to either AWS IoT Core, or Avnet's IoTConnect on AWS.  This tutorial will guide you through all the steps required to send ML events up to the cloud and even show them on a dashboard in near real-time.

This document includes all the details required to . . . 

1. Request a free IoTConnect on AWS trial account
1. Create a new IoT device in IoTConnect on AWS
1. Configure the Avnet Out-of-Box (OOB) application to connect to the new device on IoTConnect
1. Run the demo and verify that inference data is sent to IoTConnect
1. Use a template to create a pre-built IoTConnect dashboard

# Table of Contents

1. [Prerequisites](#prerequisites)
1. [Video Walkthrough](#video-walkthrough)
1. [Cloud Account Setup](#cloud-account-setup)
1. [Create a device on IoTConnect](#create-a-new-device-on-iotconnect)
    1. [Import a device Template](#create-a-device-template)
    1. [Create a new device](#create-a-new-device)
    1. [Download device X.509 certificates for the new device](#download-the-certificates-for-your-new-device)
1. [Checkpoint](#checkpoint)
1. [Collect details to connect the device to IoTConnect](#collect-required-details-from-iotconnect-on-aws)  
1. [Configure the RASynBoard OOB application to connect to IoTConnect](#configure-the-avnet-out-of-box-application)
1. [Run the demo](#run-the-demo)
1. [IoTConnect Dashboards](#iotconnect-dashboards)
1. [Exercise the Dashboard](#exercise-the-dashboard)
1. [Change the ML model](#load-a-different-ml-model)
1. [Conclusion](#conclusion)  

# Prerequisites

1. You have an [Avnet RASynBoard EVK](http://avnet.me/rasynboard)
1. You have a microSD card for the EVK
1. You have a USB-C cable to connect the EVK to your development PC
1. You have completed the [RASynBoard Getting Started Guide](./RASyBoardGettingStarted.md) OR You have loaded the [most recent release, easier,](./UsingRASynbBoardReleases.md) onto your RASynBoard
1. You have exercised the **5-keyword ML model "up", "down", "back", "next", "ok-syntiant"**
1. You have a [USB to TTL debug cable](https://www.adafruit.com/product/954) to view debug from the OOB application

# Video Walkthrough

There is also a [video](http://avnet.me/IoTConnectOnAWS) showing how to complete this tutorial.  

# Cloud Account Setup
This guide requires an IoTConnect account on AWS.

>**NOTE:**  
> If you have already created an IoTConnect account on AWS, or were provided an account as part of a training or workshop, skip this section.

If you need to create an account, a free 2-month subscription is available.
Please follow the 
[Creating a New IoTConnect Account](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md)
guide and ensure to select the [AWS version](https://subscription.iotconnect.io/subscribe?cloud=aws) during registration:

![IoTConnect on Azure](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/media/iotc-aws.png)

Once you have an account on IoTConnect come back to this document to move forward . . .

# Create a New Device on IoTConnect

1. Open the IoTConnect website.  The URL will depend on your account, your welcome email will have the URL to open IoTConnect.
2. Here are the high level tasks we need to complete in IoTConnect . . . 
    1. Import a device template
    1. Create a new IoT device
    1. Download the certificates for your device

## Create a Device Template

The first thing we need to do is create a device template, since the template is a required input when we create a new IoTDevice.  A device template is a self-contained file that defines a set of resources, configuration, and rules for devices on IoTConnect.  

You can manually create or import a pre-defined template.  In this tutorial, we'll import a template.  Importing a template is a quick way to use a template that someone else has already created.  Templates can be large, and manually inputting all the details can be tedious.  I've included [a template](./assets/IoTConnect/RASyV15_template.JSON) that works with the OOB application in this GitHub repo.  You can also manually edit an existing template, it's just a JSON document.  However, editing templates is beyond the scope of this document.

1. Open the **devices** page

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices.jpg width="250">
<br />

2. Select the **Templates tab** from the bottom of the page

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate1.jpg width="500">
<br />

3. Click on the **Create Template** button in the upper right hand corner of the browser window

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate2.jpg width="150">
<br />

4. Click on the **Import** button in the upper right hand corner

<p align="center">
    <br />
    <img src=./assets/images/iotcImport.jpg width="100">
<br />

5. The **Import** dialog opens
    1. Use the **Browse** link to find the ```/docs/assets/IoTConnect/RASyV15_template.JSON``` file
    1. Click the **Save** button

<p align="center">
    <br />
    <img src=./assets/images/iotcImport1.jpg width="500">
<br />

6. The template is imported and the **Edit Template** page opens
7. Select the **Attributes (16)** tab

Here you can review the contents of the template.  **attributes** are the telemetry items that the application will send to IoTConnect each time the NDP120 detects an inference event, i.e., "Up."  When we define attributes we're typically defining simple ```{"key": value}``` JSON pairs.  The application must use the exact same key when sending telemetry data.  The ```keys``` defined in the template match the Avnet OOB application implementation.  If you change or add additional telemetry items, just make sure the application matches your IoTConnect template attributes.  

Below is an example of a simple ```{"key": value}``` pair ```{"modeDescription": "<some string>"}```

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate3.jpg width="800">
<br />

Here's the line of code that constructs the telemetry JSON:  

``` 
    // Create the JSON
    snprintf(telemetryMsg, sizeof(telemetryMsg), "{\"modeDescription\":\"%s\",\"numNetworks\":%d,\"numLabels\":%d}",
                                                 get_mode_description(), total_nn, num_labels);
```

Here's the debug you'll see when the application first connects to IoTConnect:

```
{"modeDescription":"5-keywords Single Mic","numNetworks":1,"numLabels":6}
```

The imported template also includes attributes that are JSON Objects ```label_0 -- label_9```.  If we look under the hood at one of these items you'll see that it's made up of three simple ```{"key": value}``` pairs.  We decided to use objects so that the application can load any ML model and populate the label_x objects to describe the inference events for any model.  

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate5.jpg width="400">
<br />

When the application first connects to IoTConnect it sends up details about the model that's loaded, then using data directly from the model it sends up JSON objects detailing the inferenceIndex ```infIdx```, the Inference String ```infStr``` and the initial Inference Count ```infCnt``` of zero.  When we run the IoTConnect dashboard, you'll see how these objects are utilized.

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate6.jpg width="750">
<br />

<p align="center">
    <br />
    <img src=./assets/images/iotcTemplate7.jpg width="700">
<br />

## Create a New Device

1. Open the **devices** page

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices.jpg width="200">
<br />

2. Select the **Devices tab** from the bottom of the page

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices1.jpg width="100">
<br />

3. Select the **Create Device** button in the upper right hand corner

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices2.jpg width="150">
<br />

4. The **Create Device** form opens.  
    1. Give your device a Unique ID
    1. Fill out a Display Name
    1. For the Entity field, you likely only have one option and it will be different than mine; select the only option shown in your interface. 
    1. Select the template we just imported **RASyV1_5**
    1. Leave the default **Auto-generated (recommended)** selection for the Device Certificate
    1. Click on the **Save** button 

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo7.jpg width="500">
<br />

5. Your device will be shown in the devices list

## Download the certificates for your new Device

When we created the new device with **Self-Signed Certificates** IoTConnect generated the device certificate and the device public key certificate.

1. Open the device by clicking on the Unique ID link, in my case **RASynBoard**.  

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices4.jpg width="1000">
<br />

2. Click on the **Connection Info** link 

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices5.jpg width="700">
<br />

3. The **Connection Info** pop-up opens, showing all the nitty gritty details for your device on AWS.  
4. Click on the certificate download link

<p align="center">
    <br />
    <img src=./assets/images/iotcCerts1.jpg width="700">
<br />

5. Your certificates are downloaded to your **Downloads** folder
6. Close the **Connection Info** pop-up window

<p align="center">
    <br />
    <img src=./assets/images/iotcCerts2.jpg width="200">
<br />

<p align="center">
    <br />
    <img src=./assets/images/iotcCerts3.jpg width="350">
<br />

# Checkpoint

So far we have . . . 

- Requested a free IoTConnect on AWS account
- Received the free account access
- Imported a device template describing the telemetry items that the OOB application sends
- Created a new device using the template
- Downloaded the device's certificates

# Update the Avnet Out-of-Box application with details for our new device

At this point it's important that all the prerequisites detailed at [the top of this document](#prerequisites) have been completed.  Please review them and make sure you're ready to proceed.

## Collect required details from IoTConnect on AWS

There are three IoTConnect details that we need to configure the OOB application to connect as our new device.  We'll add them to the config.ini file on the RASynBoard microSD card.  I recommend opening up a text document to capture these items.   

### Company ID (CPID) and Environment (env)

The company ID and the Environment are both displayed on the **Key Vault** page. 
> **Note** you can copy the strings by selecting the copy link to the left of the text

<p align="center">
    <br />
    <img src=./assets/images/iotcKeyVault1.jpg width="200">
<br />

<p align="center">
    <br />
    <img src=./assets/images/iotcKeyVault2.jpg width="500">
<br />

### Device Unique ID 

The Unique ID can be found on the Devices page

<p align="center">
    <br />
    <img src=./assets/images/iotcDevices.jpg width="200">
<br />

Find your device and copy the Unique ID by using the copy link to the left of your device's Unique ID

<p align="center">
    <br />
    <img src=./assets/images/iotcDevicesID.jpg width="900">
<br />

# Configure the Avnet Out-of-Box application

We're getting close now!  The next step is to configure the OOB application by adding the device certificates and updating the config.ini file that's located on the microSD card.  Since you already completed the [RASynBoard Getting Started Guide](./RASyBoardGettingStarted.md) you updated your microSD card with all the files from the /ndp120/synpkg_files/* folder.  

## Copy the Device Certificates to the microSD card

1. Extract the two certificates from the zip file you downloaded from IoTConnect
1. Copy them to the microSD card into the /certs directory
1. The certs directory should already contain the AmazonRootCA1.pem file, if not copy it from the repo /ndp120/synkpg_files/certs folder

<p align="center">
    <br />
    <img src=./assets/images/iotcCerts4.jpg width="400">
<br />

## Update the Config.ini file

1. Open the config.ini file by either removing the microSD card from the RASynBoard and using a microSD card reader, or by running the OOB application and connecting your USB-C cable to the USB-C connector on the core board.

If you use the RASynBoard OOB application . . . 

1. Make sure the OOB application is loaded onto the device (if you completed the [RASynBoard Getting Started Guide](./RASyBoardGettingStarted.md) this is done)
1. Make sure that the Jumper is installed on J3 across pins 1-2 (this will auto boot the application)
1. Connect your development PC to the USB-C connector on the core board (NOT the I/O board)

<p align="center">
    <br />
    <img src=./assets/images/iotcMicroSDCardAccess.jpg width="400">
<br />

Once the application boots, the microSD card will enumerate on your development PC as an external USB drive

<p align="center">
    <br />
    <img src=./assets/images/iotcUsbDrive.jpg width="600">
<br />

1. Update the following items in your config.ini

<p align="center">
    <br />
    <img src=./assets/images/iotcConfig1.jpg width="600">
<br />

My updated file looks like this . . .

<p align="center">
    <br />
    <img src=./assets/images/iotcConfig2.jpg width="700">
<br />

# Run the demo

Now that everything is configured, we can run the demo!  Cycle power on the device by disconnecting/reconnecting the USB-C connector from your development PC.

> **Note**: When the application starts up it outputs the configuration so you can confirm that your settings are being used . . .

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo1.jpg width="600">
<br />

> **Note**: The debug will show the different states that the IoTConnect thread processes while connecting to IoTConnect.  If your device does not connect watch the debug for **ERROR** debug messages.

1. As soon as the MQTT connection is established, queued up telemetry messages are sent to IoTConnect.

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo2.jpg width="700">
<br />

The OOB application implements a queue for telemetry data.  This way if telemetry data is generated before the application is connected to IoTConnect, the message is not lost.  As soon as the MQTT connection is established the IoTConnect thread looks for messages in the queue and if present they get sent to IoTConnect!  Additionally, the application verifies that there is a valid MQTT connection before sending any telemetry.  If not, the state machine is rolled back to re-establish the MQTT connection.

3. Let's go back to IoTConnect and verify that the device is on-line
4. Open the **Devices page** and verify that your device is on-line

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo3.jpg width="800">
<br />

5. Now open your device by clicking on the **Unique ID** link
6. Click on the **Live Data** link at the left
7. If not automatically selected, select the **Telemetry** tab 

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo4.jpg width="400">
<br />

8. Now speak one of the keywords "Up"
> **Note** the debug shows the telemetry that's being sent

> **Note** that in IoTConnect the telemetry shows up in the Live Data --> Telemetry tab!

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo5.jpg width="800">
<br />

<p align="center">
    <br />
    <img src=./assets/images/iotcDemo6.jpg width="850">
<br />

# IoTConnect Dashboards

One of my favorite IoTConnect features is the ability to build a custom dashboard in minutes.  In this section we'll import a dashboard that I've already build.  

1. In the IoTConnect web interface select **Create Dashboard** from the top of the interface

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard1.jpg width="800">
<br />

2. The **Create Dynamic Dashboard** window opens
    1. Select the template we just imported **RASyV1_5**
    1. Select the device we just created **RASynBoard** for me
    1. Give your dashboard a name
    1. Use the **Browse** button to browse to the file [RASynBoard_dashboard_export.json](./assets/IoTConnect/RASynBoard_dashboard_export.json) from the [GitHub repo](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo)
    1. Click the **Save** button

3. The new dashboard opens in edit mode
4. Click the **Save** button to exit edit mode
5. The dashboard is displayed

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard3.jpg width="1200">
<br />

## Exercise the dashboard

To drive data to the dashboard just speak one of the keywords "up", "back," "down,", "next," "ok-syntiant."  If you review the dashboard you see some interesting data.

> **Note** the dashboard will work with any ML model loaded on the RASynBoard.  You just need to perform some action to trigger an event for the model you have loaded.  For example, if the circular motion model is loaded, just move the device in a circle.

The **Most Recent Event** Multi Tile shows the last inference event and it's inference index.  

> **Note** that the JSON ```key``` is displayed in the tile.

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard4.jpg width="300">
<br />

The **ML Inference Counts** show each label from the ML model and how many times each keyword was detected since power-up.  

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard5.jpg width="600">
<br />

The **Live Line Chart** shows each inference count and also the inferenceIdx that is sent with each keyword detection.  Speak the different keywords to drive new data to the chart.

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard6.jpg width="1100">
<br />

### Load a different ML model
Remember how we implemented the ```label_x``` telemetry objects.  Because of that the **ML Inference Count** Multi tiles can display the inference string from any model that gets loaded.  Let's try the Alexa model.  This model is not real interesting but its performance is outstanding.

1. Open the config.ini file from the microSD card
2. Change the ```Mode``` setting to ```Mode=6```

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard7.jpg width="600">
<br />

3. Power cycle your RASynBoard for the new configuration to take effect
> **Note** that the Alexa Single Mic model is loaded

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard8.jpg width="400">
<br />

4. Now go back to the dashboard
> **Note** that the **Current Model Information** and the **ML Inference Counts** Multi Tiles now specify details from the Alexa model
5. Test the "Alexa" keyword with this model, the performance is over the top! 

<p align="center">
    <br />
    <img src=./assets/images/iotcDashboard9.jpg width="1200">
<br />

# Conclusion

Cloud enabling a device adds additional return on investment by allowing service teams to proactively monitor devices in the field without having to roll a truck.  When we combine ML at the Edge with a cloud solution the use cases are unlimited.

I hope you were successful in sending telemetry data to IoTConnect on AWS.  If you encounter any issues or have questions feel free to [open an issue in the repo](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues)


