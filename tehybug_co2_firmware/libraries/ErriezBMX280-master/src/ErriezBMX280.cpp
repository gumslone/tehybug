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
 * \file ErriezBMX280.cpp
 * \brief BMP280/BME280 sensor library for Arduino.
 * \details
 *      BMP280 supports temperature and pressure
 *      BME280 supports temperature, pressure and humidity
 * \details
 *     Source:          https://github.com/Erriez/ErriezBMX280
 *     Documentation:   https://erriez.github.io/ErriezBMX280
 */

#include "ErriezBMX280.h"

/*!
 * \brief Constructor
 * \param i2cAddr
 *      I2C address
 */
ErriezBMX280::ErriezBMX280(uint8_t i2cAddr) : _i2cAddr(i2cAddr), _t_fine(0)
{

}

/*!
 * \brief Sensor initialization
 * \retval true
 *      BMP280 or BME280 sensor detected
 * \retval false
 *      Error: No (supported) sensor detected
 */
bool ErriezBMX280::begin()
{
    // Read chip ID
    _chipID = read8(BME280_REG_CHIPID);

    // Check sensor ID BMP280 or BME280
    if ((_chipID != CHIP_ID_BMP280) && ((_chipID != CHIP_ID_BME280))) {
        // BMP280 / BME280 not found
        return false;
    }

    // Generate soft-reset
    write8(BME280_REG_RESET, RESET_KEY);

    // Wait for copy completion NVM data to image registers
    delay(10);
    while ((read8(BMX280_REG_STATUS) & (1 << STATUS_IM_UPDATE))) {
        delay(10);
    }

    // See datasheet 4.2.2 Trimming parameter readout
    readCoefficients();

    // Set default sampling
    setSampling();

    // Wait for first completed conversion
    delay(100);

    // BMP280 or BME280 detected
    return true;
}

/*!
 * \brief Get chip ID
 * \return
 *      Chip ID as read with begin()
 */
uint8_t ErriezBMX280::getChipID()
{
    // Return chip ID
    return _chipID;
}

/*!
 * \brief Read temperature
 * \return
 *      Temperature (float)
 */
float ErriezBMX280::readTemperature()
{
    int32_t var1, var2, adc_T;
    float temperature;

    // Read temperature registers
    adc_T = read24(BMX280_REG_TEMP);
    adc_T >>= 4;

    // See datasheet 4.2.3 Compensation formulas
    var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1))) * ((int32_t)_dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)_dig_T1)) *
            ((adc_T >> 4) - ((int32_t)_dig_T1))) >> 12) *
            ((int32_t)_dig_T3)) >> 14;

    _t_fine = var1 + var2;

    temperature = ((_t_fine * 5) + 128) >> 8;

    return temperature / 100.0;
}

/*!
 * \brief Read pressure
 * \return
 *      Pressure (float)
 */
float ErriezBMX280::readPressure()
{
    int64_t var1;
    int64_t var2;
    int64_t p;
    int32_t adc_P;

    // Read temperature for t_fine
    readTemperature();

    // Read pressure registers
    adc_P = read24(BMX280_REG_PRESS);
    adc_P >>= 4;

    // See datasheet 4.2.3 Compensation formulas
    var1 = ((int64_t)_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_dig_P6;
    var2 = var2 + ((var1 * (int64_t)_dig_P5) << 17);
    var2 = var2 + (((int64_t)_dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_dig_P3) >> 8) + ((var1 * (int64_t)_dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_dig_P7) << 4);

    return (float)p / 256;
}

/*!
 * \brief Read approximate altitude
 * \param seaLevel
 *      Sea level in hPa
 * \return
 *      Altitude (float)
 */
float ErriezBMX280::readAltitude(float seaLevel)
{
    float atmospheric = readPressure() / 100.0F;

    // In Si units for Pascal
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/*!
 * \brief Read humidity (BME280 only)
 * \return
 *      Humidity (float)
 */
float ErriezBMX280::readHumidity()
{
    int32_t v_x1_u32r;
    int32_t adc_H;
    float humidity;

    if (_chipID != CHIP_ID_BME280) {
        return 0;
    }

    // Read temperature for t_fine
    readTemperature();

    // Read humidity registers
    adc_H = read16(BME280_REG_HUM);

    // See datasheet 4.2.3 Compensation formulas
    v_x1_u32r = (_t_fine - ((int32_t)76800));

    v_x1_u32r = ((((adc_H << 14) - (((int32_t)_dig_H4) << 20) - (((int32_t)_dig_H5) * v_x1_u32r)) +
                  ((int32_t)16384)) >> 15) *
                (((((((v_x1_u32r *
                       ((int32_t)_dig_H6)) >> 10) *
                     (((v_x1_u32r *
                        ((int32_t)_dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                  ((int32_t)_dig_H2) + 8192) >> 14);

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)_dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

    humidity = (v_x1_u32r >> 12);

    return humidity / 1024.0;
}

/*!
 * \brief Read coefficient registers at startup
 */
void ErriezBMX280::readCoefficients(void)
{
    _dig_T1 = read16_LE(BMX280_REG_DIG_T1);
    _dig_T2 = readS16_LE(BMX280_REG_DIG_T2);
    _dig_T3 = readS16_LE(BMX280_REG_DIG_T3);

    _dig_P1 = read16_LE(BMX280_REG_DIG_P1);
    _dig_P2 = readS16_LE(BMX280_REG_DIG_P2);
    _dig_P3 = readS16_LE(BMX280_REG_DIG_P3);
    _dig_P4 = readS16_LE(BMX280_REG_DIG_P4);
    _dig_P5 = readS16_LE(BMX280_REG_DIG_P5);
    _dig_P6 = readS16_LE(BMX280_REG_DIG_P6);
    _dig_P7 = readS16_LE(BMX280_REG_DIG_P7);
    _dig_P8 = readS16_LE(BMX280_REG_DIG_P8);
    _dig_P9 = readS16_LE(BMX280_REG_DIG_P9);

    if (_chipID == CHIP_ID_BME280) {
        _dig_H1 = read8(BME280_REG_DIG_H1);
        _dig_H2 = readS16_LE(BME280_REG_DIG_H2);
        _dig_H3 = read8(BME280_REG_DIG_H3);
        _dig_H4 = ((int8_t) read8(BME280_REG_DIG_H4) << 4) | (read8(BME280_REG_DIG_H4 + 1) & 0xF);
        _dig_H5 = ((int8_t) read8(BME280_REG_DIG_H5 + 1) << 4) | (read8(BME280_REG_DIG_H5) >> 4);
        _dig_H6 = (int8_t) read8(BME280_REG_DIG_H6);
    }
}

/*!
 * \brief Set sampling registers
 * \param mode
 *      See BMX280_Mode_e
 * \param tempSampling
 *      See BMX280_Sampling_e
 * \param pressSampling
 *      See BMX280_Sampling_e
 * \param humSampling
 *      See BMX280_Sampling_e
 * \param filter
 *      See BMX280_Filter_e
 * \param standbyDuration
 *      See BMX280_Standby_e
 */
void ErriezBMX280::setSampling(BMX280_Mode_e mode,
                               BMX280_Sampling_e tempSampling,
                               BMX280_Sampling_e pressSampling,
                               BMX280_Sampling_e humSampling,
                               BMX280_Filter_e filter,
                               BMX280_Standby_e standbyDuration)
{
    // Set in sleep mode to provide write access to the “config” register
    write8(BMX280_REG_CTRL_MEAS, BMX280_MODE_SLEEP);

    if (_chipID == CHIP_ID_BME280) {
        // See datasheet 5.4.3 Register 0xF2 “ctrl_hum”
        write8(BME280_REG_CTRL_HUM, humSampling);
    }
    // See datasheet 5.4.6 Register 0xF5 “config”
    write8(BMX280_REG_CONFIG, (standbyDuration << 5) | (filter << 2));
    // See datasheet 5.4.5 Register 0xF4 “ctrl_meas”
    write8(BMX280_REG_CTRL_MEAS, (tempSampling << 5) | (pressSampling << 2) | mode);
}

/*!
 * \brief Read from 8-bit register
 * \param reg
 *      Register address
 * \return
 *      8-bit register value
 */
uint8_t ErriezBMX280::read8(uint8_t reg)
{
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        return 0;
    }
    Wire.requestFrom(_i2cAddr, (byte)1);
    return Wire.read();
}

/*!
 * \brief Write to 8-bit register
 * \param reg
 *      Register address
 * \param value
 *      8-bit register value
 */
void ErriezBMX280::write8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

/*!
 * \brief Read from 16-bit unsigned register little endian
 * \param reg
 *      Register address
 * \return
 *      16-bit unsigned register value in little endian
 */
uint16_t ErriezBMX280::read16_LE(uint8_t reg)
{
    uint16_t value;

    value = read16(reg);

    return (value >> 8) | (value << 8);
}

/*!
 * \brief Read from 16-bit signed register little endian
 * \param reg
 *      Register address
 * \return
 *      16-bit signed register value in little endian
 */
int16_t ErriezBMX280::readS16_LE(uint8_t reg)
{
    return (int16_t)read16_LE(reg);
}

/*!
 * \brief Read from 16-bit register
 * \param reg
 *      Register address
 * \return
 *      16-bit register value
 */
uint16_t ErriezBMX280::read16(uint8_t reg)
{
    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        return 0;
    }
    Wire.requestFrom(_i2cAddr, (byte)2);
    return (Wire.read() << 8) | Wire.read();
}

/*!
 * \brief Read from 24-bit register
 * \param reg
 *      Register address
 * \return
 *      24-bit register value
 */
uint32_t ErriezBMX280::read24(uint8_t reg)
{
    uint32_t value;

    Wire.beginTransmission(_i2cAddr);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_i2cAddr, (byte)3);

    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();

    return value;
}