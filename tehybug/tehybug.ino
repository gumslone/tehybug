#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h> // Attention in the lib the #define MQTT_MAX_PACKET_SIZE must be increased to 4000!
#include <FS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <Wire.h>
#include <ErriezBMX280.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "Max44009.h"
#include "AHT20.h"
#include "DHTesp.h"
#include "UUID.h"
#include <AM2320_asukiaaa.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TickerScheduler.h>

// ds18b20 Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// gumboard Stuff
#include "Webinterface.h"
#include "Tools.h"

#define DEBUG 0

#ifndef PIXEL_ACTIVE
#define PIXEL_ACTIVE 1
#define PIXEL_COUNT 1  // Number of NeoPixels
#define PIXEL_PIN    12  // Digital IO pin connected to the NeoPixels.

Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif
// pin 4 HIGH to turn on the pixel
#define SIGNAL_LED_PIN 4


char identifier[24];

//sensors
int calibrationTemp = 0;
int calibrationHumi = 0;
int calibrationQfe = 0;
bool calibrationActive = false;

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA      1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);
bool bmx_sensor = false; // in the setup the i2c scanner searches for the sensor

bool bme680_sensor = false; // in the setup the i2c scanner searches for the sensor

Adafruit_BME680 bme680; // I2C

Max44009 Max44009Lux(0x4A);
bool max44009_sensor = false; // in the setup the i2c scanner searches for the sensor

AHT20 AHT;
bool aht20_sensor = false; // in the setup the i2c scanner searches for the sensor
DHTesp dht;
bool dht_sensor = false; // in the setup the i2c scanner searches for the sensor

AM2320_asukiaaa am2320;
bool am2320_sensor = false;

DallasTemperature ds18b20sensors;
// arrays to hold device address
DeviceAddress insideThermometer;

bool ds18b20_sensor = false;

long lastSensorUpdate = 0;
// end sensors

//Button
#define BUTTON_PIN 0

// dns
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;
//// HTTP Config
HTTPClient http;
String httpGetURL = "";
bool httpGetActive = false;
int httpGetFrequency = 900;

String httpPostURL = "";
bool httpPostActive = false;
int httpPostFrequency = 900;
String httpPostJson = "";

//// MQTT Config
bool mqttActive = false;
bool mqttRetained = false;
String  mqttUser = "";
String	mqttPassword = "";
String mqttServer = "0.0.0.0";
String mqttMasterTopic = "/tehybug";
String  mqttMessage = "";
int mqttPort = 1883;
int mqttFrequency = 900;
int mqttRetryCounter = 0;
int mqttMaxRetries = 99;
long mqttLastReconnectAttempt = 0;
long mqttReconnectWait = 10000; // wait 10 seconds and try to reconnect again
char data[80];

#define COMPILE_HOUR          (((__TIME__[0]-'0')*10) + (__TIME__[1]-'0'))
#define COMPILE_MINUTE        (((__TIME__[3]-'0')*10) + (__TIME__[4]-'0'))
#define COMPILE_SECOND        (((__TIME__[6]-'0')*10) + (__TIME__[7]-'0'))
#define COMPILE_YEAR          ((((__DATE__ [7]-'0')*10+(__DATE__[8]-'0'))*10+(__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))
#define COMPILE_SHORTYEAR     (((__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))
#define COMPILE_MONTH         ((  __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 0 : 5)   \
                                  : __DATE__ [2] == 'b' ? 1                               \
                                  : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ?  2 : 3)  \
                                  : __DATE__ [2] == 'y' ? 4                               \
                                  : __DATE__ [2] == 'l' ? 6                               \
                                  : __DATE__ [2] == 'g' ? 7                               \
                                  : __DATE__ [2] == 'p' ? 8                               \
                                  : __DATE__ [2] == 't' ? 9                               \
                                  : __DATE__ [2] == 'v' ? 10 : 11) +1)
#define COMPILE_DAY           ((__DATE__ [4]==' ' ? 0 : __DATE__  [4]-'0')*10+(__DATE__[5]-'0'))

const String version = String(COMPILE_SHORTYEAR) + IntFormat(COMPILE_MONTH) + IntFormat(COMPILE_DAY) + IntFormat(COMPILE_HOUR) + IntFormat(COMPILE_MINUTE);

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
String OldInfo = ""; // old board info
String OldSensor = ""; // old sensor info
// Websoket Vars
String websocketConnection[10];

// Time

int Year, Month, Day, Hour, Minute, Second ;

UUID uuid;

String key, temp, temp_imp, humi, dew, qfe, qfe_imp, qnh, alt, air, aiq, lux, uv, adc, tvoc, co2, eco2;

String i2c_addresses = "";

TickerScheduler ticker(5);

void SaveConfigCallback()
{
  shouldSaveConfig = true;
}

/////////////////////////////////////////////////////////////////////
String replace_placeholders(String text)
{
  text.replace("%key%", key);
  text.replace("%temp%", temp);
  text.replace("%temp_imp%", temp_imp);
  text.replace("%humi%", humi);
  text.replace("%qfe%", qfe);
  text.replace("%qnh%", qnh);
  text.replace("%alt%", alt);
  text.replace("%air%", air);
  text.replace("%lux%", lux);
  text.replace("%uv%", uv);
  text.replace("%adc%", adc);
  text.replace("%tvoc%", tvoc);
  text.replace("%eco2%", eco2);
  return text;
}
/////

void SaveConfig()
{
  //save the custom parameters to FS
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

    json["key"] = key;
    json["am2320_sensor"] = am2320_sensor;
    json["dht_sensor"] = dht_sensor;

    json["ds18b20_sensor"] = ds18b20_sensor;

    File configFile = SPIFFS.open("/config.json", "w");
    serializeJson(json, configFile);
    configFile.close();
    Log("SaveConfig", "Saved");
    //end save
  }
}

void LoadConfig()
{
  if (SPIFFS.exists("/config.json"))
  {
    //file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");

    if (configFile)
    {
      Serial.println("opened config file");

      DynamicJsonDocument json(1024);
      auto error = deserializeJson(json, configFile);

      if (!error)
      {

        if (json.containsKey("mqttActive"))
        {
          mqttActive = json["mqttActive"];
        }
        if (json.containsKey("mqttRetained"))
        {
          mqttRetained = json["mqttRetained"];
        }
        if (json.containsKey("mqttUser"))
        {
          mqttUser = json["mqttUser"].as<String>();
        }

        if (json.containsKey("mqttPassword"))
        {
          mqttPassword = json["mqttPassword"].as<String>();
        }

        if (json.containsKey("mqttServer"))
        {
          mqttServer = json["mqttServer"].as<String>();
        }

        if (json.containsKey("mqttMasterTopic"))
        {
          mqttMasterTopic = json["mqttMasterTopic"].as<String>();
        }
        if (json.containsKey("mqttMessage"))
        {
          mqttMessage = json["mqttMessage"].as<String>();
        }

        if (json.containsKey("mqttPort"))
        {
          mqttPort = json["mqttPort"];
        }
        if (json.containsKey("mqttFrequency"))
        {
          mqttFrequency = json["mqttFrequency"];
        }

        // http
        if (json.containsKey("httpGetURL"))
        {
          httpGetURL = json["httpGetURL"].as<String>();
        }
        if (json.containsKey("httpGetActive"))
        {
          httpGetActive = json["httpGetActive"];
        }
        if (json.containsKey("httpGetFrequency"))
        {
          httpGetFrequency = json["httpGetFrequency"];
        }

        if (json.containsKey("httpPostURL"))
        {
          httpPostURL = json["httpPostURL"].as<String>();
        }
        if (json.containsKey("httpPostActive"))
        {
          httpPostActive = json["httpPostActive"];
        }
        if (json.containsKey("httpPostFrequency"))
        {
          httpPostFrequency = json["httpPostFrequency"];
        }
        if (json.containsKey("httpPostJson"))
        {
          httpPostJson = json["httpPostJson"].as<String>();
        }


        if (json.containsKey("configModeActive"))
        {
          configModeActive = json["configModeActive"];
        }

        if (json.containsKey("calibrationActive"))
        {
          calibrationActive = json["calibrationActive"];
        }
        if (json.containsKey("calibrationTemp"))
        {
          calibrationTemp = json["calibrationTemp"];
        }
        if (json.containsKey("calibrationHumi"))
        {
          calibrationHumi = json["calibrationHumi"];
        }
        if (json.containsKey("calibrationQfe"))
        {
          calibrationQfe = json["calibrationQfe"];
        }
        if (json.containsKey("sleepModeActive"))
        {
          sleepModeActive = json["sleepModeActive"];
        }
        /*
          if (json.containsKey("key"))
          {
          key = json["key"].as<String>();
          }
        */
        if (json.containsKey("am2320_sensor"))
        {
          am2320_sensor = json["am2320_sensor"];
        }
        if (json.containsKey("dht_sensor"))
        {
          dht_sensor = json["dht_sensor"];
        }
        if (json.containsKey("ds18b20_sensor"))
        {
          ds18b20_sensor = json["ds18b20_sensor"];
        }
        Log("LoadConfig", "Loaded");
      }

    }
  }
  else
  {
    Log("LoadConfig", "No Configfile, init new file");
    SaveConfigCallback();
    SaveConfig();
  }
}

void createDateElements(const char *str)
{
  sscanf(str, "%d-%d-%dT%d:%d", &Year, &Month, &Day, &Hour, &Minute);
}
void createWeekdaysElements(const char *str, int *arr)
{
  sscanf(str, "%d,%d,%d,%d,%d,%d,%d", &arr[0], &arr[1], &arr[2], &arr[3], &arr[4], &arr[5], &arr[6]);
}
void SetConfig(JsonObject& json)
{

  if (json.containsKey("mqttActive"))
  {
    mqttActive = json["mqttActive"];
  }
  if (json.containsKey("mqttRetained"))
  {
    mqttRetained = json["mqttRetained"];
  }
  if (json.containsKey("mqttUser"))
  {
    mqttUser = json["mqttUser"].as<String>();
  }

  if (json.containsKey("mqttPassword"))
  {
    mqttPassword = json["mqttPassword"].as<String>();
  }

  if (json.containsKey("mqttServer"))
  {
    mqttServer = json["mqttServer"].as<String>();
  }

  if (json.containsKey("mqttMasterTopic"))
  {
    mqttMasterTopic = json["mqttMasterTopic"].as<String>();
  }
  if (json.containsKey("mqttMessage"))
  {
    mqttMessage = json["mqttMessage"].as<String>();
  }
  if (json.containsKey("mqttPort"))
  {
    mqttPort = json["mqttPort"];
  }
  if (json.containsKey("mqttFrequency"))
  {
    mqttFrequency = json["mqttFrequency"];
  }

  // http
  if (json.containsKey("httpGetURL"))
  {
    httpGetURL = json["httpGetURL"].as<String>();
  }
  if (json.containsKey("httpGetActive"))
  {
    httpGetActive = json["httpGetActive"];
  }
  if (json.containsKey("httpGetFrequency"))
  {
    httpGetFrequency = json["httpGetFrequency"];
  }

  if (json.containsKey("httpPostURL"))
  {
    httpPostURL = json["httpPostURL"].as<String>();
  }
  if (json.containsKey("httpPostActive"))
  {
    httpPostActive = json["httpPostActive"];
  }
  if (json.containsKey("httpPostFrequency"))
  {
    httpPostFrequency = json["httpPostFrequency"];
  }
  if (json.containsKey("httpPostJson"))
  {
    httpPostJson = json["httpPostJson"].as<String>();
  }

  if (json.containsKey("configModeActive"))
  {
    configModeActive = json["configModeActive"];
  }

  if (json.containsKey("calibrationActive"))
  {
    calibrationActive = json["calibrationActive"];
  }
  if (json.containsKey("calibrationTemp"))
  {
    calibrationTemp = json["calibrationTemp"];
  }
  if (json.containsKey("calibrationHumi"))
  {
    calibrationHumi = json["calibrationHumi"];
  }
  if (json.containsKey("calibrationQfe"))
  {
    calibrationQfe = json["calibrationQfe"];
  }

  if (json.containsKey("sleepModeActive"))
  {
    sleepModeActive = json["sleepModeActive"];
  }
  /*
    if (json.containsKey("key"))
    {
    key = json["key"].as<String>();
    }
  */
  if (json.containsKey("am2320_sensor"))
  {
    am2320_sensor = json["am2320_sensor"];
  }
  if (json.containsKey("dht_sensor"))
  {
    dht_sensor = json["dht_sensor"];
  }
  if (json.containsKey("ds18b20_sensor"))
  {
    ds18b20_sensor = json["ds18b20_sensor"];
  }

  SaveConfigCallback();
  SaveConfig();

  if (json.containsKey("reboot"))
  {
    if (json["reboot"])
    {
      delay(1000);
      ESP.restart();
    }
  }
}

void WifiSetup()
{
  wifiManager.resetSettings();
  ESP.restart();
  delay(300);
}

void HandleGetMainPage()
{
  server.sendHeader("Access-Control-Allow-Origin", "*");

  server.send(200, "text/html", mainPage);
}

#pragma region //////////////////////////// HTTP API ////////////////////////////
void HandleNotFound()
{
  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(204);
  }

  server.sendHeader("Location", "/update", true);
  server.send(302, "text/plain", "");
}


void Handle_wifisetup()
{
  WifiSetup();
}

void HandleSetConfig()
{
  DynamicJsonDocument json(512);
  auto error = deserializeJson(json, server.arg("plain"));
  server.sendHeader("Connection", "close");

  if (!error)
  {
    Log(("SetConfig"), ("Incomming Json length: " + String(measureJson(json))));
    // extract the data
    JsonObject object = json.as<JsonObject>();
    SetConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
    delay(500);
    //ESP.restart();
  }
  else
  {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void HandleGetConfig()
{
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetConfig());
}

void HandleGetInfo()
{
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetInfo());
}

void HandleGetSensor()
{
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", GetSensor());
}

void Handle_factoryreset()
{
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Log("Handle_factoryreset", "Failed to open config file for reset");
  }
  configFile.println("");
  configFile.close();
  WifiSetup();
  ESP.restart();
}

void HandleOtaUpdate()
{
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", "{\"update\":\"started\"}");
  //SetOTAUpdate();

}

#pragma endregion

#pragma region //////////////////////////// MQTT ////////////////////////////
void callback(char* topic, byte* payload, unsigned int length)
{
  if (payload[0] == '{')
  {
    payload[length] = '\0';
    String channel = String(topic);
    channel.replace(mqttMasterTopic, "");

    DynamicJsonDocument json(512);
    deserializeJson(json, payload);

    Log("MQTT_callback", "Incomming Json length to topic " + String(topic) + ": " + String(measureJson(json)));
    if (channel.equals("getInfo"))
    {
      client.publish((mqttMasterTopic + "matrixinfo").c_str(), GetInfo().c_str());
    }
    else if (channel.equals("getConfig"))
    {
      client.publish((mqttMasterTopic + "config").c_str(), GetConfig().c_str());
    }
    else if (channel.equals("setConfig"))
    {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      SetConfig(object);
    }
  }
}
#pragma endregion

#pragma region
//////////////////////////// Websocket ////////////////////////////

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      {
        Log("WebSocketEvent", "[" + String(num) + "] Disconnected!");
        websocketConnection[num] = "";
        break;
      }
    case WStype_CONNECTED:
      {
        // Merken für was die Connection hergstellt wurde
        websocketConnection[num] = String((char*)payload);

        // IP der Connection abfragen
        IPAddress ip = webSocket.remoteIP(num);

        // Logausgabe
        Log("WebSocketEvent", "[" + String(num) + "] Connected from " + ip.toString() + " url: " + websocketConnection[num]);

        // send message to client
        SendInfo(true);
        SendSensor(true);
        SendConfig();
        break;
      }
    case WStype_TEXT:
      {
        if (((char*)payload)[0] == '{')
        {
          DynamicJsonDocument json(512);

          deserializeJson(json, payload);

          // Logausgabe
          Log("WebSocketEvent", "Incomming Json length: " + String(measureJson(json)));

          if (websocketConnection[num] == "/setConfig")
          {
            // extract the data
            JsonObject object = json.as<JsonObject>();
            SetConfig(object);
            //delay(500);
            //ESP.restart();
          }
        }
        break;
      }
    case WStype_BIN:
      //Serial.printf("[%u] get binary length: %u\n", num, length);
      //hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}
#pragma endregion

String GetConfig()
{
  File configFile = SPIFFS.open("/config.json", "r");


  if (configFile) {
    const size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);
    DynamicJsonDocument root(1024);

    if (DeserializationError::Ok == deserializeJson(root, buf.get())) {

    }
    String timeStamp = IntFormat(year()) + "-" + IntFormat(month()) + "-" + IntFormat(day()) + "T" + IntFormat(hour()) + ":" + IntFormat(minute());
    root["clockTime"] = timeStamp;
    String json;
    serializeJson(root, json);
    return json;
  }

}

String GetInfo()
{
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
  root["key"] = key;

  String json;
  serializeJson(root, json);

  return json;
}

String GetSensor()
{
  read_sensors();
  DynamicJsonDocument root(1024);

  root["temp"] = temp;
  root["humi"] = humi;
  root["dew"] = dew;
  root["qfe"] = qfe;
  root["alt"] = alt;
  root["air"] = air;
  root["lux"] = lux;
  root["uv"] = uv;
  root["adc"] = adc;
  root["tvoc"] = tvoc;
  root["eco2"] = eco2;
  String json;
  serializeJson(root, json);

  return json;
}

/////////////////////////////////////////////////////////////////////
void MqttSendData()
{
  if (client.connected())
  {
    String payload = replace_placeholders(mqttMessage);
    payload.toCharArray(data, (payload.length() + 1));
    client.publish((mqttMasterTopic).c_str(), data, mqttRetained);
    Log(F("MqttSendData"), payload);
  }
  else
    MqttReconnect();
}

void MqttReconnect()
{
  // Loop until we're reconnected
  while (!client.connected() && mqttRetryCounter < mqttMaxRetries)
  {
    bool connected = false;
    if (mqttUser != NULL && mqttUser.length() > 0 && mqttPassword != NULL && mqttPassword.length() > 0)
    {
      Log(F("MqttReconnect"), F("MQTT connect to server with User and Password"));
      connected = client.connect(("gumboard_" + GetChipID()).c_str(), mqttUser.c_str(), mqttPassword.c_str(), "state", 0, true, "diconnected");
    }
    else
    {
      Log(F("MqttReconnect"), F("MQTT connect to server without User and Password"));
      connected = client.connect(("gumboard_" + GetChipID()).c_str(), "state", 0, true, "disconnected");
    }

    // Attempt to connect
    if (connected)
    {
      Log(F("MqttReconnect"), F("MQTT connected!"));
      mqttRetryCounter = 0;
      // ... and publish
      MqttSendData();
    }
    else
    {
      Log(F("MqttReconnect"), F("MQTT not connected!"));
      Log(F("MqttReconnect"), F("Wait 5 seconds before retrying...."));
      mqttRetryCounter++;
    }
  }

  if (mqttRetryCounter >= mqttMaxRetries)
  {
    Log(F("MqttReconnect"), F("No connection to MQTT-Server, MQTT temporarily deactivated!"));
  }
}
//time


int year()
{
  return 0;
}
int month() {
  return 0;
}
int day() {
  return 0;
}
int hour() {
  return 0;
}
int minute() {
  return 0;
}
int second() {
  return 0;
}
/////////////////////////////////////////////////////////////////////

//BUTTON
// Attach callback.
void toggleConfigMode()
{

  Serial.println(F("Config mode changed"));
  configModeActive = !configModeActive;
  if (configModeActive)
  {
    if (DEBUG)Serial.println(F("Config mode activated"));
  }
  else
  {
    if (DEBUG)Serial.println(F("Config mode deactivated"));
  }
  SaveConfigCallback();
  SaveConfig();
  yield();
  if (configModeActive == false)
  {
    //ESP.restart();
  }
}

void startDeepSleep(int freq) {
  if (DEBUG)Serial.println("Going to deep sleep...");

  ESP.deepSleep(freq * 1000000);
  yield();
}
// HTTP REQUESTS

void http_post(String post_json)
{
  http.begin(httpPostURL);      //Specify request destination
  http.addHeader("Content-Type", "application/json");  //Specify content-type header
  post_json = replace_placeholders(httpPostJson);
  int httpCode = http.POST(post_json);   //Send the request
  if (DEBUG)Serial.println(httpCode);  //Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_post"), httpPostJson);
  }
  if (httpCode > 0) { //Check the returning code
    //String payload = http.getString();                  //Get the response payload
    //Serial.println(payload);    //Print request response payload
  }

  http.end();  //Close connection
}
void http_get(String url)
{
  //String temp, humi, dew, qfe, qnh, alt, air, aiq, lux, uv, adc, tvoc, eco2;
  url = replace_placeholders(url);
  http.begin(url);      //Specify request destination
  http.addHeader("Content-Type", "text/plain");  //Specify content-type header

  int httpCode = http.GET();   //Send the request
  if (DEBUG)Serial.println(httpCode);  //Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_get"), url);
  }
  if (httpCode > 0) { //Check the returning code
    //String payload = http.getString();                  //Get the response payload
    //Serial.println(payload);    //Print request response payload
  }

  http.end();  //Close connection
}
//SENSOR

float calibrate_temp(float _v)
{
  if (calibrationActive)
  {
    _v = _v + calibrationTemp;
  }
  return _v;
}
float calibrate_humi(float _v)
{
  if (calibrationActive)
  {
    _v = _v + calibrationHumi;
  }
  return _v;
}
float calibrate_qfe(float _v)
{
  if (calibrationActive)
  {
    _v = _v + calibrationQfe;
  }
  return _v;
}

void read_sensors()
{
  if (bmx_sensor)
  {
    read_bmx280();
  }
  if (bme680_sensor)
  {
    read_bme680();
  }
  if (max44009_sensor)
  {
    read_max44009();
  }
  if (aht20_sensor)
  {
    read_aht20();
  }
  if (dht_sensor)
  {
    read_dht();
  }
  if (am2320_sensor)
  {
    read_am2320();
  }
  if (ds18b20_sensor)
  {
    read_ds18b20();
  }

}
void read_bmx280()
{
  temp = String(calibrate_temp(bmx280.readTemperature()));
  temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
  temp_imp = String(temp_imp);
  if (DEBUG)
  {
    Serial.print(F("Temperature: "));
    Serial.print(temp);
    Serial.println(" C");
  }
  if (bmx280.getChipID() == CHIP_ID_BME280) {
    humi = String(calibrate_humi(bmx280.readHumidity()));
    if (DEBUG) {
      Serial.print(F("Humidity:    "));
      Serial.print(humi);
      Serial.println(" %");
    }
  }

  qfe = String(calibrate_qfe(bmx280.readPressure() / 100.0F));
  if (DEBUG)
  {
    Serial.print(F("Pressure:    "));
    Serial.print(qfe);
    Serial.println(" hPa");
  }

  alt = String(bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
  if (DEBUG)
  {
    Serial.print(F("Altitude:    "));
    Serial.print(alt);
    Serial.println(" m");
    Serial.println();
  }

}

double RHtoAbsolute (float relHumidity, float tempC) {
  double eSat = 6.11 * pow(10.0, (7.5 * tempC / (237.7 + tempC)));
  double vaporPressure = (relHumidity * eSat) / 100; //millibars
  double absHumidity = 1000 * vaporPressure * 100 / ((tempC + 273) * 461.5); //Ideal gas law with unit conversions
  return absHumidity;
}

uint16_t doubleToFixedPoint( double number) {
  int power = 1 << 8;
  double number2 = number * power;
  uint16_t value = floor(number2 + 0.5);
  return value;
}

void read_bme680()
{
  if (! bme680.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  temp = String(calibrate_temp(bme680.temperature));
  temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
  temp_imp = String(temp_imp);
  if (DEBUG)
  {
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" *C");
  }


  qfe = String(calibrate_qfe(bme680.pressure / 100.0));
  if (DEBUG)
  {
    Serial.print("Pressure = ");
    Serial.print(qfe);
    Serial.println(" hPa");
  }

  humi = String(calibrate_humi(bme680.humidity));
  if (DEBUG)
  {
    Serial.print("Humidity = ");
    Serial.print(humi);
    Serial.println(" %");
  }


  air  = String(bme680.gas_resistance / 1000.0);
  if (DEBUG)
  {
    Serial.print("Gas = ");
    Serial.print(air);
    Serial.println(" KOhms");
  }


  alt = String(bme680.readAltitude(SEA_LEVEL_PRESSURE_HPA));
  if (DEBUG)
  {
    Serial.print("Approx. Altitude = ");
    Serial.print(alt);
    Serial.println(" m");
  }
}

void read_max44009()
{
  float max440099lux = Max44009Lux.getLux();
  int err = Max44009Lux.getError();

  // in automatic mode TIM & CDR are automatic generated
  // and read only (in manual mode they are set by the user
  int conf = Max44009Lux.getConfiguration();
  int CDR = (conf & 0x80) >> 3;
  int TIM = (conf & 0x07);
  int integrationTime = Max44009Lux.getIntegrationTime();

  if (err != 0)
  {
    Serial.print("Error:\t");
    Serial.println(err);
  }
  else
  {
    lux = String(max440099lux);
    if (DEBUG)
    {
      Serial.print("lux:\t");
      Serial.print(max440099lux);
      Serial.print("\tCDR:\t");
      Serial.print(CDR);
      Serial.print("\tTIM:\t");
      Serial.print(TIM);
      Serial.print("\t");
      Serial.print(integrationTime);
      Serial.print(" ms");
      Serial.println();
    }
  }
}

void read_aht20()
{
  float humidity, temperature;
  int ret = AHT.getSensor(&humidity, &temperature);

  if (ret)    // GET DATA OK
  {

    humi = String(calibrate_humi(humidity * 100));
    if (DEBUG)
    {
      Serial.print("humidity: ");
      Serial.print(humi);
    }

    temp = String(calibrate_temp(temperature));
    temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
    temp_imp = String(temp_imp);
    if (DEBUG)
    {
      Serial.print("%\t temperature: ");
      Serial.println(temp);
    }
  }
  else        // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }

}

void read_dht()
{
  pinMode(0, OUTPUT);    // sets the digital pin 0 as output
  digitalWrite(0, LOW); // sets the digital pin 0 on

  //delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  delay(dht.getMinimumSamplingPeriod());
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  //Serial.print(dht.getStatusString());
  //Serial.print("\t");
  humi = String(calibrate_humi(humidity));
  temp = String(calibrate_temp(temperature));
  temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
  temp_imp = String(temp_imp);
  if (DEBUG)
  {
    Serial.print(humi);
    Serial.print("\t\t");
    Serial.print(temp);
  }
  /* Serial.print("\t\t");
    Serial.print(dht.toFahrenheit(temperature), 1);
    Serial.print("\t\t");
    Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
    Serial.print("\t\t");
    Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
    delay(2000);*/
}

void read_am2320()
{
  float humidity, temperature;
  Wire.begin(0, 2);
  byte count = 0;

  while (am2320.update() != 0) {
    Serial.println("Error: Cannot update sensor values.");
    break;
    count++;
    if (count > 100)
    {
      break;
    }
    yield();
  }
  yield();
  //if (am2320.update() != 0)
  //{
  temperature = am2320.temperatureC;
  humidity = am2320.humidity;
  humi = String(calibrate_humi(humidity));
  temp = String(calibrate_temp(temperature));
  temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
  temp_imp = String(temp_imp);
  if (DEBUG)
  {
    Serial.print(humi);
    Serial.print("\t\t");
    Serial.print(temp);
  }
  //}
  /* Serial.print("\t\t");
    Serial.print(dht.toFahrenheit(temperature), 1);
    Serial.print("\t\t");
    Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
    Serial.print("\t\t");
    Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);
    delay(2000);*/
}


void read_ds18b20(void)
{
  float temperature;
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  /*
    Serial.print("Found ");
    Serial.print(ds18b20sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (ds18b20sensors.isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");

    // Method 1:
    // Search for devices on the bus and assign based on an index. Ideally,
    // you would do this to initially discover addresses on the bus and then
    // use those addresses and manually assign them (see above) once you know
    // the devices on your bus (and assuming they don't change).
    if (!ds18b20sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
    // show the addresses we found on the bus
    Serial.print("Device 0 Address: ");
    printAddress(insideThermometer);
    Serial.println();

    // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
    ds18b20sensors.setResolution(insideThermometer, 9);

    Serial.print("Device 0 Resolution: ");
    Serial.print(ds18b20sensors.getResolution(insideThermometer), DEC);
    Serial.println();

    Serial.print("Requesting temperatures...");
    ds18b20sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    temperature = sensors.getTempC(deviceAddress);

    // Check if reading was successful
    if (temperature != DEVICE_DISCONNECTED_C)
    {
    temp = String(calibrate_temp(temperature));
    temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
    temp_imp = String(temp_imp);
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(temp);
    }
    else
    {
    Serial.println("Error: Could not read temperature data");
    }*/
}

//end of sensor
void SendInfo(bool force)
{
  String Info;
  if ((webSocket.connectedClients() > 0))
  {
    Info = GetInfo();
  }

  if (webSocket.connectedClients() > 0 && OldInfo != Info)
  {
    for (int i = 0; i < sizeof websocketConnection / sizeof websocketConnection[0]; i++)
    {
      if (websocketConnection[i] == "/main" || websocketConnection[i] == "/api/info")
      {
        webSocket.sendTXT(i, Info);

      }
    }
  }
  OldInfo = Info;
}

void SendSensor(bool force)
{
  if (force)
  {
    OldSensor = "";
  }

  String Sensor;
  if ((webSocket.connectedClients() > 0))
  {
    Sensor = GetSensor();
  }
  if (webSocket.connectedClients() > 0 && OldSensor != Sensor)
  {
    for (uint i = 0; i < sizeof websocketConnection / sizeof websocketConnection[0]; i++)
    {
      if (websocketConnection[i] == "/main")
      {
        webSocket.sendTXT(i, Sensor);
      }
    }
  }

  OldSensor = Sensor;
}

void SendConfig()
{
  if (webSocket.connectedClients() > 0)
  {
    for (int i = 0; i < sizeof websocketConnection / sizeof websocketConnection[0]; i++)
    {
      if (
        websocketConnection[i] == "/settings" ||
        websocketConnection[i] == "/settime" ||
        websocketConnection[i] == "/setalarm" ||
        websocketConnection[i] == "/setsensor" ||
        websocketConnection[i] == "/setsystem"
      )
      {
        String config = GetConfig();
        webSocket.sendTXT(i, config);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void Log(String function, String message)
{

  //String timeStamp = IntFormat(year()) + "-" + IntFormat(month()) + "-" + IntFormat(day()) + "T" + IntFormat(hour()) + ":" + IntFormat(minute()) + ":" + IntFormat(second());
  String timeStamp = "";
  if (DEBUG) Serial.println("[" + timeStamp + "] " + function + ": " + message);
  if (webSocket.connectedClients() > 0)
  {
    for (int i = 0; i < sizeof websocketConnection / sizeof websocketConnection[0]; i++)
    {
      if (websocketConnection[i] == "/main")
      {
        webSocket.sendTXT(i, "{\"log\":{\"timeStamp\":\"" + timeStamp + "\",\"function\":\"" + function + "\",\"message\":\"" + message + "\"}}");
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void serve_data()
{
  // force config when no mode selected
  if (httpGetActive == false && httpPostActive == false && httpPostActive == false)
  {
    configModeActive = true;
  }

  if (httpGetActive)
  {
    http_get(httpGetURL);
    delay(1000);
    if (sleepModeActive)
    {
      startDeepSleep(httpGetFrequency);
    }
    else
    {
      delay(mqttFrequency * 1000);
    }
  }

  if (httpPostActive)
  {
    http_post(httpPostJson);
    delay(1000);

    if (sleepModeActive)
    {
      startDeepSleep(httpPostFrequency);
    }
    else
    {
      delay(mqttFrequency * 1000);
    }
  }

  if (mqttActive)
  {
    MqttSendData();
    delay(1000);
    if (sleepModeActive)
    {
      startDeepSleep(mqttFrequency);
    }
    else
    {
      delay(mqttFrequency * 1000);
    }
  }
}
void led_on()
{
  if (DEBUG)
  {
    Serial.println("Led on");
  }
  digitalWrite(SIGNAL_LED_PIN, HIGH); //on
  if (PIXEL_ACTIVE)
  {
    pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
    pixel.setPixelColor(0,  pixel.Color(  0, 0, 255));
    pixel.setBrightness(50);
    pixel.show();  // Initialize all pixels to 'off'
  }
}
void led_off()
{
  if (DEBUG)
  {
    Serial.println("Led off");
  }
  if (PIXEL_ACTIVE == 1)
  {
    pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
    pixel.setPixelColor(0,  pixel.Color(  0,   0,   0));         //  Set pixel's color (in RAM)
    pixel.setBrightness(0);
    pixel.show();
  }
  digitalWrite(SIGNAL_LED_PIN, LOW); //off
}

void configModeCallback(WiFiManager *myWiFiManager) {
  led_on();
  if (DEBUG)
  {
    Serial.println("##########  Entered wifi config mode    ##################");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
  }
}
void setupWifi() {
  if (DEBUG)Serial.println("Setup WIFI");
  wifiManager.setDebugOutput(true);
  // Set config save notify callback
  wifiManager.setSaveConfigCallback(SaveConfigCallback);
  wifiManager.setMinimumSignalQuality();
  wifiManager.setAPCallback(configModeCallback);

  // Config menu timeout 180 seconds.

  wifiManager.setConfigPortalTimeout(180);

  WiFi.hostname(identifier);
  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if (!wifiManager.autoConnect(identifier))
  {
    Serial.println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }


  if (DEBUG)Serial.println(F("Wifi connected...yeey :)"));

  if (shouldSaveConfig) {
    SaveConfig();
  }

  Serial.println("Setup: Local IP");
  Serial.println("Setup " + WiFi.localIP().toString());
  Serial.println("Setup " + WiFi.gatewayIP().toString());
  Serial.println("Setup " + WiFi.subnetMask().toString());
}

void setupMDSN()
{
  // generate module IDs
  escapedMac = WiFi.macAddress();
  escapedMac.replace(":", "");
  escapedMac.toLowerCase();
  strcpy_P(cmDNS, PSTR("tehybug-"));
  sprintf(cmDNS + 5, "%*s", 6, escapedMac.c_str() + 6);

  // Set up mDNS responder:
  if (strlen(cmDNS) > 0) {
    // "end" must be called before "begin" is called a 2nd time
    // see https://github.com/esp8266/Arduino/issues/7213
    MDNS.end();
    MDNS.begin(cmDNS);

    Serial.println(F("mDNS started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("tehybug", "tcp", 80);
    MDNS.addServiceTxt("tehybug", "tcp", "mac", escapedMac.c_str());
  }
}

void setupSensors()
{
  if (dht_sensor == false && ds18b20_sensor == false)
  {
    Wire.begin(0, 2);
    //Wire.setClock(400000);
    i2c_addresses = i2c_scanner();
    i2c_addresses = i2c_addresses + i2c_scanner();
  }

  if (strContains(i2c_addresses.c_str(), "0x77") == 1)
  {
    bmx280 = bmp280;
    bmx_sensor = true;
  }
  else if (strContains(i2c_addresses.c_str(), "0x76") == 1)
  {
    bmx_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x5c") == 1)
  {
    am2320_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x58") == 1)
  {
    //sgp30
  }
  if (strContains(i2c_addresses.c_str(), "0x77") == 1)
  {
    bme680_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x4a") == 1)
  {
    max44009_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x38") == 1)
  {
    aht20_sensor = true;
  }

  //sensors

  // bmx280 and bme680 have same address
  if (bmx_sensor)
  {
    // Initialize sensor
    while (!bmx280.begin()) {
      Serial.println(F("Error: Could not detect sensor"));
      bmx_sensor = false;
      break;
    }
    if (bmx_sensor)
    {
      // Print sensor type
      if (DEBUG)Serial.print(F("\nSensor type: "));
      switch (bmx280.getChipID()) {
        case CHIP_ID_BMP280:
          if (DEBUG)Serial.println(F("BMP280\n"));
          bme680_sensor = false;
          break;
        case CHIP_ID_BME280:
          if (DEBUG)Serial.println(F("BME280\n"));
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
      bmx280.setSampling(BMX280_MODE_SLEEP,    // SLEEP, FORCED, NORMAL
                         BMX280_SAMPLING_X16,   // Temp:  NONE, X1, X2, X4, X8, X16
                         BMX280_SAMPLING_X16,   // Press: NONE, X1, X2, X4, X8, X16
                         BMX280_SAMPLING_X16,   // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
                         BMX280_FILTER_X16,     // OFF, X2, X4, X8, X16
                         BMX280_STANDBY_MS_500);// 0_5, 10, 20, 62_5, 125, 250, 500, 1000


    }
  }
  if (bme680_sensor)
  {
    if (DEBUG)Serial.println(F("BME680 test"));

    if (!bme680.begin()) {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
      while (1);
    }
    // Set up oversampling and filter initialization
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150); // 320*C for 150 ms
  }
  if (max44009_sensor)
  {
    if (DEBUG) {
      Serial.print("\nStart max44009_setAutomaticMode : ");
      Serial.println(MAX44009_LIB_VERSION);
    }

    Max44009Lux.setAutomaticMode();
  }
  if (aht20_sensor)
  {
    if (DEBUG)Serial.println("AHT20");
    AHT.begin();
  }
  if (dht_sensor)
  {
    dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
  }

  if (ds18b20_sensor)
  {
    // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
    OneWire oneWire(ONE_WIRE_BUS);
    // Pass our oneWire reference to Dallas Temperature.
    DallasTemperature ds18b20sensors(&oneWire);
    // Start up the library
    ds18b20sensors.begin();
  }

  if (am2320_sensor)
  {
    am2320.setWire(&Wire);
  }
}



void setupMode()
{

  delay(100);
  pinMode(BUTTON_PIN, INPUT);
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    delay(300);
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      toggleConfigMode();
    }
  }
  if (configModeActive)
  {
    led_on();
  }
  else
  {
    led_off();
  }
}

void setup()
{
  uuid.seed(ESP.getChipId());
  uuid.generate();
  key = String(uuid.toCharArray());

  pinMode(SIGNAL_LED_PIN, OUTPUT);

  snprintf(identifier, sizeof(identifier), "TEHYBUG-%X", ESP.getChipId());

  Serial.begin(115200);
  while (!Serial);
  if (DEBUG)
  {
    Serial.println(F("key: "));
    Serial.println(key);
  }
  // Mounting FileSystem
  Serial.println(F("Mounting file system..."));
  if (SPIFFS.begin())
  {
    Serial.println(F("Mounted file system."));
    LoadConfig();
  }
  else
  {
    Serial.println(F("Failed to mount FS"));
  }

  setupWifi();
  setupMode();

  if (configModeActive)
  {
    if (DEBUG)Serial.println(F("Starting config mode"));
    httpUpdater.setup(&server);
    server.on(F("/api/info"), HTTP_GET, HandleGetInfo);
    server.on(F("/api/config"), HTTP_POST, HandleSetConfig);
    server.on(F("/api/config"), HTTP_GET, HandleGetConfig);
    server.on(F("/api/sensor"), HTTP_GET, HandleGetSensor);
    server.on(F("/"), HTTP_GET, HandleGetMainPage);
    server.on(F("/otaupdate"), HTTP_POST, HandleOtaUpdate);
    server.onNotFound(HandleNotFound);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Log(F("Setup"), F("Webserver started"));
  }
  else
  {
    if (DEBUG)Serial.println(F("Starting live mode"));
  }

  if (configModeActive == false && mqttActive)
  {
    client.setServer(mqttServer.c_str(), mqttPort);
    client.setCallback(callback);
    client.setBufferSize(4000);
    Log(F("Setup"), F("MQTT started"));
  }

  delay(1000);
  setupSensors();
  if (configModeActive == false)
  {
    ticker.add(0, 10033, [&](void*) {
      read_sensors();
    }, nullptr, true);
  }
}

void loop()
{
  if (configModeActive)
  {
    server.handleClient();
    yield();
    webSocket.loop();
  }
  else
  {
    read_sensors();
    yield();
    serve_data();
  }
  ticker.update();
}
