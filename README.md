# Showroom Wall Tools

This is a collection of helper classes for using the ArduinoHA integration. They were specifically created for the internship at Wildau Robotic Lab.

## Installation

Add [https://github.com/TrueJole/showroom-wall-tools.git](https://github.com/TrueJole/showroom-wall-tools.git) to your `platformio.ini` file under `lib_depends = `.

## Usage

### Simulation.h

Allows the ESP to recieve simulated events from the HomeAssistant Dashboard.

```c++
#include <Arduino.h>
#include <ArduinoHA.h>
#include <WiFi.h>

// Import the Simulation library
#include <Simulation.h>

WiFiClient network;
HADevice device("my-esp");
HAMqtt mqtt(network, device);

// Create the Simulation object and give the mqtt object as a parameter
Simulation simulation(&mqtt);

void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length) {
	// When recieving a message, forward it to the simulation object
	simulation.handleSimulation(topic, payload, length);
}

void onMqttConnected() {
	// Subcribe to the simulation topics every time the esp connects to the broker
	simulation.simulationSetup();
}

void setup() {
	Serial.begin(115200);
	
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("ESP - Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	
	// Register the callbacks with the mqtt object
	mqtt.onConnected(onMqttConnected);
	mqtt.onMessage(onMqttMessage);
	
	mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_USERNAME, MQTT_PASSWORD);
}

void loop() {
	mqtt.loop();
	
	// Check if a simulated event is ON
	if (simulation.checkStatus(SIM_WINTER)) {
		Serial.print("Its been winter for ");
		
		// Get the time since the last event change (ON or OFF)
		Serial.print(simulation.checkDuration(SIM_WINTER));
		Serial.println(" milliseconds!");
	}
	delay(1000);
}
```

### Lamp

Easy setup and handling of Lamps directly to HomeAssistant. Works with Neopixel LEDs.

```c++
#include <Lamp.h>

#define LED_PIN 6
#define NUMBER_OF_LEDS 3
Lamp lamp("Lamp name for HA", "unique-lamp-id", LED_PIN, NUMBER_OF_LEDS);

void setup() {
	// Initialise the lamp (required)
	lamp.begin();
	
	// Manually change the lamp color and brightness (optional)
	lamp.setColor(255, 255, 255);
	lamp.setBrightness(100);
}
```

### Cover

*WIP*
Easy setup and handling of a servo motor acting as any kind of HomeAssistant Cover (blinds, windows, doors, ...).

```c++
#include <Cover.h>

#define SERVO_PIN 6
#define IS_OPEN_AT_PROGRAM_START true
Cover cover("Cover name for HA", "unique-cover-id", SERVO_PIN, IS_OPEN_AT_PROGRAM_START);

void setup() {
	// Manually open and close
	cover.open();
	delay(5000);
	cover.close();
	
	// get the state, true is open
	cover.getState();
}
```

### Fan

A helper class for HVAC without any HA integration. Allows controlling a fan.

```c++
#include <Fan.h>

#define FAN_PIN 6
Fan fan(FAN_PIN);

void setup() {
	// Set the fan speed (0-100)
	fan.setSpeed(100);
	
	// turn the fan on (true) and off (false)
	fan.turnOnOff(true);
	fan.turnOnOff(false);
}
```

### HVAC

Uses a fan and LEDs to simulate an air conditioning / heating unit. Easy integration in HomeAssistant.

```c++
#include <HVAC.h>

#define FAN_PIN 6
#define LED_PIN 6
#define NUMBER_OF_LEDS 3

HVAC hvac("HVAC name for HA", "unique-hvac-id", NUMBER_OF_LEDS, LED_PIN, FAN_PIN);

void onMqttConnectedToBroker() {
	// Workaround for a bug with ArduinoHA
	// Set the target temperature once when connecting to the broker
	// to be able to change it in the HomeAssistant dashboard
	hvac.setTargetTemperature((u8_t)24);
}

void setup() {
	// Initialise the HVAC (required)
	hvac.begin();
	
	// Manually setting the target temperature in Celsius
	hvac.setTargetTemperature(24);
	
	// Setting the current temperature (for example from a thermometer)
	hvac.setCurrentTemperature(20);
	
	// Manually set the mode
	hvac.setMode(HAHVAC::OFF_MODE);
}

void loop() {
	// call loop regulary to animate the led and allow the auto mode to work
	hvac.loop();
}
```