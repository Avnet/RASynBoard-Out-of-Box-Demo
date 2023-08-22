# Avnet RASynBoard Out-Of-Box Application

The Out-of-Box (OOB) application's goal is to "Provide a working example application that exercise the RASynBoard hardware and gives development teams a strong starting point for their own custom designs and ML training data"

- [RASynBoard Product Page](http://avnet.me/RASynBoard): Inlcudes a buy link
- [RASynBoard Development Guide](http://avnet.me/rasynboard-ug): Detailed overview of the hardware

# Out-of-Box documentation
Please follow the links below to learn more about the Avnet RasynBoard Out-of-Box application

- [Getting Started](./docs/RASyBoardGettingStarted.md): All the details you need to clone, build, load and debug the application on your RASynBoard development kit
- [Application User Guide](./docs/ApplicationUserGuide.md): Details on all the application features and configuration options
- [Edge Impulse Data Forwarder instructions](./docs/EdgeImpulseDataForwarder.md): Details on using the Edge Impulse Data Forwarder CLI tool to stream 6-Axis IMU data directly into your Edge Impulse ML project
- [Edge Impulse Uploader instructions](./docs/EdgeImpulseUploader.md): Details on using the Edge Impulse Uploader CLI tool to upload audio (\*.wav) or 6-Axis IMU data (\*.csv) data files directly into your Edge Impulse ML project
- [Avnet IoTConnect Configuration Details](./docs/IoTConnect.md)
- [Out-of-Box Development Notes](): Details on the OOB application and areas of interest for engineering teams looking to leverage the application for custom applicaitons.

# Out-of-Box Releases
As Avnet adds features and bug fixes to the OOB application we push out releases.  Each release:

1. Is built from a GitHub [Repo Tag](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/tags)
2. Have been tested
3. Include release notes detailing changes since the last release
4. Are delivered as a binary image
5. Include the corresponding microSD card images 
6. Documents known issues with the release, and if available, details on working around the issue

[Link to Avnet RASynBoard OOB Releases](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/releases)

# Self Paced Training
Avnet creates training materials to help engineering teams come up to speed quickly with the Avnet RASynBoard.  Follow the links below to access the latest hands-on RASynBoard lab documents.

- [Lab0](http://avnet.me/ML-Workshop-Lab0): Walks the user through setting the tools and free on-line accounts to complete the labs
- [Lab1](http://avnet.me/ML-Workshop-Lab1): Details on sideloading the OOB binary image onto the board and exercising some of the ML features implemented in the OOB application
- [Lab2](http://avnet.me/ML-Workshop-Lab2): Details on creating an Edge Impulse ML model.  **Note:** this lab can't be completed until late October when Edge Impulse releases public support for the Avnet RASynBoard.  However, you can still stream 6-Axis IMU data to Edge Impulse, build a ML model, and test it in Edge Impulse from data streaming from your RASynBoard.

# Bugs or improvement ideas
If you encounter a bug, have questions on the OOB application, or have an idea for ways to improve the application, please open an issue on the [GitHub Issue Page](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues).  An Avnet engineer will respond to the request.