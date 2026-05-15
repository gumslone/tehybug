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
  for (size_t i = 0; i < _str.length(); i++) {
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
  String out = "";
  for (int i = 0; i < len; i++) {
    if (i > 0) {
      out = out + separator;
    }
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
  else if (key == "temp_imp" || key == "temp2_imp" || key == "dew_imp" || key == "hi_imp")
    return "°F";
  else if (key == "humi")
    return "%RH";
  else if (key == "ah")
    return "g/m³";
  else if (key == "air")
    return "kOhm";
  else if (key == "qfe")
    return "hPa";
  else if (key == "alt")
    return "m";
  else if (key == "lux")
    return "Lux";
  else if (key == "adc")
    return "ADC";
  else if (key == "co2")
    return "ppm";
  else if (key == "cr")
    return "%";

  return "";
}
String cf2name(int cs)
{
  switch(cs) {
    case Comfort_OK:
      return "OK";
    case Comfort_TooHot:
      return "Too hot";
    case Comfort_TooCold:
      return "Too cold";
    case Comfort_TooDry:
      return "Too dry";
    case Comfort_TooHumid:
      return "Too humid";
    case Comfort_HotAndHumid:
      return "Hot and humid";
    case Comfort_HotAndDry:
      return "Hot and dry";
    case Comfort_ColdAndHumid:
      return "Cold and humid";
    case Comfort_ColdAndDry:
      return "Cold and dry";
    default:
      return "Unknown";
  };
}
String key2name(const String & key)
{
  if (key == "temp" || key == "temp_imp")
    return "Temperature";
  else if (key == "temp2" || key == "temp2_imp")
    return "Temperature2";
  else if (key == "humi")
    return "Humidity";
  else if (key == "ah")
    return "Absolute humidity";
  else if (key == "cr")
    return "Comfort ratio";
  else if (key == "cs")
    return "Comfort status";
  else if (key == "dew" || key == "dew_imp")
    return "Dew point";
  else if (key == "hi" || key == "hi_imp")
    return "Heat index";
  else if (key == "air")
    return "Gas resistance";
  else if (key == "iaq")
    return "Indoor air quality";
  else if (key == "qfe")
    return "Atmospheric pressure";
  else if (key == "alt")
    return "Altitude";
  else if (key == "eco2")
    return "CO2 equivalent";
  else if (key == "co2")
    return "CO2";
  else if (key == "bvoc")
    return "breath VOC equivalent";
  else if (key == "uv")
    return "UV index";
  else if (key == "lux")
    return "Ambient light";
  else if (key == "adc")
    return "ADC";

  return "";
}
String key2icon(const String & key)
{
  if (key == "temp" || key == "temp2" || key == "temp_imp" || key == "temp2_imp")
    return "mdi:thermometer";
  else if (key == "humi")
    return "mdi:water-percent";
  else if (key == "ah")
    return "mdi:water";
  else if (key == "cr")
    return "mdi:sofa-outline";
  else if (key == "cs")
    return "mdi:sofa-outline";
  else if (key == "qfe")
    return "mdi:gauge";
  else if (key == "alt")
    return "mdi:image-filter-hdr";
  else if (key == "dew" || key == "dew_imp")
    return "mdi:water-thermometer";
  else if (key == "hi" || key == "hi_imp")
    return "mdi:sun-thermometer";
  else if (key == "air")
    return "mdi:resistor";
  else if (key == "co2" || key == "eco2")
    return "mdi:molecule-co2";
  else if (key == "iaq")
    return "mdi:airballoon-outline";
  
  return "mdi:help";
}
#endif
