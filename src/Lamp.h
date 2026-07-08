#include <Adafruit_NeoPixel.h>
#include <ArduinoHA.h> 
#include <unordered_map>


class Lamp {
  public:
    bool getState() {
      return isOn;
    }

    void turnSwitch() {
      turnOnOff(!isOn);
    }

    void turnOnOff(bool state) {
      Serial.print("Lamp - State: ");
      Serial.println(state);
      isOn = state;
      if (state) {
        onRGBColorCommand(actor.getCurrentRGBColor(), actor);
      } else {
        setColor(0, 0, 0);
      }
      actor.setState(state);
    }

    void begin() {
      led.begin();
      led.clear();
      led.show();
    }

    void setColor(u8_t r, u8_t g, u8_t b) {
      led.fill(led.Color(r, g, b));
      led.show();
      actor.setRGBColor({r, g, b});
    }

    void setBrightness(uint8_t brightness) {
      led.setBrightness(brightness);
      led.show();
      actor.setBrightness(brightness);
    }

    Lamp(const char* name, const char* id, int pin, int numberOfLEDs)
        : actor(id, HALight::BrightnessFeature | HALight::RGBFeature),
          led(numberOfLEDs, pin, NEO_GRB + NEO_KHZ800) 
      {
        lamps[&actor] = this;

        actor.setName(name);
        actor.onStateCommand(onStateCommand);
        actor.onBrightnessCommand(onBrightnessCommand);
        actor.onRGBColorCommand(onRGBColorCommand);
    }

  private:
    
    HALight actor;
    Adafruit_NeoPixel led;
    bool isOn = false;

    inline static std::unordered_map<HALight*, Lamp*> lamps;

    void forceLedUpdate() {
      led.show();
    }

    static void onStateCommand(bool state, HALight *sender) {
      Serial.print("Lamp - State: ");
      Serial.println(state);

      lamps[sender]->turnOnOff(state);
    }
    
    static void onBrightnessCommand(uint8_t brightness, HALight *sender) {
      Serial.print("Lamp - Brightness: ");
      Serial.println(brightness);

      lamps[sender]->setBrightness(brightness);
      lamps[sender]->forceLedUpdate();
    }

    static void onRGBColorCommand(HALight::RGBColor color, HALight *sender) {
      lamps[sender]->setColor(color.red, color.green, color.blue);
    }
};

