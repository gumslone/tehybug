#pragma once

#include "pitches.h"

#ifndef TONE_PIN
#define TONE_PIN 5
#endif

namespace Buzzer {
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

String state = "OFF";

  void alarm()
  {
    if(state == "ON")
    {
      digitalWrite(TONE_PIN, HIGH);
    } 
    else
    {
      digitalWrite(TONE_PIN, LOW);
    } 
  }
  void on()
  {
    tone(TONE_PIN, NOTE_C4, 1000 / 16);
    // stop the tone playing:
  }
  void off()
  {
    noTone(TONE_PIN);
  }
  void beep()
  {
    tone(TONE_PIN, NOTE_C4, 1000 / 16);
    // stop the tone playing:
    delay(150);
    noTone(TONE_PIN);
  }
  void play_melody()
  {
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
  
      // to calculate the note duration, take one second divided by the note type.f
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(TONE_PIN, melody[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(TONE_PIN);
    }
  }
}
