#include "./GPS/GPSModule.h"
#include "./LED/LEDModule.h"

#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    initGPS();
    initLED();

    Serial.println("GPS + LED Compass started. Move to open sky...");

    xTaskCreatePinnedToCore(taskGPS, "taskGPS", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskLED, "taskLED", 4096, NULL, 1, NULL, 0);
}

void loop() {}

// #include <WiFi.h>
// #include "Weather.h"
// #include "AI.h"
// #include "Places.h"

// // WiFi Config
// const char* ssid = "";
// const char* password = "";

// // Coordinates 
// double latitude = -33.8884;
// double longitude = 151.1868;

// double destinationLatitude;
// double destinationLongitude;

// // Button Config 
// #define BUTTON_PIN 14
// int buttonState;
// int lastButtonState = HIGH;
// unsigned long lastDebounceTime = 0;
// unsigned long debounceDelay = 50;

// int pressCount = 0;
// bool countingActive = false;
// unsigned long startTime = 0;
// bool windowFinished = false;

// void setup() {
// 	Serial.begin(115200);
// 	pinMode(BUTTON_PIN, INPUT_PULLUP);
// 	Serial.println("Waiting for first button press...");

// 	// Connect WiFi
// 	WiFi.begin(ssid, password);
// 	Serial.print("Connecting to WiFi");
// 	while (WiFi.status() != WL_CONNECTED) {
// 		delay(500);
// 		Serial.print(".");
// 	}
// 	Serial.println("\nWiFi Connected!");
// }

// void loop() {
// 	int reading = digitalRead(BUTTON_PIN);

// 	if (reading != lastButtonState) lastDebounceTime = millis();

// 	if ((millis() - lastDebounceTime) > debounceDelay) {
// 		if (reading != buttonState) {
// 			buttonState = reading;

// 			if (buttonState == LOW) {
// 				if (!countingActive && !windowFinished) {
// 					countingActive = true;
// 					startTime = millis();
// 					pressCount = 0;
// 					Serial.println("Started 5-second counting window!");
// 				} else if (countingActive) {
// 					pressCount++;
// 				}
// 				Serial.print("Press count: "); Serial.println(pressCount);
// 			}
// 		}
// 	}

// 	if (countingActive && (millis() - startTime > 5000)) {
// 		countingActive = false;
// 		windowFinished = true;
// 		Serial.println("Time's up! Final press count locked: " + String(pressCount));

// 		// Fetch Weather 
// 		String weatherCondition, localTime;
// 		fetchWeather(weatherCondition, localTime, latitude, longitude);

// 		// Query Gemini 
// 		String filteredPlaceTypes = queryGemini(weatherCondition, localTime);

// 		// Query Places API 
// 		fetchNearbyPlace(filteredPlaceTypes, pressCount, latitude, longitude, destinationLatitude, destinationLongitude);
// 	}

// 	lastButtonState = reading;
// }

