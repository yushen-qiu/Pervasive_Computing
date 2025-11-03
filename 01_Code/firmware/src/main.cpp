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
// 		fetchNearbyPlace(filteredPlaceTypes, pressCount, latitude, longitude, destinationLatitude,
// destinationLongitude);
// 	}

// 	lastButtonState = reading;
// }

// #include "GPS/GPS_Coords/GPS_Coords.h"
// #include "GPS/GPS_Module/GPSModule.h"
// #include "Magnetometer/Magnetometer.h"

// void setup() {
//     Serial.begin(115200);
//     initGPS();
//     setupMagnetometer();
// }

// void loop() {
//     String coords = GPS_Coords();
//     Serial.println(coords);

//     HeadingData headingData = getMagnetometerReading();
//     Serial.println("Heading: " + String(headingData.heading) +
//                    "°  Direction: " + String(headingData.direction));

//     delay(1000);
// }

#include <Arduino.h>
#include <GPS/GPS_Module/GPSModule.h>
#include <Integration/NavigationLED/NavigationLED.h>
#include <Integration/Orientation/Orientation.h>
#include <Integration/OrientationLED/OrientationLED.h>
#include <LED/LEDModule.h>
#include <Magnetometer/Magnetometer.h>

double refHeading = NAN;

void setup() {
    Serial.begin(115200);
    initLED();
    initGPS();
    xTaskCreate(taskGPS, "GPS", 4096, nullptr, 1, nullptr);
    if (!Magnetometer::begin()) {
        Serial.println("Magnetometer init failed!");
        while (1)
            delay(100);
    }
    Magnetometer::startCalibration();
    delay(10000);
    Magnetometer::stopCalibration();
    refHeading = Magnetometer::headingDeg();

    Serial.printf("%.2f°\n", refHeading);
}

void loop() {
    double destLat = -33.889938;
    double destLng = 151.192437;
    GpsFix fix;
    if (getLatestFix(fix) && fix.valid) {
        NavigationData nav = computeNavigation(fix.lat, fix.lng, destLat, destLng);
        handleNavigationLED(nav);

        OrientationResult o = computeOrientation(fix.lat, fix.lng, destLat, destLng, refHeading);
        displayOrientationLED(o);
    }

    OrientationResult o = computeOrientation(fix.lat, fix.lng, destLat, destLng, refHeading);
    displayOrientationLED(o);

    delay(500);
}
