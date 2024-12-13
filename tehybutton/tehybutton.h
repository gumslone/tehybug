#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "data_types.h"
#include "configuration.h"
#include "UUID.h"

#ifndef _TEHYBUTTON_HEADER_
#define _TEHYBUTTON_HEADER_

class TeHyButton {
  public:
    unsigned long sleepAfter = 0;
    bool shouldServeData = true;
    
    Device device{};
    DataServ serveData{};
    TeHyButtonConfig conf;
    TeHyButton(): conf(device, serveData) {
    }

   void getDeviceKey() {
      // UUID – is a 36-character alphanumeric string
      String key = device.key;
      if (key.length() != 36) {
        key = generateDeviceKey();
        setDeviceKey(key);
      }
      D_println(F("key: "));
      D_println(key);
    }
    void updateSleepAfter(unsigned long after)
    {
      sleepAfter = millis() + after;
    }
    bool shouldSleep()
    {
      return !shouldServeData && sleepAfter < millis();
    }
    void setServeData()
    {
      if (device.skipButtonActions == false)
      {
        shouldServeData = true;
        updateSleepAfter(1500);
      }
    }
  private:
    UUID m_uuid;
    void setDeviceKey(String key) {
      device.key = key;
    }
    String generateDeviceKey() {
      m_uuid.seed(ESP.getChipId());
      m_uuid.generate();
      return String(m_uuid.toCharArray());
    }

};


#endif
