# Gumboard aka mini hacking/development board

# GumBoard!

Gumboard is a tiny wifi development/hacking board based on the esp8582 M1 Module. It has most things you need on the  board to connect sensors, display, etc and start hacking.

<img src="https://github.com/gumslone/tehybug/blob/master/gumboard/images/dwc.jpg?raw=true" width="400">

# Features

 1. USB to Uard Chip for flashing programming
 2. RBG LED (WS2812b) connected to GPIO_12
 3. Mode (GPIO_0) and Reset buttons to boot into a flashing mode
 4. additional two buttons connected to GPIO_4 and GPIO_5 on the back of the board
 5. 4 Pin audio Jack Connector for connecting i2c TeHyBug sensors and Display
 6. Micro USB connector for flashing / programming and power delivery

## How to program/flash the board

For flashing and programming you can use ARDUINO IDE, select there generic ESP8285 board.
Also you can use the [ESPTool](https://github.com/espressif/esptool) to flash binaries to the board.
In the src folder you will find binaries and code examples for the board.
<img src="https://github.com/gumslone/tehybug/blob/master/gumboard/images/3.5mm_jack_plug_4i.svg_mapping.jpg?raw=true" width="300">
