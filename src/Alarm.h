#include <ArduinoHA.h> 
#include <unordered_map>

class Alarm {
  public:
    void setFrequency(u64_t frequency_) {
      frequency = frequency_;
    }
    void setDuration(u64_t duration_) {
      duration = duration_;
    }

    void playManually(u64_t frequency_, u64_t duration_) {
      Serial.println("Alarm - Played manually!");
      lastChangeTimestamp = millis();
      actor.setState(true);
      while (millis() - lastChangeTimestamp < duration_) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(1.0 / frequency_ * 1000000);
        digitalWrite(pin, LOW);
        delayMicroseconds(1.0 / frequency_ * 1000000);
      }
      lastChangeTimestamp = millis();
      actor.setState(isOn);
    }

    void loop() {
      if (!isOn) {
        digitalWrite(pin, LOW);
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
          delayMicroseconds(1.0 / frequency * 1000000);
        }
        pause = true;
        lastChangeTimestamp = millis();
      }
    }

    void turnOnOff(bool state) {
      Serial.print("Alarm - Now ");
      Serial.println(state);
      isOn = state;
      actor.setState(isOn);
      if (isOn) lastChangeTimestamp = millis();
    }

    Alarm(const char* name, const char* id, int pin_, u64_t frequency_ = 1500, u64_t duration_ = 500)
      : actor(id)
    {
      alarmMap[&actor] = this;
      
      actor.setName(name);
      actor.onCommand(onAlarmCommand);
      actor.setState(false, true);
      
      pin = pin_;
      frequency = frequency_;
      duration = duration_;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }

  private:
    
    HASwitch actor;
    int pin;
    bool isOn = false;
    bool pause = false;
    unsigned long duration;
    unsigned long frequency;
    unsigned long lastChangeTimestamp = 0;

    inline static std::unordered_map<HASwitch*, Alarm*> alarmMap;

    static void onAlarmCommand(bool state, HASwitch *sender) {
      alarmMap[sender]->turnOnOff(state);
    }
};

