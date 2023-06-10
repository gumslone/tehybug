/// <summary>
/// Adds a leading 0 to a number if it is smaller than 10
/// </summary>
String IntFormat(int _int) {
  if (_int < 10) {
    return "0" + String(_int);
  }

  return String(_int);
}

/// <summary>
/// Checks if it is a valid IP address
/// </summary>
boolean isIP(String _str) {
  for (char i = 0; i < _str.length(); i++) {
    if (!(isDigit(_str.charAt(i)) || _str.charAt(i) == '.')) {
      return false;
    }
  }
  return true;
}

/// <summary>
/// Convert UTF8 byte to ASCII
/// </summary>
byte Utf8ToAscii(byte _ascii) {
  static byte _thisByte;

  if (_ascii < 128) {
    _thisByte = 0;
    return (_ascii);
  }

  byte last = _thisByte;
  _thisByte = _ascii;
  byte _result = 0;

  switch (last) {
  case 0xC2:
    _result = _ascii - 34;
    break;
  case 0xC3:
    _result = (_ascii | 0xC0) - 34;
    break;
  case 0x82:
    if (_ascii == 0xAC) {
      _result = (0xEA);
    }
    break;
  }
  return _result;
}

/// <summary>
/// Convert UTF8 Chars to ASCII
/// </summary>
String Utf8ToAscii(String _str) {
  String _result = "";
  char _thisChar;

  for (int i = 0; i < _str.length(); i++) {
    _thisChar = Utf8ToAscii(_str.charAt(i));

    if (_thisChar != 0) {
      _result += _thisChar;
    }
  }
  return _result;
}

/// <summary>
/// Returns the chip id
/// </summary>
String GetChipID() { return String(ESP.getChipId()); }

/// <summary>
/// Convert RSSI to percentage quality
/// </summary>
int GetRSSIasQuality(int rssi) {
  int quality = 0;

  if (rssi <= -100) {
    quality = 0;
  } else if (rssi >= -50) {
    quality = 100;
  } else {
    quality = 2 * (rssi + 100);
  }
  return quality;
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

String join(int *arr, String separator, int len) {
  int i;
  String out = "";
  for (i = 0; i < len; i++) {
    if (i > 0) /* You do need this separate check, unfortunately. */
      out = out + ",";
    out = out + String(arr[i]);
  }
  return out;
}

//================================================================================
// Begin calcdayofweek( D, M, Y)
//================================================================================
uint8_t dayOfWeek(uint8_t y, uint8_t m,
                  uint8_t d) // 1 <= m <= 12,  y > 1752 (in the U.K.)
{ // https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  y -= m < 3;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) %
         7; // Sun=0, Mon=1, Tue=2, Wed=3, Thu=4, Fri=5, Sat=6
}
//================================================================================
// End calcdayofweek( D, M, Y)
//================================================================================

String i2c_scanner() {
  // i2c scanner begin
  String i2c_addresses = "";
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      i2c_addresses = i2c_addresses + "0x";
      if (address < 16) {
        Serial.print("0");
        i2c_addresses = i2c_addresses + "0";
      }

      Serial.print(address, HEX);
      i2c_addresses = i2c_addresses + String(address, HEX) + ",";
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  return i2c_addresses;
  // i2c scanner end
}
