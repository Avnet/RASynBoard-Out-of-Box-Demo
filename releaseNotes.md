# Release Notes for the Avnet Out-of-Box Application

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
Release type: Feature Release

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