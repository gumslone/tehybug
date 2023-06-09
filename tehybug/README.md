This is a different TeHyBug firmware fully written in c, (previous was partially in Lua).

This firmware supports easy OTA Updates.

It has a limited sensor support compared to the Lua firmware.
I.e. it doesnt support AM2032 Sensor in i2c mode also there are some issues with ds18b20.

This firmware is compatible with tehybug universal boards (without display) like:
* TeHyBug 18650 Universal v1 (esp-01 based) and v2 (esp-m based)
* Gumboard 
* or other tehybug boards with have audio jack connector for the sensors


Supported sensors:
* BME680
* BME280/BMP280
* DHT21/DHT22/AM2032 (in dht simulation mode)
* AHT20
* MAX44009

how to flash?
You will need an USB2UART Programmer and python esptool.

Replace /dev/cu.usbserial-1410 with your usb2serial port.

```esptool.py --port=/dev/cu.usbserial-1410  write_flash 0x00000 desired_tehybug_firmware.bin```