# Mini Test Plan
This document captures the minimal test plan to complete before releasing new code to the public repo.  If any issure are found, resolve them before releaseing, or at a minimum document the issues in the public repo's readme.md file.

1. Build the project
2. Verify that the config.ini file [NDP Firmware] --> Mode=1
3. Copy all the files from the ndp120/synpkg_files/* directory are copied to the root directory of the microSD card
4. Move the microSD card to the RASynBoard
1. Debug the project in e2 studio, or load the *.srec file to the RASynBoard
1. Run the default configuration
1. Test the 5-keywords
   1. up
   1. down
   1. next
   1. back
   1. ok-syntiant
1. Test the "down down" enter low power mode feature
1. Test the record option feature (short user button press)
   1. Green LED lights up while recording
   1. Green LED turns off after recording stops
   1. Pull *.wav file from microSD card
   1. Verify the audio recording
1. Press the user button > 3 seconds
   1. Verify NDP120 images are written to flash
   1. Remove the microSD card and test the 5-keywords again
1. Change config.ini: [Low Power Mode] -> Power_Mode=1
   1. Power up the board with the microSD card inserted
   1. Verify board enters low-power mode on startup
   1. Speak one of the 5-keywords and verify that the device enters low-power mode after each keyword is detected 
1. If you have the dual microphone hardware test [NDP Firmware] --> Mode=2
   1. Run the default configuration
   1. Test the 5-keywords
      1. up
      1. down
      1. next
      1. back
      1. ok-syntiant
   1. Test the "down down" enter low power mode feature
   1. Test the record option feature (short user button press)
      1. Green LED lights up while recording
      1. Green LED turns off after recording stops
      1. Pull *.wav file from microSD card
      1. Verify the audio recording
1. Change the mode to [NDP Firmware] --> Mode=3
   1.  Test the circular motion model
      1. Verify circular motion is detected (CW and CCW)
   1.  Press the user button
       1. Veiify a *.csv file is written to the microSD card
       2. Verify that IMU date is output to the terminal 
       
