#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "data_types.h"
#include "configuration.h"
#include "UUID.h"
#ifndef _TeHyBug_HEADER_
#define _TeHyBug_HEADER_
class TeHyBug {
  public:
    Calibration calibration{};
    Sensor sensor{};
    Device device{};
    DataServ serveData{};
    Scenarios scenarios{};
    DynamicJsonDocument sensorData;
    TeHyBugConfig conf;

    TeHyBug(DHTesp & dht): sensorData(1024), m_dht(dht), conf(calibration, sensor, device, serveData, scenarios) {
    }

    String replacePlaceholders(String text) {
      const JsonObject root = sensorData.as<JsonObject>();
      for (JsonPair keyValue : root) {
        String k = keyValue.key().c_str();
        String v = keyValue.value();
        text.replace("%" + k + "%", v);
      }
      return text;
    }

    void additionalSensorData(const String & key, float & value) {

      if (key == "temp" || key == "temp2") {
        addSensorData(key + "_imp", temp2Imp(value));
      }
      // humi should be always set after temp so the following calculation will work
      else if (key == "humi" || key == "humi2") {

        const String num = atoi(key.c_str()) > 0 ? String(atoi(key.c_str())) : "";

        const float hi = m_dht.computeHeatIndex(sensorData["temp" + num].as<float>(),
                                                sensorData[key + num].as<float>());
        addSensorData("hi" + num, hi);
        addSensorData("hi_imp" + num, temp2Imp(hi));

        const float dew = m_dht.computeDewPoint(sensorData["temp" + num].as<float>(),
                                                sensorData[key + num].as<float>());
        addSensorData("dew" + num, dew);
        addSensorData("dew_imp" + num, temp2Imp(dew));
      }
    }

    void addSensorData(const String & key, float value) {

      if (key == "temp" || key == "temp2") {
        value = calibrateValue("temp", value);
      } else if (key == "humi" || key == "humi2") {
        value = calibrateValue("humi", value);
      } else if (key == "qfe") {
        value = calibrateValue("qfe", value);
      }
      sensorData[key] = String(value, 1);
      // calculate imperial temperature also heat index and the dew point
      additionalSensorData(key, value);
    }
    void addTempHumi(const String & key_temp, float temp, const String & key_humi, float humi) {
      addSensorData(key_temp, temp);
      addSensorData(key_humi, humi);
    }

    void getDeviceKey() {
      // UUID – is a 36-character alphanumeric string
      String key = device.key;
      if (key.length() != 36) {
        key = generateDeviceKey();
        setDeviceKey(key);
      }
      D_println(F("key: "));
      D_println(key);
    }

    void handleRemoteControl(const String & data )
    {
      DynamicJsonDocument json(4096);
      const auto error = deserializeJson(json, data);
      if (!error) {
        JsonObject root = json.as<JsonObject>();
          if (json.containsKey("configMode")) {
            if (root["configMode"])
            {
              device.configMode = true;
              tickerStop = true;
            }
            else
            {
              device.configMode = false;
              tickerStart = true;
            }
          }
      }  
    }

    bool tickerStop = false;
    bool tickerStart = false;
    
  private:
    DHTesp & m_dht;
    UUID m_uuid;
    void setDeviceKey(String key) {
      device.key = key;
      sensorData["key"] = key;
    }
    String generateDeviceKey() {
      m_uuid.seed(ESP.getChipId());
      m_uuid.generate();
      return String(m_uuid.toCharArray());
    }
    float calibrateValue(const String & _n, float _v) {
      if (calibration.active) {
        if (_n == "temp")
          _v += calibration.temp;
        else if (_n == "humi")
          _v += calibration.humi;
        else if (_n == "qfe")
          _v += calibration.qfe;
      }
      return _v;
    }
};

#endif
