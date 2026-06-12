/***************************************************************************************************************************

	SenseAir S8 Library for Serial Modbus Communication

	Copyright (c) 2021 Josep Comas

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

***************************************************************************************************************************/


#include "s8_uart.h"
#include "modbus_crc.h"
#include "utils.h"


/* Initialize */
S8_UART::S8_UART(Stream &serial)
{
    mySerial = &serial;
}


/* Get firmware version */
void S8_UART::get_firmware_version(char firmver[]) {

    if (firmver == NULL) {
        return;
    }

    strcpy(firmver, "");

    // Ask software version
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR29, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        snprintf(firmver, S8_LEN_FIRMVER, "%0u.%0u", buf_msg[3], buf_msg[4]);
        LOG_DEBUG_INFO("Firmware version: ", firmver);

    } else {
        LOG_DEBUG_ERROR("Firmware version not available!");
    }

}


/* Get CO2 value in ppm */
int16_t S8_UART::get_co2() {

    int16_t co2 = 0;

    // Ask CO2 value
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR4, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        co2 = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO("CO2 value = ", co2, " ppm");

    } else {
        LOG_DEBUG_ERROR("Error getting CO2 value!");
    }

    return co2;
}


/* Read ABC period */
int16_t S8_UART::get_ABC_period() {

    int16_t period = 0;

    // Ask ABC period
    send_cmd(MODBUS_FUNC_READ_HOLDING_REGISTERS, MODBUS_HR32, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_HOLDING_REGISTERS, nb, 7)) {
        period = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO("ABC period = ", period, " hours");

    } else {
        LOG_DEBUG_ERROR("Error getting ABC period!");
    }

    return period;
}


/* Setup ABC period, default 180 hours (7.5 days) */
bool S8_UART::set_ABC_period(int16_t period) {
    uint8_t buf_msg_sent[8];
    bool result = false;

    if (period >= 0 && period <= 4800) {   // 0 = disable ABC algorithm

        // Ask set ABC period
        send_cmd(MODBUS_FUNC_WRITE_SINGLE_REGISTER, MODBUS_HR32, period);

        // Save bytes sent
        memcpy(buf_msg_sent, buf_msg, 8);

        // Wait response
        memset(buf_msg, 0, S8_LEN_BUF_MSG);
        serial_read_bytes(8, S8_TIMEOUT);

        // Check response
        if (memcmp(buf_msg_sent, buf_msg, 8) == 0) {
            result = true;
            LOG_DEBUG_INFO("Successful setting of ABC period");

        } else {
            LOG_DEBUG_ERROR("Error in setting of ABC period!");
        }

    } else {
        LOG_DEBUG_ERROR("Invalid ABC period!");
    }

    return result;
}


/* Read acknowledgement flags */
int16_t S8_UART::get_acknowledgement() {

    int16_t flags = 0;

    // Ask acknowledgement flags
    send_cmd(MODBUS_FUNC_READ_HOLDING_REGISTERS, MODBUS_HR1, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_HOLDING_REGISTERS, nb, 7)) {
        flags = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO_BINARY("Acknowledgement flags = b", flags);

    } else {
        LOG_DEBUG_ERROR("Error getting acknowledgement flags!");
    }

    return flags;
}


/* Read acknowledgement flags */
bool S8_UART::clear_acknowledgement() {
    uint8_t buf_msg_sent[8];
    bool result = false;

    // Ask clear acknowledgement flags
    send_cmd(MODBUS_FUNC_WRITE_SINGLE_REGISTER, MODBUS_HR1, 0x0000);

    // Save bytes sent
    memcpy(buf_msg_sent, buf_msg, 8);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    serial_read_bytes(8, S8_TIMEOUT);

    // Check response
    if (memcmp(buf_msg_sent, buf_msg, 8) == 0) {
        result = true;
        LOG_DEBUG_INFO("Successful clearing acknowledgement flags");

    } else {
        LOG_DEBUG_ERROR("Error clearing acknowledgement flags!");
    }

    return result;
}


/* Start a manual calibration (go to outdoors, wait 5 minutes o more and then you issue this command) */
bool S8_UART::manual_calibration() {
    bool result = clear_acknowledgement();

    if (result) {
        result = send_special_command(S8_CO2_BACKGROUND_CALIBRATION);

        if (result) {
            LOG_DEBUG_INFO("Manual calibration in background has started");

        } else {
            LOG_DEBUG_ERROR("Error starting manual calibration!");
        }

    }

    return result;
}


/* Send special command (high = command, low = parameter) */
/*
   Command = 0x7C,
   Parameter = 0x06 CO2 background calibration
   Parameter = 0x07 CO2 zero calibration
*/
bool S8_UART::send_special_command(int16_t command) {
    uint8_t buf_msg_sent[8];
    bool result = false;

    // Ask set user special command
    send_cmd(MODBUS_FUNC_WRITE_SINGLE_REGISTER, MODBUS_HR2, command);

    // Save bytes sent
    memcpy(buf_msg_sent, buf_msg, 8);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    serial_read_bytes(8, S8_TIMEOUT);

    // Check response
    if (memcmp(buf_msg_sent, buf_msg, 8) == 0) {
        result = true;
        LOG_DEBUG_INFO("Successful setting user special command");

    } else {
        LOG_DEBUG_ERROR("Error in setting user special command!");
    }

    return result;
}


/* Read meter status */
int16_t S8_UART::get_meter_status() {

    int16_t status = 0;

    // Ask meter status
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR1, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        status = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO_BINARY("Meter status = b", status);

    } else {
        LOG_DEBUG_ERROR("Error getting meter status!");
    }

    return status;
}


/* Read alarm status */
int16_t S8_UART::get_alarm_status() {

    int16_t status = 0;

    // Ask alarm status
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR2, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        status = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO_BINARY("Alarm status = b", status);

    } else {
        LOG_DEBUG_ERROR("Error getting alarm status!");
    }

    return status;
}


/* Read output status */
int16_t S8_UART::get_output_status() {

    int16_t status = 0;

    // Ask output status
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR3, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        status = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO_BINARY("Output status = b", status);

    } else {
        LOG_DEBUG_ERROR("Error getting output status!");
    }

    return status;
}


/* Read PWM output (0x3FFF = 100%)
    Raw PWM output to ppm: (raw_PWM_output / 16383.0) * 2000.0)
    2000.0 is max range of sensor (2000 ppm for normal version, extended version is 10000 ppm)
*/
int16_t S8_UART::get_PWM_output() {

    int16_t pwm = 0;

    // Ask PWM output
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR22, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        pwm = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO("PWM output (raw) = ", pwm);
        LOG_DEBUG_INFO("PWM output (to ppm, normal version) = ", (pwm / 16383.0) * 2000.0, " ppm");
        //LOG_DEBUG_INFO("PWM output (to ppm, extended version) = ", (pwm / 16383.0) * 10000.0, " ppm");

    } else {
        LOG_DEBUG_ERROR("Error getting PWM output!");
    }

    return pwm;
}


/* Read sensor type ID */
int32_t S8_UART::get_sensor_type_ID() {

    int32_t sensorType = 0;

    // Ask sensor type ID (high)
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR26, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {

        // Save sensor type ID (high)
        sensorType = (((int32_t)buf_msg[4] << 16) & 0x00FF0000);

        // Ask sensor type ID (low)
        send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR27, 0x0001);

        // Wait response
        memset(buf_msg, 0, S8_LEN_BUF_MSG);
        nb = serial_read_bytes(7, S8_TIMEOUT);

        // Check response and get data
        if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {

            sensorType |= ((buf_msg[3] << 8) & 0x0000FF00) | (buf_msg[4] & 0x000000FF);
            LOG_DEBUG_INFO_HEX("Sensor type ID = 0x", sensorType, 3);

        } else {
            LOG_DEBUG_ERROR("Error getting sensor type ID (low)!");
        }

    } else {
        LOG_DEBUG_ERROR("Error getting sensor type ID (high)!");
    }

    return sensorType;
}


/* Read sensor ID */
int32_t S8_UART::get_sensor_ID() {

    int32_t sensorID = 0;

    // Ask sensor ID (high)
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR30, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {

        // Save sensor ID (high)
        sensorID = (((int32_t)buf_msg[3] << 24) & 0xFF000000) | (((int32_t)buf_msg[4] << 16) & 0x00FF0000);

        // Ask sensor ID (low)
        send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR31, 0x0001);

        // Wait response
        memset(buf_msg, 0, S8_LEN_BUF_MSG);
        nb = serial_read_bytes(7, S8_TIMEOUT);

        // Check response and get data
        if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {

            sensorID |= ((buf_msg[3] << 8) & 0x0000FF00) | (buf_msg[4] & 0x000000FF);
            LOG_DEBUG_INFO_HEX("Sensor ID = 0x", sensorID, 4);

        } else {
            LOG_DEBUG_ERROR("Error getting sensor ID (low)!");
        }

    } else {
        LOG_DEBUG_ERROR("Error getting sensor ID (high)!");
    }

    return sensorID;
}


/* Read memory map version */
int16_t S8_UART::get_memory_map_version() {

    int16_t mmVersion = 0;

    // Ask memory map version
    send_cmd(MODBUS_FUNC_READ_INPUT_REGISTERS, MODBUS_IR28, 0x0001);

    // Wait response
    memset(buf_msg, 0, S8_LEN_BUF_MSG);
    uint8_t nb = serial_read_bytes(7, S8_TIMEOUT);

    // Check response and get data
    if (valid_response_len(MODBUS_FUNC_READ_INPUT_REGISTERS, nb, 7)) {
        mmVersion = ((buf_msg[3] << 8) & 0xFF00) | (buf_msg[4] & 0x00FF);
        LOG_DEBUG_INFO("Memory map version = ", mmVersion);

    } else {
        LOG_DEBUG_ERROR("Error getting memory map version!");
    }

    return mmVersion;
}


/* Check valid response and length of received message */
bool S8_UART::valid_response_len(uint8_t func, uint8_t nb, uint8_t len) {
    bool result = false;

    if (nb == len) {
        result = valid_response(func, nb);

    } else {
        LOG_DEBUG_ERROR("Unexpected length!");
    }

    return result;
}


/* Check if it is a valid message response of the sensor */
bool S8_UART::valid_response(uint8_t func, uint8_t nb) {

    uint16_t crc16;
    bool result = false;

    if (nb >= 7) {
        crc16 = modbus_CRC16(buf_msg, nb-2);
        if ((buf_msg[nb-2] == (crc16 & 0x00FF)) && (buf_msg[nb-1] == ((crc16 >> 8) & 0x00FF))) {

            if (buf_msg[0] == MODBUS_ANY_ADDRESS && (buf_msg[1] == MODBUS_FUNC_READ_HOLDING_REGISTERS || buf_msg[1] == MODBUS_FUNC_READ_INPUT_REGISTERS) && buf_msg[2] == nb-5) {
                LOG_DEBUG_VERBOSE("Valid response");
                result = true;

            } else {
                LOG_DEBUG_ERROR("Unexpected response!");
            }

        } else {
            LOG_DEBUG_ERROR("Checksum/length is invalid!");
        }

    } else {
        LOG_DEBUG_ERROR("Invalid length!");
    }

    return result;
}


/* Send command */
void S8_UART::send_cmd( uint8_t func, uint16_t reg, uint16_t value) {

    uint16_t crc16;

    if (((func == MODBUS_FUNC_READ_HOLDING_REGISTERS || func == MODBUS_FUNC_READ_INPUT_REGISTERS) && value >= 1) || (func == MODBUS_FUNC_WRITE_SINGLE_REGISTER)) {
        buf_msg[0] = MODBUS_ANY_ADDRESS;                // Address
        buf_msg[1] = func;                              // Function
        buf_msg[2] = (reg >> 8) & 0x00FF;               // High-input register
        buf_msg[3] = reg & 0x00FF;                      // Low-input register
        buf_msg[4] = (value >> 8) & 0x00FF;             // High-word to read or setup
        buf_msg[5] = value & 0x00FF;                    // Low-word to read or setup
        crc16 = modbus_CRC16(buf_msg, 6);
        buf_msg[6] = crc16 & 0x00FF;
        buf_msg[7] = (crc16 >> 8) & 0x00FF;
        serial_write_bytes(8);
    }
}


/* Send bytes to sensor */
void S8_UART::serial_write_bytes(uint8_t size) {

    LOG_DEBUG_VERBOSE_PACKET("Bytes to send: ", (uint8_t *)buf_msg, size);

    mySerial->write(buf_msg, size);
    mySerial->flush();
}


/* Read answer of sensor */
uint8_t S8_UART::serial_read_bytes(uint8_t max_bytes, uint32_t timeout_ms) {

    uint32_t start_t = millis();
    uint32_t end_t = start_t;
    bool readed = false;

    uint8_t nb = 0;
    if (max_bytes > 0 && timeout_ms > 0) {

        while (((end_t - start_t) <= timeout_ms) && !readed) {
            if(mySerial->available()) {
                nb = mySerial->readBytes(buf_msg, max_bytes);
                readed = true;
            }
            end_t = millis();
        }

        if (readed) {
            if (nb > 0) {
                LOG_DEBUG_VERBOSE_PACKET("Bytes received: ", (uint8_t *)buf_msg, nb);

            } else {
                LOG_DEBUG_ERROR("Unexpected reading serial port!");
            }

        } else {
            LOG_DEBUG_ERROR("Timeout reading serial port!");
        }

    } else {
        LOG_DEBUG_ERROR("Invalid parameters!");
    }

    return nb;
}

