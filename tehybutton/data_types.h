#pragma once

#ifndef _DATA_TYPES_HEADER_
#define _DATA_TYPES_HEADER_
struct Device {
  String key;
  bool configMode{false};
  bool sleepMode{true};
  bool skipButtonActions{false};
};
struct HttpGetDataServ {
  String url;
  bool active = false;
  int frequency = 900;
};
struct HttpPostDataServ {
  String url;
  bool active = false;
  int frequency = 900;
  String message;
};
struct MqttDataServ {
  bool active = false;
  bool retained = false;
  String user;
  String password;
  String server = "0.0.0.0";
  String topic = "/tehybug";
  String message;
  int port = 1883;
  int frequency = 900;
  int retryCounter = 0;
  int maxRetries = 99;
  long lastReconnectAttempt = 0;
  long reconnectWait = 10000; // wait 10 seconds and try to reconnect again
};
struct DataServ {
  HttpGetDataServ get{};
  HttpPostDataServ post{};
  MqttDataServ mqtt{};
  char data[80];
};
#endif
