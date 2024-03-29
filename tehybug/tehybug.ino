#include "AHT20.h"
#include "DHTesp.h"
#include "Max44009.h"
#include "UUID.h"
#include "bsec.h"
#include <AM2320_asukiaaa.h>
#include <Adafruit_NeoPixel.h>
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

#include <PubSubClient.h> // Attention in the lib the #define MQTT_MAX_PACKET_SIZE must be increased to 4000!
#include <TickerScheduler.h>
#include <WebSocketsServer.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

#include <OneWire.h>
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

// tehybug stuff
#include "Tools.h"
#include "Webinterface.h"
#include "i2cscanner.h"
#include "tehybug.h"

Calibration calibration{};
Sensor sensor{};
Device device{};
DataServ serveData{};
Scenarios scenarios{};

#include "configuration.h"

#if defined(ARDUINO_ESP8266_GENERIC)
#define PIXEL_ACTIVE 0
#define SIGNAL_LED_PIN 1
#define I2C_SDA 2
#define I2C_SCL 0
#else
#define I2C_SDA 0
#define I2C_SCL 2
#endif

#ifndef PIXEL_ACTIVE
#define PIXEL_ACTIVE 1
#define PIXEL_COUNT 1 // Number of NeoPixels
#define PIXEL_PIN 12  // Digital IO pin connected to the NeoPixels.

Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

// set pin 4 HIGH to turn on the pixel
#ifndef SIGNAL_LED_PIN
#define SIGNAL_LED_PIN 4
#endif
char identifier[24];

// sensors

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA 1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);

Bsec bme680;

Max44009 Max44009Lux(0x4A);

DHTesp dht;
#if !defined(ARDUINO_ESP8266_GENERIC)
DHTesp dht2;
AHT20 AHT;
#endif

AM2320_asukiaaa am2320;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just
// Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature ds18b20_sensors(&oneWire);

#if !defined(ARDUINO_ESP8266_GENERIC)
// Data wire is plugged into port 13 on the Arduino
#define SECOND_ONE_WIRE_BUS 13

// Setup a oneWire instance to communicate with any OneWire devices (not just
// Maxim/Dallas temperature ICs)
OneWire secondOneWire(SECOND_ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature second_ds18b20_sensors(&secondOneWire);
#endif

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
HTTPClient http1;
HTTPClient http2;

#define COMPILE_HOUR (((__TIME__[0] - '0') * 10) + (__TIME__[1] - '0'))
#define COMPILE_MINUTE (((__TIME__[3] - '0') * 10) + (__TIME__[4] - '0'))
#define COMPILE_SECOND (((__TIME__[6] - '0') * 10) + (__TIME__[7] - '0'))
#define COMPILE_YEAR                                                           \
  ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 +                    \
    (__DATE__[9] - '0')) *                                                     \
       10 +                                                                    \
   (__DATE__[10] - '0'))
#define COMPILE_SHORT_YEAR (((__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))
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

const String version = String(COMPILE_SHORT_YEAR) + IntFormat(COMPILE_MONTH) +
                       IntFormat(COMPILE_DAY) + IntFormat(COMPILE_HOUR) +
                       IntFormat(COMPILE_MINUTE);

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
#if !defined(ARDUINO_ESP8266_GENERIC)
ESP8266HTTPUpdateServer httpUpdater;
#endif

// Timerserver Vars
String oldInfo = "";   // old board info
String oldSensor = ""; // old sensor info
// Websoket Vars
String websocketConnection[10];

UUID uuid;

DynamicJsonDocument sensorData(1024);

TickerScheduler ticker(5);

/////////////////////////////////////////////////////////////////////
float calibrateValue(String _n, float _v) {
  if (calibration.active) {
    if (_n == "temp")
      _v += calibration.temp;
    else if (_n == "humi")
      _v += calibration.humi;
    else if (_n == "qfe")
      _v += calibration.qfe;
  }
  return _v;
}

void addTempHumi(String key_temp, float temp, String key_humi, float humi) {
  addSensorData(key_temp, temp);
  addSensorData(key_humi, humi);
}

void additionalSensorData(String key, float value) {

  if (key == "temp" || key == "temp2") {
    addSensorData(key + "_imp", temp2Imp(value));
  }
  // humi should be always set after temp so the following calculation will work
  else if (key == "humi" || key == "humi2") {

    const String num = atoi(key.c_str()) > 0 ? String(atoi(key.c_str())) : "";

    const float hi = dht.computeHeatIndex(sensorData["temp" + num].as<float>(),
                                          sensorData[key + num].as<float>());
    addSensorData("hi" + num, hi);
    addSensorData("hi_imp" + num, temp2Imp(hi));

    const float dew = dht.computeDewPoint(sensorData["temp" + num].as<float>(),
                                          sensorData[key + num].as<float>());
    addSensorData("dew" + num, dew);
    addSensorData("dew_imp" + num, temp2Imp(dew));
  }
}
void addSensorData(String key, float value) {

  if (key == "temp" || key == "temp2") {
    value = calibrateValue("temp", value);
  } else if (key == "humi" || key == "humi2") {
    value = calibrateValue("humi", value);
  } else if (key == "qfe") {
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

void WifiSetup() {
  wifiManager.resetSettings();
  ESP.restart();
  delay(300);
}

void handleGetMainPage() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", mainPage);
}

#pragma region
/* HTTP API */
void handleNotFound() {
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(204);
  }
  server.sendHeader("Location", "/update", true);
  server.send(302, "text/plain", "");
}

void handleSetConfig() {
  DynamicJsonDocument json(1024);
  auto error = deserializeJson(json, server.arg("plain"));
  server.sendHeader("Connection", "close");

  if (!error) {
    Log(("SetConfig"), ("Incoming Json length: " + String(measureJson(json))));
    // extract the data
    JsonObject object = json.as<JsonObject>();
    Config::setConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
    delay(500);
    // ESP.restart();
  } else {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void handleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", Config::getConfig());
}

void handleGetInfo() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getInfo());
}

void handleGetSensor() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getSensor());
}

void handleGetIp() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", WiFi.localIP().toString());
}

void handleFactoryReset() {
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Log("handleFactoryReset", "Failed to open config file for reset");
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
    channel.replace(serveData.mqtt.topic, "");

    DynamicJsonDocument json(512);
    deserializeJson(json, payload);

    Log("MQTT_callback", "Incoming Json length to topic " + String(topic) +
                             ": " + String(measureJson(json)));
    if (channel.equals("getInfo")) {
      client.publish((serveData.mqtt.topic + "info").c_str(),
                     getInfo().c_str());
    } else if (channel.equals("getConfig")) {
      client.publish((serveData.mqtt.topic + "config").c_str(),
                     Config::getConfig().c_str());
    } else if (channel.equals("setConfig")) {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      Config::setConfig(object);
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
    sendDeviceInfo(true);
    sendSensorData(true);
    sendConfig();
    break;
  }
  case WStype_TEXT: {
    if (((char *)payload)[0] == '{') {
      DynamicJsonDocument json(1024);
      deserializeJson(json, payload);
      Log("WebSocketEvent",
          "Incoming Json length: " + String(measureJson(json)));
      if (websocketConnection[num] == "/setConfig") {
        // extract the data
        JsonObject object = json.as<JsonObject>();
        Config::setConfig(object);
        // delay(500);
        // ESP.restart();
      }
    }
    break;
  }
  }
}
#pragma endregion

String getInfo() {
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
  root["sleepModeActive"] = device.sleepMode;
  root["deepSleepMax"] = (int)(ESP.deepSleepMax() / 1000000);
  root["key"] = device.key;

  String json;
  serializeJson(root, json);

  return json;
}

String getSensor() {
  read_sensors();
  String json;
  serializeJson(sensorData, json);
  return json;
}

/////////////////////////////////////////////////////////////////////
void mqttSendData() {
  if (client.connected()) {
    String payload = replace_placeholders(serveData.mqtt.message);
    payload.toCharArray(serveData.data, (payload.length() + 1));
    client.publish((serveData.mqtt.topic).c_str(), serveData.data,
                   serveData.mqtt.retained);
    Log(F("MqttSendData"), payload);
  } else
    mqttReconnect();
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected() &&
         serveData.mqtt.retryCounter < serveData.mqtt.maxRetries) {
    bool connected = false;
    if (serveData.mqtt.user != NULL && serveData.mqtt.user.length() > 0 &&
        serveData.mqtt.password != NULL &&
        serveData.mqtt.password.length() > 0) {
      Log(F("MqttReconnect"),
          F("MQTT connect to server with User and Password"));
      connected = client.connect(
          ("tehybug_" + GetChipID()).c_str(), serveData.mqtt.user.c_str(),
          serveData.mqtt.password.c_str(), "state", 0, true, "diconnected");
    } else {
      Log(F("MqttReconnect"),
          F("MQTT connect to server without User and Password"));
      connected = client.connect(("tehybug_" + GetChipID()).c_str(), "state", 0,
                                 true, "disconnected");
    }

    // Attempt to connect
    if (connected) {
      Log(F("MqttReconnect"), F("MQTT connected!"));
      serveData.mqtt.retryCounter = 0;
      // ... and publish
      mqttSendData();
    } else {
      Log(F("MqttReconnect"), F("MQTT not connected!"));
      Log(F("MqttReconnect"), F("Wait 5 seconds before retrying...."));
      serveData.mqtt.retryCounter++;
      updateMqttClient();
    }
  }

  if (serveData.mqtt.retryCounter >= serveData.mqtt.maxRetries) {
    Log(F("MqttReconnect"),
        F("No connection to MQTT-Server, MQTT temporarily deactivated!"));
  }
}
/////////////////////////////////////////////////////////////////////

// BUTTON
//  Attach callback.
void toggleConfigMode() {

  Serial.println(F("Config mode changed"));
  device.configMode = !device.configMode;
  if (device.configMode) {
    D_println(F("Config mode activated"));
  } else {
    D_println(F("Config mode deactivated"));
  }
  Config::saveConfigCallback();
  Config::saveConfig();
  yield();
  if (device.configMode == false) {
    // ESP.restart();
  }
}

void startDeepSleep(int freq) {
  D_println("Going to deep sleep...");
  led_off();
  ESP.deepSleep(freq * 1000000);
  yield();
}
// HTTP REQUESTS
void http_post_custom(HTTPClient &http, String url, String post_json) {
  D_print("HTTP POST: ");
  D_println(url);
  http.begin(espClient, url); // Specify request destination
  http.addHeader("Content-Type",
                 "application/json"); // Specify content-type header
  post_json = replace_placeholders(post_json);
  int httpCode = http.POST(post_json); // Send the request
  D_println(httpCode);                 // Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_post"), post_json);
  } else if (httpCode > 0) {                 // Check the returning code
    const String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  http.end(); // Close connection
}
void httpPost() {
  http_post_custom(http1, serveData.post.url, serveData.post.message);
}
void http_get_custom(HTTPClient &http, String url) {
  D_print("HTTP GET: ");
  D_println(url);
  url = replace_placeholders(url);
  http.begin(espClient, url); // Specify request destination
  http.setURL(url);
  http.addHeader("Content-Type", "text/plain"); // Specify content-type header

  int httpCode = http.GET(); // Send the request
  D_println(httpCode);       // Print HTTP return code
  if (httpCode == 200) {
    Log(F("http_get"), url);
  } else if (httpCode > 0) {                 // Check the returning code
    const String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  http.end(); // Close connection
}
void httpGet() { http_get_custom(http1, serveData.get.url); }
// SENSOR

void read_bmx280() {

  if (bmx280.getChipID() == CHIP_ID_BME280) {
    addTempHumi("temp", bmx280.readTemperature(), "humi",
                bmx280.readHumidity());
  } else if (sensor.aht20) {
    addSensorData("temp2", bmx280.readTemperature());
  } else {
    addSensorData("temp", bmx280.readTemperature());
  }
  addSensorData("qfe", (bmx280.readPressure() / 100.0F));
  addSensorData("alt", bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
}

// Helper function definitions
void checkIaqSensorStatus(void) {
  if (bme680.status != BSEC_OK) {
    if (bme680.status < BSEC_OK) {
      D_println("BSEC error code : " + String(bme680.status));
      for (;;)
        delay(1); /* Halt in case of failure */
    } else {
      D_println("BSEC warning code : " + String(bme680.status));
    }
  }

  if (bme680.bme680Status != BME680_OK) {
    if (bme680.bme680Status < BME680_OK) {
      D_println("BME680 error code : " + String(bme680.bme680Status));
      for (;;)
        delay(1); /* Halt in case of failure */
    } else {
      D_println("BME680 warning code : " + String(bme680.bme680Status));
    }
  }
}
void read_bme680() {

  if (!bme680.run()) { // If no data is available
    checkIaqSensorStatus();
    return;
  }

  D_print(String(bme680.rawTemperature));
  D_print(", " + String(bme680.pressure));
  D_print(", " + String(bme680.rawHumidity));
  D_print(", " + String(bme680.gasResistance));
  D_print(", " + String(bme680.iaq));
  D_print(", " + String(bme680.iaqAccuracy));
  D_print(", " + String(bme680.temperature));
  D_print(", " + String(bme680.humidity));
  D_print(", " + String(bme680.staticIaq));
  D_print(", " + String(bme680.co2Equivalent));
  D_println(", " + String(bme680.breathVocEquivalent));

  addSensorData("qfe", (bme680.pressure / 100.0F));
  addSensorData("eco2", bme680.co2Equivalent);
  addSensorData("bvoc", bme680.breathVocEquivalent);
  addSensorData("iaq", bme680.iaq);
  addSensorData("air", (bme680.gasResistance / 1000.0F));
  addTempHumi("temp", bme680.temperature, "humi", bme680.humidity);
}

void read_max44009() {
  const float max440099lux = Max44009Lux.getLux();
  int err = Max44009Lux.getError();

  // in automatic mode TIM & CDR are automatic generated
  // and read only (in manual mode they are set by the user
  uint8_t conf = Max44009Lux.getConfiguration();
  int CDR = (conf & 0x80) >> 3;
  int TIM = (conf & 0x07);
  int integrationTime = Max44009Lux.getIntegrationTime();

  if (err != 0) {
    D_print("Error:\t");
    D_println(err);
  } else {
    addSensorData("lux", max440099lux);
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
#if !defined(ARDUINO_ESP8266_GENERIC)
void read_aht20() {
  float humidity, temperature;
  bool ret = AHT.getSensor(&humidity, &temperature);

  if (ret) // GET DATA OK
  {
    addTempHumi("temp", temperature, "humi", (humidity * 100.0F));
  } else // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }
}
#endif
void read_dht_custom(DHTesp &dht, const String &&temp, const String &&humi) {
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  delay(dht.getMinimumSamplingPeriod());
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  addTempHumi(temp, temperature, humi, humidity);
}

void read_dht() {
  pinMode(0, OUTPUT);   // sets the digital pin 0 as output
  digitalWrite(0, LOW); // sets the digital pin 0 on
  read_dht_custom(dht, "temp", "humi");
}
#if !defined(ARDUINO_ESP8266_GENERIC)
void read_second_dht() { read_dht_custom(dht2, "temp2", "humi2"); }
#endif
void read_am2320() {
  float humidity, temperature;
  Wire.begin(I2C_SDA, I2C_SCL);
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
  addTempHumi("temp", temperature, "humi", humidity);
}

void read_ds18b20_custom(DallasTemperature &ds18b20, const String &&temp) {

  // Start up the library
  ds18b20.begin();
  // Setup a oneWire instance to communicate with any OneWire devices (not just
  // Maxim/Dallas temperature ICs) call ds18b20_sensors.requestTemperatures() to
  // issue a global temperature request to all devices on the bus
  D_print("Requesting temperatures...");
  ds18b20.requestTemperatures(); // Send the command to get temperatures
  D_println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the
  // first sensor only.
  const float tempC = ds18b20.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    D_print("Temperature for the device 1 (index 0) is: ");
    D_println(tempC);
    addSensorData(temp, tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

void read_ds18b20(void) {
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  delay(100);
  read_ds18b20_custom(ds18b20_sensors, "temp");
}
#if !defined(ARDUINO_ESP8266_GENERIC)
void read_second_ds18b20(void) {
  pinMode(SECOND_ONE_WIRE_BUS, INPUT_PULLUP);
  delay(100);
  read_ds18b20_custom(second_ds18b20_sensors, "temp2");
}

void read_adc() {
  uint8_t pin = 13;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // on
  delay(100);
  // read the analog in value
  const float sensorValue = analogRead(0);
  addSensorData("adc", sensorValue);
  digitalWrite(pin, LOW); // off
}
#endif

void read_sensors() {
  if (sensor.bmx) {
    read_bmx280();
  }
  if (sensor.bme680) {
    read_bme680();
  }
  if (sensor.max44009) {
    read_max44009();
  }
  if (sensor.dht) {
    read_dht();
  }
  if (sensor.am2320) {
    read_am2320();
  }
  if (sensor.ds18b20) {
    read_ds18b20();
  }

#if !defined(ARDUINO_ESP8266_GENERIC)
  if (sensor.aht20) {
    read_aht20();
  }
  if (sensor.adc) {
    read_adc();
  }
  if (sensor.dht_2) {
    read_second_dht();
  }
  if (sensor.ds18b20_2) {
    read_second_ds18b20();
  }
#endif
}
// end of sensor
void sendDeviceInfo(bool force) {
  if (force) {
    oldInfo = "";
  }
  String info;
  if ((webSocket.connectedClients() > 0)) {
    info = getInfo();
  }
  if (webSocket.connectedClients() > 0 && oldInfo != info) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/firststart" ||
          websocketConnection[i] == "/api/info") {
        webSocket.sendTXT(i, info);
      }
    }
  }
  oldInfo = info;
}

void sendSensorData(bool force) {
  if (force) {
    oldSensor = "";
  }
  String sensor;
  if ((webSocket.connectedClients() > 0)) {
    sensor = getSensor();
  }
  if (webSocket.connectedClients() > 0 && oldSensor != sensor) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/settings") {
        webSocket.sendTXT(i, sensor);
      }
    }
  }
  oldSensor = sensor;
}

void sendConfig() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/settings" ||
          websocketConnection[i] == "/setsensor" ||
          websocketConnection[i] == "/scenarios" ||
          websocketConnection[i] == "/setsystem") {
        String config = Config::getConfig();
        webSocket.sendTXT(i, config);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void Log(String function, String message) {
  D_println(function + ": " + message);
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main") {
        webSocket.sendTXT(i, "{\"log\":{\"function\":\"" + function +
                                 "\",\"message\":\"" + message + "\"}}");
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////
void serve_data() {
  if (serveData.get.active) {
    httpGet();
    delay(1000);
    if (device.sleepMode) {
      startDeepSleep(serveData.get.frequency);
    } /* else {
       delay(httpGetFrequency * 1000);
     }*/
  }

  if (serveData.post.active) {
    httpPost();
    delay(1000);
    if (device.sleepMode) {
      startDeepSleep(serveData.post.frequency);
    } /* else {
       delay(httpPostFrequency * 1000);
     }*/
  }

  if (serveData.mqtt.active) {
    mqttSendData();
    delay(1000);
    if (device.sleepMode) {
      startDeepSleep(serveData.mqtt.frequency);
    } /* else {
       delay(mqttFrequency * 1000);
     }*/
  }
}

void checkScenario(Scenario &s) {
  float val = 0;
  bool conditionMet = false;
  if (s.active) {

    if (sensorData.containsKey(s.data)) {
      val = sensorData[s.data];
    }
    conditionMet = (s.condition == "lt" && val < s.value)
    ||(s.condition == "gt" && val > s.value) 
    ||(s.condition == "eq" && val == s.value);

    if (conditionMet) {
      D_println("condition met");
      D_println(s.url);
      if (s.type == "post") {
        http_post_custom(http2, s.url, s.message);
      } else if (isIOScenario(s.type)) {
        uint8_t pin = ioScenarioPin(s.type);
        pinMode(pin, OUTPUT);
        digitalWrite(pin, ioScenarioLevel(s.type));
      } else {
        http_get_custom(http2, s.url);
      }
    }
    // delay(1000);
  }
}

void serve_scenario() {
  checkScenario(scenarios.scenario1);
  checkScenario(scenarios.scenario2);
  checkScenario(scenarios.scenario3);
}

void led_on() {
  D_println("Led on");
  if (SIGNAL_LED_PIN == 1) {
    pinMode(SIGNAL_LED_PIN, OUTPUT);
    digitalWrite(SIGNAL_LED_PIN, LOW); // on
  } else {
    digitalWrite(SIGNAL_LED_PIN, HIGH); // on
#if !defined(ARDUINO_ESP8266_GENERIC)
    if (PIXEL_ACTIVE) {
      pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
      pixel.setPixelColor(0, pixel.Color(0, 0, 255));
      pixel.setBrightness(50);
      pixel.show(); // Initialize all pixels to 'on'
    }
#endif
  }
}

void led_off() {
  D_println("Led off");
  if (SIGNAL_LED_PIN == 10) {
    digitalWrite(SIGNAL_LED_PIN, HIGH); // off
  } else {
#if !defined(ARDUINO_ESP8266_GENERIC)
    if (PIXEL_ACTIVE) {
      pixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
      pixel.setPixelColor(0,
                          pixel.Color(0, 0, 0)); //  Set pixel's color (in RAM)
      pixel.setBrightness(0);
      pixel.show();
    }
#endif
    digitalWrite(SIGNAL_LED_PIN, LOW); // off
  }
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
  wifiManager.setSaveConfigCallback(Config::saveConfigCallback);
  wifiManager.setMinimumSignalQuality();
  wifiManager.setAPCallback(configModeCallback);

  // Config menu timeout 180 seconds.
  wifiManager.setConfigPortalTimeout(180);

  WiFi.hostname(identifier);
  // set custom ip for portal
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  std::vector<const char *> wm_menu = {"wifi", "exit"};
  wifiManager.setShowInfoUpdate(false);
  wifiManager.setShowInfoErase(false);
  wifiManager.setMenu(wm_menu);
  wifiManager.setCustomHeadElement(
      "<style>button {background-color: #1FA67A;}</style>");
  if (!wifiManager.autoConnect(identifier, "TeHyBug123")) {
    Serial.println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    // ESP.reset();
    startDeepSleep(9000);
    delay(5000);
  }

  D_println(F("Wifi successfully connected!"));

  if (Config::shouldSaveConfig) {
    Config::saveConfig();
  }

  Serial.println("Setup: Local IP");
  Serial.println("Setup " + WiFi.localIP().toString());
  Serial.println("Setup " + WiFi.gatewayIP().toString());
  Serial.println("Setup " + WiFi.subnetMask().toString());

  setupMDSN();
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
void findI2Csensors() {
  Wire.begin(I2C_SDA, I2C_SCL);
  // Wire.setClock(400000);
  // required to scan twice to find sensors like ams2320
  i2cScanner::scan();
  i2cScanner::scan();

  if (i2cScanner::addressExists("0x77")) {
    bmx280 = bmp280;
    sensor.bmx = true;
  } else if (i2cScanner::addressExists("0x76")) {
    sensor.bmx = true;
  }
  if (i2cScanner::addressExists("0x5c")) {
    sensor.am2320 = true;
  }
  if (i2cScanner::addressExists("0x58")) {
    // sgp30
  }
  if (i2cScanner::addressExists("0x77")) {
    sensor.bme680 = true;
  }
  if (i2cScanner::addressExists("0x4a")) {
    sensor.max44009 = true;
  }
  if (i2cScanner::addressExists("0x38")) {
    sensor.aht20 = true;
  }
}

void setupSensors() {
  if (!sensor.dht && !sensor.ds18b20) {
    findI2Csensors();
  }
  // sensors
  // bmx280 and bme680 have same address
  if (sensor.bmx) {
    // Initialize sensor
    while (!bmx280.begin()) {
      D_println(F("Error: Could not detect sensor"));
      sensor.bmx = false;
      break;
    }
    if (sensor.bmx) {
      // Print sensor type
      D_print(F("\nSensor type: "));
      switch (bmx280.getChipID()) {
      case CHIP_ID_BMP280:
        D_println(F("BMP280\n"));
        sensor.bme680 = false;
        break;
      case CHIP_ID_BME280:
        D_println(F("BME280\n"));
        sensor.bme680 = false;
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
  if (sensor.bme680) {
    D_println(F("BME680 test"));

    bme680.begin(BME680_I2C_ADDR_SECONDARY, Wire);

    D_print("BSEC library version " + String(bme680.version.major) + ".");
    D_print(String(bme680.version.minor) + ".");
    D_print(String(bme680.version.major_bugfix) + ".");
    D_println(String(bme680.version.minor_bugfix));

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
    D_println(
        "Timestamp [ms], raw temperature [°C], pressure [hPa], raw "
        "relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature "
        "[°C], relative humidity [%], Static IAQ, CO2 equivalent, breath "
        "VOC equivalent");
  }
  if (sensor.max44009) {
    D_print("\nStart max44009_setAutomaticMode : ");
    D_println(MAX44009_LIB_VERSION);

    Max44009Lux.setAutomaticMode();
  }
  if (sensor.dht) {
    dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
  }
#if !defined(ARDUINO_ESP8266_GENERIC)
  if (sensor.aht20) {
    D_println("AHT20");
    AHT.begin();
  }
  if (sensor.dht_2) {
    pinMode(13, INPUT_PULLUP);
    dht2.setup(13, DHTesp::DHT22); // Connect DHT sensor to GPIO 13
  }
#endif
  if (sensor.am2320) {
    am2320.setWire(&Wire);
  }

  if (sensor.ds18b20) {
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  }
}

void turnLedOn() {
  if (device.configMode) {
    led_on();
  } else {
    led_off();
  }
}
void setupMode() {
  delay(100);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(300);
    if (digitalRead(BUTTON_PIN) == LOW) {
      toggleConfigMode();
      turnLedOn();
      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
    }
  }
  turnLedOn();
}

void updateMqttClient() {
  if (serveData.mqtt.active) {
    client.setServer(serveData.mqtt.server.c_str(), serveData.mqtt.port);
  }
}

void generateDeviceKey() {
  uuid.seed(ESP.getChipId());
  uuid.generate();
  device.key = String(uuid.toCharArray());
  D_println(F("key: "));
  D_println(device.key);
}

void setDeviceKey() {
  // UUID – is a 36-character alphanumeric string
  if (device.key.length() != 36) {
    generateDeviceKey();
  }
  sensorData["key"] = device.key;
}

void setup() {
  pinMode(SIGNAL_LED_PIN, OUTPUT);

  snprintf(identifier, sizeof(identifier), "TEHYBUG-%X", ESP.getChipId());

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // Mounting FileSystem
  D_println(F("Mounting file system..."));
  if (SPIFFS.begin()) {
    D_println(F("File system successfully mounted."));
    Config::loadConfig();
  } else {
    D_println(F("Failed to mount FS"));
  }
  // should be called after the fs mount
  setDeviceKey();

  // force config when no data serving mode is selected
  if (serveData.get.active == false && serveData.post.active == false &&
      serveData.mqtt.active == false) {
    device.configMode = true;
    D_println("Data serving mode not selected");
  }

  setupWifi();
  setupMode();

  if (device.configMode) {
    D_println(F("Starting config mode"));
#if !defined(ARDUINO_ESP8266_GENERIC)
    httpUpdater.setup(&server);
#endif
    server.on(F("/api/info"), HTTP_GET, handleGetInfo);
    server.on(F("/api/config"), HTTP_POST, handleSetConfig);
    server.on(F("/api/config"), HTTP_GET, handleGetConfig);
    server.on(F("/api/sensor"), HTTP_GET, handleGetSensor);
    server.on(F("/api/getip"), HTTP_GET, handleGetIp);
    server.on(F("/"), HTTP_GET, handleGetMainPage);
    server.onNotFound(handleNotFound);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Log(F("Setup"), F("Webserver started"));
  } else {
    D_println(F("Starting live mode"));
  }

  if (device.configMode == false && serveData.mqtt.active) {
    updateMqttClient();
    client.setCallback(callback);
    client.setBufferSize(4000);
    Log(F("Setup"), F("MQTT started"));
  }

  setupSensors();

  // setup tickers for non-deep-sleep mode
  if (!device.configMode && !device.sleepMode) {

    uint8_t ticker_num = 0;
    if (serveData.get.active) {
      ticker.add(
          ticker_num, serveData.get.frequency * 1000,
          [&](void *) {
            read_sensors();
            yield();
            httpGet();
          },
          nullptr, true);
      ticker_num++;
    }

    if (serveData.post.active) {
      ticker.add(
          ticker_num, serveData.post.frequency * 1000,
          [&](void *) {
            read_sensors();
            yield();
            httpPost();
          },
          nullptr, true);
      ticker_num++;
    }

    if (serveData.mqtt.active) {
      ticker.add(
          ticker_num, serveData.mqtt.frequency * 1000,
          [&](void *) {
            read_sensors();
            yield();
            mqttSendData();
          },
          nullptr, true);
      ticker_num++;
    }
  }
}

void loop() {
  // config mode
  if (device.configMode) {
    MDNS.update();
    server.handleClient();
    yield();
    webSocket.loop();
  }
  // deep sleep mode
  else if (device.sleepMode) {
    read_sensors();
    yield();
    serve_scenario();
    yield();
    serve_data();
  }
  // update ticker for the non-deep-sleep mode
  ticker.update();
}
