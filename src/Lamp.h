#include <Adafruit_NeoPixel.h>
#include <ArduinoHA.h> 
#include <unordered_map>


class Lamp {
  public:
    void begin() {
      led.begin();
      led.clear();
      led.show();
    }

    void setColor(u8_t r, u8_t g, u8_t b) {
      //led.setPixelColor(0, led.Color(r, g, b));
      led.fill(led.Color(r, g, b));
      led.show();
    }

    void setBrightness(uint8_t brightness) {
      led.setBrightness(brightness);
      led.show();
    }
//HADevice* device, 
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

    inline static std::unordered_map<HALight*, Lamp*> lamps;

    void forceLedUpdate() {
      led.show();
    }

    static void onStateCommand(bool state, HALight *sender) {
      Serial.print("Lamp - State: ");
      Serial.println(state);

      if (state == true) {
        onRGBColorCommand(sender->getCurrentRGBColor(), sender);
      } else {
        lamps[sender]->setColor(0, 0, 0);
      }
      sender->setState(state); // report state back to the Home Assistant
    }
    
    static void onBrightnessCommand(uint8_t brightness, HALight *sender) {
      Serial.print("Lamp - Brightness: ");
      Serial.println(brightness);

      lamps[sender]->setBrightness(brightness);
      lamps[sender]->forceLedUpdate();

      sender->setBrightness(brightness); // report brightness back to the Home Assistant
    }

    static void onRGBColorCommand(HALight::RGBColor color, HALight *sender) {
      lamps[sender]->setColor(color.red, color.green, color.blue);

      sender->setRGBColor(color); // report color back to the Home Assistant
    }
};

