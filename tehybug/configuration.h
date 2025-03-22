#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "data_types.h"
#ifndef _TeHyBug_Config_
#define _TeHyBug_Config_
class TeHyBugConfig {
  public:

    TeHyBugConfig(Calibration & calibration, Sensor & sensor, Peripherals & peripherals, Device & device, DataServ & serveData, Scenarios & scenarios, TeHyBugPixel & pixel) :
      m_calibration(calibration),
      m_sensor(sensor),
      m_peripherals(peripherals),
      m_device(device),
      m_serveData(serveData),
      m_scenarios(scenarios),
      m_pixel(pixel)
    {}
    void saveConfigCallback() {
      m_shouldSaveConfig = true;
    }
    void saveConfig(bool force = false) {
      // save the custom parameters to FS
      if (m_shouldSaveConfig || force) {

        Calibration calibration{};
        Sensor sensor{};
        Device device{};
        DataServ serveData{};
        Scenarios scenarios{};  

        DynamicJsonDocument json(3072);

        json["key"] = m_device.key;

        setIfNotDefault(json, "mqttActive", m_serveData.mqtt.active, serveData.mqtt.active);
        setIfNotDefault(json, "mqttRetained", m_serveData.mqtt.retained, serveData.mqtt.active);
        setIfNotDefault(json, "mqttUser", m_serveData.mqtt.user, serveData.mqtt.user);
        setIfNotDefault(json, "mqttPassword", m_serveData.mqtt.password, serveData.mqtt.password);
        setIfNotDefault(json, "mqttServer", m_serveData.mqtt.server, serveData.mqtt.server);
        setIfNotDefault(json, "mqttMasterTopic", m_serveData.mqtt.topic, serveData.mqtt.topic);
        setIfNotDefault(json, "mqttMessage", m_serveData.mqtt.message,  serveData.mqtt.message);
        setIfNotDefault(json, "mqttPort", m_serveData.mqtt.port, serveData.mqtt.port);
        setIfNotDefault(json, "mqttFrequency", m_serveData.mqtt.frequency, m_serveData.mqtt.frequency);

        setIfNotDefault(json, "haActive", m_serveData.ha.active, serveData.ha.active);

        setIfNotDefault(json, "httpGetURL", m_serveData.get.url,  serveData.get.url);
        setIfNotDefault(json, "httpGetActive", m_serveData.get.active, serveData.get.active);
        setIfNotDefault(json, "httpGetFrequency", m_serveData.get.frequency, serveData.get.frequency);

        setIfNotDefault(json, "httpPostURL", m_serveData.post.url, serveData.post.url);
        setIfNotDefault(json, "httpPostActive", m_serveData.post.active, serveData.post.active);
        setIfNotDefault(json, "httpPostFrequency", m_serveData.post.frequency, serveData.post.frequency);
        setIfNotDefault(json, "httpPostJson", m_serveData.post.message, serveData.post.message);

        setIfNotDefault(json, "calibrationActive", m_calibration.active, calibration.active);
        setIfNotDefault(json, "calibrationTemp",  m_calibration.temp, calibration.temp);
        setIfNotDefault(json, "calibrationHumi", m_calibration.humi, calibration.humi);
        setIfNotDefault(json, "calibrationQfe", m_calibration.qfe, calibration.qfe);

        setIfNotDefault(json, "configModeActive", m_device.configMode, device.configMode); // true by default
        setIfNotDefault(json, "sleepModeActive", m_device.sleepMode, device.sleepMode);
        setIfNotDefault(json, "lightSleepModeActive", m_device.lightSleepMode, device.lightSleepMode);
        
        setIfNotDefault(json, "dht_sensor", m_sensor.dht, sensor.dht);
        setIfNotDefault(json, "second_dht_sensor", m_sensor.dht_2, sensor.dht_2);

        setIfNotDefault(json, "ds18b20_sensor", m_sensor.ds18b20, sensor.ds18b20);
        setIfNotDefault(json, "second_ds18b20_sensor", m_sensor.ds18b20_2, sensor.ds18b20_2);
        setIfNotDefault(json, "adc_sensor", m_sensor.adc, sensor.adc);

        setIfNotDefault(json, "sc1_active", m_scenarios.scenario1.active, scenarios.scenario1.active);
        setIfNotDefault(json, "sc1_type", m_scenarios.scenario1.type, scenarios.scenario1.type);
        setIfNotDefault(json, "sc1_url", m_scenarios.scenario1.url, scenarios.scenario1.url);
        setIfNotDefault(json, "sc1_data", m_scenarios.scenario1.data, scenarios.scenario1.data);
        setIfNotDefault(json, "sc1_condition", m_scenarios.scenario1.condition, scenarios.scenario1.condition);
        setIfNotDefault(json, "sc1_value", m_scenarios.scenario1.value, scenarios.scenario1.value);
        setIfNotDefault(json, "sc1_message", m_scenarios.scenario1.message, scenarios.scenario1.message);

        setIfNotDefault(json, "sc2_active", m_scenarios.scenario2.active, scenarios.scenario2.active);
        setIfNotDefault(json, "sc2_type", m_scenarios.scenario2.type, scenarios.scenario2.type);
        setIfNotDefault(json, "sc2_url", m_scenarios.scenario2.url, scenarios.scenario2.url);
        setIfNotDefault(json, "sc2_data", m_scenarios.scenario2.data, scenarios.scenario2.data);
        setIfNotDefault(json, "sc2_condition", m_scenarios.scenario2.condition, scenarios.scenario2.condition);
        
        setIfNotDefault(json, "sc2_value", m_scenarios.scenario2.value, scenarios.scenario2.value);
        setIfNotDefault(json, "sc2_message", m_scenarios.scenario2.message, scenarios.scenario2.message);

        setIfNotDefault(json, "sc3_active", m_scenarios.scenario3.active, scenarios.scenario3.active);
        setIfNotDefault(json, "sc3_type", m_scenarios.scenario3.type, scenarios.scenario3.type);
        setIfNotDefault(json, "sc3_url", m_scenarios.scenario3.url, scenarios.scenario3.url);
        setIfNotDefault(json, "sc3_data", m_scenarios.scenario3.data, scenarios.scenario3.data);
        setIfNotDefault(json, "sc3_condition", m_scenarios.scenario3.condition, scenarios.scenario3.condition);
        setIfNotDefault(json, "sc3_value", m_scenarios.scenario3.value, scenarios.scenario3.value);
        setIfNotDefault(json, "sc3_message", m_scenarios.scenario3.message, scenarios.scenario3.message);

        setIfNotDefault(json, "rc_active", m_device.remoteControl.active, device.remoteControl.active);
        setIfNotDefault(json, "rc_url", m_device.remoteControl.url, device.remoteControl.url);

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
    bool configExists() {
      return SPIFFS.exists("/config.json");
    }

    void loadConfig() {
      if (configExists()) {
        // file exists, reading and loading
        File configFile = SPIFFS.open("/config.json", "r");

        if (configFile) {
          D_println(F("opened config file"));

          DynamicJsonDocument json(3072);
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
      
      // restart the module when reboot is requested in save config
      if (json.containsKey("reboot") && json["reboot"]) {
        m_pixel.off();
        yield();
        delay(1000);
        ESP.restart();
      }
    }

    String getConfig() {
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        const size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument root(3072);

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
    TeHyBugPixel & m_pixel;

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
      setData(json, "lightSleepModeActive", m_device.lightSleepMode);
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

    template<typename T>
    void setIfNotDefault(DynamicJsonDocument &json, const String& key, T & var, T & defaultVar)
    {
      if(var != defaultVar)
      {
        json[key] = var;
      }
    }

}; // class Config

#endif
