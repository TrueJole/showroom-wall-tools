#pragma once
#include <Arduino.h>
#include <HAMqtt.h>
#include <string>

const char* EVENT_NAMES[] = {"simulated/winter", "simulated/night", "simulated/summer", "simulated/shower", "simulated/day"};

enum class SIM_EVENTS {
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
		for (std::size_t i = 0; i < EVENTS_LENGTH; i++) {
			eventStatus[i] = false;
			eventStart[i] = millis();
		}
	}

	bool checkStatus(std::size_t event) {
		if (i > EVENTS_LENGTH-1) {
			return false;
		}
		return eventStatus[event];
	}

	uint64_t checkDuration(std::size_t event) {
		if (i > EVENTS_LENGTH-1) {
			return 0;
		}
		return millis() - eventStart[event];
	}

	void simulationSetup() {
		for (int i = 0; i < EVENTS_LENGTH; i++) {
			mqtt->subscribe(EVENT_NAMES[i]);
		}
		Serial.println("Simulation - Setup complete");
	}

	void handleSimulation(const char *topic, const uint8_t *payload, uint16_t length) {
		Serial.print("Simulation - Got message ");
		Serial.print(topic);
		Serial.print(" : ");

		char payload_data[length+1];
		strncpy(payload_data, (const char *)payload, length);
		payload_data[length] = '\0';

		Serial.println((const char *)payload_data);

		for (int i = 0; i < EVENTS_LENGTH; i++) {
			if (strcmp(topic, EVENT_NAMES[i].c_str()) == 0) {
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
	uint64_t eventStart[EVENTS_LENGTH];
	HAMqtt* mqtt;
};

