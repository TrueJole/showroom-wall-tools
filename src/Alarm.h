#pragma once
#include <ArduinoHA.h> 
#include <unordered_map>

class Alarm {
  public:
    void overrideSequence(void(*sequence_)(Alarm *alarm)) {
      Serial.print(name);
      Serial.println(" - Sequence overridden!");
      sequence = sequence_;
    }

    void setFrequency(const uint64_t frequency_) {
      frequency = frequency_;
    }
    void setDuration(const uint64_t duration_) {
      duration = duration_;
    }

    void playManually(const uint64_t frequency_, const uint64_t duration_) {
      Serial.print(name);
      Serial.println(" - Played manually!");
      lastChangeTimestamp = millis();
      actor.setState(true);
      while (millis() - lastChangeTimestamp < duration_) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(1.0 / frequency_ * 1000000);
        digitalWrite(pin, LOW);
        delayMicroseconds(50);
      }
      lastChangeTimestamp = millis();
      actor.setState(isOn);
    }

    void loop() {
      if (!isOn) {
        digitalWrite(pin, LOW);
        return;
      }
      if (sequence != nullptr) {
        Serial.print(name);
        Serial.println(" -  Playing sequence...");
        sequence(this);
        return;
      }
      if (pause) {
        if (millis() - lastChangeTimestamp < duration) {
          return;
        }
        lastChangeTimestamp = millis();
        pause = false;
      }
      else {
        while (millis() - lastChangeTimestamp < duration) {
          digitalWrite(pin, HIGH);
          delayMicroseconds(1.0 / frequency * 1000000);
          digitalWrite(pin, LOW);
          delayMicroseconds(50);
        }
        pause = true;
        lastChangeTimestamp = millis();
      }
    }

    void turnOnOff(const bool state) {
      Serial.print(name);
      Serial.print(" - Now ");
      Serial.println(state);
      isOn = state;
      actor.setState(isOn);
      if (isOn) lastChangeTimestamp = millis();
    }

    Alarm(const char* name_, const char* id, const int pin_, const uint64_t frequency_ = 1500, const uint64_t duration_ = 500)
      : actor(id),
      pin(pin_)
    {
      alarmMap[&actor] = this;
      name = name_;
      
      actor.setName(name);
      actor.onCommand(onAlarmCommand);
      actor.setState(false, true);
      
      frequency = frequency_;
      duration = duration_;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }

  private:
    HASwitch actor;
    const int pin;
    const char* name;
    bool isOn = false;
    bool pause = false;
    unsigned long duration;
    unsigned long frequency;
    unsigned long lastChangeTimestamp = 0;
    void(*sequence)(Alarm *alarm);

    inline static std::unordered_map<HASwitch*, Alarm*> alarmMap;

    static void onAlarmCommand(bool state, HASwitch *sender) {
      alarmMap[sender]->turnOnOff(state);
    }
};