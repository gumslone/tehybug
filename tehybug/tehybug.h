#pragma once
#include "data_types.h"
#include "pixel.h"
#include "configuration.h"
#include "UUID.h"
#include "time.h"
#include "eeprom.h"
#ifndef _TeHyBug_HEADER_
#define _TeHyBug_HEADER_
class TeHyBug {
  public:
    Calibration calibration{};
    Sensor sensor{};
    Device device{};
    Peripherals peripherals{};
    DataServ serveData{};
    Scenarios scenarios{};
    DynamicJsonDocument sensorData;
    TeHyBugConfig conf;
    RtcTime time;
    TeHyBugEeprom eeprom;
    TeHyBugPixel pixel;

    TeHyBug(DHTesp & dht): sensorData(1024), m_dht(dht), conf(calibration, sensor, peripherals, device, serveData, scenarios, pixel), time(conf), eeprom(time) {
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

        int atoinum = atoi(key.c_str());
        
        const String num = atoinum > 0 ? String(atoinum) : "";

        const float hi = m_dht.computeHeatIndex(sensorData["temp" + num].as<float>(),
                                                value);
        addSensorData("hi" + num, hi);
        addSensorData("hi_imp" + num, temp2Imp(hi));

        const float dew = m_dht.computeDewPoint(sensorData["temp" + num].as<float>(),
                                                value);
        addSensorData("dew" + num, dew);
        addSensorData("dew_imp" + num, temp2Imp(dew));

        m_dht.setupComfortProfile(); // required
        const float ah = m_dht.computeAbsoluteHumidity(sensorData["temp" + num].as<float>(), value);
        addSensorData("ah" + num, ah);
        
        ComfortState cs;
        const float cr = m_dht.getComfortRatio(cs, sensorData["temp" + num].as<float>(), value,  false);
        addSensorData("cr" + num, cr);
        addSensorData("cs" + num, (int)cs);
      }
    }

    void addSensorData(const String & key, float value) {
      value = calibrateValue(key, value);
      sensorData[key] = String(value, 1);
      // calculate imperial temperature also heat index and the dew point
      additionalSensorData(key, value);
    }
    void addSensorData(const String & key, int value) {
      sensorData[key] = String(value);
    }
    void addTempHumi(const String & key_temp, float temp, const String & key_humi, float humi) {
      addSensorData(key_temp, temp);
      addSensorData(key_humi, humi);
    }

    void sensorDataGarbageCollect()
    {
       if(!device.sleepMode)
       {
          sensorData.garbageCollect();
       }
    }

    void getDeviceKey() {
      // UUID – is a 36-character alphanumeric string
      String key = device.key;
      if (key.length() != 36) {
        key = generateDeviceKey();
        setDeviceKey(key);
      }
      D_print(F("key: "));
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
          if (json.containsKey("setConfig") && root["setConfig"]) {
              conf.setConfig(root);
          }
      }  
    }

    bool sleepEnabled()
    {
      return device.sleepMode || device.lightSleepMode; 
    }

    bool tickerStop{false};
    bool tickerStart{false};
    
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
    float calibrateValue(const String & key, float value) {
      if (calibration.active) {
        if (key == "temp" || key == "temp2")
          value += calibration.temp;
        else if (key == "humi" || key == "humi2")
          value += calibration.humi;
        else if (key == "qfe")
          value += calibration.qfe;
      }
      return value;
    }
};

#endif
