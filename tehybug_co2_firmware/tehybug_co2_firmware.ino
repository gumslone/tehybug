#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <ErriezBMX280.h>
#include "AHT20.h"
#include <TickerScheduler.h>
#include "s8_uart.h"
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "Config.h"
#include "Button2.h"
#include "Webinterface.h"

// dns
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;

//Button
// Digital IO pin connected to the button. This will be driven with a
// pull-up resistor so the switch pulls the pin to ground momentarily.
// On a high -> low transition the button press logic will execute.
#define BUTTON_LEFT   5
#define BUTTON_RIGHT   14
#define BUTTON_MODE   0
/////////////////////////////////////////////////////////////////
Button2 button_left;
Button2 button_right;
Button2 button_mode;

bool wifi_enabled = true;

#define PIXEL_PIN    12  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 1  // Number of NeoPixels

SCD4x mySensor;
bool scd4x_sensor = false;

#define S8_RX_PIN 4         // Rx pin 
#define S8_TX_PIN 13         // Tx pin 

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);

S8_UART *sensor_S8;
S8_sensor sensor;
bool s8_sensor = false;
unsigned long s8_last_measurenment = 0;

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA      1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);
bool bmx_sensor = false; // in the setup the i2c scanner searches for the sensor

AHT20 AHT;
bool aht20_sensor = false; // in the setup the i2c scanner searches for the sensor

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

boolean oldState = HIGH;
int     mode     = 0;    // Currently-active animation mode, 0-9


String key, temp, temp_imp, humi, dew, qfe, qfe_imp, qnh, alt, air, aiq, lux, uv, adc, tvoc, co2;

String i2c_addresses = "";

TickerScheduler ticker(5);

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oled = false;
bool update_oled_display = false;


// wifi and mqtt and http
const char* update_path = "/update";
const char* update_username = "TeHyBug";
const char* update_password = "FreshAirMakesSense";

uint8_t mqttRetryCounter = 0;

WiFiManager wifiManager;
WiFiClient wifiClient;
PubSubClient mqttClient;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;



WiFiManagerParameter custom_mqtt_server("server", "mqtt server", Config::mqtt_server, sizeof(Config::mqtt_server));
WiFiManagerParameter custom_mqtt_user("user", "MQTT username", Config::username, sizeof(Config::username));
WiFiManagerParameter custom_mqtt_pass("pass", "MQTT password", Config::password, sizeof(Config::password));

uint32_t lastMqttConnectionAttempt = 0;
const uint16_t mqttConnectionInterval = 60000; // 1 minute = 60 seconds = 60000 milliseconds

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

void saveConfigCallback() {
  shouldSaveConfig = true;
}

String getSensor()
{

  DynamicJsonDocument root(1024);
  root["co2"] = co2;

  if (Config::imperial_temp == true)
  {
    root["temperature"] = temp_imp;
  }
  else
  {
    root["temperature"] = temp;
  }
  root["humidity"] = humi;
  root["pressure"] = qfe;
  root["altitude"] = alt;
  String json;
  serializeJson(root, json);
  return json;
}

void handleMainPage()
{
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getSensor());
}

void handleSaveConfig()
{
  if (server.hasArg("imperial_temp")) {
    if (server.arg("imperial_temp"))
    {
      Config::imperial_temp = true;
    }
  }
  else
  {
    Config::imperial_temp = false;
  }
  if (server.hasArg("imperial_qfe")) {
    if (server.arg("imperial_qfe"))
    {
      Config::imperial_qfe = true;
    }
  }
  else
  {
    Config::imperial_qfe = false;
  }
  Config::save();
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "Configuration saved sucessfully!");
}

void handleGetConfig()
{
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
  snprintf(MQTT_TOPIC_AVAILABILITY, 127, "%s/%s/status", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_STATE, 127, "%s/%s/state", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_COMMAND, 127, "%s/%s/command", FIRMWARE_PREFIX, identifier);

  snprintf(MQTT_TOPIC_AUTOCONF_SENSOR, 127, "homeassistant/sensor/%s/%s_co2/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_WIFI_SENSOR, 127, "homeassistant/sensor/%s/%s_wifi/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_T_SENSOR, 127, "homeassistant/sensor/%s/%s_t/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_H_SENSOR, 127, "homeassistant/sensor/%s/%s_h/config", FIRMWARE_PREFIX, identifier);
  snprintf(MQTT_TOPIC_AUTOCONF_P_SENSOR, 127, "homeassistant/sensor/%s/%s_p/config", FIRMWARE_PREFIX, identifier);
  WiFi.hostname(identifier);

  Config::load();

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
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
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
void setupMDSN()
{
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
//BUTTON
// Attach callback.
void pressed(Button2& btn) {
  Serial.println("pressed");
}
void released(Button2& btn) {
  Serial.print("released: ");
  Serial.println(btn.wasPressedFor());
}
void changed(Button2& btn) {
  Serial.println("changed");
}
void click(Button2& btn) {
  Serial.println("click\n");
  Serial.println(btn.getPin());
  Serial.println("\n");
}
void longClickDetected(Button2& btn) {
  Serial.println("long click detected\n");
}
void longClick(Button2& btn) {
  Serial.println("long click\n");
  if (btn.getPin() == BUTTON_MODE)
  {
    Serial.println("reset wifi\n");
    resetWifiSettingsAndReboot();
  }
  if (btn.getPin() == BUTTON_RIGHT)
  {
    Serial.println("reset wifi\n");
    calibrate_sensor();
  }
}
void doubleClick(Button2& btn) {
  Serial.println("double click\n");
  Serial.println(btn.getPin());
  Serial.println("\n");
}
void tripleClick(Button2& btn) {
  Serial.println("triple click\n");
}

void setupButtons()
{
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_MODE, INPUT_PULLUP);


  Serial.println("\n\nButton Demo");

  button_left.begin(BUTTON_LEFT);
  button_left.setLongClickTime(1000);
  button_left.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_left.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time: " + String(button_left.getDoubleClickTime()) + "ms");

  //button_left.setChangedHandler(changed);
  //button_left.setPressedHandler(pressed);
  //button_left.setReleasedHandler(released);

  //button_left.setTapHandler(tap);
  button_left.setClickHandler(click);
  //button_left.setLongClickDetectedHandler(longClickDetected);
  button_left.setLongClickHandler(longClick);

  button_left.setDoubleClickHandler(doubleClick);
  //button_left.setTripleClickHandler(tripleClick);


  button_right.begin(BUTTON_RIGHT);
  button_right.setLongClickTime(1000);
  button_right.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_right.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time: " + String(button_right.getDoubleClickTime()) + "ms");

  //button_right.setChangedHandler(changed);
  //button_right.setPressedHandler(pressed);
  //button_right.setReleasedHandler(released);

  //button_right.setTapHandler(tap);
  button_right.setClickHandler(click);
  //button_right.setLongClickDetectedHandler(longClickDetected);
  button_right.setLongClickHandler(longClick);

  button_right.setDoubleClickHandler(doubleClick);
  //button_right.setTripleClickHandler(tripleClick);

  button_mode.begin(BUTTON_MODE);
  button_mode.setLongClickTime(15000);
  button_mode.setDoubleClickTime(400);

  Serial.println(" Longpress Time: " + String(button_mode.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time: " + String(button_mode.getDoubleClickTime()) + "ms");

  //button_mode.setChangedHandler(changed);
  //button_mode.setPressedHandler(pressed);
  //button_mode.setReleasedHandler(released);

  //button_mode.setTapHandler(tap);
  button_mode.setClickHandler(click);
  //button_mode.setLongClickDetectedHandler(longClickDetected);
  button_mode.setLongClickHandler(longClick);

  button_mode.setDoubleClickHandler(doubleClick);
  //button_mode.setTripleClickHandler(tripleClick);

}

void resetWifiSettingsAndReboot() {
  wifiManager.resetSettings();
  delay(3000);
  ESP.restart();
}

void mqttReconnect() {
  for (uint8_t attempt = 0; attempt < 3; ++attempt) {
    if (mqttClient.connect(identifier, Config::username, Config::password, MQTT_TOPIC_AVAILABILITY, 1, true, AVAILABILITY_OFFLINE)) {
      mqttClient.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
      publishAutoConfig();

      // Make sure to subscribe after polling the status so that we never execute commands with the default data
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      break;
    }
    delay(5000);
  }
}

bool isMqttConnected() {
  return mqttClient.connected();
}

void publishState() {
  DynamicJsonDocument wifiJson(192);
  DynamicJsonDocument stateJson(604);
  char payload[256];

  wifiJson["ssid"] = WiFi.SSID();
  wifiJson["ip"] = WiFi.localIP().toString();
  wifiJson["rssi"] = WiFi.RSSI();

  stateJson["co2"] = co2;
  if (Config::imperial_temp == true)
  {
    stateJson["temperature"] = temp_imp;
  }
  else
  {
    stateJson["temperature"] = temp;
  }
  stateJson["humidity"] = humi;
  stateJson["pressure"] = qfe;

  stateJson["wifi"] = wifiJson.as<JsonObject>();

  serializeJson(stateJson, payload);
  mqttClient.publish(&MQTT_TOPIC_STATE[0], &payload[0], true);
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) { }

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
  device["sw_version"] = "2022.09.04";

  autoconfPayload["device"] = device.as<JsonObject>();
  autoconfPayload["availability_topic"] = MQTT_TOPIC_AVAILABILITY;
  autoconfPayload["state_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["name"] = identifier + String(" WiFi");
  autoconfPayload["value_template"] = "{{value_json.wifi.rssi}}";
  autoconfPayload["unique_id"] = identifier + String("_wifi");
  autoconfPayload["unit_of_measurement"] = "dBm";
  autoconfPayload["json_attributes_topic"] = MQTT_TOPIC_STATE;
  autoconfPayload["json_attributes_template"] = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": \"{{value_json.wifi.ip}}\"}";
  autoconfPayload["icon"] = "mdi:wifi";

  serializeJson(autoconfPayload, mqttPayload);
  mqttClient.publish(&MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[0], &mqttPayload[0], true);

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
  autoconfPayload["unit_of_measurement"] = "°C";
  if (Config::imperial_temp == true)
  {
    autoconfPayload["unit_of_measurement"] = "°F";
  }
  else
  {
    autoconfPayload["unit_of_measurement"] = "°C";
  }
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
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void read_bmx280()
{
  temp = String((bmx280.readTemperature()));
  temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
  temp_imp = String(temp_imp);
  Serial.print(F("Temperature: "));
  Serial.print(temp);
  Serial.println(" C");
  if (bmx280.getChipID() == CHIP_ID_BME280) {
    humi = String((bmx280.readHumidity()));
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
void read_aht20()
{
  float humidity, temperature;
  int ret = AHT.getSensor(&humidity, &temperature);

  if (ret)    // GET DATA OK
  {
    Serial.print("humidity: ");
    humi = String((humidity * 100), 0);
    Serial.print(humi);
    Serial.print("%\t temperature: ");
    temp = String(temperature, 1);
    temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
    temp_imp = String(temp_imp);
    Serial.println(temp);
  }
  else        // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }

}

void co2_ampel(int val)
{
  if (val > 1500)
  {
    colorWipe(strip.Color(  255, 0,   0), 90);    // red
  }
  else if (val > 1000)
  {
    colorWipe(strip.Color(  255, 200,   0), 90);    // yellow
  }
  else
  {
    colorWipe(strip.Color(  0, 255,   0), 90);    // Green

  }
  strip.show();
}

void read_scd4x()
{
  if (mySensor.readMeasurement()) // readMeasurement will return true when fresh data is available
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

    if (aht20_sensor == false && bmx_sensor == false)
    {
      temp = String(mySensor.getTemperature(), 1);
      humi = String(mySensor.getHumidity(), 1);

      temp_imp  = (int)round(1.8 * temp.toFloat() + 32);
      temp_imp = String(temp_imp);
    }

    co2_ampel(co2.toInt());

  }

}
void read_s8()
{
  if (millis() - s8_last_measurenment >= 5000) // 5 seconds
  {
    //printf("Millis: %lu\n", millis());

    // Get CO2 measure
    co2 = String(sensor_S8->get_co2());
    Serial.println();
    Serial.print(F("CO2(ppm):"));
    Serial.print(co2);
    Serial.println();
    co2_ampel(co2.toInt());
    //Serial.printf("/*%u*/\n", sensor.co2);   // Format to use with Serial Studio program

    // Compare with PWM output
    //sensor.pwm_output = sensor_S8->get_PWM_output();
    //printf("PWM output = %0.0f ppm\n", (sensor.pwm_output / 16383.0) * 2000.0);

    // Wait 5 second for next measure
    s8_last_measurenment = millis();
  }

}

void calibrate_s8()
{
  if (s8_sensor)
  {
    if (oled)
    {
      display_show("Calibration started", "Put sensor", "outside and", "wait 7 min", false);
    }
    // Countdown waiting outside
    Serial.println("Now, you put the sensor outside and wait.");
    Serial.println("Countdown begins...");
    unsigned int seconds = 360;
    while (seconds > 0) {
      printf("%d minutes %d seconds left\n", seconds / 60, seconds % 60);
      delay(1000);
      seconds--;
    }
    Serial.println("Time reamining: 0 minutes 0 seconds");

    // Start manual calibration
    Serial.println("Starting manual calibration...");
    if (!sensor_S8->manual_calibration()) {
      Serial.println("Error setting manual calibration!");
      delay(1000);
    }
    delay(6000);
    // Check if background calibration is finished
    sensor.ack = sensor_S8->get_acknowledgement();
    if (sensor.ack & S8_MASK_CO2_BACKGROUND_CALIBRATION) {
      printf("Manual calibration is finished.");
    } else {
      Serial.println("Doing manual calibration...");
    }
  }
}

void calibrate_sensor()
{
  calibrate_s8();
}

void display_show(String line1, String line2, String line3, String line4, bool show_dot)
{
  if (oled)
  {
    display.clearDisplay();


    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(32, 0);
    display.println(line1);
    display.setCursor(32, 12);
    display.println(line2);
    display.setCursor(32, 24);
    display.println(line3);
    display.setCursor(32, 36);
    display.println(line4);
    if (show_dot)
    {
      display.setCursor(95, 0);
      display.println("^");
    }
    display.display();
  }
}

void update_display()
{

  if (oled && update_oled_display)
  {

    String line2;
    if (Config::imperial_temp == true)
    {
      line2 = "T: " + (temp_imp) + "F";
    }
    else
    {
      line2 = "T: " + (temp) + "C";
    }
    display_show("CO2: " + String(co2), line2, "RH: " + (humi) + "%", "P: " + qfe + "hPa", wifi_enabled);

  }

}


void read_sensors()
{
  if (bmx_sensor)
  {
    read_bmx280();
  }

  if (aht20_sensor)
  {
    read_aht20();
  }

  if (s8_sensor)
  {
    read_s8();
  }

  if (scd4x_sensor)
  {
    read_scd4x();
  }

  update_oled_display = true;

  update_display();

}

void i2c_scanner()
{
  //i2c scanner begin
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      i2c_addresses = i2c_addresses + "0x";
      if (address < 16)
      {
        Serial.print("0");
        i2c_addresses = i2c_addresses + "0";
      }

      Serial.print(address, HEX);
      i2c_addresses = i2c_addresses + String(address, HEX) + ",";
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }

  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  //i2c scanner end
}

// find in string
uint8_t strContains(const char* string, char* toFind)
{
  uint8_t slen = strlen(string);
  uint8_t tFlen = strlen(toFind);
  uint8_t found = 0;

  if ( slen >= tFlen )
  {
    for (uint8_t s = 0, t = 0; s < slen; s++)
    {
      do {

        if ( string[s] == toFind[t] )
        {
          if ( ++found == tFlen ) return 1;
          s++;
          t++;
        }
        else {
          s -= found;
          found = 0;
          t = 0;
        }

      } while (found);
    }
    return 0;
  }
  else return -1;
}

void setup()
{
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  colorWipe(strip.Color(  0, 0,   255), 90);    // Blue
  strip.show();

  /*
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); //Needed, at least in my tests WiFi doesn't power off without this for some reason
  */

  Serial.begin(115200);
  Wire.begin(0, 2);

  i2c_addresses = "";
  i2c_scanner();

  if (strContains(i2c_addresses.c_str(), "0x3c") == 1)
  {
    oled = true;
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

  if (strContains(i2c_addresses.c_str(), "0x38") == 1)
  {
    aht20_sensor = true;
  }

  if (strContains(i2c_addresses.c_str(), "0x62") == 1)
  {
    scd4x_sensor = true;
  }
  else
  {
    s8_sensor = true;
  }


  if (oled)
  {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;); // Don't proceed, loop forever
    }
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    display.setRotation(2);
    delay(2000); // Pause for 2 seconds

  }


  // bmx280 and bme680 have same address
  if (bmx_sensor)
  {
    // Initialize sensor
    while (!bmx280.begin()) {
      Serial.println(F("Error: Could not detect sensor"));
      bmx_sensor = false;
      break;
    }
    if (bmx_sensor == true)
    {
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
      bmx280.setSampling(BMX280_MODE_SLEEP,    // SLEEP, FORCED, NORMAL
                         BMX280_SAMPLING_X16,   // Temp:  NONE, X1, X2, X4, X8, X16
                         BMX280_SAMPLING_X16,   // Press: NONE, X1, X2, X4, X8, X16
                         BMX280_SAMPLING_X16,   // Hum:   NONE, X1, X2, X4, X8, X16 (BME280)
                         BMX280_FILTER_X16,     // OFF, X2, X4, X8, X16
                         BMX280_STANDBY_MS_500);// 0_5, 10, 20, 62_5, 125, 250, 500, 1000


    }
  }

  if (scd4x_sensor)
  {
    //mySensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial

    //.begin will start periodic measurements for us (see the later examples for details on how to override this)
    if (mySensor.begin() == false)
    {
      Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
      while (1)
        ;
    }
  }

  if (s8_sensor)
  {
    // First message, we are alive
    Serial.println("");
    Serial.println("Init");

    // Initialize S8 sensor
    S8_serial.begin(S8_BAUDRATE);
    sensor_S8 = new S8_UART(S8_serial);

    // Check if S8 is available
    sensor_S8->get_firmware_version(sensor.firm_version);
    int len = strlen(sensor.firm_version);
    if (len == 0) {
      Serial.println("SenseAir S8 CO2 sensor not found!");
      while (1) {
        delay(1);
      };
    }

    // Show basic S8 sensor info
    Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
    printf("Firmware version: %s\n", sensor.firm_version);
    sensor.sensor_id = sensor_S8->get_sensor_ID();
    Serial.print("Sensor ID: 0x"); printIntToHex(sensor.sensor_id, 4); Serial.println("");

    Serial.println("Setup done!");
    Serial.flush();
  }



  //The SCD4x has data ready every five seconds

  setupButtons();

  int val = digitalRead(BUTTON_LEFT);   // read the input pin
  if (val == 0)
  {
    colorWipe(strip.Color(  255, 0,   255), 90);    // Blue
    strip.show();
    Serial.println("WIFI Disabled!");
    wifi_enabled = false;
  }
  if (wifi_enabled)
  {
    setupHandle();

  }
  else
  {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); //Needed, at least in my tests WiFi doesn't power off without this for some reason
  }


  while (digitalRead(BUTTON_LEFT) == 0)
  {
    delay(1);
  }

  ticker.add(0, 10033, [&](void*) {
    read_sensors();
  }, nullptr, true);
}

void loop()
{
  ticker.update();
  yield();
  button_left.loop();
  yield();
  button_right.loop();
  yield();
  button_mode.loop();
  yield();
  if (wifi_enabled)
  {
    //ArduinoOTA.handle();
    mqttClient.loop();
    yield();
    MDNS.update();
    yield();
    server.handleClient();


    const uint32_t currentMillis = millis();
    if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval) {
      statusPublishPreviousMillis = currentMillis;


      printf("Publish state\n");
      publishState();

    }

    if (!mqttClient.connected() && currentMillis - lastMqttConnectionAttempt >= mqttConnectionInterval) {
      lastMqttConnectionAttempt = currentMillis;
      printf("Reconnect mqtt\n");
      mqttReconnect();
    }
  }

}