#pragma once
#ifndef _COMMON_FUNCTIONS_
#define _COMMON_FUNCTIONS_
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

float temp2Imp(const float & value) {
  return (1.8 * value + 32);
}

bool isIoScenario(const String &type) {
  return type.substring(0, 2) == "io";
}
uint8_t ioScenarioPin(const String &type) {
  return atoi(type.substring(2, 4).c_str());
}
uint8_t ioScenarioLevel(const String &type) {
  size_t lenz = type.length();
  return atoi(type.substring(lenz - 1, lenz).c_str());
}
String key2unit(const String & key)
{
  if (key == "temp" || key == "temp2" || key == "dew" || key == "hi")
    return "°C";
  if (key == "temp_imp" || key == "temp2_imp" || key == "dew_imp" || key == "hi_imp")
    return "°F";
  if (key == "humi")
    return "%RH";
  if (key == "air")
    return "kOhm";
  if (key == "qfe")
    return "hPa";
  if (key == "alt")
    return "m";
  if (key == "lux")
    return "Lux";
  if (key == "adc")
    return "ADC";
  if (key == "co2")
    return "ppm";

  return "";
}
String key2name(const String & key)
{
  if (key == "temp" || key == "temp_imp")
    return "Temperature";
  if (key == "temp2" || key == "temp2_imp")
    return "Temperature2";
  if (key == "humi")
    return "Humidity";
  if (key == "dew")
    return "Dew point";
  if (key == "dew_imp")
    return "Dew point";
  if (key == "hi" || key == "hi_imp")
    return "Heat index";
  if (key == "air")
    return "Gas resistance";
  if (key == "iaq")
    return "Indoor air quality";
  if (key == "qfe")
    return "Atmospheric pressure";
  if (key == "alt")
    return "Altitude";
  if (key == "eco2")
    return "CO2 equivalent";
  if (key == "co2")
    return "CO2";
  if (key == "bvoc")
    return "breath VOC equivalent";
  if (key == "uv")
    return "UV index";
  if (key == "lux")
    return "Ambient light";
  if (key == "adc")
    return "ADC";

  return "";
}
String key2icon(const String & key)
{
  if (key == "temp" || key == "temp2" || key == "temp_imp" || key == "temp2_imp")
    return "mdi:thermometer";
  if (key == "humi")
    return "mdi:water-percent";
  if (key == "qfe")
    return "mdi:gauge";
  if (key == "alt")
    return "mdi:image-filter-hdr";
  if (key == "dew"||key =="dew_imp")
    return "mdi:water-thermometer";
  if (key == "hi" || key == "hi_imp")
    return "mdi:sun-thermometer";
  if (key == "air")
    return "mdi:resistor";
  if (key == "co2"||key == "eco2")
    return "mdi:molecule-co2";
  if(key == "iaq")
    return "mdi:airballoon-outline";
  return "mdi:help";
}
#endif
