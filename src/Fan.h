#pragma once
#include <Arduino.h>

class Fan {
  public:
    void setSpeed(const uint8_t speedPercent) {
      Serial.print("Fan - set speed: ");
      Serial.println(speedPercent);
      currentSpeedPercent = min((uint8_t) 100, max((uint8_t) 0, speedPercent));
      if (isOn) {
        analogWrite(pin, 255 - 255 * currentSpeedPercent / 100);
      }
    }

    void turnOnOff(const bool state) {
      Serial.print("Fan - state now ");
      Serial.println(state);
      isOn = state;
      if (isOn) {
        analogWrite(pin, 255 - 255 * currentSpeedPercent / 100);
      } else {
        analogWrite(pin, 255);
      }
    }
    
    Fan(const int pin_) 
    : pin (pin_)
    {
      pinMode(pin, OUTPUT);
    }
    
  private:
    bool isOn = false;
    const int pin;
    int currentSpeedPercent = 100;
};