#include "AHT20.h"
#include "DHTesp.h"
#include "Max44009.h"
#include "bsec.h"
#include <AM2320_asukiaaa.h>
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

#include <WiFiClientSecureBearSSL.h>
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

#if defined(ARDUINO_ESP8266_GENERIC)
#define PIXEL_ACTIVE 0
#define SIGNAL_LED_PIN 1
#define I2C_SDA 2
#define I2C_SCL 0
#else
#define I2C_SDA 0
#define I2C_SCL 2
#endif

// tehybug stuff
#include "common_functions.h"
#include "Webinterface.h"
#include "i2cscanner.h"

#include "tehybug.h"

DHTesp dht;
TeHyBug tehybug(dht);
#include "http_request.h"
#include "ha.h"

char wifiSsid[16];
const char *wifiPassword = "TeHyBug123";
// sensors

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA 1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);

Bsec bme680;

Max44009 Max44009Lux(0x4A);

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
const IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
// HTTP Config
HTTPClient http1;
HTTPClient http2;
HTTPClient http3;

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
BearSSL::WiFiClientSecure espClient_ssl;

PubSubClient MqttClient(espClient);
WiFiManager wifiManager;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
#if !defined(ARDUINO_ESP8266_GENERIC)
ESP8266HTTPUpdateServer httpUpdater;
#endif

// Websoket Vars
String websocketConnection[10];

TickerScheduler ticker(5);
/////////////////////////////////////////////////////////////////////
#pragma region
/* HTTP API */
void handleGetMainPage() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", mainPage);
}

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
  const auto error = deserializeJson(json, server.arg("plain"));
  server.sendHeader("Connection", "close");
  if (!error) {
    Log(("SetConfig"), ("Incoming Json length: " + String(measureJson(json))));
    // extract the data
    JsonObject object = json.as<JsonObject>();
    tehybug.conf.setConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
  } else {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void handleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", tehybug.conf.getConfig());
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
  tehybug.pixel.on(255, 0, 0);
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Log("handleFactoryReset", "Failed to open config file for reset");
  }

  D_println("Factory reset!");
  configFile.println("");
  configFile.close();
  wifiManager.resetSettings();
  yield();
  ESP.restart();
}

#pragma endregion

#pragma region
/* MQTT */
void callback(char *topic, byte *payload, unsigned int length) {
  if (payload[0] == '{') {
    payload[length] = '\0';
    String channel = String(topic);
    channel.replace(tehybug.serveData.mqtt.topic, "");
    DynamicJsonDocument json(512);
    deserializeJson(json, payload);
    Log("MQTT_callback", "Incoming Json length to topic " + String(topic) +
        ": " + String(measureJson(json)));
    if (channel.equals("getInfo")) {
      MqttClient.publish((tehybug.serveData.mqtt.topic + "info").c_str(),
                     getInfo().c_str());
    } else if (channel.equals("getConfig")) {
      MqttClient.publish((tehybug.serveData.mqtt.topic + "config").c_str(),
                     tehybug.conf.getConfig().c_str());
    } else if (channel.equals("setConfig")) {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      tehybug.conf.setConfig(object);
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
        const IPAddress ip = webSocket.remoteIP(num);
        Log("WebSocketEvent", "[" + String(num) + "] Connected from " +
            ip.toString() +
            " url: " + websocketConnection[num]);
        // send message to client
        sendDeviceInfo();
        sendSensorData();
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
            tehybug.conf.setConfig(object);
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
  root["sleepModeActive"] = tehybug.device.sleepMode;
  root["deepSleepMax"] = (int)(ESP.deepSleepMax() / 1000000);
  root["key"] = tehybug.device.key;
  String json;
  serializeJson(root, json);
  return json;
}

String getSensor() {
  read_sensors();
  String json;
  serializeJson(tehybug.sensorData, json);
  return json;
}

/////////////////////////////////////////////////////////////////////
void haSendData() {
  if (MqttClient.connected()) {
    ha::publishAutoConfig(MqttClient, version, tehybug.sensorData);
    ha::publishState(MqttClient, tehybug.sensorData);
    Log(F("HomeAssistant"), F("MqttSendData"));
  } else
    mqttReconnect();
}
void mqttSendData() {
  if (MqttClient.connected()) {
    MqttClient.publish((tehybug.serveData.mqtt.topic).c_str(), tehybug.serveData.data,
                   tehybug.serveData.mqtt.retained);
    String payload = tehybug.replacePlaceholders(tehybug.serveData.mqtt.message);
    payload.toCharArray(tehybug.serveData.data, (payload.length() + 1));
    Log(F("MqttSendData"), payload);
  } else
    mqttReconnect();
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!MqttClient.connected() &&
         tehybug.serveData.mqtt.retryCounter < tehybug.serveData.mqtt.maxRetries) {
    bool connected = false;
    if (tehybug.serveData.mqtt.user != NULL && tehybug.serveData.mqtt.user.length() > 0 &&
        tehybug.serveData.mqtt.password != NULL &&
        tehybug.serveData.mqtt.password.length() > 0) {
      Log(F("MqttReconnect"),
          F("MQTT connect to server with User and Password"));
      connected = MqttClient.connect(
                    ("tehybug_" + GetChipID()).c_str(), tehybug.serveData.mqtt.user.c_str(),
                    tehybug.serveData.mqtt.password.c_str(), "state", 0, true, "diconnected");
    } else {
      Log(F("MqttReconnect"),
          F("MQTT connect to server without User and Password"));
      connected = MqttClient.connect(("tehybug_" + GetChipID()).c_str(), "state", 0,
                                 true, "disconnected");
    }

    // Attempt to connect
    if (connected) {
      Log(F("MqttReconnect"), F("MQTT connected!"));
      tehybug.serveData.mqtt.retryCounter = 0;
      // ... and publish
      if (tehybug.serveData.ha.active)
      {
        haSendData();
      }
      else
      {
        mqttSendData();
      }
    } else {
      Log(F("MqttReconnect"), F("MQTT not connected!"));
      Log(F("MqttReconnect"), F("Wait 5 seconds before retrying...."));
      tehybug.serveData.mqtt.retryCounter++;
      updateMqttClient();
    }
  }

  if (tehybug.serveData.mqtt.retryCounter >= tehybug.serveData.mqtt.maxRetries) {
    Log(F("MqttReconnect"),
        F("No connection to MQTT-Server, MQTT temporarily deactivated!"));
  }
}
/////////////////////////////////////////////////////////////////////

// BUTTON
//  Attach callback.
void toggleConfigMode() {

  D_println(F("Config mode changed"));
  tehybug.device.configMode = !tehybug.device.configMode;
  if (tehybug.device.configMode) {
    D_println(F("Config mode activated"));
  } else {
    D_println(F("Config mode deactivated"));
  }
  tehybug.conf.saveConfigCallback();
  tehybug.conf.saveConfig();
  yield();
  if (tehybug.device.configMode == false) {
    // ESP.restart();
  }
}

void startDeepSleep(int freq) {
  D_println("Going to deep sleep...");
  tehybug.pixel.off();
  pinMode(I2C_SDA, OUTPUT);
  pinMode(I2C_SCL, OUTPUT);
  ESP.deepSleep(freq * 1000000);
  yield();
}

// HTTP REQUESTS
WiFiClient & getClient(String & url)
{
  if(url.substring(0,5) == "https")
    return espClient_ssl;
  else
    return espClient;
}

void httpPost() {
  http::post(http1, getClient(tehybug.serveData.post.url), tehybug.serveData.post.url, tehybug.replacePlaceholders(tehybug.serveData.post.message));
}
void httpGet() {
  http::get(http1, getClient(tehybug.serveData.get.url), tehybug.replacePlaceholders(tehybug.serveData.get.url));
}
// SENSOR

void read_bmx280() {

  if (bmx280.getChipID() == CHIP_ID_BME280) {
    tehybug.addTempHumi("temp", bmx280.readTemperature(), "humi",
                        bmx280.readHumidity());
  } else if (tehybug.sensor.aht20) {
    tehybug.addSensorData("temp2", bmx280.readTemperature());
  } else {
    tehybug.addSensorData("temp", bmx280.readTemperature());
  }
  tehybug.addSensorData("qfe", (bmx280.readPressure() / 100.0F));
  tehybug.addSensorData("alt", bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
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

  tehybug.addSensorData("qfe", (bme680.pressure / 100.0F));
  tehybug.addSensorData("eco2", bme680.co2Equivalent);
  tehybug.addSensorData("bvoc", bme680.breathVocEquivalent);
  tehybug.addSensorData("iaq", bme680.iaq);
  tehybug.addSensorData("air", (bme680.gasResistance / 1000.0F));
  tehybug.addTempHumi("temp", bme680.temperature, "humi", bme680.humidity);
}

void read_max44009() {
  const float max440099lux = Max44009Lux.getLux();
  const int err = Max44009Lux.getError();

  // in automatic mode TIM & CDR are automatic generated
  // and read only (in manual mode they are set by the user
  const uint8_t conf = Max44009Lux.getConfiguration();
  const int CDR = (conf & 0x80) >> 3;
  const int TIM = (conf & 0x07);
  const int integrationTime = Max44009Lux.getIntegrationTime();

  if (err != 0) {
    D_print("Error:\t");
    D_println(err);
  } else {
    tehybug.addSensorData("lux", max440099lux);
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
    tehybug.addTempHumi("temp", temperature, "humi", (humidity * 100.0F));
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
  tehybug.addTempHumi(temp, temperature, humi, humidity);
}

void read_dht() {
  pinMode(0, OUTPUT);   // sets the digital pin 0 as output
  digitalWrite(0, LOW); // sets the digital pin 0 on
  read_dht_custom(dht, "temp", "humi");
}
#if !defined(ARDUINO_ESP8266_GENERIC)
void read_second_dht() {
  read_dht_custom(dht2, "temp2", "humi2");
}
#endif
void read_am2320() {
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
  tehybug.addTempHumi("temp", am2320.temperatureC, "humi", am2320.humidity);
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
    tehybug.addSensorData(temp, tempC);
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
  const uint8_t pin = 13;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); // on
  delay(100);
  // read the analog in value
  const float sensorValue = analogRead(0);
  tehybug.addSensorData("adc", sensorValue);
  digitalWrite(pin, LOW); // off
}
#endif

void read_sensors() {
  if (tehybug.sensor.bmx) {
    read_bmx280();
  }
  if (tehybug.sensor.bme680) {
    read_bme680();
  }
  if (tehybug.sensor.max44009) {
    read_max44009();
  }
  if (tehybug.sensor.dht) {
    read_dht();
  }
  if (tehybug.sensor.am2320) {
    read_am2320();
  }
  if (tehybug.sensor.ds18b20) {
    read_ds18b20();
  }

#if !defined(ARDUINO_ESP8266_GENERIC)
  if (tehybug.sensor.aht20) {
    read_aht20();
  }
  if (tehybug.sensor.adc) {
    read_adc();
  }
  if (tehybug.sensor.dht_2) {
    read_second_dht();
  }
  if (tehybug.sensor.ds18b20_2) {
    read_second_ds18b20();
  }
#endif
}
// end of sensor
void sendDeviceInfo() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/firststart" ||
          websocketConnection[i] == "/api/info") {
        String info = getInfo();
        webSocket.sendTXT(i, info);
      }
    }
  }
}

void sendSensorData() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/settings") {
        String sensor = getSensor();
        webSocket.sendTXT(i, sensor);
      }
    }
  }
}

void sendConfig() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/settings" ||
          websocketConnection[i] == "/setsensor" ||
          websocketConnection[i] == "/scenarios" ||
          websocketConnection[i] == "/setsystem") {
        String config = tehybug.conf.getConfig();
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
  if (tehybug.serveData.get.active) {
    httpGet();
    delay(1000);
    if (tehybug.device.sleepMode) {
      startDeepSleep(tehybug.serveData.get.frequency);
    }
  }

  if (tehybug.serveData.post.active) {
    httpPost();
    delay(1000);
    if (tehybug.device.sleepMode) {
      startDeepSleep(tehybug.serveData.post.frequency);
    }
  }

  if (tehybug.serveData.mqtt.active) {
    mqttSendData();
    delay(1000);
    if (tehybug.device.sleepMode) {
      startDeepSleep(tehybug.serveData.mqtt.frequency);
    }
  }

  if (tehybug.serveData.ha.active) {
    haSendData();
    delay(1000);
    if (tehybug.device.sleepMode) {
      startDeepSleep(tehybug.serveData.mqtt.frequency);
    }
  }
}

void checkScenario(Scenario &s) {
  float val = 0;
  bool conditionMet = false;
  if (s.active) {
    if (tehybug.sensorData.containsKey(s.data)) {
      val = tehybug.sensorData[s.data];
    }
    conditionMet = (s.condition == "lt" && val < s.value)
                   || (s.condition == "gt" && val > s.value)
                   || (s.condition == "eq" && val == s.value);

    if (conditionMet) {
      D_println("condition met");
      D_println(s.url);
      if (s.type == "post") {
        http::post(http2, getClient(s.url), s.url, s.message);
      } else if (isIoScenario(s.type)) {
        const uint8_t pin = ioScenarioPin(s.type);
        pinMode(pin, OUTPUT);
        digitalWrite(pin, ioScenarioLevel(s.type));
      } else {
        http::get(http2, getClient(s.url), s.url);
      }
    }
    // delay(1000);
  }
}

void serve_scenario() {
  checkScenario(tehybug.scenarios.scenario1);
  checkScenario(tehybug.scenarios.scenario2);
  checkScenario(tehybug.scenarios.scenario3);
}

void configModeCallback(WiFiManager *myWiFiManager) {
  tehybug.pixel.on();
  D_println("Entered wifi config mode");
  D_println(WiFi.softAPIP());
  D_println(myWiFiManager->getConfigPortalSSID());
}

void saveConfigCallback() {
  tehybug.conf.saveConfigCallback();
}

void setupWifi() {
  D_println("Setup WIFI");
  wifiManager.setDebugOutput(true);
  // Set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setMinimumSignalQuality();
  wifiManager.setAPCallback(configModeCallback);
  // Config menu timeout 180 seconds.
  wifiManager.setConfigPortalTimeout(180);
  WiFi.hostname(wifiSsid);
  // set custom ip for portal
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  std::vector<const char *> wm_menu = {"wifi", "exit"};
  wifiManager.setShowInfoUpdate(false);
  wifiManager.setShowInfoErase(false);
  wifiManager.setMenu(wm_menu);
  wifiManager.setCustomHeadElement("<style>button {background-color: #1FA67A;}</style>");
  if (!wifiManager.autoConnect(wifiSsid, wifiPassword)) {
    Serial.println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    // ESP.reset();
    startDeepSleep(9000);
    delay(5000);
  }

  D_println(F("Wifi successfully connected!"));
  tehybug.conf.saveConfig();
  
  if(tehybug.device.configMode)
  {
    WiFi.softAP(wifiSsid, wifiPassword);
  }
  else
  {
    WiFi.softAPdisconnect(true);
  }
  Serial.println("Setup IP : " + WiFi.localIP().toString());
  D_println("Setup " + WiFi.gatewayIP().toString());
  D_println("Setup " + WiFi.subnetMask().toString());

  setupMDSN();
}

void setupMDSN() {
  // generate module IDs
  String escapedMac = WiFi.macAddress();
  escapedMac.replace(":", "");
  escapedMac.toLowerCase();
  // Set up mDNS responder:
    // "end" must be called before "begin" is called a 2nd time
    // see https://github.com/esp8266/Arduino/issues/7213
    MDNS.end();
    MDNS.begin("tehybug");
    D_println(F("mDNS started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "mac", escapedMac.c_str());
    MDNS.addServiceTxt("http", "tcp", "device", "TeHyBug");
    MDNS.addServiceTxt("http", "tcp", "version", version);
    MDNS.addServiceTxt("http", "tcp", "endpoint", "/");
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
    tehybug.sensor.bmx = true;
  } else if (i2cScanner::addressExists("0x76")) {
    tehybug.sensor.bmx = true;
  }
  if (i2cScanner::addressExists("0x5c")) {
    tehybug.sensor.am2320 = true;
  }
  if (i2cScanner::addressExists("0x77")) {
    tehybug.sensor.bme680 = true;
  }
  if (i2cScanner::addressExists("0x4a")) {
    tehybug.sensor.max44009 = true;
  }
  if (i2cScanner::addressExists("0x38")) {
    tehybug.sensor.aht20 = true;
  }
  if (i2cScanner::addressExists("0x50")) {
    tehybug.peripherals.eeprom = true;
  }
  if (i2cScanner::addressExists("0x68")) {
    tehybug.peripherals.ds3231 = true;
  }
}

void setupSensors() {
  if (!tehybug.sensor.dht && !tehybug.sensor.ds18b20) {
    findI2Csensors();
  }
  // sensors
  // bmx280 and bme680 have same address
  if (tehybug.sensor.bmx) {
    // Initialize sensor
    while (!bmx280.begin()) {
      D_println(F("Error: Could not detect sensor"));
      tehybug.sensor.bmx = false;
      break;
    }
    if (tehybug.sensor.bmx) {
      // Print sensor type
      D_print(F("\nSensor type: "));
      switch (bmx280.getChipID()) {
        case CHIP_ID_BMP280:
          D_println(F("BMP280\n"));
          tehybug.sensor.bme680 = false;
          break;
        case CHIP_ID_BME280:
          D_println(F("BME280\n"));
          tehybug.sensor.bme680 = false;
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
      BMX280_Mode_e sampling = BMX280_MODE_NORMAL; // SLEEP, FORCED, NORMAL
      if (tehybug.device.sleepMode)
      {
        sampling = BMX280_MODE_SLEEP;
      }
      bmx280.setSampling(
        sampling,               // SLEEP, FORCED, NORMAL
        BMX280_SAMPLING_X16,    // Temp:  NONE, X1, X2, X4, X8, X16
        BMX280_SAMPLING_X16,    // Press: NONE, X1, X2, X4, X8, X16
        BMX280_SAMPLING_X16,    // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
        BMX280_FILTER_X16,      // OFF, X2, X4, X8, X16
        BMX280_STANDBY_MS_500); // 0_5, 10, 20, 62_5, 125, 250, 500, 1000
    }
  }
  if (tehybug.sensor.bme680) {
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
  if (tehybug.sensor.max44009) {
    D_print("\nStart max44009_setAutomaticMode : ");
    D_println(MAX44009_LIB_VERSION);

    Max44009Lux.setAutomaticMode();
  }
  if (tehybug.sensor.dht) {
    dht.setup(2, DHTesp::DHT22); // Connect DHT sensor to GPIO 2
  }
#if !defined(ARDUINO_ESP8266_GENERIC)
  if (tehybug.sensor.aht20) {
    D_println("AHT20");
    AHT.begin();
  }
  if (tehybug.sensor.dht_2) {
    pinMode(13, INPUT_PULLUP);
    dht2.setup(13, DHTesp::DHT22); // Connect DHT sensor to GPIO 13
  }
  
  if (tehybug.peripherals.eeprom) {
    tehybug.eeprom.setup();
  }
  if (tehybug.peripherals.ds3231) {
    tehybug.time.setup();
  }
#endif
  if (tehybug.sensor.am2320) {
    am2320.setWire(&Wire);
  }
  if (tehybug.sensor.ds18b20) {
    pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  }
}

void turnLedOn() {
  if (tehybug.device.configMode) {
    tehybug.pixel.on();
  } else {
    tehybug.pixel.off();
  }
}
void checkModeButton() {
  delay(100);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (digitalRead(BUTTON_PIN) == LOW) {
    unsigned long pressed = millis();
    bool toggled = false;
    delay(300);
    if (digitalRead(BUTTON_PIN) == LOW) {
      while (digitalRead(BUTTON_PIN) == LOW) {
        if(!toggled)
        {
          toggled = true;
          toggleConfigMode();
          turnLedOn();
        }
        delay(10);
        if((millis() - pressed) >= 20000)
        {
          handleFactoryReset();
        }
      }
    }
  }
  
  turnLedOn();
}

void updateMqttClient() {
  if (tehybug.serveData.mqtt.active || tehybug.serveData.ha.active) {
    MqttClient.setServer(tehybug.serveData.mqtt.server.c_str(), tehybug.serveData.mqtt.port);
  }
}

void firstStart()
{
  // test mode for first start
  if(tehybug.conf.firstStart())
  {
    findI2Csensors();
    if(i2cScanner::devicesFound > 0)
    {
      // show green color when sensors are found on first start
      // required for testing the mini board
      tehybug.pixel.on(0, 255, 0);
      delay(5000);
    }
  }
}

void setup() {
  snprintf(wifiSsid, sizeof(wifiSsid), "TEHYBUG-%X", ESP.getChipId());
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // reduce buffer size and ignore certificate verrification
  espClient_ssl.setBufferSizes(256, 256);
  espClient_ssl.setInsecure();

  // Mounting FileSystem
  D_println(F("Mounting file system..."));
  if (SPIFFS.begin()) {
    D_println(F("File system successfully mounted."));
    tehybug.conf.loadConfig();
  } else {
    D_println(F("Failed to mount FS"));
  }
  
  // should be called after the fs mount
  tehybug.getDeviceKey();
  
  // run a small first start test
  firstStart();
  
  setupWifi();

  // force config when no data serving mode is selected
  if (tehybug.conf.firstStart() || (!tehybug.serveData.get.active && !tehybug.serveData.post.active && !tehybug.serveData.mqtt.active && !tehybug.serveData.ha.active)) {
    tehybug.device.configMode = true;
    D_println("Data serving mode not selected or first start");
  }

  checkModeButton();

  if (tehybug.device.configMode) {
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

  // setup homeassistant
  if (tehybug.device.configMode == false && (tehybug.serveData.mqtt.active || tehybug.serveData.ha.active)) {
    updateMqttClient();
    MqttClient.setKeepAlive(10);
    MqttClient.setCallback(callback);
    MqttClient.setBufferSize(4000);
    if (tehybug.serveData.ha.active)
    {
      ha::setupHandle(tehybug.device);
    }
    Log(F("Setup"), F("MQTT started"));
  }
  
  setupSensors();
  
  // process changes requested by remote control
  if (tehybug.device.configMode == false && tehybug.device.remoteControl.active) {
     const String rcData = http::get(http2, espClient, tehybug.device.remoteControl.url);
     tehybug.handleRemoteControl(rcData);
  }
  
  // setup tickers for non-deep-sleep mode
  if (!tehybug.device.configMode && !tehybug.device.sleepMode) {

    uint8_t ticker_num = 0;
    if (tehybug.serveData.get.active) {
      ticker.add(
        ticker_num, tehybug.serveData.get.frequency * 1000,
      [&](void *) {
        read_sensors();
        yield();
        httpGet();
      },
      nullptr, true);
      ticker_num++;
    }

    if (tehybug.serveData.post.active) {
      ticker.add(
        ticker_num, tehybug.serveData.post.frequency * 1000,
      [&](void *) {
        read_sensors();
        yield();
        httpPost();
      },
      nullptr, true);
      ticker_num++;
    }

    if (tehybug.serveData.mqtt.active) {
      ticker.add(
        ticker_num, tehybug.serveData.mqtt.frequency * 1000,
      [&](void *) {
        read_sensors();
        yield();
        mqttSendData();
      },
      nullptr, true);
      ticker_num++;
    }

    if (tehybug.serveData.ha.active) {
      ticker.add(
        ticker_num, tehybug.serveData.mqtt.frequency * 1000,
      [&](void *) {
        read_sensors();
        yield();
        haSendData();
      },
      nullptr, true);
      ticker_num++;
    }

  }
}

void loop() {
  // config mode
  if (tehybug.device.configMode) {
    MDNS.update();
    server.handleClient();
    yield();
    webSocket.loop();
  }
  // deep sleep mode
  else if (tehybug.device.sleepMode) {
    read_sensors();
    yield();
    serve_scenario();
    yield();
    serve_data();
  }
  
  if (tehybug.tickerStop && tehybug.device.configMode)
  {
    tehybug.tickerStop = false;
    ticker.disableAll();
    tehybug.pixel.on();
  }
  if (tehybug.tickerStart && !tehybug.device.configMode)
  {
    tehybug.tickerStart = false;
    ticker.enableAll();
    tehybug.pixel.off();
  }
  // update ticker for the non-deep-sleep mode
  ticker.update();
}
