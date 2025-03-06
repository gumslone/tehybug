#pragma once
#define AVAILABILITY_ONLINE "online"
#define AVAILABILITY_OFFLINE "offline"

namespace ha {
  char identifier[16];
  char MQTT_TOPIC_STATE[64];
  char MQTT_TOPIC_AVAILABILITY[64];
  //char MQTT_TOPIC_COMMAND[64];
  
void setupHandle(const Device &device) {
  const String deviceName = "TEHYBUG";
  snprintf(identifier, sizeof(identifier), "%s-%X", deviceName.c_str(), ESP.getChipId());
  snprintf(MQTT_TOPIC_STATE, 63, "%s/%s/state", deviceName.c_str(), identifier);
  snprintf(MQTT_TOPIC_AVAILABILITY, 63, "%s/%s/status", deviceName.c_str(),
           identifier);
  //snprintf(MQTT_TOPIC_COMMAND, 63, "%s/%s/command", deviceName.c_str(),
  //         identifier);
}

 void publishAutoConfig(PubSubClient & mqttClient, const String & version, DynamicJsonDocument & sensorData) {
  
  char mqttPayload[1024];
  DynamicJsonDocument device(256);
  DynamicJsonDocument autoconfPayload(1024);
  StaticJsonDocument<64> identifiersDoc;
  JsonArray identifiers = identifiersDoc.to<JsonArray>();
  identifiers.add(identifier);
  
  device["identifiers"] = identifiers;
  device["manufacturer"] = "TeHyBug";
  device["model"] = "TeHyBug Universal/Mini";
  device["name"] = identifier;
  device["sw_version"] = version;
  
  {
    const String wifiTopic = "homeassistant/sensor/"+String(identifier)+"/"+String(identifier)+"_wifi/config";  
    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["name"] = "WiFi";
    autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
    autoconfPayload["unique_id"] = String(identifier) + "_wifi";
    autoconfPayload["unit_of_measurement"] = "dBm";
    autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["json_attributes_template"] = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": \"{{value_json.wifi.ip}}\"}";
    autoconfPayload["icon"] = "mdi:wifi";
    serializeJson(autoconfPayload, mqttPayload);
    mqttClient.publish(wifiTopic.c_str(), &mqttPayload[0], true);
    autoconfPayload.clear();
  }
  const JsonObject root = sensorData.as<JsonObject>();
  for (JsonPair keyValue : root) {
    const String k = keyValue.key().c_str();
    if(k == "key")
     continue;
    const String v = keyValue.value();
    const String topic = "homeassistant/sensor/"+String(identifier)+"/"+String(identifier)+"_"+k+"/config";
  
    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
    autoconfPayload["name"] = key2name(k);
    autoconfPayload["value_template"] = "{{value_json." + k + "}}";
    if(k != "cs" && k != "cs2")
      autoconfPayload["unit_of_measurement"] = key2unit(k);
    autoconfPayload["icon"] = key2icon(k);
    autoconfPayload["unique_id"] = String(identifier) + "_sensor_" + k;
    serializeJson(autoconfPayload, mqttPayload);
    mqttClient.publish(topic.c_str(), &mqttPayload[0], true);
    autoconfPayload.clear();
  }
  device.clear();
  identifiersDoc.clear();
}

void publishState(PubSubClient & mqttClient, DynamicJsonDocument & sensorData) {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument stateJson(512);
  char payload[256];

  wifiJson["ssid"] = WiFi.SSID();
  wifiJson["ip"] = WiFi.localIP().toString();
  wifiJson["rssi"] = WiFi.RSSI();

  stateJson["wifi"] = wifiJson.as<JsonObject>();

  const JsonObject root = sensorData.as<JsonObject>();
  for (JsonPair keyValue : root) {
    const String k = keyValue.key().c_str();
    if(k == "key")
     continue;
    if(k == "cs"||k == "cs2")
      stateJson[k] = cf2name(keyValue.value().as<int>());
    else
      stateJson[k] = keyValue.value().as<double>();
  }

  serializeJson(stateJson, payload);
  mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
  stateJson.clear();
  wifiJson.clear();
}

}
