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
#include "common_functions.h"
#include "http_request.h"
#include "Webinterface.h"
#include "tehybutton.h"

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

TeHyButton tehybutton{};

WiFiClient espClient;
PubSubClient client(espClient);
WiFiManager wifiManager;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266HTTPUpdateServer httpUpdater;

// Websoket Vars
String websocketConnection[10];

TickerScheduler ticker(5);

void saveConfigCallback() {
  tehybutton.conf.saveConfigCallback();
}

/////////////////////////////////////////////////////////////////////

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
    tehybutton.conf.setConfig(object);
    server.send(200, "application/json", "{\"response\":\"OK\"}");
    // delay(500);
    // ESP.restart();
  } else {
    server.send(406, "application/json", "{\"response\":\"Not Acceptable\"}");
  }
}

void handleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", tehybutton.conf.getConfig());
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
    channel.replace(tehybutton.serveData.mqtt.topic, "");

    DynamicJsonDocument json(512);
    deserializeJson(json, payload);

    Log("MQTT_callback", "Incoming Json length to topic " + String(topic) +
        ": " + String(measureJson(json)));
    if (channel.equals("getInfo")) {
      client.publish((tehybutton.serveData.mqtt.topic + "info").c_str(),
                     getInfo().c_str());
    } else if (channel.equals("getConfig")) {
      client.publish((tehybutton.serveData.mqtt.topic + "config").c_str(),
                     tehybutton.conf.getConfig().c_str());
    } else if (channel.equals("setConfig")) {
      // extract the data
      JsonObject object = json.as<JsonObject>();
      tehybutton.conf.setConfig(object);
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
        sendDeviceInfo();
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
            tehybutton.conf.setConfig(object);
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
  root["sleepMode"] = tehybutton.device.sleepMode;
  root["deepSleepMax"] = (int)(ESP.deepSleepMax() / 1000000);
  root["key"] = tehybutton.device.key;

  String json;
  serializeJson(root, json);

  return json;
}

/////////////////////////////////////////////////////////////////////
void mqttSendData() {
  if (client.connected()) {
    String payload = replace_placeholders(tehybutton.serveData.mqtt.message);
    payload.toCharArray(tehybutton.serveData.data, (payload.length() + 1));
    client.publish((tehybutton.serveData.mqtt.topic).c_str(), tehybutton.serveData.data,
                   tehybutton.serveData.mqtt.retained);
    Log(F("MqttSendData"), payload);
  } else
    mqttReconnect();
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected() &&
         tehybutton.serveData.mqtt.retryCounter < tehybutton.serveData.mqtt.maxRetries) {
    bool connected = false;
    if (tehybutton.serveData.mqtt.user != NULL && tehybutton.serveData.mqtt.user.length() > 0 &&
        tehybutton.serveData.mqtt.password != NULL &&
        tehybutton.serveData.mqtt.password.length() > 0) {
      Log(F("MqttReconnect"),
          F("MQTT connect to server with User and Password"));
      connected = client.connect(
                    ("tehybutton_" + GetChipID()).c_str(), tehybutton.serveData.mqtt.user.c_str(),
                    tehybutton.serveData.mqtt.password.c_str(), "state", 0, true, "diconnected");
    } else {
      Log(F("MqttReconnect"),
          F("MQTT connect to server without User and Password"));
      connected = client.connect(("tehybutton_" + GetChipID()).c_str(), "state", 0,
                                 true, "disconnected");
    }

    // Attempt to connect
    if (connected) {
      Log(F("MqttReconnect"), F("MQTT connected!"));
      tehybutton.serveData.mqtt.retryCounter = 0;
      // ... and publish
      mqttSendData();
    } else {
      Log(F("MqttReconnect"), F("MQTT not connected!"));
      Log(F("MqttReconnect"), F("Wait 5 seconds before retrying...."));
      tehybutton.serveData.mqtt.retryCounter++;
    }
  }

  if (tehybutton.serveData.mqtt.retryCounter >= tehybutton.serveData.mqtt.maxRetries) {
    Log(F("MqttReconnect"),
        F("No connection to MQTT-Server, MQTT temporarily deactivated!"));
  }
}
/////////////////////////////////////////////////////////////////////

// BUTTON
//  Attach callback.
void toggleConfigMode() {

  D_println(F("Config mode changed"));
  tehybutton.device.configMode = !tehybutton.device.configMode;
  if (tehybutton.device.configMode) {
    D_println(F("Config mode activated"));
  } else {
    D_println(F("Config mode deactivated"));
  }
  tehybutton.conf.saveConfig(true);
  yield();
  if (tehybutton.device.configMode == false) {
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
  text.replace("%state%", button_state);
  return text;
}

// HTTP REQUESTS
void httpPost() {
  http::post(http1, espClient, tehybutton.serveData.post.url, tehybutton.serveData.post.message);
}
void httpGet() {
  http::get(http1, espClient, tehybutton.serveData.get.url);
}

void sendDeviceInfo() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/main" ||
          websocketConnection[i] == "/firststart" ||
          websocketConnection[i] == "/api/info") {
        String Info = getInfo();
        webSocket.sendTXT(i, Info);
      }
    }
  }
}

void sendConfig() {
  if (webSocket.connectedClients() > 0) {
    for (uint8_t i = 0;
         i < sizeof websocketConnection / sizeof websocketConnection[0]; i++) {
      if (websocketConnection[i] == "/settings" ||
          websocketConnection[i] == "/setsystem") {
        String cfg = tehybutton.conf.getConfig();
        webSocket.sendTXT(i, cfg);
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

  if (tehybutton.serveData.get.active) {
    httpGet();
  }

  if (tehybutton.serveData.post.active) {
    httpPost();
  }

  if (tehybutton.serveData.mqtt.active) {
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
    D_println(F("Setup: Wifi failed to connect and hit timeout"));
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    //ESP.reset();
    startDeepSleep(9000);
    delay(5000);
  }

  D_println(F("Wifi connected...yeey :)"));

  tehybutton.conf.saveConfig();

  D_println("Setup: Local IP");
  D_println("Setup " + WiFi.localIP().toString());
  D_println("Setup " + WiFi.gatewayIP().toString());
  D_println("Setup " + WiFi.subnetMask().toString());

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
  if (tehybutton.device.configMode) {
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

void pressed(Button2& btn) {
  D_println("pressed");
  button_state = "pressed";
}
void released(Button2& btn) {
  D_print("released: ");
  D_println(btn.wasPressedFor());
  button_state = btn.wasPressedFor();
}
void changed(Button2& btn) {
  D_println("changed");
}
void click(Button2& btn) {
  D_println("click\n");
  button_state = "click";
  tehybutton.setServeData();
}
void longClickDetected(Button2& btn) {
  D_println("long click detected\n");
}
void longClick(Button2& btn) {
  D_println("long click\n");
  button_state = "long click";
  tehybutton.setServeData();
}
void doubleClick(Button2& btn) {
  D_println("double click\n");
  button_state = "double click";
  tehybutton.setServeData();
}
void tripleClick(Button2& btn) {
  D_println("triple click\n");
  button_state = "triple click";
  //setServeData();
}

// HTTP REQUESTS
void latchOff()
{
  digitalWrite(2, LOW);
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  snprintf(identifier, sizeof(identifier), "TEHYBUTTON-%X", ESP.getChipId());

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);

  button.begin(BUTTON_PIN);
  button.setLongClickTime(1000);
  button.setDoubleClickTime(400);

  D_println(" Longpress Time: " + String(button.getLongClickTime()) + "ms");
  D_println(" DoubleClick Time: " + String(button.getDoubleClickTime()) + "ms");

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

  // Mounting FileSystem
  D_println(F("Mounting file system..."));
  if (SPIFFS.begin()) {
    D_println(F("Mounted file system."));
    tehybutton.conf.loadConfig();
  } else {
    D_println(F("Failed to mount FS"));
  }

  // force config when no mode selected
  if (tehybutton.serveData.get.active == false && tehybutton.serveData.post.active == false &&
      tehybutton.serveData.mqtt.active == false) {
    tehybutton.device.configMode = true;
    D_println("Data serving mode not selected");
  }

  setupWifi();
  setupMode();
  doSetPixelColor(pixel.Color(  0,   255,   0));         //  Set pixel's color (in RAM)

  if (tehybutton.device.configMode) {
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

  if (tehybutton.device.configMode == false && tehybutton.serveData.mqtt.active) {
    client.setServer(tehybutton.serveData.mqtt.server.c_str(), tehybutton.serveData.mqtt.port);
    client.setCallback(callback);
    client.setBufferSize(4000);
    Log(F("Setup"), F("MQTT started"));
  }
}

void loop() {
  // config mode
  if (tehybutton.device.configMode) {
    MDNS.update();
    server.handleClient();
    yield();
    webSocket.loop();
    // update ticker for the non-deep-sleep mode
    // ticker.update();
  }
  // deep sleep mode
  else if (tehybutton.device.sleepMode) {
    // Get current button state.
    button.loop();
    yield();

    if (tehybutton.shouldServeData == true)
    {
      tehybutton.shouldServeData = false;
      serve_data();
      yield();
      tehybutton.updateSleepAfter(1000);
    }

    if (digitalRead(BUTTON_PIN) == LOW) {
      tehybutton.updateSleepAfter(1500);
    }
    else if (tehybutton.shouldSleep())
    {
      latchOff();
    }
  }

}
