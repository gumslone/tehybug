#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "data_types.h"
#ifndef _TeHyBug_Config_
#define _TeHyBug_Config_
class TeHyBugConfig {
  public:

    TeHyBugConfig(Calibration & calibration, Sensor & sensor, Peripherals & peripherals, Device & device, DataServ & serveData, Scenarios & scenarios) :
      m_calibration(calibration),
      m_sensor(sensor),
      m_peripherals(peripherals),
      m_device(device),
      m_serveData(serveData),
      m_scenarios(scenarios)
    {}
    void saveConfigCallback() {
      m_shouldSaveConfig = true;
    }
    void saveConfig(bool force = false) {
      // save the custom parameters to FS
      if (m_shouldSaveConfig || force) {
        DynamicJsonDocument json(4096);

        json["mqttActive"] = m_serveData.mqtt.active;
        json["mqttRetained"] = m_serveData.mqtt.retained;
        json["mqttUser"] = m_serveData.mqtt.user;
        json["mqttPassword"] = m_serveData.mqtt.password;
        json["mqttServer"] = m_serveData.mqtt.server;
        json["mqttMasterTopic"] = m_serveData.mqtt.topic;
        json["mqttMessage"] = m_serveData.mqtt.message;
        json["mqttPort"] = m_serveData.mqtt.port;
        json["mqttFrequency"] = m_serveData.mqtt.frequency;

        json["haActive"] = m_serveData.ha.active;

        json["httpGetURL"] = m_serveData.get.url;
        json["httpGetActive"] = m_serveData.get.active;
        json["httpGetFrequency"] = m_serveData.get.frequency;

        json["httpPostURL"] = m_serveData.post.url;
        json["httpPostActive"] = m_serveData.post.active;
        json["httpPostFrequency"] = m_serveData.post.frequency;
        json["httpPostJson"] = m_serveData.post.message;

        json["calibrationActive"] = m_calibration.active;
        json["calibrationTemp"] = m_calibration.temp;
        json["calibrationHumi"] = m_calibration.humi;
        json["calibrationQfe"] = m_calibration.qfe;

        json["configModeActive"] = m_device.configMode;
        json["sleepModeActive"] = m_device.sleepMode;

        json["key"] = m_device.key;
        json["dht_sensor"] = m_sensor.dht;
        json["second_dht_sensor"] = m_sensor.dht_2;

        json["ds18b20_sensor"] = m_sensor.ds18b20;
        json["second_ds18b20_sensor"] = m_sensor.ds18b20_2;
        json["adc_sensor"] = m_sensor.adc;

        json["sc1_active"] = m_scenarios.scenario1.active;
        json["sc1_type"] = m_scenarios.scenario1.type;
        json["sc1_url"] = m_scenarios.scenario1.url;
        json["sc1_data"] = m_scenarios.scenario1.data;
        json["sc1_condition"] = m_scenarios.scenario1.condition;
        json["sc1_value"] = m_scenarios.scenario1.value;
        json["sc1_message"] = m_scenarios.scenario1.message;

        json["sc2_active"] = m_scenarios.scenario2.active;
        json["sc2_type"] = m_scenarios.scenario2.type;
        json["sc2_url"] = m_scenarios.scenario2.url;
        json["sc2_data"] = m_scenarios.scenario2.data;
        json["sc2_condition"] = m_scenarios.scenario2.condition;
        json["sc2_value"] = m_scenarios.scenario2.value;
        json["sc2_message"] = m_scenarios.scenario2.message;

        json["sc3_active"] = m_scenarios.scenario3.active;
        json["sc3_type"] = m_scenarios.scenario3.type;
        json["sc3_url"] = m_scenarios.scenario3.url;
        json["sc3_data"] = m_scenarios.scenario3.data;
        json["sc3_condition"] = m_scenarios.scenario3.condition;
        json["sc3_value"] = m_scenarios.scenario3.value;
        json["sc3_message"] = m_scenarios.scenario3.message;

        json["rc_active"] = m_device.remoteControl.active;
        json["rc_url"] = m_device.remoteControl.url;

        File configFile = SPIFFS.open("/config.json", "w");
        serializeJson(json, configFile);
        configFile.close();
        D_println(F("Config saved"));
        // end save
      }
    }

    void validateDataFrequency(int &freq) {
      const int maxDS = (int)(ESP.deepSleepMax() / 1000000);
      if (freq > maxDS) {
        freq = maxDS;
      }
    }

    void loadConfig() {
      if (SPIFFS.exists("/config.json")) {
        // file exists, reading and loading
        File configFile = SPIFFS.open("/config.json", "r");

        if (configFile) {
          D_println(F("opened config file"));

          DynamicJsonDocument json(4096);
          const auto error = deserializeJson(json, configFile);

          if (!error) {
            JsonObject documentRoot = json.as<JsonObject>();
            setConfigParameters(documentRoot);

            D_println(F("Config loaded"));
          } else {
            switch (error.code()) {
              case DeserializationError::Ok:
                D_println(F("Deserialization succeeded"));
                break;
              case DeserializationError::InvalidInput:
                D_println(F("Invalid input!"));
                break;
              case DeserializationError::NoMemory:
                D_println(F("Not enough memory"));
                break;
              default:
                D_println(F("Deserialization failed"));
                break;
            }
          }
        }
      } else {
        D_println(F("No configfile found, create a new file"));
        m_firstStart = true; //probably the device was factory reset or new
        saveConfig(true);
      }
    }

    void setConfig(JsonObject &json) {
      setConfigParameters(json);
      saveConfig(true);
    }

    String getConfig() {
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        const size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument root(4096);

        if (DeserializationError::Ok == deserializeJson(root, buf.get())) {}
        String json;
        serializeJson(root, json);
        return json;

      }
    }
    bool firstStart()
    {
      return m_firstStart;
    }

    bool rtcActive()
    {
      return m_peripherals.ds3231; 
    }
    bool eepromActive()
    {
      return m_peripherals.eeprom; 
    }

  private:

    bool m_shouldSaveConfig{false};
    bool m_firstStart{false};
    Calibration & m_calibration;
    Sensor & m_sensor;
    Device & m_device;
    DataServ & m_serveData;
    Scenarios & m_scenarios;
    Peripherals & m_peripherals;

    void setConfigParameters(const JsonObject &json) {
      D_println("Config:");
      if (DEBUG) {
        for (JsonPair kv : json) {
          D_print(kv.key().c_str());
          D_print(" = ");
          D_println(kv.value().as<String>());
        }
        D_println();
      }
      
      setData(json, "mqttActive", m_serveData.mqtt.active);
      setData(json, "mqttRetained", m_serveData.mqtt.retained);
      setData(json, "mqttUser", m_serveData.mqtt.user);
      setData(json, "mqttPassword", m_serveData.mqtt.password);
      setData(json, "mqttServer", m_serveData.mqtt.server);
      setData(json, "mqttMasterTopic", m_serveData.mqtt.topic);
      setData(json, "mqttMessage", m_serveData.mqtt.message);
      setData(json, "mqttPort", m_serveData.mqtt.port);
      
      if (json.containsKey("mqttFrequency")) {
        m_serveData.mqtt.frequency = json["mqttFrequency"].as<int>();
        validateDataFrequency(m_serveData.mqtt.frequency);
      }
      
      setData(json, "haActive", m_serveData.ha.active);
      
      // http
      setData(json, "httpGetURL", m_serveData.get.url);
      setData(json, "httpGetActive", m_serveData.get.active);
      
      if (json.containsKey("httpGetFrequency")) {
        m_serveData.get.frequency = json["httpGetFrequency"].as<int>();
        validateDataFrequency(m_serveData.get.frequency);
      }

      setData(json, "httpPostURL", m_serveData.post.url);
      setData(json, "httpPostActive", m_serveData.post.active);
      
      if (json.containsKey("httpPostFrequency")) {
        m_serveData.post.frequency = json["httpPostFrequency"].as<int>();
        validateDataFrequency(m_serveData.post.frequency);
      }
      
      setData(json, "httpPostJson", m_serveData.post.message);
      setData(json, "configModeActive", m_device.configMode);
      setData(json, "calibrationActive", m_calibration.active);
      setData(json, "calibrationTemp", m_calibration.temp);
      setData(json, "calibrationHumi", m_calibration.humi);
      setData(json, "calibrationQfe", m_calibration.qfe);
      setData(json, "sleepModeActive", m_device.sleepMode);
      setData(json, "dht_sensor", m_sensor.dht);
      setData(json, "second_dht_sensor", m_sensor.dht_2);
      setData(json, "ds18b20_sensor", m_sensor.ds18b20);
      setData(json, "second_ds18b20_sensor", m_sensor.ds18b20_2);
      setData(json, "adc_sensor", m_sensor.adc);

      // scenarios
      setData(json, "sc1_active", m_scenarios.scenario1.active);
      setData(json, "sc1_type", m_scenarios.scenario1.type);
      setData(json, "sc1_url", m_scenarios.scenario1.url);
      setData(json, "sc1_data", m_scenarios.scenario1.data);
      setData(json, "sc1_condition", m_scenarios.scenario1.condition);
      setData(json, "sc1_value", m_scenarios.scenario1.value);
      setData(json, "sc1_message", m_scenarios.scenario1.message);

      setData(json, "sc2_active", m_scenarios.scenario2.active);
      setData(json, "sc2_type", m_scenarios.scenario2.type);
      setData(json, "sc2_url", m_scenarios.scenario2.url);
      setData(json, "sc2_data", m_scenarios.scenario2.data);
      setData(json, "sc2_condition", m_scenarios.scenario2.condition);
      setData(json, "sc2_value", m_scenarios.scenario2.value);
      setData(json, "sc2_message", m_scenarios.scenario2.message);
      
      setData(json, "sc3_active", m_scenarios.scenario3.active);
      setData(json, "sc3_type", m_scenarios.scenario3.type);
      setData(json, "sc3_url", m_scenarios.scenario3.url);
      setData(json, "sc3_data", m_scenarios.scenario3.data);
      setData(json, "sc3_condition", m_scenarios.scenario3.condition);
      setData(json, "sc3_value", m_scenarios.scenario3.value);
      setData(json, "sc3_message", m_scenarios.scenario3.message);

      setData(json, "rc_active", m_device.remoteControl.active);
      setData(json, "rc_url", m_device.remoteControl.url);
    }
    
    template<typename T>
    void setData(const JsonObject &json, const String& key, T & var)
    {
      if (json.containsKey(key)) {
        var = json[key].as<T>();
      }
    }
    


}; // class Config

#endif
