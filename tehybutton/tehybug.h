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
};
struct Calibration {
  bool active{false};
  float temp{0};
  float humi{0};
  float qfe{0};
};
struct Scenario {
  bool active = false;
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
