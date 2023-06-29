# Mini Test Plan
This document captures the minimal features to test before releasing new code to the public repo

1. Build the project
2. Verify that the config.ini file [NDP Firmware] --> Mode=1
3. Copy all the files from the ndp120/synpkg_files/* directory are copied to the root directory of the microSD card
4. Move the microSD card to the RASynBoard
1. Debug the project in e2 studio, or load the *.srec file to the RASynBoard
1. Run the default configuration
1. Test the 5 keywords
   1. up
   2. down
   3. next
   4. back
   5. ok-syntiant
1. Test the down down enter low power mode feature
1. Test the record option feature (short user button press)
   1. Green LED lights up while recording
   1. Green LED turns off after recording stops
   1. Pull *.wav file from microSD card
   1. Verify the audio recording
1. If you have the dual microphone hardware test [NDP Firmware] --> Mode=2
1. Change the mode to [NDP Firmware] --> Mode=3
   1.  Test the circular motion model
      1. Verify circular motion is detected (CW and CCW)
   3.  Press the user button
       1. Veiify a *.csv file is written to the microSD card
       2. Verify that IMU date is output to the terminal 
       3. P
