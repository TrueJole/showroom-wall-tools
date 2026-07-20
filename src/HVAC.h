#pragma once
#include <ArduinoHA.h> 
#include <Arduino.h>
#include <unordered_map>
#include "Fan.h"
#include <Adafruit_NeoPixel.h>

#define ANIMATION_SPEED 0.001

class HVAC {
  public:
    void begin() {
      led.begin();
      led.clear();
      led.show();

      fan.setSpeed(100);
      fan.turnOnOff(false);
      
      actor.setCurrentMode(HAHVAC::AutoMode);
      setMode(HAHVAC::AutoMode);
    }

    void loop() {
      //actor.setCurrentTemperature(currentTemperature);
      if (autoMode) {
        if (currentTemperature < targetTemperature) setMode(HAHVAC::HeatMode);
        else if (currentTemperature > targetTemperature) setMode(HAHVAC::CoolMode);
        else setMode(HAHVAC::FanOnlyMode);
      }
        
      uint64_t duration = millis() - ledAnimationStart;
      led.setBrightness(100);
      switch (ledAnimation) {
        case LED_OFF:
          led.setBrightness(0);
          break;
        case LED_COOL:
          led.fill(led.Color(0, 150 + 40 * sin(duration*ANIMATION_SPEED), 255));
          break;
        case LED_HEAT:
          led.fill(led.Color(255, 110 + 50 * sin(duration*ANIMATION_SPEED), 0));
          break;
        case LED_DRY:
          led.fill(led.Color(222, 214, 50));
          break;
      }
      led.show();
    }

    void setMode(HAHVAC::Mode mode_) {
      if (mode != mode_) {
        mode = mode_;
        ledAnimationStart = millis();
        switch (mode) {
          case HAHVAC::OffMode:
            ledAnimation = LED_OFF;
            fan.turnOnOff(false);
            break;
          case HAHVAC::CoolMode:
            ledAnimation = LED_COOL;
            fan.turnOnOff(true);
            break;
          case HAHVAC::HeatMode:
            ledAnimation = LED_HEAT;
            fan.turnOnOff(false);
            break;
          case HAHVAC::DryMode:
            ledAnimation = LED_DRY;
            fan.turnOnOff(true);
            break;
          case HAHVAC::FanOnlyMode:
            ledAnimation = LED_OFF;
            fan.turnOnOff(true);
            break;
          default:
            ledAnimation = LED_OFF;
            fan.turnOnOff(false);
            break;
        }
      }
    }

    void setCurrentTemperature(float temperature) {
      currentTemperature = temperature;
      actor.setCurrentTemperature(temperature);
    }

    void setTargetTemperature(float temperature) {
      targetTemperature = temperature;
      actor.setTargetTemperature(temperature);
      Serial.print("HVAC - Target temperature: ");
    	Serial.println(temperature);
    }

    HVAC(const char* name, const char* id, int ledCount_, int ledPin, int fanPin)
      : actor(id, HAHVAC::TargetTemperatureFeature | HAHVAC::PowerFeature | HAHVAC::ModesFeature),
        fan(fanPin),
        led(ledCount_, ledPin, NEO_GRB + NEO_KHZ800)
    {
      ledCount = ledCount_;
      hvacMap[&actor] = this;
      
      actor.setName(name);

      actor.onTargetTemperatureCommand(onTargetTemperatureCommand);
      //actor.onPowerCommand(onPowerCommand);
      actor.onModeCommand(onModeCommand);
      actor.setMinTemp(10);
      actor.setMaxTemp(30);
      actor.setTempStep(1);
    }

  private:
    enum LED_ANIMATION {
      LED_OFF,
      LED_COOL,
      LED_HEAT,
      LED_DRY,
      LED_ANIMATION_LENGTH
    };

    HAHVAC::Mode mode = HAHVAC::OffMode;
    HAHVAC actor;
    bool autoMode = false;

    Fan fan;
    
    Adafruit_NeoPixel led;
    int ledCount;
    LED_ANIMATION ledAnimation = LED_OFF;
    unsigned long ledAnimationStart = 0;

    float targetTemperature = 0;
    float currentTemperature = 0;

    inline static std::unordered_map<HAHVAC*, HVAC*> hvacMap;

    static void onModeCommand(HAHVAC::Mode mode, HAHVAC* sender) {
      Serial.print("HVAC - Mode: ");
      if (mode == HAHVAC::AutoMode) hvacMap[sender]->autoMode = true;
      else {
        hvacMap[sender]->autoMode = false;
        hvacMap[sender]->setMode(mode);
      }
      if (mode == HAHVAC::OffMode) {
        Serial.println("off");
      } else if (mode == HAHVAC::AutoMode) {
        Serial.println("auto");
      } else if (mode == HAHVAC::CoolMode) {
        Serial.println("cool");
      } else if (mode == HAHVAC::HeatMode) {
        Serial.println("heat");
      } else if (mode == HAHVAC::DryMode) {
        Serial.println("dry");
      } else if (mode == HAHVAC::FanOnlyMode) {
        Serial.println("fan only");
      }
    sender->setMode(mode); // report mode back to the HA panel
  }
  static void onTargetTemperatureCommand(HANumeric temperature, HAHVAC* sender) {
  	hvacMap[sender]->setTargetTemperature(temperature.toFloat());
    sender->setTargetTemperature(temperature); // report target temperature back to the HA panel
  }
};