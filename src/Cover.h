#include <ArduinoHA.h> 
#include <unordered_map>
#include <ESP32Servo.h>

class Cover {
  public:
    bool getState() {
      return isOpen;
    }

    void open() {
      if (!isOpen) {
        isOpen = true;
        Serial.println("Covers - Open");
        actor.setState(HACover::StateOpening); // report state back to the HA

        motor.write(180);
        delay(2000);
        motor.write(90);
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
        motor.write(0);
        delay(2000);
        motor.write(90);
        actor.setState(HACover::StateClosed);
      } 
      else {
        Serial.println("Covers - Already Closed");
        actor.setState(HACover::StateClosed);
      }
    } 

    Cover(const char* name, const char* id, int pin, bool isOpen_)
        : actor(id)
      {
        coversMap[&actor] = this;

        actor.setName(name);
        actor.onCommand(onCoversCommand);
        motor.attach(pin);
	      motor.write(90);

        isOpen = isOpen_;
        if (isOpen) actor.setState(HACover::StateOpen, true);
        else actor.setState(HACover::StateClosed, true);
        
    }

  private:
    
    HACover actor;
    Servo motor;
    bool isOpen;

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

