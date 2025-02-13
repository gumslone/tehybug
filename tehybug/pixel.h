#pragma once
// set pin 4 HIGH to turn on the pixel
#ifndef SIGNAL_LED_PIN
#define SIGNAL_LED_PIN 4
#endif

#ifndef PIXEL_ACTIVE
#define PIXEL_ACTIVE 1
#define PIXEL_COUNT 1 // Number of NeoPixels
#define PIXEL_PIN 12  // Digital IO pin connected to the NeoPixels.
#endif
#include <Adafruit_NeoPixel.h>

#ifndef _TeHyBugPixel_HEADER_
#define _TeHyBugPixel_HEADER_
class TeHyBugPixel
{
public:  
  TeHyBugPixel() : m_neoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800) {
  }
  void on(uint8_t r=0, uint8_t g=0, uint8_t b=255, uint8_t brightness=50) {
    D_println("Led on");
    pinMode(SIGNAL_LED_PIN, OUTPUT);
    if (SIGNAL_LED_PIN == 1) {
      digitalWrite(SIGNAL_LED_PIN, LOW); // on
    } else {
      digitalWrite(SIGNAL_LED_PIN, HIGH); // on
      
      if(PIXEL_ACTIVE){
        setPixel(r, g, b, brightness);
      }
    }
  }

  void off() {
    D_println("Led off");
    if (SIGNAL_LED_PIN == 1) {
      digitalWrite(SIGNAL_LED_PIN, HIGH); // off
    } else {
      if(PIXEL_ACTIVE)
      {
        setPixel(0, 0, 0, 0);
      }
      digitalWrite(SIGNAL_LED_PIN, LOW); // off
    }
  }

private:

  void setPixel(uint8_t r=0, uint8_t g=0, uint8_t b=255, uint8_t brightness=50) {
        m_neoPixel.begin(); // Initialize NeoPixel strip object (REQUIRED)
        m_neoPixel.setPixelColor(0, m_neoPixel.Color(r, g, b));
        m_neoPixel.setBrightness(brightness);
        m_neoPixel.show(); // Initialize all pixels to 'on'
  }

  Adafruit_NeoPixel m_neoPixel;
};




#endif
