# Remove the debug UART hardware interface

The OOB application uses the g_uart4 interface for the debug console (from the PMOD connector).  These pins can also be used as a i2c interface.  If you need to use these pins for i2c, you must remove the g_uart4 hardware interface and remove all references to the interface from the OOB source code.  This branch shows all the changes necessary to remove the g_uart4 interface and the corresponding code changes.  Note the changes in this branch will default/force the OOB debug to come out the USB-C connector on the RASynBoard core board.

Please review the code changes including the hardware configuration changes captured in the updated configuration.xml file.


