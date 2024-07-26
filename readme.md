# RASynPuck Demo    

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo1.jpg width="300">
    <img src=./docs/assets/images/puckDemo2.jpg width="177">
<br />

This project is an example of how the current Avnet RASynBoard Out of Box (OOB) application can be repurposed for a custom application.  The standard application implements many features that a deployed application needs like cloud connectivity and the interface to the NDP120.  Lets leverage these features and enhance the application to fit our needs.

This documentation was written to compliment the current [RASynPuckDemo](./docs/RASynPuckDemo.md) documentation.  
- This document details how to leverage the generic Avnet Out of Box application for a custom solution
- The existing [RASynPuckDemo](./docs/RASynPuckDemo.md) document details how to configure the RASynBoard for a cloud connected core board only deployment

This branch (RASynPuckDemo) contains code changes specific for the RASynPuck Demo.  The RASynPuck demo was created to be a small battery powered demo that can easily travel to trade shows or customer sites to show some of the capabilities of the RASynBoard and Avnet's IoTConnect cloud solution.  

The code changes include . . . 

- Add logic to suppress NDP120 events after sending an inference telemetry message i.e., ```Rolling```, then wait a period of time and send the ```Idle``` telemetry message.  This allows the gif images to play to completion on the IoTConnect Dashboard.

- Change the length of time the RGB LED is turned on when the NDP120 detects and inference event.  The new period is the same period between sending inference telemetry i.e., ```Rolling``` and the ```Idle``` message.  This is a good visual indication that NDP120 events are suppressed.

- Add a new config.ini setting called [Puck Demo]-->Idle_Delay that defines the time period between when the application sends an inference telemetry message i.e., ```Rolling``` and sending the ```Idle``` telemetry message

## The Story 

To implement this demo I need . . . 
1. A demo fixture to hold the Core board and a battery
1. Custom gif images that mimic the motion events from the NDP120
1. A ML model to capture the event I wanted to show
1. Customizations to the Avnet RASynBoard OOB application to suit the demo
1. Implement a cloud based dashboard to show inference events using animated gifs 

### Demo Fixture

When I first started to work on the RASynBoard project I created a 3D printable enclosure that holds the RASynBoard core board and a 500mAH battery.  My *.stl files can be found [here](https://www.thingiverse.com/thing:6273171).  I used my resin printer to capture the fine details of the design.  

### Custom gif images

IoTConnect has a very cool dashboard feature/widget called ```transformations``` that can show a graphic or gif based on a telemetry value.  More on this later, but I wanted to create animated gif images that showed the motion detected by the RASynBoard.  I used the animation feature in Fusion 360 (now just Fusion) to create the animations, then captured video of the animations with Camtasia then converted the short videos to gif images using a free utility called ffmpeg.

Here are my gif images.  Note I also created a static image of the Puck at rest, the modified application will send up an ```idle``` message to show the static image between motion events.  When creating the gifs and image I took the time to ensure all images started and ended in the same location to enhance the visualization.

<p align="left">
    <br />
    <img src=https://images2.imgbox.com/ef/2c/Fpd0jH7K_o.gif width="250">
    <img src=https://images2.imgbox.com/aa/dd/j2jucSRz_o.gif width="250">
    <img src=https://images2.imgbox.com/94/b8/FHQHw4rG_o.gif width="250">
    <img src=https://images2.imgbox.com/38/05/XQ1QYv4s_o.gif width="250">
    <img src=https://images2.imgbox.com/57/c5/yNSmAnoP_o.jpg width="250">
<br />

### Create a ML model to capture motion events

I recently updated my RASynBoard workshop lab documents.  I followed my [Lab1 document](http://avnet.me/ML-WorkshopV2-Lab1) to create a new IMU model in Edge Impulse and added the twisting motion event.  The RASynPuck ML model was also delivered in the latest [RASynBoard release 1.6.2](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/releases).

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo11.jpg width="650">
<br />

### Customize the OOB application
#### Out of Box configuration settings

I leveraged a couple standard Out of Box features for the demo

- [IoTConnect on AWS cloud connectivity](./docs/IoTConnect.md) for my cloud connectivity and dashboard
- I also disable the "down down" low power entry feature.  Currently the low power features don't play nice with an MQTT connection.
<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo7.jpg width="800">
<br />

##### Code changes

I wanted to allow the gifs to play to completion before allowing another gif to start so I had to . . .

- Implement a way to disable inference events until the current gif completed
- Implement a way to send up the ```Idle``` telemetry message after the gif completed or a specific delay
  - It's pretty much impossible to get this timing exactly correct since there are variable telemetry transmission times and the IoTConnect back end has to process the message then show the correct gif.
- Modify how long the RGB is turned to match the period between sending a telemetry message i.e., ```Rolling``` and sending the ```Idle``` telemetry message.  This gives the user a visual indication that inferencing is disabled while the RGB LED is on.

To implement these requirements I created a new thread called ```telemetryTiming``` and a boolean flag called ```supressNdp120Events```.  When the NDP120 detects an event I added a call to ```suppressNdp120Event()``` that sets the boolean flag in the ```telemetryTiming``` context.  

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo12.jpg width="600">
<br />

Then I added code to block on a new Event Group bit.  The Event bit is set when ```suppressNdp120Event()``` is called.  The thread is unblocked when the bit is set and a delay is implemented.  Once the delay expires then we send the Idle message and set the boolean supressNdp120Events to false. 

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo9.jpg width="900">
<br />

I also added a new config.ini entry to allow me to change the timing between when an inference event is sent and sending the idle message without having to rebuild the application
<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo8.jpg width="800">
<br />

To suppress the NDP120 events, I added a call to check the suppress status to the ```ndp_thread_entry()``` function right after the NDP120 signals that there is a new event.  If we're suppressing events, we just service the semaphores.  If we're not suppressing events, just run the existing code logic.

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo10.jpg width="800">
<br />

### Create an IoTConnect Dashboard

At this point I have my custom application sending inference events to IoTConnect.  Since I followed the [IoTConnect on AWS cloud connectivity](./docs/IoTConnect.md) documentation, my device exists and was created using the [RASyV1_6_template device template](./docs/assets/IoTConnect/RASyV1_6_template.JSON).  I also imported the RASynPuck_dashboard.json dashboard template.  

## Test the Dashboard

1. Make sure your device is on-line and connected to IoTConnect
2. Open the dashboard
3. Move your RASynBoard to trigger one of the motion events

<p align="center">
    <br />
    <img src=./docs/assets/images/puckDemo19.jpg width="1200">
<br />

## Deploy the demo on just a core board

There is another write-up on how to get your AWS or IoTConnect certificates and your configuration flashed onto your core board's SPI flash.  See the document [here](./docs/RASynPuckDemo.md).

## Conclusion
The goal of this project was to show that you can start with the Avnet RASynBoard Out of Box application and customize it to fit your project requirements.  If you have any questions or issue, please open an issue in the GitHub repo [here](https://github.com/Avnet/RASynBoard-Out-of-Box-Demo/issues) 