#pragma once
namespace ha {
  char identifier[24];
  char MQTT_TOPIC_AVAILABILITY[128];
  char MQTT_TOPIC_STATE[128];
  char MQTT_TOPIC_COMMAND[128];
  
  char MQTT_TOPIC_AUTOCONF_T_SENSOR[128];
  char MQTT_TOPIC_AUTOCONF_H_SENSOR[128];
  char MQTT_TOPIC_AUTOCONF_P_SENSOR[128];
  char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];
  char MQTT_TOPIC_AUTOCONF_CO2_SENSOR[128];

void setupHandle(Device &device) {
  snprintf(identifier, sizeof(identifier), "TEHYBUG-%X", ESP.getChipId());
  snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", device.key.c_str(),
           identifier);
  snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", device.key.c_str(), identifier);
  snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", device.key.c_str(),
           identifier);

  snprintf(MQTT_TOPIC_AUTOCONF_CO2_SENSOR, 127,
           "homeassistant/sensor/%s/%s_co2/config", device.key.c_str(),
           identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127,
           "homeassistant/sensor/%s/%s_wifi/config", device.key.c_str(),
           identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_T_SENSOR, 127,
           "homeassistant/sensor/%s/%s_t/config", device.key.c_str(), identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_H_SENSOR, 127,
           "homeassistant/sensor/%s/%s_h/config", device.key.c_str(), identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_P_SENSOR, 127,
           "homeassistant/sensor/%s/%s_p/config", device.key.c_str(), identifier);
}

 void publishAutoConfig(PubSubClient mqttClient) {
  char mqttPayload[2048];
  DynamicJsonDocument device(256);
  DynamicJsonDocument autoconfPayload(1024);
  StaticJsonDocument<64> identifiersDoc;
  JsonArray identifiers = identifiersDoc.to<JsonArray>();

  identifiers.add(identifier);

  device["identifiers"] = identifiers;
  device["manufacturer"] = "TeHyBug";
  device["model"] = "TeHyBug Mini";
  device["name"] = identifier;
  device["sw_version"] = "2023.10.08";

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" WiFi");
  autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
  autoconfPayload["unique_id"] = identifier + String("_wifi");
  autoconfPayload["unit_of_measurement"] = "dBm";
  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] =
      "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": "
      "\"{{value_json.wifi.ip}}\"}";
  autoconfPayload["icon"] = "mdi:wifi";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[0], &mqttPayload[0],
                     true);

  autoconfPayload.clear();
/*
  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" CO2");
  autoconfPayload["unit_of_measurement"] = "ppm";
  autoconfPayload["value_template"] = "{{value_json.co2}}";
  autoconfPayload["unique_id"] = identifier + String("_co2");
  autoconfPayload["icon"] = "mdi:air-filter";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_CO2_SENSOR[0], &mqttPayload[0], true);

  autoconfPayload.clear();
*/
  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" Temperature");
  autoconfPayload["unit_of_measurement"] = "°C";

  autoconfPayload["value_template"] = "{{value_json.temperature}}";
  autoconfPayload["unique_id"] = identifier + String("_temperature");
  autoconfPayload["icon"] = "mdi:thermometer";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_T_SENSOR[0], &mqttPayload[0], true);

  autoconfPayload.clear();

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" Humidity");
  autoconfPayload["unit_of_measurement"] = "%RH";
  autoconfPayload["value_template"] = "{{value_json.humidity}}";
  autoconfPayload["unique_id"] = identifier + String("_humidity");
  autoconfPayload["icon"] = "mdi:water-percent";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_H_SENSOR[0], &mqttPayload[0], true);

  autoconfPayload.clear();

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" Barometric air pressure");
  autoconfPayload["unit_of_measurement"] = "hpa";
  autoconfPayload["value_template"] = "{{value_json.pressure}}";
  autoconfPayload["unique_id"] = identifier + String("_pressure");
  autoconfPayload["icon"] = "mdi:gauge";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_P_SENSOR[0], &mqttPayload[0], true);

  autoconfPayload.clear();
}

void publishState(PubSubClient mqttClient, DynamicJsonDocument sensorData) {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument stateJson(604);
  char payload[256];

  wifiJson["ssid"] = WiFi.SSID();
  wifiJson["ip"] = WiFi.localIP().toString();
  wifiJson["rssi"] = WiFi.RSSI();

  stateJson["temperature"] = sensorData["temp"];
  stateJson["humidity"] = sensorData["humi"];
  stateJson["pressure"] = sensorData["qfe"];

  stateJson["wifi"] = wifiJson.as<JsonObject>();

  serializeJson(stateJson, payload);
  mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
  stateJson.clear();
}

}
