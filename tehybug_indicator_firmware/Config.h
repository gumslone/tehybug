#pragma once

#include <ArduinoJson.h>
#include <FS.h>

namespace Config {
char mqtt_server[80] = "example.tld";

char username[24] = "";
char password[65] = "";

bool imperial_temp = false;
bool imperial_qfe = false;
bool imperial_alt = false;
bool offline_mode = false;

void save() {
  DynamicJsonDocument json(512);
  json["mqtt_server"] = mqtt_server;
  json["username"] = username;
  json["password"] = password;

  json["imperial_temp"] = imperial_temp;
  json["imperial_qfe"] = imperial_qfe;
  json["imperial_alt"] = imperial_alt;
  json["offline_mode"] = offline_mode;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return;
  }

  serializeJson(json, configFile);
  configFile.close();
}

void load() {
  if (SPIFFS.begin()) {

    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        const size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(512);

        if (DeserializationError::Ok == deserializeJson(json, buf.get())) {
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(username, json["username"]);
          strcpy(password, json["password"]);
          imperial_temp = json["imperial_temp"];
          imperial_qfe = json["imperial_qfe"];
          imperial_alt = json["imperial_alt"];
          offline_mode = json["offline_mode"];
        }
      }
    }
  }
}

void reset() {
  if (SPIFFS.begin()) {

    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "w");
      configFile.println("");
      configFile.close();
    }
  }
}

} // namespace Config
