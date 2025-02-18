#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

#include "AHT20.h"
#include "bsec.h"
#include "Button2.h"
#include "Config.h"
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include "Webinterface.h"
#include "s8_uart.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <ErriezBMX280.h>
#include <TickerScheduler.h>
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






const String version = "16.02.2025";



// dns
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
char cmDNS[33];
String escapedMac;

// Button
//  Digital IO pin connected to the button. This will be driven with a
//  pull-up resistor so the switch pulls the pin to ground momentarily.
//  On a high -> low transition the button press logic will execute.
#define BUTTON_LEFT 5
#define BUTTON_RIGHT 14
#define BUTTON_MODE 0
/////////////////////////////////////////////////////////////////
Button2 button_left;
Button2 button_right;
Button2 button_mode;

#define PIXEL_PIN 12 // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 1 // Number of NeoPixels

SCD4x mySensor;
bool scd4x_sensor = false;

#define S8_RX_PIN 4  // Rx pin
#define S8_TX_PIN 13 // Tx pin

SoftwareSerial S8_serial(S8_RX_PIN, S8_TX_PIN);

S8_UART *sensor_S8;
S8_sensor sensor;
bool s8_sensor = false;
unsigned long last_measurenment = 0;

// Adjust sea level for altitude calculation
#define SEA_LEVEL_PRESSURE_HPA 1026.25

// Create BMX280 object I2C address 0x76 or 0x77
ErriezBMX280 bmx280 = ErriezBMX280(0x76);
ErriezBMX280 bmp280 = ErriezBMX280(0x77);
bool bmx_sensor = false; // in the setup the i2c scanner searches for the sensor

Bsec bme680;
bool bme680_sensor = false;

AHT20 AHT;
bool aht20_sensor =
    false; // in the setup the i2c scanner searches for the sensor

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

DynamicJsonDocument sensorData(1023);

String i2c_addresses = "";

TickerScheduler ticker(5);

#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS                                                         \
  0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS                                                         \
  0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oled = false;
bool update_oled_display = false;

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

WiFiManagerParameter custom_mqtt_server("server", "mqtt server",
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

char MQTT_TOPIC_AUTOCONF_WIFI_SENSOR[128];
char MQTT_TOPIC_AUTOCONF_SENSOR[128];

#include "ha.h"


bool shouldSaveConfig = false;

void saveConfigCallback() { shouldSaveConfig = true; }

float temp2Imp(const float & value) {
  return (1.8 * value + 32);
}
void additionalSensorData(const String & key, float & value) {

  if (key == "temp" || key == "temp2") {
    addSensorData(key + "_imp", temp2Imp(value));
  }
}
void addSensorData(const String & key, float value) {
      sensorData[key] = String(value, 1);
      // calculate imperial temperature also heat index and the dew point
      additionalSensorData(key, value);
}

String getSensor() {

  DynamicJsonDocument root(1024);

  root["devices"] = i2c_addresses;
  root["ip"] = WiFi.localIP().toString();

  const JsonObject js = sensorData.as<JsonObject>();
  for (JsonPair keyValue : js) {
    const String k = keyValue.key().c_str();
    root[k] = keyValue.value().as<double>();
  }
  
  String json;
  serializeJson(root, json);
  return json;
}

void handleMainPage() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", getSensor());
}

void handleSaveConfig() {
  if (server.hasArg("imperial_temp")) {
    if (server.arg("imperial_temp")) {
      Config::imperial_temp = true;
    }
  } else {
    Config::imperial_temp = false;
  }
  if (server.hasArg("imperial_qfe")) {
    if (server.arg("imperial_qfe")) {
      Config::imperial_qfe = true;
    }
  } else {
    Config::imperial_qfe = false;
  }
  if (server.hasArg("scd40_single_shot")) {
    if (server.arg("scd40_single_shot")) {
      Config::scd40_single_shot = true;
    }
  } else {
    Config::scd40_single_shot = false;
  }
  Config::save();
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "Configuration saved sucessfully!");
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
  ha::setupHandle(FIRMWARE_PREFIX);
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

void mqttReconnect() {
  for (uint8_t attempt = 0; attempt < 3; ++attempt) {
    if (mqttClient.connect(identifier, Config::username, Config::password,
                           MQTT_TOPIC_AVAILABILITY, 1, true,
                           AVAILABILITY_OFFLINE)) {
      mqttClient.publish(MQTT_TOPIC_AVAILABILITY, AVAILABILITY_ONLINE, true);
      // Make sure to subscribe after polling the status so that we never
      // execute commands with the default data
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      break;
    }
    delay(5000);
  }
}

bool isMqttConnected() { return mqttClient.connected(); }

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {}



// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);              //  Set pixel's color (in RAM)
    strip.show();                               //  Update strip to match
    delay(wait);                                //  Pause for a moment
  }
}

void read_bmx280() {
  Serial.print(F("Temperature: "));
  Serial.print((bmx280.readTemperature()));
  Serial.println(" C");
  if (bmx280.getChipID() == CHIP_ID_BME280) {
    Serial.print(F("Humidity:    "));
    Serial.print(bmx280.readHumidity());
    Serial.println(" %");
    addSensorData("humi", bmx280.readHumidity());
  }

  Serial.print(F("Pressure:    "));
  Serial.print(String((bmx280.readPressure() / 100.0F), 0));
  Serial.println(" hPa");

  Serial.print(F("Altitude:    "));
  Serial.print(bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
  Serial.println(" m");
  Serial.println();
  addSensorData("temp", (bmx280.readTemperature()));
  addSensorData("qfe", (bmx280.readPressure() / 100.0F));
  addSensorData("alt", bmx280.readAltitude(SEA_LEVEL_PRESSURE_HPA));
}
void read_aht20() {
  float humidity, temperature;
  int ret = AHT.getSensor(&humidity, &temperature);

  if (ret) // GET DATA OK
  {
    Serial.print("humidity: ");
    Serial.print((humidity * 100));
    Serial.print("%\t temperature: ");
    Serial.println(temperature);
    addSensorData("temp", temperature);
    addSensorData("humi", (humidity * 100));
  } else // GET DATA FAIL
  {
    Serial.println("GET DATA FROM AHT20 FAIL");
  }
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
  D_print(", " + String(bme680.pressure / 100.0));
  D_print(", " + String(bme680.rawHumidity));
  D_print(", " + String(bme680.gasResistance));
  D_print(", " + String(bme680.iaq));
  D_print(", " + String(bme680.iaqAccuracy));
  D_print(", " + String(bme680.temperature));
  D_print(", " + String(bme680.humidity));
  D_print(", " + String(bme680.staticIaq));
  D_print(", " + String(bme680.co2Equivalent));
  D_println(", " + String(bme680.breathVocEquivalent));

  addSensorData("temp", bme680.temperature);
  addSensorData("humi", (bme680.humidity));
  addSensorData("qfe", bme680.pressure / 100.0);
  addSensorData("iaq", bme680.iaq);
  addSensorData("air", (bme680.gasResistance / 1000.0F));
  
}

void co2_ampel(int val) {
  if (val > 1500) {
    colorWipe(strip.Color(255, 0, 0), 90); // red
  } else if (val > 1000) {
    colorWipe(strip.Color(255, 200, 0), 90); // yellow
  } else {
    colorWipe(strip.Color(0, 255, 0), 90); // Green
  }
  strip.show();
}

void read_scd4x() {
if (millis() - last_measurenment >= 5000) // 5 seconds
  {
    if (mySensor.readMeasurement()) // readMeasurement will return true when fresh
                                    // data is available
    {
      
      addSensorData("co2", mySensor.getCO2());
      D_println();
      D_print(F("CO2(ppm):"));
      D_print(sensorData["co2"].as<String>());
      
      if (aht20_sensor == false && bmx_sensor == false && bme680_sensor == false) {
        addSensorData("temp", mySensor.getTemperature());
        addSensorData("humi", mySensor.getHumidity());
      }
      else
      {
        addSensorData("temp2", mySensor.getTemperature());
        addSensorData("humi2", mySensor.getHumidity()); 
        D_print(F("\tTemperature(C):"));
        D_print(sensorData["temp2"].as<String>());
  
        D_print(F("\tHumidity(%RH):"));
        D_print(sensorData["humi2"].as<String>()); 
      }
      
      D_println();
  
      co2_ampel(sensorData["co2"].as<int>());
    }
  
    if (Config::scd40_single_shot)
    {
      mySensor.measureSingleShot(); // Request fresh data (should take 5 seconds)
    }
    // Wait 5 second for next measure
    last_measurenment = millis();
  }
}
void read_s8() {
  if (millis() - last_measurenment >= 5000) // 5 seconds
  {
    // printf("Millis: %lu\n", millis());

    // Get CO2 measure
    addSensorData("co2", sensor_S8->get_co2());
    
    D_println();
    D_print(F("CO2(ppm):"));
    D_print(sensorData["co2"].as<String>());
    D_println();
    co2_ampel(sensorData["co2"].as<int>());
    // Serial.printf("/*%u*/\n", sensor.co2);   // Format to use with Serial
    // Studio program

    // Compare with PWM output
    // sensor.pwm_output = sensor_S8->get_PWM_output();
    // printf("PWM output = %0.0f ppm\n", (sensor.pwm_output / 16383.0) *
    // 2000.0);

    // Wait 5 second for next measure
    last_measurenment = millis();
  }
}

void calibrate_s8() {
  if (s8_sensor) {
    if (oled) {
      display_show("Calibration started", "Put sensor", "outside and",
                   "wait 7 min", true);
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

void calibrate_sensor() { calibrate_s8(); }

void display_show(const String line1, const String line2, const String line3,
                  const String line4, bool offline) {
  if (oled) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(32, 0);
    display.println(line1);
    if (line2 != "") {
      display.setCursor(32, 12);
      display.println(line2);
    }
    if (line3 != "") {
      display.setCursor(32, 24);
      display.println(line3);
    }
    if (line4 != "") {
      display.setCursor(32, 36);
      display.println(line4);
    }
    
    if (offline == false) {
      display.setCursor(95, 0);
      display.println("^");
    }
    display.display();
  }
}

void update_display() {

  if (oled && update_oled_display) {

    String line1, line2, line3, line4;

    if (sensorData.containsKey("co2")) {
      line1 = "CO2: " + sensorData["co2"].as<String>();
    }

    if (sensorData.containsKey("temp")) {
      if (Config::imperial_temp == true) {
        line2 = "T: " + sensorData["temp_imp"].as<String>() + (char)247 + "F";
      } else {
        line2 = "T: " + sensorData["temp"].as<String>() + (char)247 + "C";
      }
    }
    if (sensorData.containsKey("humi")) {
      line3 = "RH: " + sensorData["humi"].as<String>() + "%";
    }
    if (sensorData.containsKey("qfe")) {
      line4 = "P: " + sensorData["qfe"].as<String>() + "hPa";
    }
    if (line1 == "" && line2 == "" && line3 == "") {
      line1 = "Reading";
      line2 = "sensors";
      line3 = "...";
    }
    display_show(line1, line2, line3, line4, Config::offline_mode);
  }
}

void read_sensors() {

  if (s8_sensor) {
    read_s8();
  }

  if (scd4x_sensor) {
    read_scd4x();
  }

  if (bmx_sensor) {
    read_bmx280();
  }

  if (bme680_sensor) {
    read_bme680();
  }

  if (aht20_sensor) {
    read_aht20();
  }

  update_oled_display = true;

  update_display();
}

void i2c_scanner() {
  // i2c scanner begin
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      i2c_addresses = i2c_addresses + "0x";
      if (address < 16) {
        Serial.print("0");
        i2c_addresses = i2c_addresses + "0";
      }

      Serial.print(address, HEX);
      i2c_addresses = i2c_addresses + String(address, HEX) + ",";
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
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

  // i2c scanner end
}

// find in string
uint8_t strContains(const char *string, char *toFind) {
  uint8_t slen = strlen(string);
  uint8_t tFlen = strlen(toFind);
  uint8_t found = 0;

  if (slen >= tFlen) {
    for (uint8_t s = 0, t = 0; s < slen; s++) {
      do {

        if (string[s] == toFind[t]) {
          if (++found == tFlen)
            return 1;
          s++;
          t++;
        } else {
          s -= found;
          found = 0;
          t = 0;
        }

      } while (found);
    }
    return 0;
  } else
    return -1;
}


void setupOled()
{
  if (oled) {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
        ; // Don't proceed, loop forever
    }
    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    display.setRotation(2);
    delay(2000); // Pause for 2 seconds
  }
}
void setupDevices()
{
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  // bmx280 and bme680 have same address
  if (bmx_sensor) {
    // Initialize sensor
    while (!bmx280.begin()) {
      Serial.println(F("Error: Could not detect sensor"));
      bmx_sensor = false;
      break;
    }
    if (bmx_sensor == true) {
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

  if (bme680_sensor) {
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

  if (s8_sensor) {
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
      // while (1) {
      //   delay(1);
      // };
      s8_sensor = false;
    }
    if (s8_sensor) {
      // Show basic S8 sensor info
      Serial.println(">>> SenseAir S8 NDIR CO2 sensor <<<");
      printf("Firmware version: %s\n", sensor.firm_version);
      sensor.sensor_id = sensor_S8->get_sensor_ID();
      Serial.print("Sensor ID: 0x");
      printIntToHex(sensor.sensor_id, 4);
      Serial.println("");

      Serial.println("Setup done!");
      Serial.flush();
    }
  }

  
  if (scd4x_sensor) {
    if (Config::scd40_single_shot)
    {
      Serial.println(F("SCD4X single shot measurement"));
      if (mySensor.begin(false, true, false) == false) // Do not start periodic measurements
      //measBegin_________/     |     |
      //autoCalibrate__________/      |
      //skipStopPeriodicMeasurements_/
      {
        Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
        while (1)
          ;
      }
  
      //Let's call measureSingleShot to start the first conversion
      bool success = mySensor.measureSingleShot();
      if (success == false)
      {
        Serial.println(F("measureSingleShot failed. Are you sure you have a SCD41 connected? Freezing..."));
        while (1)
          ;    
      }
    }
    else
    {
      Serial.println(F("SCD4X perodic measurement"));
      // The SCD4x has data ready every five seconds
      // mySensor.enableDebugging(); // Uncomment this line to get helpful debug
      // messages on Serial
  
      //.begin will start periodic measurements for us (see the later examples for
      // details on how to override this)
      if (mySensor.begin() == false) {
        Serial.println(
            F("Sensor not detected. Please check wiring. Freezing..."));
        while (1)
          ;
      }
    }
  }

}

void connectToWiFi()
{
  int tryCount = 0;
  while ( WiFi.status() != WL_CONNECTED )
  {
    tryCount++;
    WiFi.reconnect();
    yield();
    if ( tryCount == 10 )
    {
      ESP.restart();
    }
  }
  //WiFi.onEvent( WiFiEvent );
} // void connectToWiFi()
void checkWifi()
{
  if ( !wifiClient.connected() || !WiFi.status() == WL_CONNECTED || WiFi.localIP().toString() == "0.0.0.0") {
    connectToWiFi();
  }
}

void setup() {
  /*
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); //Needed, at least in my tests WiFi doesn't power off without this
    for some reason
  */

  Serial.begin(115200);
  Wire.begin(0, 2);

  i2c_addresses = "";
  i2c_scanner();

  if (strContains(i2c_addresses.c_str(), "0x3c") == 1) {
    oled = true;
    setupOled();
  }

  if (strContains(i2c_addresses.c_str(), "0x77") == 1) {
    bmx280 = bmp280;
    bmx_sensor = true;
  } else if (strContains(i2c_addresses.c_str(), "0x76") == 1) {
    bmx_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x77") == 1) {
    bme680_sensor = true;
  }
  if (strContains(i2c_addresses.c_str(), "0x38") == 1) {
    aht20_sensor = true;
  }

  if (strContains(i2c_addresses.c_str(), "0x62") == 1) {
    scd4x_sensor = true;
  } else {
    s8_sensor = true;
  }
  // load the config
  Config::load();
  delay(10);
  //setup i2c devices
  setupDevices();
  setupButtons();

  int val = digitalRead(BUTTON_LEFT); // read the input pin
  if (val == 0) {
    colorWipe(strip.Color(255, 0, 255), 90); // pink
    strip.show();
    Serial.println("WIFI toggled!");
    Config::offline_mode = !Config::offline_mode;
    Config::save();
    if (oled == true) {
      String line3 = "OFF";

      if (Config::offline_mode)
        line3 = "ON";

      display_show("Offline", "mode:", line3, "", true);
    }
  }

  while (digitalRead(BUTTON_LEFT) == 0) {
    delay(1);
  }

  if (Config::offline_mode == false) {
    colorWipe(strip.Color(0, 0, 255), 90); // Blue
    strip.show();
    setupHandle();

  } else {
    colorWipe(strip.Color(0, 0, 0), 90); // off
    strip.show();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1); // Needed, at least in my tests WiFi doesn't power off without
              // this for some reason
  }
  
  ticker.add(
      0, 10033, [&](void *) { read_sensors(); }, nullptr, true);
}

void loop() {
  ticker.update();
  yield();
  button_left.loop();
  yield();
  button_right.loop();
  yield();
  button_mode.loop();
  yield();
  if (Config::offline_mode == false) {
    // ArduinoOTA.handle();
    mqttClient.loop();
    yield();
    MDNS.update();
    yield();
    server.handleClient();
    checkWifi();
    const uint32_t currentMillis = millis();
    if (currentMillis - statusPublishPreviousMillis >= statusPublishInterval) {
      statusPublishPreviousMillis = currentMillis;

      printf("Publish state\n");
      ha::publishAutoConfig(mqttClient, version, sensorData);
      ha::publishState(mqttClient, sensorData);
    }

    if (!mqttClient.connected() &&
        currentMillis - lastMqttConnectionAttempt >= mqttConnectionInterval) {
      lastMqttConnectionAttempt = currentMillis;
      printf("Reconnect mqtt\n");
      mqttReconnect();
    }
  }
}
