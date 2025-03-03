#pragma once
#ifndef _DataTypes_HEADER_
#define _DataTypes_HEADER_
struct RemoteControl {
  bool active{false};
  String url{};
};
struct Device {
  String key;
  bool configMode{false};
  bool sleepMode{true};
  RemoteControl remoteControl{};
};
struct Sensor {
  bool bmx{false};
  bool bme680{false};
  bool max44009{false};
  bool aht20{false};
  bool dht{false};
  bool dht_2{false};
  bool am2320{false};
  bool ds18b20{false};
  bool ds18b20_2{false};
  bool adc{false};
  bool scd4x{false};
} __attribute__((packed));
struct Peripherals {
  bool eeprom{false};
  bool ds3231{false};
} __attribute__((packed));
struct Calibration {
  bool active{false};
  float temp{0};
  float humi{0};
  float qfe{0};
};
struct Scenario {
  bool active{false};
  String type{};
  String url{};
  String data{};
  String condition{};
  float value{};
  String message{};
};
struct Scenarios {
  Scenario scenario1{};
  Scenario scenario2{};
  Scenario scenario3{};
};
struct HttpGetDataServ {
  String url;
  bool active{false};
  int frequency{900};
};
struct HttpPostDataServ {
  String url;
  bool active{false};
  int frequency{900};
  String message;
};
struct MqttDataServ {
  bool active{false};
  bool retained{false};
  String user;
  String password;
  String server{"0.0.0.0"};
  String topic{"/tehybug"};
  String message;
  int port{1883};
  int frequency{900};
  uint8_t retryCounter{0};
  uint8_t maxRetries{10};
  unsigned long lastReconnectAttempt{0};
  unsigned long reconnectWait{10000}; // wait 10 seconds and try to reconnect again
};
struct HaDataServ {
  bool active{false};
};
struct DataServ {
  HttpGetDataServ get{};
  HttpPostDataServ post{};
  MqttDataServ mqtt{};
  HaDataServ ha{};
  char data[90];
};
#endif
