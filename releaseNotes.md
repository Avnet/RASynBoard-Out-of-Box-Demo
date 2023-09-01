## Version: 1.3.0
Release type: **Feature Release**

Date: Sept 1, 2023

### New Features
- AABGT-43: Added logic to connect to Avnet's IoTConnect and send inference event details as telemetry
- AABGT-44: Added functionality to store Wi-Wi SSID and password in config.ini file
- AAGBT-75: Added [documentation](./docs/ApplicationUserGuide.md) on all the OOB application configuration items and features
- AAGBT-80: Added [IoTConnect documentation](./docs/IoTConnect.md)
- AAGBT-85: Added functionality to store the contents of the config.ini file to SPI flash when the user presses the user button for > 3 seconds.  Additionally when the device boots without the microSD card present, the configuration is read from SPI Flash.
- AABGT-86: Added functionality to read AWS certificates from the microSD card
- AAGBT-90: Added [documentation](./docs/UsingRASynbBoardReleases.md) on how to load one of the OOB releases without the Renesas e^2 studio

### Bug Fixes
- AABGT-84: Corrected an issue where BLE inference data was not sent when running the core board in stand alone mode.  Corrected by AABGT-85.
- AABGT-72: Corrected an issue where the application would not expose the microSD card when the card was inserted into the I/O board and the card had no files on the filesystem

### Known Issues

- AABGT-79: When connected to the Avnet IoTConnect cloud solution and the device enters, then exist low-power mode, the IoTConnect state machine has to start from the beginning sequence.  

# Release Notes for the Avnet Out-of-Box Application

## Version: 1.2.0
Release type: **Feature Release**

Date: July 18, 2023

### New Features
- AABGT-60: Added USB PMSC feature to expose the RASynBoard's microSD card to the development PC when connected to the RASynBoard USB-C connector on the core board.  This allows the developer to modify the config.ini file without removing the microSD card from the RASynBoard.  Additionally audio (*.wav) and imu (*.csv) recording files can be copied from the microSD card without removing the microSD card from the RASynBoard.
- AABGT-50: Added new config.ini entry [BLE Mode] --> BLE_Enabled=x to control the application broadcasting inference results over BLE.  The feature defaults to off (don't send inference results over BLE)

### Bug Fixes
- AABGT-63: When [Low Power Mode] --> Power_Mode=1 is enabled, the application will automatically enter low power mode after startup.  Note when this feature is enabled the USB PMSC feature is not avaliable since the device stays in low power mode until an inference result is detected.
- AABGT-70: Corrected an issue where audio and imu capture files on the microSD card would be overwritten if the application was restarted.

### Known Issues
- AABGT-72: If the microSD card does not have any files the USB PMSC feature fails.

## Version: 1.1.0
Release type: Feature Release

### New Features
- Supports capturing 6-axis IMU data to the debug uart or to a file
- IMU data capture follows ```Recording_Period=xx``` configuration item
- Added Edge Impulse [Data Uploader instructions](./docs/EdgeImpulseUploader.md)
- Added Edge Impulse [Data Forwarder instructions](./docs/EdgeImpulseDataForwarder.md)
- When the core board boots without a core board, send uart debug to USB-C on core board

### Known Issues
- When running the circular motion model ```mode=3``` the application hangs when the user presses the user button to record imu data. 
  - To capture IMU data, select ```mode=1``` and add a new line ```Button_shift=imu``` at the end of the ```[Function_1]``` block
- When in low-power mode ```Power_Mode=1``` the application does not start-up in low-power mode
  - The application will enter low-power mode after the first inference result is detected
  
## Version: 1.0.0
Release type: **Feature Release**

Initial release

### Supported Features
- Supports selecting which ML model to run using the config.ini file for configuration options
- Supports audio data for ML models
- Supports IMU data for ML models
- Record training data with short button press < 400 ms
  - Records audio data by default
  - Records IMU data with config.ini option: ```Button_shift=imu```
- Write ndp120 images to SPI Flash with long button press > 3 sec
- Enters low-power mode when ```Mode=1``` or ```mode=2``` and the "down down" command is detected
- Supports always return to low-power mode when ```Power_Mode=1``` in the config.ini file
- Supports user defined recording period by setting ```Recording_Period=xx``` in the config.ini file

### Supported ML models
#### ```Mode=1```
Audio model detects 5-keywords using the RASynBoard's build in digital microphone

- "ok syntiant"
- "up"
- "down"
- "back next"

#### ```Mode=2```
Audio model detects 5-keywords using the RASynBoard's optional 2-microphone hardware

- "ok syntiant"
- "up"
- "down"
- "back next"

#### ```Mode=3```
IMU data model detects when the RASynBoard is moved in a circle while laying flat on a table


### Known Issues
- When running the circular motion model ```mode=3``` the application hangs when the user presses the user button to record imu data. 
  - To capture IMU data, select ```mode=1``` and add a new line ```Button_shift=imu``` at the end of the ```[Function_1]``` block
- When in low-power mode ```Power_Mode=1``` the application does not start-up in low-power mode
  - The application will enter low-power mode after the first inference result is detected