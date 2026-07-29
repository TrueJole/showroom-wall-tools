#pragma once
#include <ArduinoHA.h> 
#include <unordered_map>
#include <ESP32Servo.h>

class Cover {
  public:
    enum class MotorType {
      TYPE_SERVO,
      TYPE_NO_LIMIT,
      TYPES_LENGTH
    };

    void setSpeed(const int speed_) {
      speed = min(90, max(-90,speed_));
    }

    void setDuration(const uint64_t duration_) {
      duration = duration_;
    }

    void setState(const bool isOpen_) {
      isOpen = isOpen_;
      if (isOpen) actor.setState(HACover::StateOpen, true);
      else actor.setState(HACover::StateClosed, true);
    }

    void setAngles(const int openAngle_, const int closedAngle_) {
      openAngle = openAngle_;
      closedAngle = closedAngle_;
    }

    bool getState() {
      return isOpen;
    }

    void open() {
      if (!isOpen) {
        isOpen = true;
        Serial.println("Covers - Open");
        actor.setState(HACover::StateOpening); // report state back to the HA

        if (type == MotorType::TYPE_NO_LIMIT) {
          motor.write(90 + speed);
          delay(duration);
          motor.write(90);
        }
        else if (type == MotorType::TYPE_SERVO) {
          motor.write(openAngle);
          delay(500);
        }
        
        actor.setState(HACover::StateOpen);
      } 
      else {
        Serial.println("Covers - Already Open");
        actor.setState(HACover::StateOpen);
      }
    } 

    void close() {
      if (isOpen) {
        isOpen = false;
        Serial.println("Covers - Close");
        actor.setState(HACover::StateClosing); // report state back to the HA
        if (type == MotorType::TYPE_NO_LIMIT) {
          motor.write(90 - speed);
          delay(duration);
          motor.write(90);
        }
        else if (type == MotorType::TYPE_SERVO) {
          motor.write(closedAngle);
          delay(500);
        }
        actor.setState(HACover::StateClosed);
      } 
      else {
        Serial.println("Covers - Already Closed");
        actor.setState(HACover::StateClosed);
      }
    } 

    Cover(const char* name, const char* id, const int pin, const MotorType type_)
      : actor(id),
      type(type_)
    {
      coversMap[&actor] = this;

      actor.setName(name);
      actor.onCommand(onCoversCommand);
      motor.attach(pin);
    }

  private:
    
    HACover actor;
    Servo motor;
    bool isOpen;
    const MotorType type;
    // No Limit Motor
    int speed = 90;
    uint64_t duration = 2000;
    // Servo Motor
    int openAngle = 0;
    int closedAngle = 180;

    inline static std::unordered_map<HACover*, Cover*> coversMap;

    static void onCoversCommand(HACover::CoverCommand cmd, HACover *sender) {
      if (cmd == HACover::CommandOpen) {
        coversMap[sender]->open();
      } else if (cmd == HACover::CommandClose) {
        coversMap[sender]->close();
      } else if (cmd == HACover::CommandStop) {
        Serial.println("Covers - Stop");
        //sender->setState(HACover::StateStopped); // report state back to the HA
      } else {
        Serial.println("Covers - Something went wrong!");
      }
    }
};

