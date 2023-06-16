This is a different TeHyBug firmware fully written in C/C++, (previous was partially in Lua).

This firmware supports easy OTA Updates.

It has a limited sensor support compared to the Lua firmware.
I.e. it doesnt support AM2032 Sensor in i2c mode also there are some issues with ds18b20.

This firmware is compatible with tehybug universal boards (without display) like:
* TeHyBug 18650 Universal v1 (esp-01 based) and v2 (esp-m based)
* Gumboard 
* or other tehybug boards with have audio jack connector for the sensors

## Supported sensors:
* BME680
* BME280/BMP280
* DHT21/DHT22/AM2032 (in dht simulation mode)
* AHT20
* MAX44009
* DS18B20

## Port B (green) supported sensors:
* BME680
* BME280/BMP280
* DHT21/DHT22/AM2032 (in dht simulation mode)
* AHT20
* MAX44009
* DS18B20
* other i2c and one wire sensors (requires code modification)

## Port A (black) supported sensors:
* DHT21/DHT22/AM2032 (in dht simulation mode)
* DS18B20
* ADC soil moisture sensor
* other ADC and one wire sensors (requires code modification)

## How to program/flash the board
To flash firmware use the .esp8285.bin file.
For flashing and programming you can use ARDUINO IDE, select there generic ESP8285 board.
Also you can use the [ESPTool](https://github.com/espressif/esptool) to flash binaries to the board or other tools which are described at: https://nodemcu.readthedocs.io/en/latest/flash/

Replace /dev/cu.usbserial-1410 with your usb2serial port.

```esptool.py --port=/dev/cu.usbserial-1410  write_flash 0x00000 desired_tehybug_firmware.bin```

## Pinmapping
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/3.5mm_jack_plug_4i.svg_mapping.jpg?raw=true" width="300">

## Upload new firmware via web interface

To update the firmware from OTA WebInterface open http://tehybug.local/update in your browser, if this doesnt work, try to find out its IP from your router admin menu or use any local network ip scanner app for your mobile phone to get the device ip and then open http://<ip_address<ip address>>/update with your browser.

Demo web configuration page: https://tehybug.com/tehybug/v1/html/demo.html

## Configuration first steps
- Connect external sensor to the board 3,5mm audio jack connector.
- Connect the power supply to micro USB port
- TeHyBug will boot, the led will turn blue and the logo will appear on the display (if display is available)
- Connect to a TeHyBug wifi network like the image below
- <img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/wifi.png?raw=true" width="600">
- open http://192.168.4.1/ in your browser, and click the configuration button
- <img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/config.png?raw=true">
