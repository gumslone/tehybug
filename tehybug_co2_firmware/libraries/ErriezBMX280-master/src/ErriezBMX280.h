/*
 * MIT License
 *
 * Copyright (c) 2020 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * \file ErriezBMX280.h
 * \brief BMP280/BME280 sensor library for Arduino.
 * \details
 *      BMP280 supports temperature and pressure
 *      BME280 supports temperature, pressure and humidity
 * \details
 *     Source:          https://github.com/Erriez/ErriezBMX280
 *     Documentation:   https://erriez.github.io/ErriezBMX280
 */

#ifndef ERRIEZ_BME280_H_
#define ERRIEZ_BME280_H_

#include <Arduino.h>
#include <Wire.h>

// I2C address
#define BMX280_I2C_ADDR             0x76    //!< I2C address
#define BMX280_I2C_ADDR_ALT         0x77    //!< I2C alternative address

// Register defines
#define BMX280_REG_DIG_T1           0x88    //!< Temperature coefficient register
#define BMX280_REG_DIG_T2           0x8A    //!< Temperature coefficient register
#define BMX280_REG_DIG_T3           0x8C    //!< Temperature coefficient register

#define BMX280_REG_DIG_P1           0x8E    //!< Pressure coefficient register
#define BMX280_REG_DIG_P2           0x90    //!< Pressure coefficient register
#define BMX280_REG_DIG_P3           0x92    //!< Pressure coefficient register
#define BMX280_REG_DIG_P4           0x94    //!< Pressure coefficient register
#define BMX280_REG_DIG_P5           0x96    //!< Pressure coefficient register
#define BMX280_REG_DIG_P6           0x98    //!< Pressure coefficient register
#define BMX280_REG_DIG_P7           0x9A    //!< Pressure coefficient register
#define BMX280_REG_DIG_P8           0x9C    //!< Pressure coefficient register
#define BMX280_REG_DIG_P9           0x9E    //!< Pressure coefficient register

#define BME280_REG_DIG_H1           0xA1    //!< Humidity coefficient register
#define BME280_REG_DIG_H2           0xE1    //!< Humidity coefficient register
#define BME280_REG_DIG_H3           0xE3    //!< Humidity coefficient register
#define BME280_REG_DIG_H4           0xE4    //!< Humidity coefficient register
#define BME280_REG_DIG_H5           0xE5    //!< Humidity coefficient register
#define BME280_REG_DIG_H6           0xE7    //!< Humidity coefficient register

#define BME280_REG_CHIPID           0xD0    //!< Chip ID register
#define BME280_REG_RESET            0xE0    //!< Reset register

#define BME280_REG_CTRL_HUM         0xF2    //!< BME280: Control humidity register
#define BMX280_REG_STATUS           0XF3    //!< Status register
#define BMX280_REG_CTRL_MEAS        0xF4    //!< Control measure register
#define BMX280_REG_CONFIG           0xF5    //!< Config register
#define BMX280_REG_PRESS            0xF7    //!< Pressure data register
#define BMX280_REG_TEMP             0xFA    //!< Temperature data register
#define BME280_REG_HUM              0xFD    //!< Humidity data register

// Bit defines
#define CHIP_ID_BMP280              0x58    //!< BMP280 chip ID
#define CHIP_ID_BME280              0x60    //!< BME280 chip ID
#define RESET_KEY                   0xB6    //!< Reset value for reset register
#define STATUS_IM_UPDATE            0       //!< im_update bit in status register

/*!
 * \brief Sleep mode bits ctrl_meas register
 */
typedef enum {
    BMX280_MODE_SLEEP = 0b00,               //!< Sleep mode
    BMX280_MODE_FORCED = 0b01,              //!< Forced mode
    BMX280_MODE_NORMAL = 0b11               //!< Normal mode
} BMX280_Mode_e;

/*!
 * \brief Sampling bits registers ctrl_hum, ctrl_meas
 */
typedef enum {
    BMX280_SAMPLING_NONE = 0b000,           //!< Sampling disabled
    BMX280_SAMPLING_X1 = 0b001,             //!< x1 Sampling
    BMX280_SAMPLING_X2 = 0b010,             //!< x2 Sampling
    BMX280_SAMPLING_X4 = 0b011,             //!< x4 Sampling
    BMX280_SAMPLING_X8 = 0b100,             //!< x8 Sampling
    BMX280_SAMPLING_X16 = 0b101             //!< x16 Sampling
} BMX280_Sampling_e;

/*!
 * \brief Filter bits config register
 */
typedef enum {
    BMX280_FILTER_OFF = 0b000,              //!< Filter off
    BMX280_FILTER_X2 = 0b001,               //!< x2 Filter
    BMX280_FILTER_X4 = 0b010,               //!< x4 Filter
    BMX280_FILTER_X8 = 0b011,               //!< x8 Filter
    BMX280_FILTER_X16 = 0b100               //!< x16 Filter
} BMX280_Filter_e;

/*!
 * \brief Standby duration bits config register
 */
typedef enum {
    BMX280_STANDBY_MS_0_5 = 0b000,          //!< 0.5m standby
    BMX280_STANDBY_MS_10 = 0b110,           //!< 10ms standby
    BMX280_STANDBY_MS_20 = 0b111,           //!< 20ms standby
    BMX280_STANDBY_MS_62_5 = 0b001,         //!< 62.5 standby
    BMX280_STANDBY_MS_125 = 0b010,          //!< 125ms standby
    BMX280_STANDBY_MS_250 = 0b011,          //!< 250ms standby
    BMX280_STANDBY_MS_500 = 0b100,          //!< 500ms standby
    BMX280_STANDBY_MS_1000 = 0b101          //!< 1s standby
} BMX280_Standby_e;

/*!
 * \brief BMX280 class
 */
class ErriezBMX280
{
public:
    // Constructor
    ErriezBMX280(uint8_t i2cAddr);

    // Initialization
    bool begin();
    uint8_t getChipID();

    // BMP280/BME280
    float readTemperature();
    float readPressure();
    float readAltitude(float seaLevel);

    // BME280 only
    float readHumidity();

    // Configuration
    void setSampling(BMX280_Mode_e mode = BMX280_MODE_NORMAL,
                     BMX280_Sampling_e tempSampling = BMX280_SAMPLING_X16,
                     BMX280_Sampling_e pressSampling = BMX280_SAMPLING_X16,
                     BMX280_Sampling_e humSampling = BMX280_SAMPLING_X16,
                     BMX280_Filter_e filter = BMX280_FILTER_OFF,
                     BMX280_Standby_e standbyDuration = BMX280_STANDBY_MS_0_5);

    // Register access
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    uint16_t read16_LE(uint8_t reg); // little endian unsigned
    int16_t readS16_LE(uint8_t reg); // little endian signed
    uint32_t read24(uint8_t reg);
    void write8(uint8_t reg, uint8_t value);

private:
    uint8_t _i2cAddr;   //!< I2C address
    uint8_t _chipID;    //!< Chip iD
    int32_t _t_fine;    //!< Temperature variable

    uint16_t _dig_T1;
    int16_t _dig_T2;
    int16_t _dig_T3;

    uint16_t _dig_P1;
    int16_t _dig_P2;
    int16_t _dig_P3;
    int16_t _dig_P4;
    int16_t _dig_P5;
    int16_t _dig_P6;
    int16_t _dig_P7;
    int16_t _dig_P8;
    int16_t _dig_P9;

    uint8_t _dig_H1;
    int16_t _dig_H2;
    uint8_t _dig_H3;
    int16_t _dig_H4;
    int16_t _dig_H5;
    int8_t _dig_H6;

    // Read coefficient registers
    void readCoefficients(void);
};

#endif // ERRIEZ_BMX280_H_