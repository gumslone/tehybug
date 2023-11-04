This is a different TeHyBug firmware fully written in C/C++, (previous was partially in Lua).

This firmware supports easy OTA Updates.

<img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/2022-06-10T21_41_23.878Z-IMG_3707.jpg?raw=true" width="500">

This firmware is compatible with tehybug universal boards (without display) like:
* TeHyBug 18650 Universal v1 (esp-01 based) and v2 (esp-m based)
* Gumboard 
* or other tehybug boards with have audio jack connector for the sensors
* It is also compatible with any other ESP8266/ESP8285 dev boards like wemos, lolin, nodemcu etc. See the pin mapping images. Only the indicator led will not work and the power saving mode with deep sleep will probably not work either.

## Buttoms
- Reset: forces TeHyBug to reboot/restart
- Mode button: activates the configuration mode during the device boot

## Modes
- Live mode: when your device is configured to serve data (via http/mqtt) and you enable the powersaving deep sleep and deactivate the config mode in the system settings. <img width="402" alt="Bildschirmfoto 2023-11-04 um 16 26 51" src="https://github.com/gumslone/tehybug/assets/12110353/2b2524da-0643-447a-abb0-873b50236c4e">

- Config mode: TeHyBug serves a web interface at http://tehybug.local where you can configure everything.


To return back to Config mode from the Live mode:
1. hit the RESET button
2. after that push and hold the MODE button untill the LED turns blue
3. release the MODE button.

## Port B (green) supported sensors:
* BME680
* BME280/BMP280
* DHT21/DHT22/AM2032 (in dht simulation mode)
* AHT20
* MAX44009
* DS18B20
* other i2c and one wire sensors (requires code modification)
  
### Pinmapping Port B
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/tehybug_port_b_pinmapping.png?raw=true" width="300">

## Port A (black) supported sensors:
* DHT21/DHT22/AM2032 (in dht simulation mode)
* DS18B20
* ADC soil moisture sensor
* other ADC and one wire sensors (requires code modification)

### Pinmapping Port A
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/tehybug_port_a_pinmapping.png?raw=true" width="300">

## Upload new firmware via web interface (recommended)

To update the firmware from OTA WebInterface open http://tehybug.local/update in your browser, if this doesnt work, try to find out its IP from your router admin menu or use any local network ip scanner app for your mobile phone to get the device ip and then open http://<ip_address<ip address>>/update with your browser.

## How to program/flash the board (advanced users only)
To flash firmware use the .esp8285.bin file.
For flashing and programming you can use ARDUINO IDE, select there generic ESP8285 board.
Also you can use the [ESPTool](https://github.com/espressif/esptool) to flash binaries to the board or other tools which are described at: https://nodemcu.readthedocs.io/en/latest/flash/

Replace /dev/cu.usbserial-1410 with your usb2serial port.

```esptool.py --port=/dev/cu.usbserial-1410  write_flash 0x00000 desired_tehybug_firmware.bin```



## WebGui
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/webgui.png?raw=true" width="800">

Demo web configuration page: https://tehybug.com/tehybug/v1/html/demo.html

## Configuration first steps
- Connect an external sensor to the board 3,5mm audio jack connector.
- Connect the power supply to micro USB port
- TeHyBug will boot, the LED will turn solid blue
- Connect to a TeHyBug wifi network like the image below (Password: TeHyBug123)
- <img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/wifimanager.png?raw=true" width="350">
- open http://192.168.4.1/ in your browser, and click the configuration button
- <img src="https://github.com/gumslone/tehybug/blob/master/tehybug/images/credentials.png?raw=true" width="350">
- Provide credentials of your WIFI network and save them
- If your credentials were correct, the TeHyBug WIFI network will disapear
- TeHyBug will connect to your network and boot in a configuration mode with solid blue LED light
- open with your browser http://tehybug.local/ and the configuration page should open. (if this didnt work. Find out the TeHyBug IP Addres from your router and open it with yoour browser)
- Follow the instructions on the configuration page.
