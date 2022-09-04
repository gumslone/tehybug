# TeeHyBug CO2 Sensor Firmware

TeHyBug firmware for SCD4X and Senseair S8 sensor with support for OLED Display and additional external sensors like AHT10 and BMX280.

## Features:
- OLED Display support
- RGB indication for normal (green), medium (yellow) and high (red) CO2 level
- HomeAssistant MQTT Autodiscovery mode (just provide the mqtt brocker ip and the device will appear in your HA)
- Web Server for OTA Updates, simply upload new firmware versions via web interface
- Web Server that serves the sensor data in Json Format
- Possible to turn off WIFI and work in offline display only mode (Hit reset and hold the Button IO_5 when you see the TeHyBug logo, once the LED turns purple, release the button). To find out that the wifi is on, there will be a small dot on the right side of the display with sensordata
- Hold the mode button for 15 seconds to factory reset your device and delete  the wifi configuration


TeHyBug Mini CO2 SCD4X Enclosure for 3D Printing is available on Thingiverse: https://www.thingiverse.com/thing:5494215

## How to program/flash the board
To flash firmware use the .esp8285.bin file.
For flashing and programming you can use ARDUINO IDE, select there generic ESP8285 board.
Also you can use the [ESPTool](https://github.com/espressif/esptool) to flash binaries to the board or other tools which are described at: https://nodemcu.readthedocs.io/en/latest/flash/


## Upload new firmware via web interface

To update the firmware from OTA WebInterface open http://tehybug.local/update in your browser, if this doesnt work, try to find out its IP from your router admin menu or use any local network ip scanner app for your mobile phone to get the device ip and then open http://<ip_address<ip address>>/update with your browser.

To show the temperature in Fahrenheit instead of Celsius, open http://tehybug.local/config or http://<ip_address<ip address>>/config with your browser to save the configuration.

For the update page you will have to provide a username and a passord:
  
Username: TeHyBug
  
Password: FreshAirMakesSense
 
## Pinmapping
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/3.5mm_jack_plug_4i.svg_mapping.jpg?raw=true" width="300">

## HomeAssistant
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/ha.png?raw=true" width="400">

