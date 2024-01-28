#pragma once

namespace i2cScanner {

String addresses;

// scan i2c devices
void scan() {
  // i2c scanner begin
  uint8_t error, address;
  int nDevices;
  D_println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      D_println("I2C device found at address 0x");
      addresses = addresses + "0x";
      if (address < 16) {
        D_print("0");
        addresses = addresses + "0";
      }

      D_println(address, HEX);
      addresses = addresses + String(address, HEX) + ",";
      D_println("  !");

      nDevices++;
    } else if (error == 4) {
      D_print("Unknown error at address 0x");
      if (address < 16)
        D_print("0");
      D_println(address, HEX);
    }
  }
  if (nDevices == 0)
    D_println("No I2C devices found\n");
  else
    D_println("done\n");

  // i2c scanner end
}

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

bool addressExists(char *addr) {
  return strContains(addresses.c_str(), addr) == 1;
}

} // namespace i2cScanner
