# Avnet RASynBoard Out-Of-Box Application

The Out-of-Box (OOB) application's goal is to "Provide a working example application that exercise the RASynBoard hardware and gives development teams a strong starting point for their own custom designs and ML training data"

- [RASynBoard Product Page](http://avnet.me/RASynBoard): Includes a buy link
- [RASynBoard Development Guide](http://avnet.me/rasynboard-ug): Detailed overview of the hardware
- [RASynBoard Video Series](./docs/RASnyVideoSeries.md)
    - Collection of short videos to help users get started with the RASynBoard OOB application

# Out-of-Box documentation
Please follow the links below to learn more about the Avnet RASynBoard Out-of-Box application

- [Getting Started](./docs/RASyBoardGettingStarted.md): All the details you need to clone, build, load and debug the application on your RASynBoard development kit
    - [Video walkthrough (23 minutes)](http://avnet.me/RASynGettingStartedVideo)
- [Application User Guide](./docs/ApplicationUserGuide.md): Details on all the application features and configuration options
- [Using RASynBoard Releases](./docs/UsingRASynbBoardReleases.md): Details on loading one of the OOB releases onto your RASynBoard if you don't want to build the application yourself
    - [Video walkthrough (11 minutes)](http://avnet.me/RASynUsingReleasesVideo)
- [RASynBoard Troubleshooting guide](./docs/RASynTroubleshootingGuide.md)
    - Document to address common issues encountered when running and debugging the OOB application

## Edge Impulse Data ingestions documentation
Use the links below to learn about the different ways to collect training/testing data with your RASynBoard and transfer it to your Edge Impulse ML project.

- [Edge Impulse RASynBoard Documentation](https://docs.edgeimpulse.com/docs/development-platforms/officially-supported-mcu-targets/avnet-rasynboard)
- [Edge Impulse Daemon instructions](./docs/RASynEdgeImpulseDataIngestion.md): Details on using the Edge Impulse RASynBoard application and the ```edge-impulse-daemon``` utility to send training data from your RASynBoard directly into your Edge Impulse ML project
    - [Video walkthrough (xx minutes)](http://avnet.me/RASynDataIngestionVideo)
- [Edge Impulse Data Forwarder instructions](./docs/EdgeImpulseDataForwarder.md): Details on using the Edge Impulse Data Forwarder CLI tool to stream 6-Axis IMU data directly into your Edge Impulse ML project
- [Edge Impulse Uploader instructions](./docs/EdgeImpulseUploader.md): Details on using the Edge Impulse Uploader CLI tool to upload audio (\*.wav) or 6-Axis IMU data (\*.csv) data files directly into your Edge Impulse ML project

## Connecting your RASynBoard to a cloud solution
Use the links below to learn how to connect your RASynBoard to one of the currently supported cloud solutions.

- [Avnet IoTConnect Configuration](./docs/IoTConnect.md):  Details to connect your RASynBoard to Avnet's IoTConnect on AWS cloud solution
    - [Video walkthrough (31 minutes)](http://avnet.me/IoTConnectOnAWS)
- [AWS IoT Core Configuration](./docs/awsIoTCore.md):  Details to connect your RASynBoard to AWS IoT Core
    - [Video walkthrough (18 minutes)](http://avnet.me/RASynAwsIotCore)
- [Wi-Fi Configuration](./docs/WiFiSetup.md):  Details on how to configure the OOB application to connect to your Wi-Fi network. 
    - [Video tutorial (9 minutes)](http://avnet.me/RASynWiFi)

## Software development documentation
Use the links below to learn how to modify the Avnet OOB application to target your specific application requirements.

- [Out-of-Box Development Notes](./docs/OobDevelopmentNotes.md): Details on the OOB application and areas of interest for engineering teams looking to leverage the application for custom applications.
- [Add a Temperature and Humidity Sensor to the OOB Application](./docs/addTempHumiditySensor.md): Details on how to add the [Renesas HS300X temperature and humidity sensor](https://www.renesas.com/us/en/products/sensor-products/humidity-sensors/us082-hs3001evz-relative-humidity-sensor-pmod-board-renesas-quick-connect-iot) to the OOB application.

# Out-of-Box Releases
As Avnet adds features and bug fixes to the OOB application we push out releases.  Each release:

1. Has a unique version string
2. Is built from a GitHub [Repo Tag](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/tags)
3. Have been tested
4. Include release notes detailing changes since the last release
5. Are delivered as a binary image
6. Include the corresponding microSD card images 
7. Documents known issues with the release, and if available, details on working around the issue

[Link to Avnet RASynBoard OOB Releases](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/releases)

# Self Paced Training
Avnet creates training materials to help engineering teams come up to speed quickly with the Avnet RASynBoard.  Follow the links below to access the latest hands-on RASynBoard lab documents.

- [Lab0](http://avnet.me/ML-Workshop-Lab0): Walks the user through setting the tools and free on-line accounts to complete the labs
- [Lab1](http://avnet.me/ML-Workshop-Lab1): Details on sideloading the OOB binary image onto the board and exercising some of the ML features implemented in the OOB application
- [Lab2](http://avnet.me/ML-Workshop-Lab2): Details on creating an Edge Impulse ML model.  **Note:** this lab can't be completed until late October when Edge Impulse releases public support for the Avnet RASynBoard.  However, you can still stream 6-Axis IMU data to Edge Impulse, build a ML model, and test it in Edge Impulse from data streaming from your RASynBoard.

# Bugs or improvement ideas
If you encounter a bug, have questions on the OOB application, or have an idea for ways to improve the application, please open an issue on the [GitHub Issue Page](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues).  An Avnet engineer will respond to the request.