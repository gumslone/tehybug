# BMP280/BME280 sensor library for Arduino

[![Build Status](https://travis-ci.org/Erriez/ErriezBMX280.svg?branch=master)](https://travis-ci.org/Erriez/ErriezBMX280)

This is a BMP280/BME280 temperature/pressure/humidity sensor library for Arduino.

![BMX280](https://raw.githubusercontent.com/Erriez/ErriezBMX280/master/extras/BMX280.png)


## Arduino library features

- Measurements:
    - BMP280: Temperature / pressure / approximate altitude
    - BME280: Temperature / pressure / approximate altitude / humidity
- Three operation modes:
  - Normal mode: Continues conversion
  - Forced mode: One-time conversion
  - Standby mode: Low-power, no conversion
- Sampling configuration
- Chip detect / read chip ID
- I2C interface only
- Small flash/RAM footprint


## BMP280/BME280 sensor specifications

- Operating voltage: 1.71V .. 3.6V max
- Low current:
    - 1.8 uA @ 1 Hz humidity and temperature
    - 2.8 uA @ 1 Hz pressure and temperature
    - 3.6 uA @ 1 Hz humidity, pressure and temperature
    - 0.1 uA in sleep mode
- Operating range: -40...+85 °C, 0...100 % rel. humidity, 300...1100 hPa
- I2C bus interface: max 3.4 MHz
- No additional electronic components needed


## Hardware

**Connection Arduino board - BMX280 sensor**

| Pins board - BMX280            | VCC  | GND  |    SDA     |    SCL     |
| ------------------------------ | :--: | :--: | :--------: | :--------: |
| Arduino UNO (ATMega328 boards) |  5V  | GND  |     A4     |     A5     |
| Arduino Mega2560               |  5V  | GND  |    D20     |    D21     |
| Arduino Leonardo               |  5V  | GND  |     D2     |     D3     |
| Arduino DUE (ATSAM3X8E)        | 3V3  | GND  |     20     |     21     |
| ESP8266                        | 3V3  | GND  | GPIO4 (D2) | GPIO5 (D1) |
| ESP32                          | 3V3  | GND  |   GPIO21   |   GPIO22   |

Other unlisted MCU's may work, but are not tested.


## Examples

Examples | Erriez BMP280/BME280 sensor:

* [ErriezBMX280](https://github.com/Erriez/ErriezBMX280/blob/master/examples/ErriezBMX280/ErriezBMX280.ino)


## Documentation

- [Doxygen online HTML](https://erriez.github.io/ErriezBMX280)
- [Doxygen PDF](https://github.com/Erriez/ErriezBMX280/raw/master/ErriezBMX280.pdf)
- [BMP280 chip datasheet](https://github.com/Erriez/ErriezBMX280/blob/master/extras/BMP280_datasheet.pdf)
- [BME280 chip datasheet](https://github.com/Erriez/ErriezBMX280/blob/master/extras/BME280_datasheet.pdf)


## Example

```c++
#include <Wire.h>
#include <ErriezBMX280.h>

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA      1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);


void setup()
{
    // Initialize serial
    delay(500);
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    Serial.println(F("\nErriez BMP280/BMX280 example"));

    // Initialize I2C bus
    Wire.begin();
    Wire.setClock(400000);

    // Initialize sensor
    while (!bmx280.begin()) {
        Serial.println(F("Error: Could not detect sensor"));
        delay(3000);
    }

    // Print sensor type
    Serial.print(F("\nSensor type: "));
    switch (bmx280.getChipID()) {
        case CHIP_ID_BMP280:
            Serial.println(F("BMP280\n"));
            break;
        case CHIP_ID_BME280:
            Serial.println(F("BME280\n"));
            break;
        default:
            Serial.println(F("Unknown\n"));
            break;
    }
}

void loop()
{
    Serial.print(F("Temperature: "));
    Serial.print(bmx280.readTemperature());
    Serial.println(" C");

    if (bmx280.getChipID() == CHIP_ID_BME280) {
        Serial.print(F("Humidity:    "));
        Serial.print(bmx280.readHumidity());
        Serial.println(" %");
    }

    Serial.print(F("Pressure:    "));
    Serial.print(bmx280.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print(F("Altitude:    "));
    Serial.print(bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
    Serial.println(" m");

    Serial.println();

    delay(1000);
}
```
**Output**
```c++
Erriez BMP280/BMX280 example

Sensor type: BME280

Temperature: 28.50 C
Humidity:    45.13 %
Pressure:    1024.88 hPa
Altitude:    11.27 m

Temperature: 28.55 C
Humidity:    45.21 %
Pressure:    1024.89 hPa
Altitude:    11.21 m

...
```

### Set sampling

The sensor sampling and mode can be configured with function `setSampling()`. Recommended modes of
operation according to the datasheet chapter "Recommended modes of operation":

```c++
// Set sampling
//
// Weather
//  - forced mode, 1 sample / minute
//  - pressure ×1, temperature ×1, humidity ×1
//  - filter off
//
// Humidity sensing
//  - forced mode, 1 sample / second
//  - pressure ×0, temperature ×1, humidity ×1
//  - filter off
//
// Indoor navigation
//  - normal mode, t standby = 0.5 ms
//  - pressure ×16, temperature ×2, humidity ×1
//  - filter coefficient 16
//
// Gaming
//  - forced mode, t standby = 0.5 ms
//  - pressure ×1, temperature ×1, humidity ×1
//  - filter off
bmx280.setSampling(BMX280_MODE_NORMAL,    // SLEEP, FORCED, NORMAL
                   BMX280_SAMPLING_X16,   // Temp:  NONE, X1, X2, X4, X8, X16
                   BMX280_SAMPLING_X16,   // Press: NONE, X1, X2, X4, X8, X16
                   BMX280_SAMPLING_X16,   // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
                   BMX280_FILTER_X16,     // OFF, X2, X4, X8, X16
                   BMX280_STANDBY_MS_500);// 0_5, 10, 20, 62_5, 125, 250, 500, 1000
 ```

## Library dependencies

- Built-in ```Wire.h```


## Library installation

Please refer to the [Wiki](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches/wiki) page.


## Other Arduino Libraries and Sketches from Erriez

* [Erriez Libraries and Sketches](https://github.com/Erriez/ErriezArduinoLibrariesAndSketches)

