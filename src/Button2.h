#pragma once
#include <ArduinoHA.h> 
#include <unordered_map>
#include <Arduino.h>

// DO NOT USE BUTTON AND BUTTON2 TOGETHER
#include <JC_Button_ESP.h>

class Button2 {
  public:
    bool isPressed() {
      return pressed;
    }

    void manualPress(uint32_t duration) {
      Serial.print("Button - Digital press for ");
      Serial.print(duration);
      Serial.println(" milliseconds!");

      actor.setState(true);
      pressReady = true;
      pressed = true;
      // delay kritisch, lieber in loop
      delay(duration);

      actor.setState(false);
      pressed = false;
    } 

    void loop() {
      JCbutton.read();
      
      if (JCbutton.wasPressed()) {
        Serial.println("Button - Now pressed!");
        pressReady = true;
      }

      if (JCbutton.isPressed()) {
        actor.setState(true);
        pressed = true;
      }
      else {
        actor.setState(false);
        pressed = false;
      }
    }

    bool handlePress() {
      if (pressReady) {
        pressReady = false;
        return true;
      }
      return false;
    }

    Button2(const char* name, const char* id, int pin, bool pullUp, char* device_class = (char*) "connectivity")
      : actor(id),
        JCbutton(pin, 25UL, pullUp, pullUp)
    {
      actor.setName(name);
      actor.setDeviceClass(device_class);
      JCbutton.begin();
    }

  private:
    HABinarySensor actor;
    Button JCbutton;
    bool pressReady = false;
    bool pressed = false;
};