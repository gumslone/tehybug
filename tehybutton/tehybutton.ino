#include "UUID.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <PubSubClient.h> // Attention in the lib the #define MQTT_MAX_PACKET_SIZE must be increased to 4000!
#include <TickerScheduler.h>
#include <WebSocketsServer.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include "Button2.h"


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
#include "tehybug.h"

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

#if defined(ARDUINO_ESP8266_GENERIC)
#define  PIXEL_ACTIVE 0
#define  SIGNAL_LED_PIN 10
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

//Button
// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON_PIN   14
/////////////////////////////////////////////////////////////////
Button2 button;

String button_state = "none";

#define MODE_PIN 0

// dns
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;
// HTTP Config
HTTPClient http1;
HTTPClient http2;

DataServ serveData{};

bool skipButtonActions = false;
unsigned long sleepAfter = 0;
bool shouldServeData = true;

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

UUID uuid;

DynamicJsonDocument sensorData(1024);

Scenarios scenarios{};

TickerScheduler ticker(5);

void saveConfigCallback() {
  shouldSaveConfig = true;
}

/////////////////////////////////////////////////////////////////////
void saveConfig() {
  // save the custom parameters to FS
  if (shouldSaveConfig) {
    DynamicJsonDocument json(4096);

    json["mqttActive"] = serveData.mqtt.active;
    json["mqttRetained"] = serveData.mqtt.retained;
    json["mqttUser"] = serveData.mqtt.user;
    json["mqttPassword"] = serveData.mqtt.password;
    json["mqttServer"] = serveData.mqtt.server;
    json["mqttMasterTopic"] = serveData.mqtt.topic;
    json["mqttMessage"] = serveData.mqtt.message;
    json["mqttPort"] = serveData.mqtt.port;
    json["mqttFrequency"] = serveData.mqtt.frequency;

    json["httpGetURL"] = serveData.get.url;
    json["httpGetActive"] = serveData.get.active;
    json["httpGetFrequency"] = serveData.get.frequency;

    json["httpPostURL"] = serveData.post.url;
    json["httpPostActive"] = serveData.post.active;
    json["httpPostFrequency"] = serveData.post.frequency;
    json["httpPostJson"] = serveData.post.message;

    json["configModeActive"] = configModeActive;
    json["skipButtonActions"] = skipButtonActions;

    json["key"] = sensorData["key"];

    File configFile = SPIFFS.open("/config.json", "w");
    serializeJson(json, configFile);
    configFile.close();
    Log("SaveConfig", "Saved");
    // end save
  }
}

void setConfigParameters(JsonObject &json) {
  D_println("Config:");
  if (DEBUG)
  {
    for (JsonPair kv : json) {
      D_print(kv.key().c_str());
      D_print(" = ");
      D_println(kv.value().as<String>());
    }
  }
  D_println();
  if (json.containsKey("mqttActive")) {
    serveData.mqtt.active = json["mqttActive"].as<bool>();
  }
  if (json.containsKey("mqttRetained")) {
    serveData.mqtt.retained = json["mqttRetained"].as<bool>();
  }
  if (json.containsKey("mqttUser")) {
    serveData.mqtt.user = json["mqttUser"].as<String>();
  }
  if (json.containsKey("mqttPassword")) {
    serveData.mqtt.password = json["mqttPassword"].as<String>();
  }
  if (json.containsKey("mqttServer")) {
    serveData.mqtt.server = json["mqttServer"].as<String>();
  }
  if (json.containsKey("mqttMasterTopic")) {
    serveData.mqtt.topic = json["mqttMasterTopic"].as<String>();
  }
  if (json.containsKey("mqttMessage")) {
    serveData.mqtt.message = json["mqttMessage"].as<String>();
  }
  if (json.containsKey("mqttPort")) {
    serveData.mqtt.port = json["mqttPort"].as<int>();
  }

  // http
  if (json.containsKey("httpGetURL")) {
    serveData.get.url = json["httpGetURL"].as<String>();
  }
  if (json.containsKey("httpGetActive")) {
    serveData.get.active = json["httpGetActive"].as<bool>();
  }

  if (json.containsKey("httpPostURL")) {
    serveData.post.url = json["httpPostURL"].as<String>();
  }
  if (json.containsKey("httpPostActive")) {
    serveData.post.active = json["httpPostActive"].as<bool>();
  }
  if (json.containsKey("httpPostJson")) {
    serveData.post.message = json["httpPostJson"].as<String>();
  }

  if (json.containsKey("configModeActive")) {
    configModeActive = json["configModeActive"].as<bool>();
  }

  if (json.containsKey("skipButtonActions"))
  {
    skipButtonActions = json["skipButtonActions"].as<bool>();
  }

}

void loadConfig() {
  if (SPIFFS.exists("/config.json")) {
    // file exists, reading and loading
    File configFile = SPIFFS.open("/config.json", "r");

    if (configFile) {
      Log("LoadConfig", "opened config file");

      DynamicJsonDocument json(4096);
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
    saveConfigCallback();
    saveConfig();
  }
}

void setConfig(JsonObject &json) {
  setConfigParameters(json);
  saveConfigCallback();
  saveConfig();

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
    setConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
    // delay(500);
    // ESP.restart();
  } else {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void handleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getConfig());
}

void handleGetInfo() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getInfo());
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
                     getConfig().c_str());
    } else if (channel.equals("setConfig")) {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      setConfig(object);
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
            setConfig(object);
            // delay(500);
            // ESP.restart();
          }
        }
        break;
      }
  }
}
#pragma endregion

String getConfig() {
  File configFile = SPIFFS.open("/config.json", "r");

  if (configFile) {
    const size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);
    DynamicJsonDocument root(4096);

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
  root["sleepModeActive"] = sleepModeActive;
  root["deepSleepMax"] = (int)(ESP.deepSleepMax() / 1000000);
  root["key"] = sensorData["key"];

  String json;
  serializeJson(root, json);

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
                    ("tehybutton_" + GetChipID()).c_str(), serveData.mqtt.user.c_str(),
                    serveData.mqtt.password.c_str(), "state", 0, true, "diconnected");
    } else {
      Log(F("MqttReconnect"),
          F("MQTT connect to server without User and Password"));
      connected = client.connect(("tehybutton_" + GetChipID()).c_str(), "state", 0,
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
  configModeActive = !configModeActive;
  if (configModeActive) {
    D_println(F("Config mode activated"));
  } else {
    D_println(F("Config mode deactivated"));
  }
  saveConfigCallback();
  saveConfig();
  yield();
  if (configModeActive == false) {
    // ESP.restart();
  }
}

void startDeepSleep(int freq) {
  D_println("Going to deep sleep...");
  delay(freq * 1000);
  ESP.restart();
  //ESP.deepSleep(freq * 1000000);
  yield();
}
// HTTP REQUESTS
/////////////////////////////////////////////////////////////////////
String replace_placeholders(String text)
{
  JsonObject root = sensorData.as<JsonObject>();
  for (JsonPair keyValue : root) {
    String k = keyValue.key().c_str();
    String v = keyValue.value();
    text.replace("%" + k + "%", v);
  }
  text.replace("%state%", button_state);

  return text;
}
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
  }
  if (httpCode > 0) {                  // Check the returning code
    String payload = http.getString(); // Get the response
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
  }
  if (httpCode > 0) {                  // Check the returning code
    String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  http.end(); // Close connection
}
void httpGet() {
  http_get_custom(http1, serveData.get.url);
}

void sendDeviceInfo(bool force) {
  if (force) {
    OldInfo = "";
  }
  String Info;
  if ((webSocket.connectedClients() > 0)) {
    Info = getInfo();
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

void sendConfig() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/settings" ||
          websocketConnection[i] == "/setsystem") {
        String config = getConfig();
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

  doSetPixelColor(pixel.Color(  0,   255,   0));         //  Set pixel's color (in RAM)


  if (serveData.get.active) {
    httpGet();
  }

  if (serveData.post.active) {
    httpPost();
  }

  if (serveData.mqtt.active) {
    mqttSendData();
  }
}

void led_on() {
  D_println("Led on");

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

}
// pixel
void doSetPixelColor(uint32_t color)
{
  pixel.setPixelColor(0, color);         //  Set pixel's color (in RAM)
  pixel.setBrightness(33);
  pixel.show();                          //  Update strip to match
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
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setMinimumSignalQuality();
  wifiManager.setAPCallback(configModeCallback);

  // Config menu timeout 180 seconds.
  wifiManager.setConfigPortalTimeout(180);

  WiFi.hostname(identifier);
  // set custom ip for portal
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  std::vector<const char *> wm_menu  = {"wifi", "exit"};
  wifiManager.setShowInfoUpdate(false);
  wifiManager.setShowInfoErase(false);
  wifiManager.setMenu(wm_menu);
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setCustomHeadElement("<style>button {background-color: #1FA67A;}</style>");
  if (!wifiManager.autoConnect(identifier, "TeHyBug123")) {
    Serial.println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    startDeepSleep(9000);
    delay(5000);
  }

  D_println(F("Wifi connected...yeey :)"));

  if (shouldSaveConfig) {
    saveConfig();
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
  strcpy_P(cmDNS, PSTR("tehybutton"));
  // Set up mDNS responder:
  if (strlen(cmDNS) > 0) {
    // "end" must be called before "begin" is called a 2nd time
    // see https://github.com/esp8266/Arduino/issues/7213
    MDNS.end();
    MDNS.begin(cmDNS);
    D_println(cmDNS);
    D_println(F("mDNS started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("tehybutton", "tcp", 80);
    MDNS.addServiceTxt("tehybutton", "tcp", "mac", escapedMac.c_str());
  }
}

void turnLedOn()
{
  if (configModeActive) {
    led_on();
  } else {
    led_off();
  }
}
void setupMode() {
  pinMode(MODE_PIN, INPUT_PULLUP);
  if (digitalRead(MODE_PIN) == LOW) {
    delay(300);
    if (digitalRead(MODE_PIN) == LOW) {
      toggleConfigMode();
      turnLedOn();
      while (digitalRead(MODE_PIN) == LOW) {
        delay(10);
      }
    }
  }
  turnLedOn();
}

void setServeData()
{
  if (skipButtonActions == false)
  {
    shouldServeData = true;
    sleepAfter = millis() + 1500;
  }
}

void pressed(Button2& btn) {
  Serial.println("pressed");
  button_state = "pressed";
  setServeData();
}
void released(Button2& btn) {
  Serial.print("released: ");
  Serial.println(btn.wasPressedFor());
  button_state = btn.wasPressedFor();
  setServeData();
}
void changed(Button2& btn) {
  Serial.println("changed");
}
void click(Button2& btn) {
  Serial.println("click\n");
  button_state = "click";
  setServeData();
}
void longClickDetected(Button2& btn) {
  Serial.println("long click detected\n");
  sleepAfter = millis() + 5000;

}
void longClick(Button2& btn) {
  Serial.println("long click\n");
  button_state = "long click";
  setServeData();
}
void doubleClick(Button2& btn) {
  Serial.println("double click\n");
  button_state = "double click";
  setServeData();
}
void tripleClick(Button2& btn) {
  Serial.println("triple click\n");
  button_state = "triple click";
  //setServeData();
}

// HTTP REQUESTS
void latchOff()
{
  digitalWrite(2, LOW);
}

void setup() {
  uuid.seed(ESP.getChipId());
  uuid.generate();
  sensorData["key"] = String(uuid.toCharArray());

  snprintf(identifier, sizeof(identifier), "TEHYBUTTON-%X", ESP.getChipId());

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);

  Serial.println("\n\nButton Demo");

  button.begin(BUTTON_PIN);
  button.setLongClickTime(1000);
  button.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time: " + String(button.getDoubleClickTime()) + "ms");

  //button.setChangedHandler(changed);
  button.setPressedHandler(pressed);
  button.setReleasedHandler(released);

  // button.setTapHandler(tap);
  button.setClickHandler(click);
  button.setLongClickDetectedHandler(longClickDetected);
  button.setLongClickHandler(longClick);

  button.setDoubleClickHandler(doubleClick);
  button.setTripleClickHandler(tripleClick);

  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(2, HIGH);

  pixel.begin();
  pixel.show();
  doSetPixelColor(pixel.Color(  255,   0,   0));

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  D_println(F("key: "));
  D_println(sensorData["key"].as<String>());
  // Mounting FileSystem
  D_println(F("Mounting file system..."));
  if (SPIFFS.begin()) {
    D_println(F("Mounted file system."));
    loadConfig();
  } else {
    D_println(F("Failed to mount FS"));
  }

  // force config when no mode selected
  if (serveData.get.active == false && serveData.post.active == false &&
      serveData.mqtt.active == false) {
    configModeActive = true;
    D_println("Data serving mode not selected");
  }

  setupWifi();
  setupMode();
  doSetPixelColor(pixel.Color(  0,   255,   0));         //  Set pixel's color (in RAM)

  if (configModeActive) {
    D_println(F("Starting config mode"));
    httpUpdater.setup(&server);
    server.on(F("/api/info"), HTTP_GET, handleGetInfo);
    server.on(F("/api/config"), HTTP_POST, handleSetConfig);
    server.on(F("/api/config"), HTTP_GET, handleGetConfig);
    server.on(F("/"), HTTP_GET, handleGetMainPage);
    server.onNotFound(handleNotFound);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Log(F("Setup"), F("Webserver started"));
  } else {
    D_println(F("Starting live mode"));
  }

  if (configModeActive == false && serveData.mqtt.active) {
    client.setServer(serveData.mqtt.server.c_str(), serveData.mqtt.port);
    client.setCallback(callback);
    client.setBufferSize(4000);
    Log(F("Setup"), F("MQTT started"));
  }
}

void loop() {
  // config mode
  if (configModeActive) {
    MDNS.update();
    server.handleClient();
    yield();
    webSocket.loop();
    // update ticker for the non-deep-sleep mode
    // ticker.update();
  }
  // deep sleep mode
  else if (sleepModeActive) {
    // Get current button state.
    button.loop();
    yield();

    if (shouldServeData == true)
    {
      shouldServeData = false;
      serve_data();
      yield();
      sleepAfter = millis() + 1000;
    }

    if (digitalRead(BUTTON_PIN) == LOW) {
      sleepAfter = millis() + 1500;
    }

    if (shouldServeData == false && sleepAfter < millis())
    {
      latchOff();
    }
  }

}
