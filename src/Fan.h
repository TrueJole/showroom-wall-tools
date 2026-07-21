#pragma once
#include <Arduino.h>

class Fan {
  public:
    void setSpeed(uint8_t speedPercent) {
      Serial.print("Fan - set speed: ");
      Serial.println(speedPercent);
      currentSpeedPercent = min((uint8_t) 100, max((uint8_t) 0, speedPercent));
      if (isOn) {
        analogWrite(pin, 255 * currentSpeedPercent / 100);
      }
    }
    void turnOnOff(bool state) {
      Serial.print("Fan - state now ");
      Serial.println(state);
      isOn = state;
      if (isOn) {
        analogWrite(pin, 255 * currentSpeedPercent / 100);
      } else {
        analogWrite(pin, 0);
      }
    }
    
    Fan(int pin_) {
      pin = pin_;
      pinMode(pin, OUTPUT);
    }
    
  private:
    bool isOn = false;
    int pin;
    int currentSpeedPercent = 100;
};