TeHyBug firmware for SCD4X and Senseair S8 sensor with support for OLED Display and additional external sensors like AHT10 and BMX280.

Features:
- OLED Display support
- RGB indication for normal (green), medium (yellow) and high (red) CO2 level
- HomeAssistant MQTT Autodiscovery mode (just provide the mqtt brocker ip and the device will appear in your HA)
- Web Server for OTA Updates, simply upload new firmware versions via web interface
- Web Server that serves the sensor data in Json Format
- Possible to turn off WIFI and work in offline display only mode (Hit reset and hold the Button IO_5 when you see the TeHyBug logo, once the LED turns purple, release the button). To find out that the wifi is on, there will be a small dot on the right side of the display with sensordata
- Hold the mode button for 15 seconds to factory reset your device and delete  the wifi configuration
