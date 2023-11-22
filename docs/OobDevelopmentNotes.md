# Avnet Out-of-Box Development Notes

This document captures some common development activities around modifying the Avnet RASynBoard Out-of-Box (OOB) application.  The OOB application is a good starting point to develop custom ML at the Edge applications for your ML based solutions. 

This document and images were written based on the V1.4.0 release.  However, all the concepts will remain relavant as the OOB application is further improved.  If the line numbers in the current release are different than my images, just search the file to find the new locations.

I've tried to capture some common application tasks.  If I missed your specific case, please open a [GitHub issue](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues) and I'll see what can be done to help.

# Renesas e^2 studio

# Threads

# Inferencing Events

# Application Configuration Items

The OOB application leverages a config.ini text file located in the root directory of the microSD card for runtime configuration items.  By defining runtime configuration items, we don't need to rebuild the application to enable/disable any OOB features.  This section details how to add new configuration items to the config.ini file and where to add code in the application.

For this example I'll be adding two new configuration items . . .
1. [AWS]->Endpoint
1. [AWS]->Device_Unique_ID

## Define the new items and add them to the config.ini file

**Note** I edited the config.ini file on the microSD card and added a new section called ```AWS``` and two new keys items under the ```AWS``` section.

![](./assets/images/aws01.jpg "")

## Decide if the new items need to be accessible when booting from SPI Flash

There are two basic RASynBoard hardware configurations to consider . . .  

- Boot from microSD Card
  - Configuration items are read from the config.ini file
  - Core board attached to I/O board and microSD card inserted into I/O board
- Boot from SPI Flash
  - Configuration items are read from SPI Flash
    - config.ini contents were writing to SPI flash by pressing the user button > 3 seconds
    - MicroSD card removed
    - Core board removed from the I/O board

In most cases you'll want to have your new configuration item in SPI Flash.  That way when you deploy your solution on the core board only, your configuration items will be available.  But in some cases you may not need the configuration items in Flash.  For example the ```aws_rootCA_file_name``` string is only used when the certificate configuration is set to read the certificates from the microSD card.

In the cases where you don't need your configuration items in SPI Flash, define your new configuration variables in ```src\fat_loat.c``` in the local global variable area; then continue to follow the instruction below but reference your variables instead of the configuration structure.

**The rest of this section assumes that you'll need to access your new configuration from SPI flash down the road.**

## Add code to parse the new Keys
Now that we've defined what the new configuration items are we can add the code to parse the new items and add helper functions to retrieve the key values from anywhere in the application.

We need to complete the following steps . . .
1. Add new variables for the keys
1. Add code to ```src\fat_load.c::read_config_file( void )``` to parse the data from the config.ini file and update our new variables
1. Add helper functions to ```get``` the new configuration items
1. Add code to output the new configuration items at startup 
1. Verify the functionality

### Add new variables for the keys
- Open ```src\fat_load.h```
- Locate the ```config_ini_items``` struct and add your new configuration items.
  - If you're new item is an enumeration, add the enum definition to ```src\fat_load.h```
  - See ```AWS_CERT_LOAD_FROM_TYPE``` for an example of an enumeration configuration item

**Note** that since we added our new variables to the ```config_ini_items``` struct our new items will automatically be written to SPI Flash and read from SPI Flash using existing logic.

![](./assets/images/aws02.jpg "")

### Add code to ```read_config_file(void)``` to parse the data from the config.ini file

- Open ```src\fat_load.c```
- Find the function ```src\fat_load.c::read_config_file(void)```

If we review the different ways to read configuration items in this function we'll see that there are two different function calls to get data from the config.ini file depending on the data type.

```c++,
int ini_gets(const TCHAR *Section, const TCHAR *Key, const TCHAR *DefValue,
             TCHAR *Buffer, int BufferSize, const TCHAR *Filename)
```
 
  - This is the function we use to pull string data from the config.ini file
  - Copy an existing call and change the ```Section```, ```Key```, ```DefValue```, ```Buffer```, and ```BufferSize``` arguments to target your new items
    - ```Section```: The new section; in our example **AWS**
    - ```Key```: One of the new keys; in our example **Endpoint**  
    - ```DefValue```: A default value for the item for the case where the item is not found in the config.ini file
    - ```Buffer```: The string variable name
    - ```BufferSize```: The max number of characters that the function will pull into your ```char*``` variable

```c++,
long ini_getl(const TCHAR *Section, const TCHAR *Key, long DefValue, const TCHAR *Filename)
```
  - This is the function we can use to pull numerical data
  - Note that the function returns a type ```long``` value

I added the following lines to read the new AWS configuration items

![](./assets/images/aws03.jpg "")

### Add helper functions to ```get``` the configuration items

We'll want to access our configuration items from other files in the project, so let's add a couple helper functions.

- Add the function prototypes to ```src\fat_load.h```

![](./assets/images/aws04.jpg "")

- Add the function definitions to ```src\fat_load.c```

![](./assets/images/aws05.jpg "")

### Add code to output the new configuration items at startup

I like to output the configuration items on startup to show the user the current configuration.  We can also use this code to validate that our new configuration items are being correctly parsed from the config.ini file.

- Find the ```src\fat_load.c::printConfig(void)``` function
- Add code to output the new configuration items

![](./assets/images/aws07.jpg "")

### Verify the functionality

- Build and run the updated application
- Verify that the output matches the strings added to the config.ini file 

![](./assets/images/aws06.jpg "")


# Cloud connectivity implementations

# Modifying the hardware configuration


# Areas for improvement

