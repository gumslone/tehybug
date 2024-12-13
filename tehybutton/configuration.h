#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include "data_types.h"
#ifndef _TeHyButtonConfig_
#define _TeHyButtonConfig_
class TeHyButtonConfig {
  public:

    TeHyButtonConfig(Device & device, DataServ & serveData) :
      m_device(device),
      m_serveData(serveData)  {}
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
        
        json["httpGetURL"] = m_serveData.get.url;
        json["httpGetActive"] = m_serveData.get.active;
        json["httpGetFrequency"] = m_serveData.get.frequency;

        json["httpPostURL"] = m_serveData.post.url;
        json["httpPostActive"] = m_serveData.post.active;
        json["httpPostFrequency"] = m_serveData.post.frequency;
        json["httpPostJson"] = m_serveData.post.message;

        json["configModeActive"] = m_device.configMode;
        json["sleepModeActive"] = m_device.sleepMode;
        json["sleepModeActive"] = m_device.skipButtonActions;

        json["key"] = m_device.key;
       

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
        saveConfigCallback();
        saveConfig();
      }
    }

    void setConfig(JsonObject &json) {
      setConfigParameters(json);
      saveConfigCallback();
      saveConfig();

      if (json.containsKey("reboot") && json["reboot"]) {
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
        DynamicJsonDocument root(4096);

        if (DeserializationError::Ok == deserializeJson(root, buf.get())) {}
        String json;
        serializeJson(root, json);
        return json;

      }
    }

  private:

    bool m_shouldSaveConfig = false;
    Device & m_device;
    DataServ & m_serveData;
    
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
      setData(json, "skipButtonActions", m_device.skipButtonActions);
      setData(json, "sleepModeActive", m_device.sleepMode);
     
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
