// DEPRACATED
#pragma once
#include <Arduino.h>
#include <Lamp.h>

class Button {
  public:
    void registerSwitchLamp(Lamp *lamp) {
      switchLamp = lamp;
    }

    bool getState() {
      return isPressed;
    }

    bool handleRelease() {
      if (releaseReady) {
        releaseReady = false;
        return true;
      }
      return false;
    }

    void manualPress(unsigned long duration, bool force) {
      bool previousState = isPressed;

      // Ignore if already pressed and not forcing
      if (previousState && !force) {
        return;
      }

      isPressed = true;
      if (onPressCallback != nullptr) {
        onPressCallback();
      }
      
      delay(duration);

      isPressed = previousState;
      if (onReleaseCallback != nullptr) {
        onReleaseCallback();
      }
      if (switchLamp != nullptr) switchLamp->turnSwitch();

      releaseReady = true;
    }

    void loop() {
      bool state = digitalRead(pin);
      if (digitalRead(pin) != pullUp) {
        isPressed = true;
        if (!lastState) {
          if (onPressCallback != nullptr) {
            onPressCallback();
          }
        }
      }
      else {
        isPressed = false;
        if (lastState) {
          releaseReady = true;
          if (onReleaseCallback != nullptr) {
            onReleaseCallback();
          }
          if (switchLamp != nullptr) switchLamp->turnSwitch();
        }
      }
      lastState = isPressed;
    }

    void onPress(void(*onPress)()) {
      onPressCallback = onPress;
    }

    void onRelease(void(*onRelease)()) {
      onReleaseCallback = onRelease;
    }

    Button(int pin_, bool pullUp_) {
      pin = pin_;
      pullUp = pullUp_;
      
      pinMode(pin, INPUT);
    }

  private:
    int pin;
    bool pullUp;
    void(*onPressCallback)();
    void(*onReleaseCallback)();
    Lamp *switchLamp;
    bool isPressed = false;
    bool lastState = false;
    bool releaseReady = false;
};