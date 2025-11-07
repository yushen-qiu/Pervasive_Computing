// Main application: WiFi, sensors, button, APIs, LEDs
#include "LED/LEDModule.h"
#include "apis/AI.h"
#include "apis/Places.h"
#include "apis/Weather.h"
#include "sensors/GPS.h"
#include "sensors/Magnetometer.h"
#include "sensors/Orientation.h"

#include <Arduino.h>
#include <WiFi.h>
#include <config/Pins.h>
#include <config/Secrets.h>

double destLat;
double destLng;
double refHeading = NAN;

int           buttonState;
int           lastButtonState  = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay    = 50;

int           pressCount     = 0;
bool          countingActive = false;
unsigned long startTime      = 0;
bool          windowFinished = false;

double latitude  = 0;
double longitude = 0;

bool   apiFlag = false;
GpsFix fix;

String weatherCondition, localTime;

bool demoFixStart  = true;
bool demoFixTarget = true;

void setup() {
    Serial.begin(115200);
    pinMode(Pins::BUTTON, INPUT_PULLUP);

    Serial.println("[STARTING THE SETUP]");

    initLED();
    turnOffLED();

    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    Serial.println("[CONNECTING WIFI]");
    while (WiFi.status() != WL_CONNECTED) {
        delay(2000);
        Serial.println(">> retrying WIFI!");
    }

    Serial.println("\n[WIFI CONNECTED]\n");

    initGPS();
    xTaskCreate(taskGPS, "GPS", 4096, nullptr, 1, nullptr);

    showColorLED(0, 255, 0);
    if (!Magnetometer::begin())
        Serial.println("[ERROR] Magnetometer init failed");
}

void loop() {
    if (Magnetometer::isCalibrating()) {
        delay(50);
        return;
    } else {
        turnOffLED();
        refHeading = Magnetometer::headingDeg();
    }

    int reading = digitalRead(Pins::BUTTON);
    if (reading != lastButtonState)
        lastDebounceTime = millis();

    if (apiFlag == false) {
        showColorLED(255, 165, 0);

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
                        turnOffLED();
                        delay(50);
                        showColorLED(255, 165, 0);
                    }

                    Serial.print("Press count: ");
                    Serial.println(pressCount);
                }
            }
        }

        if (countingActive && (millis() - startTime > 5000)) {
            showColorLED(255, 0, 0);
            countingActive = false;
            windowFinished = true;

            while (latitude == 0 && longitude == 0) {
                Serial.println("Getting the lat/long again");
                latitude  = getLat();
                longitude = getLng();

                showColorLED(0, 0, 0);
                delay(1000);
                showColorLED(255, 0, 0);
            }

            showColorLED(255, 255, 255);
            Serial.println("Final press count: " + String(pressCount));

            // Fetch Weather
            fetchWeather(weatherCondition, localTime, latitude, longitude);

            // Query Gemini
            String filteredPlaceTypes = queryGemini(weatherCondition, localTime);

            // Select destination
            if (demoFixTarget) {
                destLat = -33.8899019914124;
                destLng = 151.19237615118595;
                Serial.printf("[DEMO] Using fixed destination: %.8f, %.8f\n", destLat, destLng);
            } else {
                // Query Places API
                fetchNearbyPlace(filteredPlaceTypes, pressCount, latitude, longitude, destLat,
                                 destLng);
            }
            apiFlag = true;
        }

        lastButtonState = reading;
        return;
    }

    if (getLatestFix(fix) && fix.valid) {
        NavigationData nav = computeNavigation(fix.lat, fix.lng, destLat, destLng);
        handleNavigationLED(nav);

        OrientationResult o = computeOrientation(fix.lat, fix.lng, destLat, destLng, refHeading);
        displayOrientationLED(o);
    }

    delay(500);
}
