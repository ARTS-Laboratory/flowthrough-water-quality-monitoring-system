# V0.1

Hello! Hopefully the work I did here was somewhat helpful. If you have questions about any of the physical structure or the Arduino relay system, please feel free to contact me at the following:

bmontroy25@governors.school (will work until the end of may, 2025)
braedenmontroy1@gmail.com (will always work)
228-382-8652 (quickest answer)

# Pumps
The pumps we are using are 12V Grothen pumps. You'll need about 2 amps to run both of them at full power at the same time.
(https://www.amazon.com/dp/B0863TCH65)

# Relay
I've left a picture of the relay labelled in the same folder as this. It should have everything you need to get this going. Currently, the system works by powering the pumps whenever the D13 on the Arduino is on. As I was leaving, we were working on making this output a switch toggle.

<p align="center">
<img src="system_development/Moultrie/v0.1/labelled_relay.PNG" alt="drawing" width="988"/>
</p>
<p align="center">

# Structure
The structure is currently made from 8020 and a sheet of what I think is Delrin. It doesn't fit in the box completely, as it needs about an inch to be chopped off the height in order to fit correctly, but all of the components fit inside of it. You can find CAD for this in the CAD folder in this directory. The next steps in terms of structure are to lower the right (or left) side to compensate for the PXIe, move all of the pumps to one side, use plastic screws and put the magnet on the other side, and make the connection between the pumps and the tubing and the sensor array watertight so water can actually run through the system.

# Water Sensor Array
As I was leaving, Ali was working on making this save to the SD card.  The sensors themselves can take data, they just need to be calibrated correctly and set up to save on an SD card rather than pushing their data to Thingspeak.
