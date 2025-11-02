
// Main application: button-driven flow that fetches Weather -> AI -> Places.

#include <AI.h>
#include <Places.h>
#include <Weather.h>
#include <config/Config.h>

#include <Arduino.h>
#include <WiFi.h>

// Coordinates (could be replaced by GPS later)
static double latitude  = Config::DEFAULT_LAT;
static double longitude = Config::DEFAULT_LNG;

// Destination from Places API
static double destinationLatitude  = 0;
static double destinationLongitude = 0;

// Button debounce and counting window
static int                 buttonState      = HIGH;
static int                 lastButtonState  = HIGH;
static unsigned long       lastDebounceTime = 0;
static const unsigned long debounceDelay    = Config::BUTTON_DEBOUNCE_MS;

static int           pressCount     = 0;
static bool          countingActive = false;
static unsigned long startTime      = 0;
static bool          windowFinished = false;

static void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    if (millis() - start > Config::WIFI_CONNECT_TIMEOUT_MS) {
            Serial.println("\nWiFi connect timeout");
            return;
        }
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Waiting for first button press...");

    connectWiFi();
}

void loop() {
    int reading = digitalRead(Config::BUTTON_PIN);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == LOW) { // pressed
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

  if (countingActive && (millis() - startTime > Config::COUNT_WINDOW_MS)) {
        countingActive = false;
        windowFinished = true;
        Serial.println("Time's up! Final press count locked: " + String(pressCount));

        // Fetch Weather
        String weatherCondition, localTime;
        fetchWeather(weatherCondition, localTime, latitude, longitude);

        // Query Gemini
        String filteredPlaceTypes = queryGemini(weatherCondition, localTime);

        // Query Places API
        fetchNearbyPlace(filteredPlaceTypes, pressCount, latitude, longitude, destinationLatitude,
                         destinationLongitude);

        if (destinationLatitude != 0 || destinationLongitude != 0) {
            Serial.printf("Destination: %.6f, %.6f\n", destinationLatitude, destinationLongitude);
        }

        // Reset for next round after a short pause
        delay(1000);
        windowFinished = false;
        pressCount     = 0;
        Serial.println("Ready for next input.");
    }

    lastButtonState = reading;
}
