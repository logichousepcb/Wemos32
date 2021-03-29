This is a board I created to connect a Wemos D1 Mini or ESP32 to my windows and door sensors of my house to automate events in Home Assistant.

They windows and door binary sensors plug into the front ports on the device RJ45 connectors. Eech RJ45 connector has 4 ports with each twisted pair on the connector connecting to a pin and ground. There are 8 ports for a total of 32 binary sensors.

The I2C bus is also exposed on a header for connections of additional devices such as an OLED on other I2C based sensors.

There is a USB passthrough that you can connect the ESP to so the USB port is exposed on the front of the board.

If you would like one of these, contact logixpcb@gmail.com

** Version 1.6 (03/19/2021) - I merged the 16 port and higher along with ESP32/ESP8266 Wifi/LAN support all in one code base.  The extended secrets file needs to be included in this version. 
I have added some support for output to control relays to tinker with.  My goal is Version 2 will have output control.

** Version 1.65 (03/29/2021) - Working auto discovery with the binary sensors and dsb1820 temperature sensor.  This will change the MQTT output from the previous format.  If you chose to use this, you will have to update your entities in Home Assistant.  You will also have to update your config file as I have change some variables from digits to boolean.  Next version will have auto discovery with output control for relays.
