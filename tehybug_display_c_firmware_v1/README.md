Firmware made for TeHyBug display Weatherstation which is available at tindie:
 https://www.tindie.com/products/25408/
<img src="https://raw.githubusercontent.com/gumslone/tehybug/master/tehybug_display_c_firmware_v1/images/Bildschirmfoto%202022-02-13%20um%2019.54.41.png" width=400>

enclosure for 3d printing is available at thingiverse:
https://www.thingiverse.com/thing:5744609

features:
- RTC (realtime clock with battery backup)
- 1.3 Inch OLED Display
- 2 Ports, Port A for i2c and Port B ADC/1Wire Sensors
- Supports TeHyBug Universal sensors
- 3 Buttons, 2 left and right and one on the top
- USB2Uart IC that allows flashing programming via USB from Arduino IDE/Platformio or others.
- Buzzer that can be used as an alarm
- Single ws2812b RGB LED that works as an Indicator

### Mode Button

- hold the Mode button for 15 seconds to do a faactory reset (works only with already configured devices)
- hold the Mode button and hit the Reset button to boot the device into a flashing mode (in case you want to flash a firmware manually via USB)

### Reset Button

 - hit it to reboot the device, works in combination with other buttons
 
### IO_5 Button

- toggles offline or online mode, hold the IO_5 button and hit the reset button, keep holding the IO_5 Button until the led becomes purple. This will toggle the online or an offline (no wifi connection) mode

### IO_14 Button

- starts senseair s8 sensor calibration, hold it for 15 seconds to start the calibation. Put the device outside into a fresh air for the calibration.

## Pinmapping
  
<img src="https://github.com/gumslone/tehybug/blob/master/tehybug_co2_firmware/images/3.5mm_jack_plug_4i.svg_mapping.jpg?raw=true" width="300">
