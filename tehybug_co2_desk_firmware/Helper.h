#pragma once

// find in string
uint8_t strContains(const char *string, char *toFind) {
  uint8_t slen = strlen(string);
  uint8_t tFlen = strlen(toFind);
  uint8_t found = 0;

  if (slen >= tFlen) {
    for (uint8_t s = 0, t = 0; s < slen; s++) {
      do {

        if (string[s] == toFind[t]) {
          if (++found == tFlen)
            return 1;
          s++;
          t++;
        } else {
          s -= found;
          found = 0;
          t = 0;
        }

      } while (found);
    }
    return 0;
  } else
    return -1;
}

// calculate CRC according to datasheet section 5.17
uint8_t CalcCrc(uint8_t data[2]) {
  uint8_t crc = 0xFF;
  for (uint8_t i = 0; i < 2; i++) {
    crc ^= data[i];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31u;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}

void i2c_scanner(HardwareSerial *serialport, String & i2c_addresses) {
  // i2c scanner begin
  uint8_t error, address;
  uint8_t nDevices;

  serialport->println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      serialport->print("I2C device found at address 0x");
      i2c_addresses = i2c_addresses + "0x";
      if (address < 16) {
        serialport->print("0");
        i2c_addresses = i2c_addresses + "0";
      }

      serialport->print(address, HEX);
      i2c_addresses = i2c_addresses + String(address, HEX) + ",";
      serialport->println("  !");

      nDevices++;
    } else if (error == 4) {
      serialport->print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      serialport->println(address, HEX);
    }
  }
  if (nDevices == 0)
    serialport->println("No I2C devices found\n");
  else
    serialport->println("done\n");

  // i2c scanner end
}

void temp2imp(const String val, String & var)
{
  float _t = 1.8 * val.toFloat() + 32;
  var = String(_t, 1);
}
