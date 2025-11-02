#include <app/App.h>
#include <config/Config.h>
#include <net/AiClient.h>
#include <net/PlacesClient.h>
#include <net/WeatherClient.h>
#include <net/WiFiUtil.h>
#include <sensors/LightSensor.h>
#include <sensors/GPSModule.h>

void App::begin() {
    Serial.begin(115200);
    pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Waiting for first button press...");

    // Connect WiFi (non-blocking beyond configured timeout)
    (void)WiFiUtil::connect(Config::WIFI_SSID, Config::WIFI_PASSWORD,
                            Config::WIFI_CONNECT_TIMEOUT_MS);

    // Start light sensor calibration (5s)
    LightSensor::begin(5000);

    // Start GPS (serial + background task)
    initGPS();
    xTaskCreate(taskGPS, "GPS", 4096, nullptr, 1, nullptr);
}

void App::tick() {
    // Update light sensor reading and log periodically
    LightSensor::tick();
    if (millis() - lastLightPrint_ > 1000) {
        lastLightPrint_ = millis();
    }

    int reading = digitalRead(Config::BUTTON_PIN);

    if (reading != lastButtonState_) {
        lastDebounceTime_ = millis();
    }

    if ((millis() - lastDebounceTime_) > Config::BUTTON_DEBOUNCE_MS) {
        if (reading != buttonState_) {
            buttonState_ = reading;

            if (buttonState_ == LOW) { // pressed
                if (!countingActive_ && !windowFinished_) {
                    countingActive_ = true;
                    startTime_      = millis();
                    pressCount_     = 0;
                    Serial.println("Started 5-second counting window!");
                } else if (countingActive_) {
                    pressCount_++;
                }
                Serial.print("Press count: ");
                Serial.println(pressCount_);
            }
        }
    }

    if (countingActive_ && (millis() - startTime_ > Config::COUNT_WINDOW_MS)) {
        countingActive_ = false;
        windowFinished_ = true;
        Serial.println("Time's up! Final press count locked: " + String(pressCount_));

        runFlow();

        // Reset for next round after a short pause
        delay(1000);
        windowFinished_ = false;
        pressCount_     = 0;
        Serial.println("Ready for next input.");
    }

    lastButtonState_ = reading;
}

void App::runFlow() {
    // Use GPS coordinates if we have a valid fix; otherwise fall back to defaults
    double latitude  = Config::DEFAULT_LAT;
    double longitude = Config::DEFAULT_LNG;

    GpsFix fix;
    if (getLatestFix(fix) && fix.valid) {
        latitude  = fix.lat;
        longitude = fix.lng;
    }

    // Fetch Weather
    String weatherCondition, localTime;
    fetchWeather(weatherCondition, localTime, latitude, longitude);

    // Query Gemini
    String filteredPlaceTypes = queryGemini(weatherCondition, localTime);

    // Query Places API
    fetchNearbyPlace(filteredPlaceTypes, pressCount_, latitude, longitude, destLat_, destLng_);

    if (destLat_ != 0 || destLng_ != 0) {
        Serial.printf("Destination: %.6f, %.6f\n", destLat_, destLng_);
    }
}
