#include "AHT20.h"
#include "DHTesp.h"
#include "Max44009.h"
#include "UUID.h"
#include "bsec.h"
#include <AM2320_asukiaaa.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ErriezBMX280.h>
#include <FS.h>
#include <OneWire.h>
#include <PubSubClient.h> // Attention in the lib the #define MQTT_MAX_PACKET_SIZE must be increased to 4000!
#include <TickerScheduler.h>
#include <WebSocketsServer.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <Wire.h>

#define DEBUG 1

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__)
#define D_print(...) Serial.print(__VA_ARGS__)
#define D_write(...) Serial.write(__VA_ARGS__)
#define D_println(...) Serial.println(__VA_ARGS__)
#else
#define D_SerialBegin(...)
#define D_print(...)
#define D_write(...)
#define D_println(...)
#endif

// gumboard Stuff
#include "Tools.h"
#include "Webinterface.h"

#ifndef PIXEL_ACTIVE
#define PIXEL_ACTIVE 1
#define PIXEL_COUNT 1 // Number of NeoPixels
#define PIXEL_PIN 12  // Digital IO pin connected to the NeoPixels.

Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif
// set pin 4 HIGH to turn on the pixel
#define SIGNAL_LED_PIN 4

char identifier[24];

// sensors
int calibrationTemp = 0;
int calibrationHumi = 0;
int calibrationQfe = 0;
bool calibrationActive = false;

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA 1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);
bool bmx_sensor = false; // in the setup the i2c scanner searches for the sensor

bool bme680_sensor = false;

Bsec bme680;
String output;

Max44009 Max44009Lux(0x4A);
bool max44009_sensor = false;

AHT20 AHT;
bool aht20_sensor = false;
DHTesp dht;
DHTesp dht2;
bool dht_sensor = false;
bool second_dht_sensor = false;

AM2320_asukiaaa am2320;
bool am2320_sensor = false;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just
// Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature ds18b20_sensors(&oneWire);

// Data wire is plugged into port 13 on the Arduino
#define SECOND_ONE_WIRE_BUS 13

// Setup a oneWire instance to communicate with any OneWire devices (not just
// Maxim/Dallas temperature ICs)
OneWire secondOneWire(SECOND_ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature second_ds18b20_sensors(&secondOneWire);

bool ds18b20_sensor = false;
bool second_ds18b20_sensor = false;

bool adc_sensor = false;

// end sensors

// Button
#define BUTTON_PIN 0

// dns
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;
// HTTP Config
HTTPClient http;
String httpGetURL = "";
bool httpGetActive = false;
int httpGetFrequency = 900;

String httpPostURL = "";
bool httpPostActive = false;
int httpPostFrequency = 900;
String httpPostJson = "";

// MQTT Config
bool mqttActive = false;
bool mqttRetained = false;
String mqttUser = "";
String mqttPassword = "";
String mqttServer = "0.0.0.0";
String mqttMasterTopic = "/tehybug";
String mqttMessage = "";
int mqttPort = 1883;
int mqttFrequency = 900;
int mqttRetryCounter = 0;
int mqttMaxRetries = 99;
long mqttLastReconnectAttempt = 0;
long mqttReconnectWait = 10000; // wait 10 seconds and try to reconnect again
char data[80];

#define COMPILE_HOUR (((__TIME__[0] - '0') * 10) + (__TIME__[1] - '0'))
#define COMPILE_MINUTE (((__TIME__[3] - '0') * 10) + (__TIME__[4] - '0'))
#define COMPILE_SECOND (((__TIME__[6] - '0') * 10) + (__TIME__[7] - '0'))
#define COMPILE_YEAR                                                           \
  ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 +                    \
    (__DATE__[9] - '0')) *                                                     \
   10 +                                                                    \
   (__DATE__[10] - '0'))
#define COMPILE_SHORTYEAR (((__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))
#define COMPILE_MONTH                                                          \
  ((__DATE__[2] == 'n'   ? (__DATE__[1] == 'a' ? 0 : 5)                        \
    : __DATE__[2] == 'b' ? 1                                                   \
    : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 2 : 3)                        \
    : __DATE__[2] == 'y' ? 4                                                   \
    : __DATE__[2] == 'l' ? 6                                                   \
    : __DATE__[2] == 'g' ? 7                                                   \
    : __DATE__[2] == 'p' ? 8                                                   \
    : __DATE__[2] == 't' ? 9                                                   \
    : __DATE__[2] == 'v' ? 10                                                  \
    : 11) +                                               \
   1)
#define COMPILE_DAY                                                            \
  ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 + (__DATE__[5] - '0'))

const String version = String(COMPILE_SHORTYEAR) + IntFormat(COMPILE_MONTH) +
                       IntFormat(COMPILE_DAY) + IntFormat(COMPILE_HOUR) +
                       IntFormat(COMPILE_MINUTE);

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266HTTPUpdateServer httpUpdater;

bool configModeActive = true;

// System Vars
bool sleepModeActive = true;
bool shouldSaveConfig = false;

// Timerserver Vars
long lastClockUpdate = 0;
bool clockSecondBlink = true;
String OldInfo = "";   // old board info
String OldSensor = ""; // old sensor info
// Websoket Vars
String websocketConnection[10];

// Time
int Year, Month, Day, Hour, Minute, Second;

UUID uuid;

DynamicJsonDocument sensorData(1024);

String i2c_addresses = "";

TickerScheduler ticker(5);

void SaveConfigCallback() {
  shouldSaveConfig = true;
}

/////////////////////////////////////////////////////////////////////
float calibrateValue(String _n, float _v) {
  if (calibrationActive) {
    if (_n == "temp") _v += calibrationTemp;
    else if (_n == "humi") _v += calibrationHumi;
    else if (_n == "qfe") _v += calibrationQfe;
  }
  return _v;
}

void addTempHumi(String key_temp, float temp, String key_humi, float humi) {
  addSensorData(key_temp, temp);
  addSensorData(key_humi, humi);
}

void additionalSensorData(String key, float value) {

  if (key == "temp" || key == "temp2") {
    addSensorData(key + "_imp", (float)temp2Imp(value));
  }
  // humi should be always set after temp so the following calculation will work
  else if (key == "humi" || key == "hum2") {

    String num = String(atoi(key.c_str()));

    float hi = dht.computeHeatIndex(sensorData["temp" + num].as<float>(),
                                    sensorData[key + num].as<float>());
    addSensorData("hi" + num, (float)hi);
    addSensorData("hi_imp" + num, (float)temp2Imp(hi));

    float dew = dht.computeDewPoint(sensorData["temp" + num].as<float>(),
                                    sensorData[key + num].as<float>());
    addSensorData("dew" + num, (float)dew);
    addSensorData("dew_imp" + num, (float)temp2Imp(dew));
  }
}
void addSensorData(String key, float value) {

  if (key == "temp" || key == "temp2") {
    value = calibrateValue("temp", value);
  }
  else if (key == "humi" || key == "humi2") {
    value = calibrateValue("humi", value);
  }
  else if (key == "qfe") {
    value = calibrateValue("qfe", value);
  }

  sensorData[key] = String(value, 1);
  // calculate imperial temperature also heat index and the dew point
  additionalSensorData(key, value);
}
String replace_placeholders(String text) {
  JsonObject root = sensorData.as<JsonObject>();
  for (JsonPair keyValue : root) {
    String k = keyValue.key().c_str();
    String v = keyValue.value();
    text.replace("%" + k + "%", v);
  }
  return text;
}

void createDateElements(const char *str) {
  sscanf(str, "%d-%d-%dT%d:%d", &Year, &Month, &Day, &Hour, &Minute);
}
void createWeekdaysElements(const char *str, int *arr) {
  sscanf(str, "%d,%d,%d,%d,%d,%d,%d", &arr[0], &arr[1], &arr[2], &arr[3],
         &arr[4], &arr[5], &arr[6]);
}
/////

void SaveConfig() {
  // save the custom parameters to FS
  if (shouldSaveConfig) {
    DynamicJsonDocument json(1024);

    json["mqttActive"] = mqttActive;
    json["mqttRetained"] = mqttRetained;
    json["mqttUser"] = mqttUser;
    json["mqttPassword"] = mqttPassword;
    json["mqttServer"] = mqttServer;
    json["mqttMasterTopic"] = mqttMasterTopic;
    json["mqttMessage"] = mqttMessage;
    json["mqttPort"] = mqttPort;
    json["mqttFrequency"] = mqttFrequency;

    json["httpGetURL"] = httpGetURL;
    json["httpGetActive"] = httpGetActive;
    json["httpGetFrequency"] = httpGetFrequency;

    json["httpPostURL"] = httpPostURL;
    json["httpPostActive"] = httpPostActive;
    json["httpPostFrequency"] = httpPostFrequency;
    json["httpPostJson"] = httpPostJson;

    json["calibrationActive"] = calibrationActive;
    json["calibrationTemp"] = calibrationTemp;
    json["calibrationHumi"] = calibrationHumi;
    json["calibrationQfe"] = calibrationQfe;

    json["configModeActive"] = configModeActive;

    json["sleepModeActive"] = sleepModeActive;

    json["key"] = sensorData["key"];
    json["dht_sensor"] = dht_sensor;
    json["second_dht_sensor"] = second_dht_sensor;

    json["ds18b20_sensor"] = ds18b20_sensor;
    json["second_ds18b20_sensor"] = second_ds18b20_sensor;
    json["adc_sensor"] = adc_sensor;

    File configFile = SPIFFS.open("/config.json", "w");
    serializeJson(json, configFile);
    configFile.close();
    Log("SaveConfig", "Saved");
    // end save
  }
}

void setConfigParameters(JsonObject &json) {

  D_println("Config:");
  for (JsonPair kv : json) {
    D_print(kv.key().c_str());
    D_print(" = ");
    D_println(kv.value().as<String>());
  }
  D_println();
  if (json.containsKey("mqttActive")) {
    mqttActive = json["mqttActive"];
  }
  if (json.containsKey("mqttRetained")) {
    mqttRetained = json["mqttRetained"];
  }
  if (json.containsKey("mqttUser")) {
    mqttUser = json["mqttUser"].as<String>();
  }

  if (json.containsKey("mqttPassword")) {
    mqttPassword = json["mqttPassword"].as<String>();
  }

  if (json.containsKey("mqttServer")) {
    mqttServer = json["mqttServer"].as<String>();
  }

  if (json.containsKey("mqttMasterTopic")) {
    mqttMasterTopic = json["mqttMasterTopic"].as<String>();
  }
  if (json.containsKey("mqttMessage")) {
    mqttMessage = json["mqttMessage"].as<String>();
  }
  if (json.containsKey("mqttPort")) {
    mqttPort = json["mqttPort"];
  }
  if (json.containsKey("mqttFrequency")) {
    mqttFrequency = json["mqttFrequency"];
  }

  // http
  if (json.containsKey("httpGetURL")) {
    httpGetURL = json["httpGetURL"].as<String>();
  }
  if (json.containsKey("httpGetActive")) {
    httpGetActive = json["httpGetActive"];
  }
  if (json.containsKey("httpGetFrequency")) {
    httpGetFrequency = json["httpGetFrequency"];
  }

  if (json.containsKey("httpPostURL")) {
    httpPostURL = json["httpPostURL"].as<String>();
  }
  if (json.containsKey("httpPostActive")) {
    httpPostActive = json["httpPostActive"];
  }
  if (json.containsKey("httpPostFrequency")) {
    httpPostFrequency = json["httpPostFrequency"];
  }
  if (json.containsKey("httpPostJson")) {
    httpPostJson = json["httpPostJson"].as<String>();
  }

  if (json.containsKey("configModeActive")) {
    configModeActive = json["configModeActive"];
  }
  if (json.containsKey("calibrationActive")) {
    calibrationActive = json["calibrationActive"];
  }
  if (json.containsKey("calibrationTemp")) {
    calibrationTemp = json["calibrationTemp"];
  }
  if (json.containsKey("calibrationHumi")) {
    calibrationHumi = json["calibrationHumi"];
  }
  if (json.containsKey("calibrationQfe")) {
    calibrationQfe = json["calibrationQfe"];
  }
  if (json.containsKey("sleepModeActive")) {
    sleepModeActive = json["sleepModeActive"];
  }
  if (json.containsKey("dht_sensor")) {
    dht_sensor = json["dht_sensor"];
  }
  if (json.containsKey("second_dht_sensor")) {
    second_dht_sensor = json["second_dht_sensor"];
  }
  if (json.containsKey("ds18b20_sensor")) {
    ds18b20_sensor = json["ds18b20_sensor"];
  }
  if (json.containsKey("second_ds18b20_sensor")) {
    second_ds18b20_sensor = json["second_ds18b20_sensor"];
  }
  if (json.containsKey("adc_sensor")) {
    adc_sensor = json["adc_sensor"];
  }
}

void LoadConfig() {
  if (SPIFFS.exists("/config.json")) {
    // file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");

    if (configFile) {
      Log("LoadConfig", "opened config file");

      DynamicJsonDocument json(1024);
      auto error = deserializeJson(json, configFile);

      if (!error) {
        JsonObject documentRoot = json.as<JsonObject>();
        setConfigParameters(documentRoot);

        Log("LoadConfig", "Loaded");
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
    Log("LoadConfig", "No configfile found, create a new file");
    SaveConfigCallback();
    SaveConfig();
  }
}
void SetConfig(JsonObject &json) {
  setConfigParameters(json);
  SaveConfigCallback();
  SaveConfig();

  if (json.containsKey("reboot")) {
    if (json["reboot"]) {
      delay(1000);
      ESP.restart();
    }
  }
}

void WifiSetup() {
  wifiManager.resetSettings();
  ESP.restart();
  delay(300);
}

void HandleGetMainPage() {
  server.sendHeader("Access-Control-Allow-Origin", "*");

  server.send(200, "text/html", mainPage);
}

#pragma region
/* HTTP API */
void HandleNotFound() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(204);
  }

  server.sendHeader("Location", "/update", true);
  server.send(302, "text/plain", "");
}

void Handle_wifisetup() {
  WifiSetup();
}

void HandleSetConfig() {
  DynamicJsonDocument json(1024);
  auto error = deserializeJson(json, server.arg("plain"));
  server.sendHeader("Connection", "close");

  if (!error) {
    Log(("SetConfig"), ("Incoming Json length: " + String(measureJson(json))));
    // extract the data
    JsonObject object = json.as<JsonObject>();
    SetConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
    delay(500);
    // ESP.restart();
  } else {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void HandleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetConfig());
}

void HandleGetInfo() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetInfo());
}

void HandleGetSensor() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetSensor());
}

void Handle_factoryreset() {
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Log("Handle_factoryreset", "Failed to open config file for reset");
  }
  configFile.println("");
  configFile.close();
  WifiSetup();
  ESP.restart();
}

#pragma endregion

#pragma region
/* MQTT */
void callback(char *topic, byte *payload, unsigned int length) {
  if (payload[0] == '{') {
    payload[length] = '\0';
    String channel = String(topic);
    channel.replace(mqttMasterTopic, "");

    DynamicJsonDocument json(512);
    deserializeJson(json, payload);

    Log("MQTT_callback", "Incoming Json length to topic " + String(topic) +
        ": " + String(measureJson(json)));
    if (channel.equals("getInfo")) {
      client.publish((mqttMasterTopic + "matrixinfo").c_str(),
                     GetInfo().c_str());
    } else if (channel.equals("getConfig")) {
      client.publish((mqttMasterTopic + "config").c_str(), GetConfig().c_str());
    } else if (channel.equals("setConfig")) {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      SetConfig(object);
    }
  }
}
#pragma endregion

#pragma region
/* Websocket */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {

  switch (type) {
    case WStype_DISCONNECTED: {
        Log("WebSocketEvent", "[" + String(num) + "] Disconnected!");
        websocketConnection[num] = "";
        break;
      }
    case WStype_CONNECTED: {
        websocketConnection[num] = String((char *)payload);
        IPAddress ip = webSocket.remoteIP(num);
        Log("WebSocketEvent", "[" + String(num) + "] Connected from " +
            ip.toString() +
            " url: " + websocketConnection[num]);

        // send message to client
        SendInfo(true);
        SendSensor(true);
        SendConfig();
        break;
      }
    case WStype_TEXT: {
        if (((char *)payload)[0] == '{') {
          DynamicJsonDocument json(512);

          deserializeJson(json, payload);

          Log("WebSocketEvent",
              "Incoming Json length: " + String(measureJson(json)));

          if (websocketConnection[num] == "/setConfig") {
            // extract the data
            JsonObject object = json.as<JsonObject>();
            SetConfig(object);
            // delay(500);
            // ESP.restart();
          }
        }
        break;
      }
  }
}
#pragma endregion

String GetConfig() {
  File configFile = SPIFFS.open("/config.json", "r");

  if (configFile) {
    const size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);
    DynamicJsonDocument root(1024);

    if (DeserializationError::Ok == deserializeJson(root, buf.get())) {
    }
    String timeStamp = IntFormat(year()) + "-" + IntFormat(month()) + "-" +
                       IntFormat(day()) + "T" + IntFormat(hour()) + ":" +
                       IntFormat(minute());
    root["clockTime"] = timeStamp;
    String json;
    serializeJson(root, json);
    return json;
  }
}

String GetInfo() {
  DynamicJsonDocument root(1024);

  root["gumboardVersion"] = version;
  root["sketchSize"] = ESP.getSketchSize();
  root["freeSketchSpace"] = ESP.getFreeSketchSpace();
  root["wifiRSSI"] = String(WiFi.RSSI());
  root["wifiQuality"] = GetRSSIasQuality(WiFi.RSSI());
  root["wifiSSID"] = WiFi.SSID();
  root["ipAddress"] = WiFi.localIP().toString();
  root["freeHeap"] = ESP.getFreeHeap();
  root["chipID"] = ESP.getChipId();
  root["cpuFreqMHz"] = ESP.getCpuFreqMHz();
  root["sleepModeActive"] = sleepModeActive;
  root["key"] = sensorData["key"];

  String json;
  serializeJson(root, json);

  return json;
}

String GetSensor() {
  read_sensors();
  String json;
  serializeJson(sensorData, json);
  return json;
}

/////////////////////////////////////////////////////////////////////
void MqttSendData() {
  if (client.connected()) {
    String payload = replace_placeholders(mqttMessage);
    payload.toCharArray(data, (payload.length() + 1));
    client.publish((mqttMasterTopic).c_str(), data, mqttRetained);
    Log(F("MqttSendData"), payload);
  } else
    MqttReconnect();
}

void MqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected() && mqttRetryCounter < mqttMaxRetries) {
    bool connected = false;
    if (mqttUser != NULL && mqttUser.length() > 0 && mqttPassword != NULL &&
        mqttPassword.length() > 0) {
      Log(F("MqttReconnect"),
          F("MQTT connect to server with User and Password"));
      connected =
        client.connect(("tehybug_" + GetChipID()).c_str(), mqttUser.c_str(),
                       mqttPassword.c_str(), "state", 0, true, "diconnected");
    } else {
      Log(F("MqttReconnect"),
          F("MQTT connect to server without User and Password"));
      connected = client.connect(("tehybug_" + GetChipID()).c_str(), "state", 0,
                                 true, "disconnected");
    }

    // Attempt to connect
    if (connected) {
      Log(F("MqttReconnect"), F("MQTT connected!"));
      mqttRetryCounter = 0;
      // ... and publish
      MqttSendData();
    } else {
      Log(F("MqttReconnect"), F("MQTT not connected!"));
      Log(F("MqttReconnect"), F("Wait 5 seconds before retrying...."));
      mqttRetryCounter++;
    }
  }

  if (mqttRetryCounter >= mqttMaxRetries) {
    Log(F("MqttReconnect"),
        F("No connection to MQTT-Server, MQTT temporarily deactivated!"));
  }
}
/////////////////////////////////////////////////////////////////////

// BUTTON
//  Attach callback.
void toggleConfigMode() {

  Serial.println(F("Config mode changed"));
  configModeActive = !configModeActive;
  if (configModeActive) {
    D_println(F("Config mode activated"));
  } else {
    D_println(F("Config mode deactivated"));
  }
  SaveConfigCallback();
  SaveConfig();
  yield();
  if (configModeActive == false) {
    //ESP.restart();
  }
}

void startDeepSleep(int freq) {
  D_println("Going to deep sleep...");

  ESP.deepSleep(freq * 1000000);
  yield();
}
// HTTP REQUESTS

void http_post() {
  http.begin(httpPostURL); // Specify request destination
  http.addHeader("Content-Type",
                 "application/json"); // Specify content-type header
  String post_json = replace_placeholders(httpPostJson);
  int httpCode = http.POST(post_json); // Send the request
  D_println(httpCode);                 // Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_post"), post_json);
  }
  if (httpCode > 0) { // Check the returning code
    // String payload = http.getString();                  //Get the response
    // payload Serial.println(payload);    //Print request response payload
  }

  http.end(); // Close connection
}
void http_get() {
  String url = replace_placeholders(httpGetURL);
  http.begin(url);                              // Specify request destination
  http.addHeader("Content-Type", "text/plain"); // Specify content-type header

  int httpCode = http.GET(); // Send the request
  D_println(httpCode);       // Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_get"), url);
  }
  if (httpCode > 0) { // Check the returning code
    // String payload = http.getString();                  //Get the response
    // payload Serial.println(payload);    //Print request response payload
  }

  http.end(); // Close connection
}
// SENSOR

void read_bmx280() {

  if (bmx280.getChipID() == CHIP_ID_BME280) {
    addTempHumi("temp", (float)bmx280.readTemperature(), "humi",
                (float)bmx280.readHumidity());
  } else if (aht20_sensor) {
    addSensorData("temp2", (float)bmx280.readTemperature());
  }
  else
  {
    addSensorData("temp", (float)bmx280.readTemperature());
  }

  addSensorData("qfe", (float)(bmx280.readPressure() / 100.0F));

  addSensorData("alt", (float)bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
}

// Helper function definitions
void checkIaqSensorStatus(void) {
  if (bme680.status != BSEC_OK) {
    if (bme680.status < BSEC_OK) {
      output = "BSEC error code : " + String(bme680.status);
      D_println(output);
      for (;;)
        delay(1); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(bme680.status);
      D_println(output);
    }
  }

  if (bme680.bme680Status != BME680_OK) {
    if (bme680.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(bme680.bme680Status);
      D_println(output);
      for (;;)
        delay(1); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(bme680.bme680Status);
      D_println(output);
    }
  }
}
void read_bme680() {

  if (!bme680.run()) { // If no data is available
    checkIaqSensorStatus();
    return;
  }

  output = String(bme680.rawTemperature);
  output += ", " + String(bme680.pressure);
  output += ", " + String(bme680.rawHumidity);
  output += ", " + String(bme680.gasResistance);
  output += ", " + String(bme680.iaq);
  output += ", " + String(bme680.iaqAccuracy);
  output += ", " + String(bme680.temperature);
  output += ", " + String(bme680.humidity);
  output += ", " + String(bme680.staticIaq);
  output += ", " + String(bme680.co2Equivalent);
  output += ", " + String(bme680.breathVocEquivalent);
  D_println(output);

  addSensorData("qfe", (float)(bme680.pressure / 100.0F));
  addSensorData("eco2", (float)bme680.co2Equivalent);
  addSensorData("bvoc", (float)bme680.breathVocEquivalent);
  addSensorData("iaq", (float)bme680.iaq);
  addSensorData("air", (float)(bme680.gasResistance / 1000.0F));

  addTempHumi("temp", (float)bme680.temperature, "humi",
              (float)bme680.humidity);
}

void read_max44009() {
  float max440099lux = Max44009Lux.getLux();
  int err = Max44009Lux.getError();

  // in automatic mode TIM & CDR are automatic generated
  // and read only (in manual mode they are set by the user
  int conf = Max44009Lux.getConfiguration();
  int CDR = (conf & 0x80) >> 3;
  int TIM = (conf & 0x07);
  int integrationTime = Max44009Lux.getIntegrationTime();

  if (err != 0) {
    D_print("Error:\t");
    D_println(err);
  } else {
    addSensorData("lux", (float)max440099lux);
    D_print("lux:\t");
    D_print(max440099lux);
    D_print("\tCDR:\t");
    D_print(CDR);
    D_print("\tTIM:\t");
    D_print(TIM);
    D_print("\t");
    D_print(integrationTime);
    D_print(" ms");
    D_println();
  }
}

void read_aht20() {
  float humidity, temperature;
  int ret = AHT.getSensor(&humidity, &temperature);

  if (ret) // GET DATA OK
  {
    addTempHumi("temp", (float)temperature, "humi", (float)(humidity * 100.0F));
  } else // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }
}

void read_dht() {
  pinMode(0, OUTPUT);   // sets the digital pin 0 as output
  digitalWrite(0, LOW); // sets the digital pin 0 on

  // delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  delay(dht.getMinimumSamplingPeriod());
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  addTempHumi("temp", (float)temperature, "humi", (float)humidity);
}
void read_second_dht() {

  // delay(dht.getMinimumSamplingPeriod());
  float humidity = dht2.getHumidity();
  float temperature = dht2.getTemperature();
  delay(dht2.getMinimumSamplingPeriod());
  humidity = dht2.getHumidity();
  temperature = dht2.getTemperature();
  addTempHumi("temp2", (float)temperature, "humi2", (float)humidity);
}
void read_am2320() {
  float humidity, temperature;
  Wire.begin(0, 2);
  uint8_t count = 0;

  while (am2320.update() != 0) {
    Serial.println("Error: Cannot update the am2320 sensor values.");
    break;
    count++;
    if (count > 100) {
      break;
    }
    yield();
  }

  temperature = am2320.temperatureC;
  humidity = am2320.humidity;
  addTempHumi("temp", (float)temperature, "humi", (float)(humidity));
}

void read_ds18b20(void) {
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  // Start up the library
  ds18b20_sensors.begin();
  // Setup a oneWire instance to communicate with any OneWire devices (not just
  // Maxim/Dallas temperature ICs) call ds18b20_sensors.requestTemperatures() to
  // issue a global temperature request to all devices on the bus
  D_print("Requesting temperatures...");
  ds18b20_sensors.requestTemperatures(); // Send the command to get temperatures
  D_println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the
  // first sensor only.
  float tempC = ds18b20_sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    D_print("Temperature for the device 1 (index 0) is: ");
    D_println(tempC);
    addSensorData("temp", (float)tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

void read_second_ds18b20(void) {
  pinMode(SECOND_ONE_WIRE_BUS, INPUT_PULLUP);
  // Start up the library
  second_ds18b20_sensors.begin();
  // Setup a oneWire instance to communicate with any OneWire devices (not just
  // Maxim/Dallas temperature ICs) call ds18b20_sensors.requestTemperatures() to
  // issue a global temperature request to all devices on the bus
  D_print("Requesting second port temperatures...");
  second_ds18b20_sensors
  .requestTemperatures(); // Send the command to get temperatures
  D_println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the
  // first sensor only.
  float tempC = second_ds18b20_sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    D_print("Temperature for the second port device 1 (index 0) is: ");
    D_println(tempC);
    addSensorData("temp2", (float)tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

void read_adc() {
  uint8_t pin = 13;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // on
  delay(100);
  // read the analog in value
  int sensorValue = analogRead(0);
  addSensorData("adc", (float)sensorValue);
  digitalWrite(pin, LOW); // off
}

void read_sensors() {
  if (bmx_sensor) {
    read_bmx280();
  }
  if (bme680_sensor) {
    read_bme680();
  }
  if (max44009_sensor) {
    read_max44009();
  }
  if (aht20_sensor) {
    read_aht20();
  }
  if (dht_sensor) {
    read_dht();
  }
  if (second_dht_sensor) {
    read_second_dht();
  }
  if (am2320_sensor) {
    read_am2320();
  }
  if (ds18b20_sensor) {
    read_ds18b20();
  }
  if (second_ds18b20_sensor) {
    read_second_ds18b20();
  }
  if (adc_sensor) {
    read_adc();
  }
}
// end of sensor
void SendInfo(bool force) {
  if (force) {
    OldInfo = "";
  }
  String Info;
  if ((webSocket.connectedClients() > 0)) {
    Info = GetInfo();
  }

  if (webSocket.connectedClients() > 0 && OldInfo != Info) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/firststart" ||
          websocketConnection[i] == "/api/info") {
        webSocket.sendTXT(i, Info);
      }
    }
  }
  OldInfo = Info;
}

void SendSensor(bool force) {
  if (force) {
    OldSensor = "";
  }

  String Sensor;
  if ((webSocket.connectedClients() > 0)) {
    Sensor = GetSensor();
  }
  if (webSocket.connectedClients() > 0 && OldSensor != Sensor) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/settings") {
        webSocket.sendTXT(i, Sensor);
      }
    }
  }

  OldSensor = Sensor;
}

void SendConfig() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/settings" ||
          websocketConnection[i] == "/setsensor" ||
          websocketConnection[i] == "/setsystem") {
        String config = GetConfig();
        webSocket.sendTXT(i, config);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void Log(String function, String message) {
  String timeStamp = "";
  D_println("[" + timeStamp + "] " + function + ": " + message);
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main") {
        webSocket.sendTXT(i, "{\"log\":{\"timeStamp\":\"" + timeStamp +
                          "\",\"function\":\"" + function +
                          "\",\"message\":\"" + message + "\"}}");
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void serve_data() {
  if (httpGetActive) {
    http_get();
    delay(1000);
    if (sleepModeActive) {
      startDeepSleep(httpGetFrequency);
    }/* else {
      delay(httpGetFrequency * 1000);
    }*/
  }

  if (httpPostActive) {
    http_post();
    delay(1000);
    if (sleepModeActive) {
      startDeepSleep(httpPostFrequency);
    }/* else {
      delay(httpPostFrequency * 1000);
    }*/
  }

  if (mqttActive) {
    MqttSendData();
    delay(1000);
    if (sleepModeActive) {
      startDeepSleep(mqttFrequency);
    }/* else {
      delay(mqttFrequency * 1000);
    }*/
  }
}
void led_on() {
  D_println("Led on");
  digitalWrite(SIGNAL_LED_PIN, HIGH); // on
  if (PIXEL_ACTIVE) {
    pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
    pixel.setPixelColor(0, pixel.Color(0, 0, 255));
    pixel.setBrightness(50);
    pixel.show(); // Initialize all pixels to 'off'
  }
}
void led_off() {
  D_println("Led off");
  if (PIXEL_ACTIVE == 1) {
    pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
    pixel.setPixelColor(0, pixel.Color(0, 0, 0)); //  Set pixel's color (in RAM)
    pixel.setBrightness(0);
    pixel.show();
  }
  digitalWrite(SIGNAL_LED_PIN, LOW); // off
}

void configModeCallback(WiFiManager *myWiFiManager) {
  led_on();
  D_println("Entered wifi config mode");
  D_println(WiFi.softAPIP());
  D_println(myWiFiManager->getConfigPortalSSID());
}
void setupWifi() {

  D_println("Setup WIFI");
  wifiManager.setDebugOutput(true);
  // Set config save notify callback
  wifiManager.setSaveConfigCallback(SaveConfigCallback);
  wifiManager.setMinimumSignalQuality();
  wifiManager.setAPCallback(configModeCallback);

  // Config menu timeout 180 seconds.
  wifiManager.setConfigPortalTimeout(180);

  WiFi.hostname(identifier);
  // set custom ip for portal
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if (!wifiManager.autoConnect(identifier)) {
    Serial.println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  D_println(F("Wifi connected...yeey :)"));

  if (shouldSaveConfig) {
    SaveConfig();
  }

  Serial.println("Setup: Local IP");
  Serial.println("Setup " + WiFi.localIP().toString());
  Serial.println("Setup " + WiFi.gatewayIP().toString());
  Serial.println("Setup " + WiFi.subnetMask().toString());
}

void setupMDSN() {
  // generate module IDs
  escapedMac = WiFi.macAddress();
  escapedMac.replace(":", "");
  escapedMac.toLowerCase();
  strcpy_P(cmDNS, PSTR("tehybug"));
  // Set up mDNS responder:
  if (strlen(cmDNS) > 0) {
    // "end" must be called before "begin" is called a 2nd time
    // see https://github.com/esp8266/Arduino/issues/7213
    MDNS.end();
    MDNS.begin(cmDNS);
    D_println(cmDNS);
    D_println(F("mDNS started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("tehybug", "tcp", 80);
    MDNS.addServiceTxt("tehybug", "tcp", "mac", escapedMac.c_str());
  }
}
// Helper function definitions

void setupSensors() {
  if (dht_sensor == false && ds18b20_sensor == false) {
    Wire.begin(0, 2);
    // Wire.setClock(400000);
    i2c_addresses = i2c_scanner();
    i2c_addresses = i2c_addresses + i2c_scanner();
  }

  if (strContains(i2c_addresses.c_str(), "0x77") == 1) {
    bmx280 = bmp280;
    bmx_sensor = true;
  } else if (strContains(i2c_addresses.c_str(), "0x76") == 1) {
    bmx_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x5c") == 1) {
    am2320_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x58") == 1) {
    // sgp30
  }
  if (strContains(i2c_addresses.c_str(), "0x77") == 1) {
    bme680_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x4a") == 1) {
    max44009_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x38") == 1) {
    aht20_sensor = true;
  }

  // sensors
  // bmx280 and bme680 have same address
  if (bmx_sensor) {
    // Initialize sensor
    while (!bmx280.begin()) {
      D_println(F("Error: Could not detect sensor"));
      bmx_sensor = false;
      break;
    }
    if (bmx_sensor) {
      // Print sensor type
      D_print(F("\nSensor type: "));
      switch (bmx280.getChipID()) {
        case CHIP_ID_BMP280:
          D_println(F("BMP280\n"));
          bme680_sensor = false;
          break;
        case CHIP_ID_BME280:
          D_println(F("BME280\n"));
          bme680_sensor = false;
          break;
        default:
          Serial.println(F("Unknown\n"));
          break;
      }

      // Set sampling - Recommended modes of operation
      //
      // Weather
      //  - forced mode, 1 sample / minute
      //  - pressure ×1, temperature ×1, humidity ×1
      //  - filter off
      //
      // Humidity sensing
      //  - forced mode, 1 sample / second
      //  - pressure ×0, temperature ×1, humidity ×1
      //  - filter off
      //
      // Indoor navigation
      //  - normal mode, t standby = 0.5 ms
      //  - pressure ×16, temperature ×2, humidity ×1
      //  - filter coefficient 16
      //
      // Gaming
      //  - forced mode, t standby = 0.5 ms
      //  - pressure ×1, temperature ×1, humidity ×1
      //  - filter off
      bmx280.setSampling(
        BMX280_MODE_SLEEP,      // SLEEP, FORCED, NORMAL
        BMX280_SAMPLING_X16,    // Temp:  NONE, X1, X2, X4, X8, X16
        BMX280_SAMPLING_X16,    // Press: NONE, X1, X2, X4, X8, X16
        BMX280_SAMPLING_X16,    // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
        BMX280_FILTER_X16,      // OFF, X2, X4, X8, X16
        BMX280_STANDBY_MS_500); // 0_5, 10, 20, 62_5, 125, 250, 500, 1000
    }
  }
  if (bme680_sensor) {
    D_println(F("BME680 test"));

    bme680.begin(BME680_I2C_ADDR_SECONDARY, Wire);

    output = "\nBSEC library version " + String(bme680.version.major) + "." +
             String(bme680.version.minor) + "." +
             String(bme680.version.major_bugfix) + "." +
             String(bme680.version.minor_bugfix);
    D_println(output);
    checkIaqSensorStatus();
    bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    bme680.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();
    // Print the header
    output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw "
             "relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature "
             "[°C], relative humidity [%], Static IAQ, CO2 equivalent, breath "
             "VOC equivalent";
    D_println(output);
  }
  if (max44009_sensor) {
    D_print("\nStart max44009_setAutomaticMode : ");
    D_println(MAX44009_LIB_VERSION);

    Max44009Lux.setAutomaticMode();
  }
  if (aht20_sensor) {
    D_println("AHT20");
    AHT.begin();
  }
  if (dht_sensor) {
    dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
  }
  if (second_dht_sensor) {
    pinMode(13, INPUT_PULLUP);
    dht2.setup(13, DHTesp::DHT22); // Connect DHT sensor to GPIO 13
  }
  if (am2320_sensor) {
    am2320.setWire(&Wire);
  }

  if (ds18b20_sensor) {
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  }
}

void setupMode() {

  delay(100);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(300);
    if (digitalRead(BUTTON_PIN) == LOW) {
      toggleConfigMode();
      if (configModeActive) {
        led_on();
      } else {
        led_off();
      }
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
    }
  }
  if (configModeActive) {
    led_on();
  } else {
    led_off();
  }
}

void setup() {
  uuid.seed(ESP.getChipId());
  uuid.generate();
  sensorData["key"] = String(uuid.toCharArray());

  pinMode(SIGNAL_LED_PIN, OUTPUT);

  snprintf(identifier, sizeof(identifier), "TEHYBUG-%X", ESP.getChipId());

  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }
  D_println(F("key: "));
  D_println(sensorData["key"].as<String>());
  // Mounting FileSystem
  D_println(F("Mounting file system..."));
  if (SPIFFS.begin()) {
    D_println(F("Mounted file system."));
    LoadConfig();
  } else {
    D_println(F("Failed to mount FS"));
  }

  // force config when no mode selected
  if (httpGetActive == false && httpPostActive == false &&
      mqttActive == false) {
    configModeActive = true;
    D_println("Data serving mode not selected");
  }

  setupWifi();
  setupMode();

  if (configModeActive) {
    D_println(F("Starting config mode"));
    httpUpdater.setup(&server);
    server.on(F("/api/info"), HTTP_GET, HandleGetInfo);
    server.on(F("/api/config"), HTTP_POST, HandleSetConfig);
    server.on(F("/api/config"), HTTP_GET, HandleGetConfig);
    server.on(F("/api/sensor"), HTTP_GET, HandleGetSensor);
    server.on(F("/"), HTTP_GET, HandleGetMainPage);
    server.onNotFound(HandleNotFound);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Log(F("Setup"), F("Webserver started"));
  } else {
    D_println(F("Starting live mode"));
  }

  if (configModeActive == false && mqttActive) {
    client.setServer(mqttServer.c_str(), mqttPort);
    client.setCallback(callback);
    client.setBufferSize(4000);
    Log(F("Setup"), F("MQTT started"));
  }

  setupSensors();

  // setup tickers for non-deep-sleep mode
  if (!configModeActive && !sleepModeActive) {

    uint8_t ticker_num = 0;
    if (httpGetActive)
    {
      ticker.add(
      ticker_num, httpGetFrequency * 1000, [&](void *) {
        read_sensors();
        yield();
        http_get();
      }, nullptr, true);
      ticker_num++;
    }

    if (httpPostActive)
    {
      ticker.add(
      ticker_num, httpPostFrequency * 1000, [&](void *) {
        read_sensors();
        yield();
        http_post();
      }, nullptr, true);
      ticker_num++;
    }

    if (mqttActive)
    {
      ticker.add(
      ticker_num, mqttFrequency * 1000, [&](void *) {
        read_sensors();
        yield();
        MqttSendData();
      }, nullptr, true);
      ticker_num++;
    }

  }
}

void loop() {
  // config mode
  if (configModeActive) {
    server.handleClient();
    yield();
    webSocket.loop();
  }
  // deep sleep mode
  else if (sleepModeActive) {
    read_sensors();
    yield();
    serve_data();
  }
  // update ticker for the non-deep-sleep mode
  ticker.update();
}
