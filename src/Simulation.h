#include <Arduino.h>
#include <HAMqtt.h>
#include <string>

const char* EVENT_NAMES[] = {"winter", "night", "summer", "shower", "day"};

enum events {
	SIM_WINTER,
	SIM_NIGHT,
	SIM_SUMMER,
	SIM_SHOWER,
	SIM_DAY,
	EVENTS_LENGTH
};

class Simulation {
	
public:
	Simulation(HAMqtt *mqtt_) {
		mqtt = mqtt_;
	}

	bool checkStatus(int event) {
		return eventStatus[event];
	}

	int checkDuration(int event) {
		return millis() - eventStart[event];
	}

	void simulationSetup() {
		for (int i = 0; i < EVENTS_LENGTH; i++) {
			std::string topic = std::string("simulated/") + EVENT_NAMES[i];
			mqtt->subscribe(topic.c_str());
		}
		Serial.println("Simulation - Setup complete");
	}

	void handleSimulation(const char *topic, const uint8_t *payload, uint16_t length) {
		Serial.print("Simulation - Got message ");
		Serial.print(topic);
		Serial.print(" : ");

		char payload_data[256];
		strncpy(payload_data, (const char *)payload, length);
		payload_data[length] = '\0';

		Serial.println((const char *)payload_data);

		for (int i = 0; i < EVENTS_LENGTH; i++) {
			std::string event = std::string("simulated/") + EVENT_NAMES[i];
			if (strcmp(topic, event.c_str()) == 0) {
				if (strcmp(payload_data, "ON") == 0) {
					eventStatus[i] = true;
					eventStart[i] = millis();
				} 
				else {
					eventStatus[i] = false;
				}
				Serial.println("Simulation - Message handled!");
				return;
			}
		}
		Serial.println("Simulation - Message unhandled!");
	}

private:
	bool eventStatus[EVENTS_LENGTH];
	unsigned long eventStart[EVENTS_LENGTH];
	HAMqtt* mqtt;
};

