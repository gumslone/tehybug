#include "AHT20.h"
#include "Button2.h"
#include "Calibration.h"
#include "Config.h"
#include "Helper.h"
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "Webinterface.h"
#include "images.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ErriezBMX280.h>
#include <PubSubClient.h>
#include <TickerScheduler.h>
#include <WiFiManager.h>
#include <Wire.h>

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.50" b/w// 200x200
#include <GxDEPG0150BN2/GxDEPG0150BN2.h> // combbined GxDEPG0150BN and GxGDEH0154D67 1.50" b/w// 200x200

// #include <GxGDEH0154D67/GxGDEH0154D67.h> // 1.54" b/w// 200x200 works
// #include <GxDEPG0154BxS800FxX_BW/GxDEPG0154BxS800FxX_BW.h>   // 1.54" b/w//
// 152x152

// FreeFonts from Adafruit_GFX
// small display
#include <Fonts/FreeSans7pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

// large display
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

// BUSY -> DISSCONNECTED, RST -> DISSCONNECTED, DC -> GPIO0, CS -> GPIO15(CS),
// CLK -> GPIO14(SCLK), SDI -> GPIO13(MOSI), GND -> GND, 3.3V -> 3.3V
GxIO_Class io(SPI, /*CS*/ 16, /*DC*/ 15, -1);
GxEPD_Class display(io, -1, -1); // no RST, no BUSY

// dns
const uint8_t DNS_PORT = 53;
const IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;

// Button
//  Digital IO pin connected to the button. This will be driven with a
//  pull-up resistor so the switch pulls the pin to ground momentarily.
//  On a high -> low transition the button press logic will execute.
#define BUTTON_LEFT 5
#define BUTTON_RIGHT 4
#define BUTTON_MODE 0
/////////////////////////////////////////////////////////////////
Button2 button_left;
Button2 button_right;
Button2 button_mode;

#define PIXEL_PIN 12  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 2 // Number of NeoPixels

SCD4x mySensor;
bool scd4x_sensor = false;

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA 1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);
bool bmx_sensor = false; // in the setup the i2c scanner searches for the sensor

AHT20 AHT;
bool aht20_sensor =
    false; // in the setup the i2c scanner searches for the sensor

bool remote_sensor = false;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

String key, temp, temp_imp, humi, dew, qfe, qfe_imp, qnh, alt, air, aiq, lux,
    uv, adc, tvoc, co2;

String i2c_addresses = "";

TickerScheduler ticker(5);

bool epaper = true;
byte update_epaper_display_counter = 100;
bool update_epaper_display = true;

// wifi and mqtt and http
const char *update_path = "/update";
const char *update_username = "TeHyBug";
const char *update_password = "FreshAirMakesSense";

uint8_t mqttRetryCounter = 0;

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient mqttClient;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
// HTTP Config
HTTPClient http1;
HTTPClient http2;

WiFiManagerParameter custom_mqtt_server("server", "MQTT server",
                                        Config::mqtt_server,
                                        sizeof(Config::mqtt_server));
WiFiManagerParameter custom_mqtt_user("user", "MQTT username", Config::username,
                                      sizeof(Config::username));
WiFiManagerParameter custom_mqtt_pass("pass", "MQTT password", Config::password,
                                      sizeof(Config::password));

uint32_t lastMqttConnectionAttempt = 0;
const uint16_t mqttConnectionInterval =
    60000; // 1 minute = 60 seconds = 60000 milliseconds

uint32_t statusPublishPreviousMillis = 0;
const uint16_t statusPublishInterval = 30000; // 30 seconds = 30000 milliseconds

char identifier[24];
#define FIRMWARE_PREFIX "tehybug-co2-sensor"
#define AVAILABILITY_ONLINE "online"
#define AVAILABILITY_OFFLINE "offline"
char MQTT_TOPIC_AVAILABILITY[128];
char MQTT_TOPIC_STATE[128];
char MQTT_TOPIC_COMMAND[128];

char MQTT_TOPIC_AUTOCONF_T_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_H_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_P_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_SENSOR[128];

bool shouldSaveConfig = false;

void saveConfigCallback() { shouldSaveConfig = true; }

String getSensor() {

  DynamicJsonDocument root(1024);
  root["co2"] = co2;
  
  root["temperature"] = Config::imperial_temp ? temp_imp : temp;

  root["humidity"] = humi;
  root["pressure"] = qfe;
  root["altitude"] = alt;
  root["ip"] = WiFi.localIP().toString();
  String json;
  serializeJson(root, json);
  return json;
}

void handleMainPage() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getSensor());
}

void handleSaveConfig() {
  Config::imperial_temp = (server.hasArg("imperial_temp") && server.arg("imperial_temp")) ? true : false;
  Config::imperial_qfe = (server.hasArg("imperial_qfe") && server.arg("imperial_qfe")) ? true : false;
  Config::led_brightness = (server.hasArg("led_brightness") && server.arg("led_brightness") && server.arg("led_brightness").toInt() >=0 
                            && server.arg("led_brightness").toInt() <= 255) ? server.arg("led_brightness").toInt() : 200;
  
  Config::save();
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "Configuration saved successfully!");
}

void handleGetConfig() {
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", configPage);
}

void setupHandle() {

  Serial.println("\n");
  Serial.println("Hello from esp8266-tehybug-co2-sensor");
  Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
  Serial.printf("Boot Version: %u\n", ESP.getBootVersion());
  Serial.printf("Boot Mode: %u\n", ESP.getBootMode());
  Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Reset reason: %s\n", ESP.getResetReason().c_str());

  delay(3000);

  snprintf(identifier, sizeof(identifier), "TEHYBUG-CO2-%X", ESP.getChipId());
  snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", FIRMWARE_PREFIX,
           identifier);
  snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", FIRMWARE_PREFIX,
           identifier);

  snprintf(MQTT_TOPIC_AUTOCONF_SENSOR, 127,
           "homeassistant/sensor/%s/%s_co2/config", FIRMWARE_PREFIX,
           identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127,
           "homeassistant/sensor/%s/%s_wifi/config", FIRMWARE_PREFIX,
           identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_T_SENSOR, 127,
           "homeassistant/sensor/%s/%s_t/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_H_SENSOR, 127,
           "homeassistant/sensor/%s/%s_h/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_P_SENSOR, 127,
           "homeassistant/sensor/%s/%s_p/config", FIRMWARE_PREFIX, identifier);
  WiFi.hostname(identifier);

  setupWifi();
  setupOTA();
  mqttClient.setServer(Config::mqtt_server, 1883);
  mqttClient.setKeepAlive(10);
  mqttClient.setBufferSize(2048);
  mqttClient.setCallback(mqttCallback);

  Serial.printf("Hostname: %s\n", identifier);
  Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

  mqttReconnect();
  setupMDSN();

  httpUpdater.setup(&server, update_path, update_username, update_password);
  server.on(F("/"), HTTP_GET, handleMainPage);
  server.on(F("/config"), HTTP_POST, handleSaveConfig);
  server.on(F("/config"), HTTP_GET, handleGetConfig);
  server.begin();
}

void setupOTA() {
  ArduinoOTA.onStart([]() { Serial.println("Start"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    switch(error) {
      case OTA_AUTH_ERROR:
        Serial.println("Auth Failed");
        break;
      case OTA_BEGIN_ERROR:
        Serial.println("Begin Failed");
        break;
      case OTA_CONNECT_ERROR:
        Serial.println("Connect Failed");
        break;
      case OTA_RECEIVE_ERROR:
        Serial.println("Receive Failed");
        break;
      case OTA_END_ERROR:
        Serial.println("End Failed");
        break;
    }
  });

  ArduinoOTA.setHostname(identifier);

  // This is less of a security measure and more a accidential flash prevention
  ArduinoOTA.setPassword(identifier);
  ArduinoOTA.begin();
}

void setupWifi() {
  wifiManager.setDebugOutput(false);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

  WiFi.hostname(identifier);

  std::vector<const char *> wm_menu  = {"wifi", "exit"};
  wifiManager.setShowInfoUpdate(false);
  wifiManager.setShowInfoErase(false);
  wifiManager.setMenu(wm_menu);
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.setCustomHeadElement("<style>button {background-color: #1FA67A;}</style>");
  
  wifiManager.autoConnect(identifier);
  mqttClient.setClient(wifiClient);

  strcpy(Config::mqtt_server, custom_mqtt_server.getValue());
  strcpy(Config::username, custom_mqtt_user.getValue());
  strcpy(Config::password, custom_mqtt_pass.getValue());

  if (shouldSaveConfig) {
    Config::save();
  } else {
    // For some reason, the read values get overwritten in this function
    // To combat this, we just reload the config
    // This is most likely a logic error which could be fixed otherwise
    Config::load();
  }
}
void setupMDSN() {
  // generate module IDs
  escapedMac = WiFi.macAddress();
  escapedMac.replace(":", "");
  escapedMac.toLowerCase();
  strcpy_P(cmDNS, PSTR("tehybug-"));
  sprintf(cmDNS + 5, "%*s", 6, escapedMac.c_str() + 6);
  // Set up mDNS responder:
  strcpy_P(cmDNS, PSTR("tehybug"));
  if (strlen(cmDNS) > 0) {
    // "end" must be called before "begin" is called a 2nd time
    // see https://github.com/esp8266/Arduino/issues/7213
    MDNS.end();
    MDNS.begin(cmDNS);
    Serial.println(cmDNS);
    Serial.println(F("mDNS started"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "mac", escapedMac.c_str());
  }
}
// BUTTON
//  Attach callback.
void pressed(Button2 &btn) { Serial.println("pressed"); }
void released(Button2 &btn) {
  Serial.print("released: ");
  Serial.println(btn.wasPressedFor());
}
void changed(Button2 &btn) { Serial.println("changed"); }
void click(Button2 &btn) {
  Serial.println("click\n");
  Serial.println(btn.getPin());
  Serial.println("\n");
}
void longClickDetected(Button2 &btn) {
  Serial.println("long click detected\n");
}
void longClick(Button2 &btn) {
  Serial.println("long click\n");
  if (btn.getPin() == BUTTON_MODE) {
    Serial.println("reset wifi\n");
    resetWifiSettingsAndReboot();
  }
  if (btn.getPin() == BUTTON_RIGHT) {
    Serial.println("calibrate sensor\n");
    calibrate_sensor();
  }
}

void doubleClick(Button2 &btn) {
  Serial.println("double click\n");
  Serial.println(btn.getPin());
  Serial.println("\n");
}

void tripleClick(Button2 &btn) { Serial.println("triple click\n"); }

void setupButtons() {
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_MODE, INPUT_PULLUP);

  Serial.println("\n\nButton Demo");

  button_left.begin(BUTTON_LEFT);
  button_left.setLongClickTime(1000);
  button_left.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_left.getLongClickTime()) +
                 "ms");
  Serial.println(
      " DoubleClick Time: " + String(button_left.getDoubleClickTime()) + "ms");

  // button_left.setChangedHandler(changed);
  // button_left.setPressedHandler(pressed);
  // button_left.setReleasedHandler(released);

  // button_left.setTapHandler(tap);
  button_left.setClickHandler(click);
  // button_left.setLongClickDetectedHandler(longClickDetected);
  button_left.setLongClickHandler(longClick);

  button_left.setDoubleClickHandler(doubleClick);
  // button_left.setTripleClickHandler(tripleClick);

  button_right.begin(BUTTON_RIGHT);
  button_right.setLongClickTime(1000);
  button_right.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_right.getLongClickTime()) +
                 "ms");
  Serial.println(
      " DoubleClick Time: " + String(button_right.getDoubleClickTime()) + "ms");

  // button_right.setChangedHandler(changed);
  // button_right.setPressedHandler(pressed);
  // button_right.setReleasedHandler(released);

  // button_right.setTapHandler(tap);
  button_right.setClickHandler(click);
  // button_right.setLongClickDetectedHandler(longClickDetected);
  button_right.setLongClickHandler(longClick);

  button_right.setDoubleClickHandler(doubleClick);
  // button_right.setTripleClickHandler(tripleClick);

  button_mode.begin(BUTTON_MODE);
  button_mode.setLongClickTime(15000);
  button_mode.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_mode.getLongClickTime()) +
                 "ms");
  Serial.println(
      " DoubleClick Time: " + String(button_mode.getDoubleClickTime()) + "ms");

  // button_mode.setChangedHandler(changed);
  // button_mode.setPressedHandler(pressed);
  // button_mode.setReleasedHandler(released);

  // button_mode.setTapHandler(tap);
  button_mode.setClickHandler(click);
  // button_mode.setLongClickDetectedHandler(longClickDetected);
  button_mode.setLongClickHandler(longClick);

  button_mode.setDoubleClickHandler(doubleClick);
  // button_mode.setTripleClickHandler(tripleClick);
}

void resetWifiSettingsAndReboot() {
  wifiManager.resetSettings();
  delay(500);
  ESP.eraseConfig();
  delay(3000);
  ESP.restart();
}

// HTTP REQUESTS
void http_post_custom(HTTPClient &http, String url, String post_json) {
  Serial.println("HTTP POST: ");
  Serial.println(url);
  http.begin(wifiClient, url); // Specify request destination
  http.addHeader("Content-Type",
                 "application/json"); // Specify content-type header
  int httpCode = http.POST(post_json); // Send the request
  Serial.println(httpCode);                 // Print HTTP return code
  if (httpCode > 0) {                  // Check the returning code
    String payload = http.getString(); // Get the response
    // payload
    Serial.println(payload); // Print request response payload
  }
  http.end(); // Close connection
}

void http_get_custom(HTTPClient &http, String url) {
  Serial.println("HTTP GET: ");
  Serial.println(url);
  http.begin(wifiClient, url); // Specify request destination
  http.setURL(url);
  http.addHeader("Content-Type", "text/plain"); // Specify content-type header

  int httpCode = http.GET(); // Send the request
  Serial.println(httpCode);       // Print HTTP return code

  if (httpCode > 0) {                  // Check the returning code
    String payload = http.getString(); // Get the response
    // Parse response
    DynamicJsonDocument json(2048);
    deserializeJson(json, http.getStream());
    
    // Read values
    if (json.containsKey("co2")) {
      co2 = json["co2"].as<String>();
    }
    if (json.containsKey("temperature")) {
      temp = json["temperature"].as<String>();
    }
    if (json.containsKey("humidity")) {
      humi = json["humidity"].as<String>();
    }
    if (json.containsKey("pressure")) {
      qfe = json["pressure"].as<String>();
    }
    if (json.containsKey("altitude")) {
      alt = json["altitude"].as<String>();
    }
    
    // payload
    Serial.println(payload); // Print request response payload
  }
  http.end(); // Close connection
}

void mqttReconnect() {
  for (uint8_t attempt = 0; attempt < 3; ++attempt) {
    if (mqttClient.connect(identifier, Config::username, Config::password,
                           MQTT_TOPIC_AVAILABILITY, 1, true,
                           AVAILABILITY_OFFLINE)) {
      mqttClient.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
      publishAutoConfig();
      // Make sure to subscribe after polling the status so that we never
      // execute commands with the default data
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      break;
    }
    delay(5000);
  }
}

bool isMqttConnected() { return mqttClient.connected(); }

void publishState() {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument stateJson(604);
  char payload[256];

  wifiJson["ssid"] = WiFi.SSID();
  wifiJson["ip"] = WiFi.localIP().toString();
  wifiJson["rssi"] = WiFi.RSSI();

  stateJson["co2"] = co2;
  stateJson["temperature"] = Config::imperial_temp ? temp_imp : temp;
  
  stateJson["humidity"] = humi;
  stateJson["pressure"] = qfe;

  stateJson["wifi"] = wifiJson.as<JsonObject>();

  serializeJson(stateJson, payload);
  mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
}

void mqttCallback(const char *topic, const uint8_t *payload, const unsigned int length) {}

void publishAutoConfig() {
  char mqttPayload[2048];
  DynamicJsonDocument device(256);
  DynamicJsonDocument autoconfPayload(1024);
  StaticJsonDocument<64> identifiersDoc;
  JsonArray identifiers = identifiersDoc.to<JsonArray>();

  identifiers.add(identifier);

  device["identifiers"] = identifiers;
  device["manufacturer"] = "TeHyBug";
  device["model"] = "FreshAirMakesSense";
  device["name"] = identifier;
  device["sw_version"] = "2024.04.01";

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

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" CO2");
  autoconfPayload["unit_of_measurement"] = "ppm";
  autoconfPayload["value_template"] = "{{value_json.co2}}";
  autoconfPayload["unique_id"] = identifier + String("_co2");
  autoconfPayload["icon"] = "mdi:air-filter";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_SENSOR[0], &mqttPayload[0], true);

  autoconfPayload.clear();

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" Temperature");
  autoconfPayload["unit_of_measurement"] = Config::imperial_temp ? "°F" : "°C";

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

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, uint8_t wait) {
  strip.setBrightness(Config::led_brightness);
  for (uint8_t i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);              //  Set pixel's color (in RAM)
    strip.show();                               //  Update strip to match
    delay(wait);                                //  Pause for a moment
  }
}

void read_bmx280() {
  temp = String((bmx280.readTemperature()));
  temp2imp(temp, temp_imp);
  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.println(" C");
  if (bmx280.getChipID() == CHIP_ID_BME280) {
    humi = String(bmx280.readHumidity(), 0);
    Serial.print(F("Humidity:    "));
    Serial.print(humi);
    Serial.println(" %");
  }

  qfe = String((bmx280.readPressure() / 100.0F), 0);
  Serial.print(F("Pressure:    "));
  Serial.print(qfe);
  Serial.println(" hPa");

  alt = String(bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
  Serial.print(F("Altitude:    "));
  Serial.print(alt);
  Serial.println(" m");
  Serial.println();
}
void read_aht20() {
  float humidity, temperature;
  bool ret = AHT.getSensor(&humidity, &temperature);
  if (ret) // GET DATA OK
  {
    Serial.print("humidity: ");
    humi = String((humidity * 100), 0);
    Serial.print(humi);
    Serial.print("%\t temperature: ");
    temp = String(temperature, 1);
    temp2imp(temp, temp_imp);
    Serial.println(temp);
  } else // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }
}

void co2_ampel(const int & val) {
  if (val > 1500) {
    colorWipe(strip.Color(255, 0, 0), 10); // red
  } else if (val > 1000) {
    colorWipe(strip.Color(255, 200, 0), 10); // yellow
  } else {
    colorWipe(strip.Color(0, 255, 0), 10); // green
  }
  strip.show();
}

void read_scd4x() {
  if (mySensor.readMeasurement()) // readMeasurement will return true when fresh
                                  // data is available
  {
    Serial.println();
    co2 = String(mySensor.getCO2());
    Serial.print(F("CO2(ppm):"));
    Serial.print(co2);

    Serial.print(F("\tTemperature(C):"));
    Serial.print(mySensor.getTemperature(), 1);

    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(mySensor.getHumidity(), 1);

    Serial.println();

    if (!aht20_sensor && !bmx_sensor) {
      float calib_temp = (Config::offline_mode)
                             ? Calibration::scd4x_offline_temp
                             : Calibration::scd4x_online_temp;
      float calib_humi = (Config::offline_mode)
                             ? Calibration::scd4x_offline_humi
                             : Calibration::scd4x_online_humi;

      temp = String((mySensor.getTemperature() + calib_temp), 1);
      humi = String((mySensor.getHumidity() + calib_humi), 0);

      temp2imp(temp, temp_imp);
    }

    co2_ampel(co2.toInt());
  }
}

void calibrate_scd4x() {
  if (!scd4x_sensor)
  {
    return;
  }
    ticker.disable(0);
    display_show("Calibration started", "Put sensor", "outside and",
                 "wait for 15 minutes", "", "", true);

    float co2, temperature, humidity;
    uint16_t calibration;
    uint8_t data[12], counter, repetition;
    uint8_t ret;

    // init I2C
    Wire.begin(0, 2);

    // wait until sensors are ready, > 1000 ms according to datasheet
    delay(1000);

    // start scd measurement in periodic mode, will update every 5 s
    Wire.beginTransmission(0x62);
    Wire.write(0x21);
    Wire.write(0xb1);
    Wire.endTransmission();

    // wait for first measurement to be finished
    delay(2000);

    Serial.println("# CO2 values before recalibration");

    // measure 5 times
    for (repetition = 0; repetition < 5; repetition++) {
      // read measurement data: 2 bytes co2, 1 byte CRC,
      // 2 bytes T, 1 byte CRC, 2 bytes RH, 1 byte CRC,
      // 2 bytes sensor status, 1 byte CRC
      // stop reading after 12 bytes (not used)
      // other data like  ASC not included
      Wire.requestFrom(0x62, 12);
      counter = 0;
      while (Wire.available()) {
        data[counter++] = Wire.read();
      }

      // floating point conversion according to datasheet
      co2 = (float)((uint16_t)data[0] << 8 | data[1]);
      // convert T in degC
      temperature =
          -45 + 175 * (float)((uint16_t)data[3] << 8 | data[4]) / 65536;
      // convert RH in %
      humidity = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65536;

      delay(2000);
    }

    // wait for another 5 minutes to equilibrate sensor to ambient
    Serial.println("# Waiting 5 minutes for equilibration");
    delay(5 * 60 * 1000);

    // stop scd measurement
    Wire.beginTransmission(0x62);
    Wire.write(0x3f);
    Wire.write(0x86);
    ret = Wire.endTransmission();
    Serial.println(ret);

    // wait for sensor
    delay(20);

    // assuming an external reference shows 650 ppm
    calibration = 400;

    // prepare buffer with data for calibration
    // calculate CRC for each 2 bytes of data
    data[0] = (calibration & 0xff00) >> 8;
    data[1] = calibration & 0x00ff;
    data[2] = CalcCrc(data);

    // send command for perform_forced_recalibration
    Wire.beginTransmission(0x62);
    Wire.write(0x36);
    Wire.write(0x2F);
    // append data for calibration
    // 2 bytes calibraion, CRC
    Wire.write(data[0]);
    Wire.write(data[1]);
    Wire.write(data[2]);
    ret = Wire.endTransmission();
    Serial.println(ret);

    delay(400);

    // read data: 2 bytes correction, 1 byte CRC
    Wire.requestFrom(0x62, 3);
    counter = 0;
    while (Wire.available()) {
      data[counter++] = Wire.read();
    }

    if (CalcCrc(data) != data[2])
      Serial.println("# ERROR: recalibration CRC return value");

    calibration = ((uint16_t)data[0] << 8 | data[1]);

    Serial.print("# Value after recalibration\n# ");
    Serial.println(calibration - 32768);

    // output format
    Serial.println("CO2(ppm)\tTemperature(degC)\tRelativeHumidity(percent)");

    // start scd measurement again in periodic mode, will update every 2 s
    Wire.beginTransmission(0x62);
    Wire.write(0x21);
    Wire.write(0xb1);
    Wire.endTransmission();

    // wait for first measurement to be finished (> 5 s)
    display_show("Calibration finished", "", "", "", "", "", true);
    delay(10000);
    ticker.enable(0);
}

void calibrate_sensor() { calibrate_scd4x(); }

void read_remote() {
  http_get_custom(http1, "http://tehybug.local");
}


void display_show(const String line1, const String line2, const String line3,
                  const String line4, const String line5, const String line6,
                  bool partial) {
  if (epaper) {
    display.fillScreen(GxEPD_WHITE);

    display.setFont(&FreeSans12pt7b);
    display.setCursor(0, 0);
    display.println();
    display.println(line1);
    if (line2 != "") {
      display.println(line2);
    }
    if (line3 != "") {
      display.println(line3);
    }
    if (line4 != "") {
      display.println(line4);
    }
    if (line5 != "") {
      display.println(line5);
    }
    if (line6 != "") {
      display.println(line6);
    }
    if (partial) {
      display.updateWindow(0, 0, display.width(), display.height());
    } else {
      display.update();
    }
  }
}

#if defined(_GxDEPG0154BxS800FxX_BW_H_)
void update_display() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  uint16_t x, y;

  if (epaper && update_epaper_display) {
    if (update_epaper_display_counter >= 30) {
      display.setRotation(1);
      display.fillScreen(GxEPD_WHITE);
      display.setTextSize(1);
      display.setTextColor(GxEPD_BLACK);
      display.update();
      delay(500);
    }
    display.fillScreen(GxEPD_WHITE);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);

    if (temp != "") {
      display.setFont(&FreeSans18pt7b);
      display.setCursor(0, 30);
      if (Config::imperial_temp) {
        uint8_t index = temp_imp.indexOf('.');
        String temp_a = temp_imp.substring(0, index);
        String temp_b = temp_imp.substring(index + 1, index + 2);
        temp_a += ".";
        display.getTextBounds(temp_a, 0, 0, &tbx, &tby, &tbw, &tbh);
        display.println(temp_a);
        display.setFont(&FreeSans7pt7b);
        display.setCursor(tbw - 4, 11);
        display.println("o");
        display.setCursor(tbw + 6, 14);
        display.println("F");
        display.setCursor(tbw + 6, 30);
        display.println(temp_b);
      } else {
        uint8_t index = temp.indexOf('.');
        String temp_a = temp.substring(0, index);
        String temp_b = temp.substring(index + 1, index + 2);
        temp_a += ".";
        display.getTextBounds(temp_a, 0, 0, &tbx, &tby, &tbw, &tbh);
        display.println(temp_a);
        display.setFont(&FreeSans7pt7b);
        display.setCursor(tbw - 4, 11);
        display.println("o");
        display.setCursor(tbw + 6, 14);
        display.println("C");
        display.setCursor(tbw + 6, 30);
        display.println(temp_b);
      }
    }

    if (humi != "") {
      display.setFont(&FreeSans18pt7b);
      display.setCursor(90, 30);
      display.println(humi);
      display.setFont(&FreeSans7pt7b);
      display.setCursor(130, 14);
      display.println("%");
      display.setCursor(130, 30);
      display.println("RH");
    }
    if (co2 != "") {
      display.setTextSize(1);
      display.setFont(&FreeSansBold24pt7b);
      display.getTextBounds(co2, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      x = ((display.width() - tbw) / 2) - tbx;
      // y = ((display.height() - tbh) / 2) - tby;
      display.setCursor(x, 86);
      display.println(co2);
      display.setTextSize(1);
      display.setCursor(70, 106);
      display.setFont(&FreeSans9pt7b);
      display.println("CO2 PPM");
    }

    if (qfe != "") {
      display.setFont(&FreeSans18pt7b);
      display.setCursor(0, 150);
      display.println(qfe);
      display.getTextBounds(qfe, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      // x = ((display.width() - tbw) / 2) - tbx;
      // y = ((display.height() - tbh) / 2) - tby;
      display.setFont(&FreeSans9pt7b);
      display.setCursor(tbw + 6, 149);
      display.println("hpa");
    }

    if (!Config::offline_mode) {
      uint16_t x = display.width() - 16;
      uint16_t y = display.height() - 16;
      display.drawExampleBitmap(wifi_icon_small, x, y, 16, 16, GxEPD_BLACK,
                                GxEPD::bm_invert);
    }

    display.updateWindow(0, 0, display.width(), display.height());
    update_epaper_display_counter++;
  }
}
// #endif
#else
// #if defined(_GxDEPG0150BN_H_)
void update_display() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  uint16_t x, y;

  if (epaper && update_epaper_display) {
    if (update_epaper_display_counter >= 30) {
      update_epaper_display_counter = 0;
      display.setRotation(1);
      display.fillScreen(GxEPD_WHITE);
      display.setTextSize(1);
      display.setTextColor(GxEPD_BLACK);
      display.update();
      delay(500);
    }

    display.fillScreen(GxEPD_WHITE);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);

    if (temp != "") {
      if (Config::imperial_temp) {
        display.setFont(&FreeSans24pt7b);
        display.setCursor(0, 44);
        uint8_t index = temp_imp.indexOf('.');
        String temp_a = temp_imp.substring(0, index);
        String temp_b = temp_imp.substring(index + 1, index + 2);
        temp_a += ".";
        display.getTextBounds(temp_a, 0, 0, &tbx, &tby, &tbw, &tbh);
        display.println(temp_a);
        display.setFont(&FreeSans9pt7b);
        display.setCursor(tbw - 5, 21);
        display.println("o");
        display.setCursor(tbw + 6, 24);
        display.println("F");
        display.setCursor(tbw + 6, 44);
        display.println(temp_b);
      } else {
        display.setFont(&FreeSans24pt7b);
        display.setCursor(0, 44);
        uint8_t index = temp.indexOf('.');
        String temp_a = temp.substring(0, index);
        String temp_b = temp.substring(index + 1, index + 2);
        temp_a += ".";
        display.getTextBounds(temp_a, 0, 0, &tbx, &tby, &tbw, &tbh);
        display.println(temp_a);
        display.setFont(&FreeSans9pt7b);
        display.setCursor(tbw - 5, 21);
        display.println("o");
        display.setCursor(tbw + 6, 24);
        display.println("C");
        display.setCursor(tbw + 6, 44);
        display.println(temp_b);
      }
    }

    if (humi != "") {
      display.setFont(&FreeSans24pt7b);
      display.setCursor(120, 44);
      display.println(humi);
      display.setFont(&FreeSans9pt7b);
      display.setCursor(174, 24);
      display.println("%");
      display.setCursor(174, 44);
      display.println("RH");
    }
    if (co2 != "") {
      display.setTextSize(2);
      display.setFont(&FreeSansBold18pt7b);
      display.getTextBounds(co2, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      x = ((display.width() - tbw) / 2) - tbx;
      // y = ((display.height() - tbh) / 2) - tby;
      display.setCursor(x, 120);
      display.println(co2);
      display.setTextSize(1);
      display.setCursor(92, 146);
      display.setFont(&FreeSans12pt7b);
      display.println("CO2 PPM");
    }

    if (qfe != "") {
      display.setFont(&FreeSans18pt7b);
      display.setCursor(0, 197);
      display.println(qfe);

      display.getTextBounds(qfe, 0, 0, &tbx, &tby, &tbw, &tbh);
      // center the bounding box by transposition of the origin:
      // x = ((display.width() - tbw) / 2) - tbx;
      // y = ((display.height() - tbh) / 2) - tby;

      display.setFont(&FreeSans9pt7b);
      display.setCursor(tbw + 6, 197);
      display.println("hpa");
    }

    if (!Config::offline_mode) {
      uint16_t x = display.width() - 24;
      uint16_t y = display.height() - 24;

      display.drawExampleBitmap(wifi_icon, x, y, 24, 24, GxEPD_BLACK,
                                GxEPD::bm_invert);
    }

    display.updateWindow(0, 0, display.width(), display.height());
    update_epaper_display_counter++;
  }
}
#endif

void read_sensors() {
  if (bmx_sensor) {
    read_bmx280();
  }

  if (aht20_sensor) {
    read_aht20();
  }

  if (scd4x_sensor) {
    read_scd4x();
  }

  if(remote_sensor){
    read_remote();
  }

  update_epaper_display = true;
  update_display();
}

void setup() {

  if (epaper) {
    display.init(); // disable diagnostic output on Serial
    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
    display.fillScreen(GxEPD_WHITE);
    uint16_t x = (display.width() - 180) / 2;
    uint16_t y = 5;

    display.drawExampleBitmap(tehybug_logo_white, x, y, 180, 180, GxEPD_BLACK,
                              GxEPD::bm_invert);
    display.update();
    delay(500);
  }

  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  colorWipe(strip.Color(0, 0, 255), 10); // Blue
  strip.show();

  /*
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); //Needed, at least in my tests WiFi doesn't power off without this
    for some reason
  */

  Serial.begin(115200);
  Wire.begin(0, 2);

  i2c_addresses = "";
  i2c_scanner(&Serial, i2c_addresses);

  if (strContains(i2c_addresses.c_str(), "0x77") == 1) {
    bmx280 = bmp280;
    bmx_sensor = true;
  } else if (strContains(i2c_addresses.c_str(), "0x76") == 1) {
    bmx_sensor = true;
  }

  if (strContains(i2c_addresses.c_str(), "0x38") == 1) {
    aht20_sensor = true;
  }

  if (strContains(i2c_addresses.c_str(), "0x62") == 1) {
    scd4x_sensor = true;
  }

  if(i2c_addresses == "")
  {
    remote_sensor = true;
  }
  
  // bmx280 and bme680 have same address
  if (bmx_sensor) {
    // Initialize sensor
    while (!bmx280.begin()) {
      Serial.println(F("Error: Could not detect sensor"));
      bmx_sensor = false;
      break;
    }
    if (bmx_sensor) {
      // Print sensor type
      Serial.print(F("\nSensor type: "));
      switch (bmx280.getChipID()) {
      case CHIP_ID_BMP280:
        Serial.println(F("BMP280\n"));

        break;
      case CHIP_ID_BME280:
        Serial.println(F("BME280\n"));

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
          BMX280_MODE_NORMAL,      // SLEEP, FORCED, NORMAL
          BMX280_SAMPLING_X16,    // Temp:  NONE, X1, X2, X4, X8, X16
          BMX280_SAMPLING_X16,    // Press: NONE, X1, X2, X4, X8, X16
          BMX280_SAMPLING_X16,    // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
          BMX280_FILTER_X16,      // OFF, X2, X4, X8, X16
          BMX280_STANDBY_MS_500); // 0_5, 10, 20, 62_5, 125, 250, 500, 1000
    }
  }

  if (scd4x_sensor) {
    // The SCD4x has data ready every five seconds
    // mySensor.enableDebugging(); // Uncomment this line to get helpful debug
    // messages on Serial

    //.begin will start periodic measurements for us (see the later examples for
    // details on how to override this)
    if (!mySensor.begin()) {
      Serial.println(
          F("Sensor not detected. Please check wiring. Freezing..."));
      while (1)
        ;
    }
  }

  setupButtons();

  // load the config
  Config::load();

  if (digitalRead(BUTTON_LEFT) == 0) {
    colorWipe(strip.Color(255, 0, 255), 10); // pink
    strip.show();
    Serial.println("WIFI toggled!");
    Config::offline_mode = !Config::offline_mode;
    Config::save();
    if (epaper) {
      String line3 = "OFF";
      String line4 = "WIFI enabled!";
      if (Config::offline_mode) {
        line3 = "ON";
        line4 = " WIFI disabled!";
      }
      display_show("Offline", "mode:", line3, line4, "", "", true);
    }
  }

  while (digitalRead(BUTTON_LEFT) == 0) {
    delay(1);
  }

  if (!Config::offline_mode) {
    colorWipe(strip.Color(0, 0, 255), 10); // Blue
    strip.show();
    setupHandle();
    WiFi.mode(WIFI_STA);
    delay(1);
  } else {
    colorWipe(strip.Color(0, 0, 0), 10); // off
    strip.show();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); // Needed, at least in my tests WiFi doesn't power off without
    // this for some reason
  }

  ticker.add(
      0, 10033, [&](void *) { read_sensors(); }, nullptr, true);
}


void onlineModeLoop()
{
    // ArduinoOTA.handle();
    mqttClient.loop();
    yield();
    MDNS.update();
    yield();
    server.handleClient();

    const uint32_t currentMillis = millis();
    if (!mqttClient.connected() &&
        currentMillis - lastMqttConnectionAttempt >= mqttConnectionInterval) {
      lastMqttConnectionAttempt = currentMillis;
      printf("Reconnect mqtt\n");
      mqttReconnect();
    }
    else if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval) {
      statusPublishPreviousMillis = currentMillis;
      printf("Publish state\n");
      publishState();
    }
}


void loop() {
  ticker.update();
  yield();
  button_left.loop();
  button_right.loop();
  button_mode.loop();
  yield();
  if (!Config::offline_mode) {
    onlineModeLoop();
  }
}
