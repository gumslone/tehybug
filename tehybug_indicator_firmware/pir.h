#pragma once

#ifndef PIR_PIN
#define PIR_PIN 4
#endif

class Pir {
public:
  Pir(uint8_t pin): m_pin(pin)
  {}
  void setup(){
    pinMode(m_pin, INPUT);
  }
  String read()
  {
    int val = digitalRead(m_pin); // read sensor value
    if (val == HIGH) { // check if the sensor is HIGH
      m_state = "detected";
    } else {
      m_state = "clear";
    }
    return m_state;
  }
  String getState()
  {
    return m_state;
  }
  void setState(const String state)
  {
    m_state = state;
  }

private:
  String m_state{"clear"};
  uint8_t m_pin{4};
};
