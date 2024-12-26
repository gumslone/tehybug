#pragma once

#ifndef REED_PIN
#define REED_PIN 13
#endif

class ReedSwitch {
public:
  ReedSwitch(uint8_t pin): m_pin(pin)
  {}
  void setup(){
    pinMode(m_pin, INPUT);
  }
  String read()
  {
    int val = digitalRead(m_pin); // read sensor value
    if (val == HIGH) { // check if the sensor is HIGH
      m_state = "open";
    } else {
      m_state = "closed";
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
  String m_state{"closed"};
  uint8_t m_pin{13}; 
};
