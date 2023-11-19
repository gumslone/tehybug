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
// time
// Time
int Year, Month, Day, Hour, Minute, Second;

int year() { return 0; }
int month() { return 0; }
int day() { return 0; }
int hour() { return 0; }
int minute() { return 0; }
int second() { return 0; }

void createDateElements(const char *str) {
  sscanf(str, "%d-%d-%dT%d:%d", year(), month(), day(), hour(), minute());
}
void createWeekdaysElements(const char *str, int *arr) {
  sscanf(str, "%d,%d,%d,%d,%d,%d,%d", &arr[0], &arr[1], &arr[2], &arr[3],
         &arr[4], &arr[5], &arr[6]);
}
//================================================================================
// Begin dayOfWeek( D, M, Y)
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
// End dayOfWeek( D, M, Y)
//================================================================================

float temp2Imp(float value) { return (1.8 * value + 32); }

bool isIOScenario(const String & type)
{
  return type.substring(0, 2) == "io";
}
uint8_t ioScenarioPin(const String & type)
{
  return atoi(type.substring(2, 4).c_str());
}
uint8_t ioScenarioLevel(const String & type)
{
    size_t lenz = type.length();
    return atoi(type.substring(lenz-1, lenz).c_str());
}
