#pragma once
#ifndef _SensorData_
#define _SensorData_
enum class SensorDataKeys {
  nan = 0,
  temp = 1,
  temp_imp = 2,
  temp2 = 3,
  temp2_imp = 4,
  humi = 5,
  dew = 6,
  dew_imp = 7,
  hi = 8,
  hi_imp = 9,
  air = 10,
  iaq = 11,
  qfe = 12,
  alt = 13,
  co2 = 14,
  eco2 = 15,
  bvoc = 16,
  uv = 17,
  lux = 18,
  adc = 19,
  key = 20,
  end = 21,
};
class SensorData {

  public:
  SensorData()
  {
    clearAll();
  }
  String toKey(SensorDataKeys key)
  {
    switch (key)
    {
    case SensorDataKeys::temp:
      return "temp";
    case SensorDataKeys::temp_imp:
      return "temp_imp";
    case SensorDataKeys::temp2:
      return "temp2";
    case SensorDataKeys::temp2_imp:
      return "temp2_imp";
    case SensorDataKeys::humi:
      return "humi";
    case SensorDataKeys::dew:
      return "dew";
    case SensorDataKeys::dew_imp:
      return "dew_imp";
    case SensorDataKeys::hi:
      return "hi";
    case SensorDataKeys::hi_imp:
      return "hi_imp";
    case SensorDataKeys::air:
      return "air";
    case SensorDataKeys::iaq:
      return "iaq";
    case SensorDataKeys::qfe:
      return "qfe"; 
    case SensorDataKeys::alt:
      return "alt";
    case SensorDataKeys::co2:
      return "co2";
    case SensorDataKeys::eco2:
      return "eco2";
    case SensorDataKeys::bvoc:
      return "bvoc";
    case SensorDataKeys::uv:
      return "uv";
    case SensorDataKeys::lux:
      return "lux";
    case SensorDataKeys::adc:
      return "adc";
    case SensorDataKeys::key:
      return "key";
    default:
      return "";
    }
  }
  SensorDataKeys toEnum(String key)
  {
    if (key == "temp")
      return SensorDataKeys::temp;
    if (key == "temp_imp")
      return SensorDataKeys::temp_imp;
    if (key == "temp2")
      return SensorDataKeys::temp2;
    if (key == "temp2_imp")
      return SensorDataKeys::temp2_imp;
    if (key == "humi")
      return SensorDataKeys::humi;
    if (key == "dew")
      return SensorDataKeys::dew;
    if (key == "dew_imp")
      return SensorDataKeys::dew_imp;
    if (key == "hi")
      return SensorDataKeys::hi;
    if (key == "hi_imp")
      return SensorDataKeys::hi_imp;
    if (key == "air")
      return SensorDataKeys::air;
    if (key == "iaq")
      return SensorDataKeys::iaq;
    if (key == "qfe")
      return SensorDataKeys::qfe;
    if (key == "alt")
      return SensorDataKeys::alt;
    if (key == "co2")
      return SensorDataKeys::co2;
    if (key == "eco2")
      return SensorDataKeys::eco2;
    if (key == "bvoc")
      return SensorDataKeys::bvoc;
    if (key == "uv")
      return SensorDataKeys::uv;
    if (key == "lux")
      return SensorDataKeys::lux;
    if (key == "adc")
      return SensorDataKeys::adc;
    if (key == "key")
      return SensorDataKeys::key;
    return SensorDataKeys::nan;
  }

  void set(SensorDataKeys key, float value)
  {
    m_data[(int)key] = value;
    m_filledParameters[(int)key] = true;
  }
  float get(SensorDataKeys key)
  {
    return m_data[(int)key];
  }
  bool isFilled(SensorDataKeys key)
  {
    return m_filledParameters[(int)key];
  }
  void clear()
  {
    clearAll();
  }
  void clear(SensorDataKeys key)
  {
    m_filledParameters[(int)key] = false;
    m_data[(int)key] = NAN;
  }
  void clearAll()
  {
    for (int i = 0; i < (int)SensorDataKeys::end; i++)
    {
      m_filledParameters[i] = false;
      m_data[i] = NAN;
    }
  }
  String toJson()
  {
    String json = "{";
    for (int i = 0; i < (int)SensorDataKeys::end; i++)
    {
      if (m_filledParameters[i])
      {
        if(i == (int)SensorDataKeys::key)
          continue;
          //json += "\"" + toKey((SensorDataKeys)i) + "\":\"" + String(m_data[i]) + "\",";
          json += "\"" + toKey((SensorDataKeys)i) + "\":" + String(m_data[i], 1) + ",";

      }
    }
    json.remove(json.length() - 1);
    json += "}";
    return json;
  }


  String valueToString(SensorDataKeys key)
  {
    return String(m_data[(int)key], 1);
  }
  String valueToString(SensorDataKeys key, int precision)
  {
    return String(m_data[(int)key], precision);
  }

  private:
  std::array<bool, (int)SensorDataKeys::end> m_filledParameters{};
  std::array<float, (int)SensorDataKeys::end> m_data{};
  
};
#endif
