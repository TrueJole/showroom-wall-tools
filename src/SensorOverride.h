#pragma once
#include <ArduinoHA.h> 
#include <unordered_map>

enum SENOR_TYPES {
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    CO2,
    TVOC,
    TYPES_LENGTH
}; 

class SensorOverride {
    public:
        float applyOverride(float initalValue) {
            if (!isOverridden) {
                return initalValue;
            }
            return overrideValue;
        }

        SensorOverride(const char* slider_name, const char* slider_id, const char* button_name, const char* button_id, SENOR_TYPES type) 
            : slider(slider_id),
            button(button_id)
        {
            sliders[&slider] = this;
            buttons[&button] = this;
            //name = name_;
            slider.setName(slider_name);
            slider.onCommand(onChange);

            button.setName(button_name);
            button.onCommand(onPress);

            switch (type) {
                case TEMPERATURE: {
                    slider.setUnitOfMeasurement("°C");
                    slider.setMin(-20);
                    slider.setMax(50);
                    slider.setIcon("mdi:thermometer-alert");
                    slider.setStep(1);
                    //sensor.setCurrentState(false);
                } break;
                case HUMIDITY: {
                    slider.setUnitOfMeasurement("%");
                    slider.setMin(0);
                    slider.setMax(100);
                    slider.setIcon("mdi:water-percent-alert");
                    slider.setStep(1);
                } break;
                case LIGHT: {
                    slider.setUnitOfMeasurement("lx");
                    slider.setMin(0);
                    slider.setMax(2000);
                    slider.setIcon("mdi:weather-sunny-alert");
                    slider.setStep(50);
                } break;
                case CO2: {
                    slider.setUnitOfMeasurement("ppm");
                    slider.setMin(400);
                    slider.setMax(3000);
                    slider.setIcon("mdi:molecule-co2");
                    slider.setStep(100);
                } break;
                case TVOC: {
                    slider.setUnitOfMeasurement("ppb");
                    slider.setMin(0);
                    slider.setMax(300);
                    slider.setIcon("mdi:molecule");
                    slider.setStep(10);
                } break;
            };
        }

    private:
        HANumber slider;
        HASwitch button;
        //const char* name;
        SENOR_TYPES type;
        inline static std::unordered_map<HANumber*, SensorOverride*> sliders;
        inline static std::unordered_map<HASwitch*, SensorOverride*> buttons;
        float overrideValue = 0;
        bool isOverridden = false;

        static void onChange(HANumeric number, HANumber* sender) {
            Serial.print("Override - ");
            Serial.println(number.toFloat());
            sliders[sender]->overrideValue = number.toFloat();
            sender->setState(number);
        }

        static void onPress(bool state, HASwitch* sender) {
            Serial.print("Override - ");
            Serial.println(state);
            buttons[sender]->isOverridden = state;
            buttons[sender]->slider.setAvailability(state);
            //buttons[sender]->slider. (state);
            sender->setState(state);
        }
};
