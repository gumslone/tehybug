#pragma once
namespace ha {
  char identifier[24];
  char MQTT_TOPIC_STATE[128];
  char MQTT_TOPIC_AVAILABILITY[128];
  
String key2unit(const String & key)
{
  if (key == "temp" || key == "temp2" || key == "dew" || key == "hi")
    return "°C";
  if (key == "temp_imp" || key == "temp2_imp" || key == "dew_imp" || key == "hi_imp")
    return "°F";
  if (key == "humi")
    return "%RH";
  if (key == "air")
    return "kOhm";
  if (key == "qfe")
    return "hPa";
  if (key == "alt")
    return "m";
  if (key == "lux")
    return "Lux";
  if (key == "adc")
    return "ADC";

  return "";
}
String key2name(const String & key)
{
  if (key == "temp" || key == "temp_imp")
    return "Temperature";
  if (key == "temp2" || key == "temp2_imp")
    return "Temperature2";
  if (key == "humi")
    return "Humidity";
  if (key == "dew")
    return "Dew point";
  if (key == "dew_imp")
    return "Dew point";
  if (key == "hi" || key == "hi_imp")
    return "Heat index";
  if (key == "air")
    return "Gas resistance";
  if (key == "iaq")
    return "Indoor air quality";
  if (key == "qfe")
    return "Atmospheric pressure";
  if (key == "alt")
    return "Altitude";
  if (key == "eco2")
    return "CO2 equivalent";
  if (key == "bvoc")
    return "breath VOC equivalent";
  if (key == "uv")
    return "UV index";
  if (key == "lux")
    return "Ambient light";
  if (key == "adc")
    return "ADC";

  return "";
}
String key2icon(const String & key)
{
  if (key == "temp" || key == "temp2" || key == "temp_imp" || key == "temp2_imp")
    return "mdi:thermometer";
  if (key == "humi")
    return "mdi:water-percent";
  if (key == "qfe")
    return "mdi:gauge";
  if (key == "alt")
    return "mdi:image-filter-hdr";
  if (key == "dew"||key =="dew_imp")
    return "mdi:water-thermometer";
  if (key == "hi" || key == "hi_imp")
    return "mdi:sun-thermometer";
  return "";
}

void setupHandle(String deviceName) {
  snprintf(identifier, sizeof(identifier), "%s-%X", deviceName.c_str(), ESP.getChipId());
  snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", deviceName.c_str(), identifier);
  snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", deviceName.c_str(),
           identifier);
}

 void publishAutoConfig(PubSubClient & mqttClient, const String & version, DynamicJsonDocument & sensorData) {
  
  char mqttPayload[2048];
  DynamicJsonDocument device(256);
  DynamicJsonDocument autoconfPayload(1024);
  StaticJsonDocument<64> identifiersDoc;
  JsonArray identifiers = identifiersDoc.to<JsonArray>();
  identifiers.add(identifier);
  
  const String wifitopic = "homeassistant/sensor/"+String(identifier)+"/"+String(identifier)+"_wifi/config";

  device["identifiers"] = identifiers;
  device["manufacturer"] = "TeHyBug";
  device["model"] = "TeHyBug Universal/Mini";
  device["name"] = identifier;
  device["sw_version"] = version;
    
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
  mqttClient.publish(wifitopic.c_str(), &mqttPayload[0],
                     true);

  autoconfPayload.clear();

  const JsonObject root = sensorData.as<JsonObject>();
  for (JsonPair keyValue : root) {
    const String k = keyValue.key().c_str();
    if(k == "key")
     continue;
    const String v = keyValue.value();
    const String topic = "homeassistant/sensor/"+String(identifier)+"/"+String(identifier)+"_"+k+"/config";
  
    autoconfPayload["device"] = device.as<JsonObject>();
    autoconfPayload["name"] = key2name(k);
    autoconfPayload["value_template"] = "{{value_json." + k + "}}";
    autoconfPayload["unit_of_measurement"] = key2unit(k);
    autoconfPayload["icon"] = key2icon(k);
    autoconfPayload["unique_id"] = String(identifier) + "_sensor_" + k;
    autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
    serializeJson(autoconfPayload, mqttPayload);
    mqttClient.publish(topic.c_str(), &mqttPayload[0], true);
    autoconfPayload.clear();
  }
}

void publishState(PubSubClient & mqttClient, DynamicJsonDocument & sensorData) {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument stateJson(604);
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
    const String v = keyValue.value();
    stateJson[k] = v;
  }

  serializeJson(stateJson, payload);
  mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
  stateJson.clear();
}

}
