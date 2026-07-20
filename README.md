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

### Lamp.h

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
	
	// Manually changing the on/off state (on = true)
	lamp.turnOnOff(true);
	
	// Switch the on/off state
	lamp.turnSwitch();
	
	// Get the lamp state
	lamp.getState();
}
```

### Cover.h

Easy setup and handling of a servo motor acting as any kind of HomeAssistant Cover (blinds, windows, doors, ...).

```c++
#include <Cover.h>

#define SERVO_PIN 6

// What kind of motor the cover uses
#define CONTINOUS_MOTOR_TYPE Cover::TYPE_NO_LIMIT
#define SERVO_MOTOR_TYPE Cover::TYPE_SERVO

Cover cover("Cover name for HA", "unique-cover-id", SERVO_PIN, TYPE_SERVO);

void setup() {
	// Configure the cover for TYPE_NO_LIMIT
	// Set duration of motor run time
	cover.setDuration(2000);
	// Set absolute speed (max 90)
	cover.setSpeed(90);
	
	// Configure the cover for TYPE_SERVO
	#define OPEN_ANGLE 180
	#define CLOSE_ANGLE 0
	cover.setAngles(OPEN_ANGLE, CLOSE_ANGLE);
	
	// Set the HA state without activating the motor
	covor.setState(true);

	// Manually open and close
	cover.open();
	delay(5000);
	cover.close();
	
	// get the state, true is open
	cover.getState();
}
```

### Fan.h

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

### HVAC.h

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

### Button2.h
(Button.h is used for manual control without HA integration only and is deprecated)
Connects a button to HomeAssistant.

```c++
#include <Button2.h>

#define BUTTON_PIN 6

// define how the button is connected to the board (pull up = true, pull down = false)
#define PULLUP_BUTTON true

// optionally set the device class tp change the way the button is displayed in HA
// default is "connectivity"
#define DEVICE_CLASS "plug"

Button2 button("Button name for HA", "unique-button-id", BUTTON_PIN, PULLUP_BUTTON, DEVICE_CLASS);

void setup() {
	// Check if the button is currently pressed
	button.isPressed();
	
	// Manually imitate a button press
	#define DURATION 1000
	button.manualPress(DURATION);
	
	// Check if the button was pressed at any time since the last call
	button.handlePress();
}

void loop() {
	// Call this as often as possible
	button.loop();
}
```

### Alarm.h
Use a buzzer to playback sounds. Connects to HomeAssistant.

```c++
#include <Alarm.h>

#define BUZZER_PIN 6

// optionally define a default frequency to use
#define DEFAULT_FREQUENCY 3000

// optionally define a default duration to use
// the buzzer will always play for this duration, then pause for this duration
#define DEFAULT_DURATION 500

Alarm myAlarm("Alarm name for HA", "unique-alarm-id", BUZZER_PIN, DEFAULT_FREQUENCY, DEFAULT_DURATION);

void mySequence(Alarm *alarm) {
	// In the overidden sequence, you may use different lengths and frequencies
	// Keep in mind that a long manual sequence will block other parts of your code
	alarm->playManually(2637, 500);
	delay(50);
	alarm->playManually(2093, 1100);
	delay(200);
};

void setup() {
	// Change the default frequency
	myAlarm.setFrequency(DEFAULT_FREQUENCY);
	
	// Change the default duration
	myAlarm.setDuration(DEFAULT_DURATION);
	
	// Manually play a certain frequency for a certain duration
	myAlarm.playManually(DEFAULT_FREQUENCY, DEFAULT_DURATION);
	
	// Manually turn the alarm on (true) or off (false)
	myAlarm.turnOnOff(true);
	
	// Override the default playback sequence with a function pointer
	// This will be contiously called instead when the alarm is on
	myAlarm.overrideSequence(mySequence);
}

void loop() {
	// Call this as often as possible
	myAlarm.loop();
}
```

### Animation.h
WIP
Used to display bitmaps on an Adafruit SSD1306 Display. Also contains the ability to animate snowflakes.
```c++
#include <Animation.h>

// Currently, pins are predefined in the library and cannot be changed
#define CS_PIN 7
#define RST_PIN 0
#define DC_PIN 6
#define CLK_PIN 5
#define DATA_PIN 4

#define BYTES_PER_BITMAP 1024
#define END_FRAME 1024
#define BITMAP_WIDTH 64
#define BITMAP_HEIGHT 128
#define DISPLAY_COLOR SSD1306_WHITE
#define DISPLAY_ROTATION 1

Display display;
const unsigned char bitmaps [5][BYTES_PER_BITMAP] = {{...}, ...};

void setup() {
	display.animationSetup(DISPLAY_ROTATION);
	
	// Display a single image
	int START_FRAME = 0;
	int END_FRAME = 0;
	int FPS = 1;
  	display.drawAnimation(bitmaps[0], FPS, START_FRAME, END_FRAME, BYTES_PER_BITMAP, DISPLAY_COLOR, DISPLAY_ROTATION, BITMAP_WIDTH, BITMAP_HEIGHT);
  	
  	// Display an animation
  	int START_FRAME = 1;
	int END_FRAME =4;
	int FPS = 5;
  	display.drawAnimation(bitmaps[0], FPS, START_FRAME, END_FRAME, BYTES_PER_BITMAP, DISPLAY_COLOR, DISPLAY_ROTATION, BITMAP_WIDTH, BITMAP_HEIGHT);
  	
  	// Call this once to enable snowflakes
  	display.enableSnow();

  	// Call this once to disable snowflakes
  	display.disableSnow();
}

void loop() {
	// Call this as often as possible
	display.loop();
}
```