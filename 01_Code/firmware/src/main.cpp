#include "AI.h"
#include "LED/LEDModule.h"
#include "Magnetometer/Magnetometer.h"
#include "Orientation.h"
#include "OrientationLED.h"
#include "Places.h"
#include "Weather.h"

#include <Arduino.h>
#include <GPS/GPS_Module/GPSModule.h>
#include <WiFi.h>

const char* ssid     = "";
const char* password = "";
double      destLat;
double      destLng;
double      refHeading = NAN;
#define BUTTON_PIN 14

int           buttonState;
int           lastButtonState  = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay    = 50;
int           pressCount       = 0;
bool          countingActive   = false;
unsigned long startTime        = 0;
bool          windowFinished   = false;
double        latitude         = 0;
double        longitude        = 0;
bool          queryGeminiFlag  = false;
GpsFix        fix;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Waiting for first button press...");

    // Connect WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("Retry connecting the WIFI!");
    }
    Serial.println("\nWiFi Connected!");

    initLED();
    initGPS();
    xTaskCreate(taskGPS, "GPS", 4096, nullptr, 1, nullptr);

    if (!Magnetometer::begin()) {
        Serial.println("Magnetometer init failed!");
        while (1)
            delay(100);
    }

    showColor(0, 255, 0);
    Magnetometer::startCalibration();
    delay(10000);
    Magnetometer::stopCalibration();
    refHeading = Magnetometer::headingDeg();
    turnOff();

    Serial.printf("%.2f°\n", refHeading);
}

void loop() {
    int reading = digitalRead(BUTTON_PIN);
    if (reading != lastButtonState)
        lastDebounceTime = millis();

    if (queryGeminiFlag == false) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != buttonState) {
                buttonState = reading;

                if (buttonState == LOW) {
                    if (!countingActive && !windowFinished) {
                        countingActive = true;
                        startTime      = millis();
                        pressCount     = 0;
                        Serial.println("Started 5-second counting window!");
                    } else if (countingActive) {
                        pressCount++;
                    }
                    Serial.print("Press count: ");
                    Serial.println(pressCount);
                }
            }
        }

        if (countingActive && (millis() - startTime > 5000)) {
            countingActive = false;
            windowFinished = true;

            while (latitude == 0 && longitude == 0) {
                Serial.printf("Getting the lat/long again");
                latitude  = getLat();
                longitude = getLng();
                delay(1000);
            }

            Serial.println("Time's up! Final press count locked: " + String(pressCount));

            // Fetch Weather
            String weatherCondition, localTime;
            fetchWeather(weatherCondition, localTime, latitude, longitude);

            // Query Gemini
            String filteredPlaceTypes = queryGemini(weatherCondition, localTime);

            // Query Places API
            fetchNearbyPlace(filteredPlaceTypes, pressCount, latitude, longitude, destLat, destLng);
            queryGeminiFlag = true;
        }
        lastButtonState = reading;

    } else {
        if (getLatestFix(fix) && fix.valid) {
            NavigationData nav = computeNavigation(fix.lat, fix.lng, destLat, destLng);
            handleNavigationLED(nav);

            OrientationResult o =
                    computeOrientation(fix.lat, fix.lng, destLat, destLng, refHeading);
            displayOrientationLED(o);
        }

        OrientationResult o = computeOrientation(fix.lat, fix.lng, destLat, destLng, refHeading);
        displayOrientationLED(o);
        delay(500);
    }
}
