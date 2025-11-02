// Project configuration (non-secrets) + secrets include.
#pragma once

#include <config/Secrets.h>

#ifndef CONFIG_SECRETS_INCLUDED
#error "Missing include/config/Secrets.h. Copy include/config/Secrets.example.h to include/config/Secrets.h and fill your values."
#endif

namespace Config {
    // Service URLs and identifiers
    static const char* URL_GEMINI_BASE = "https://generativelanguage.googleapis.com/v1beta";
    static const char* GEMINI_MODEL    = "gemini-2.5-flash";
    static const char* URL_PLACES_SEARCH_NEARBY =
            "https://places.googleapis.com/v1/places:searchNearby";
    static const char* URL_WEATHER_CURRENT = "http://api.weatherapi.com/v1/current.json";

    // App pins
    static const int BUTTON_PIN       = 14; // Default button pin
    static const int LIGHT_SENSOR_PIN = 36; // ADC input (GPIO36)

    // LED configuration
    static constexpr int LED_PIN          = 4;
    static constexpr int NUM_LEDS         = 16;
    static constexpr int LED_BRIGHTNESS   = 150;
    static constexpr int LED_NORTH_OFFSET = 4; // index offset so LED at "front" is north

    // Defaults
    static const double DEFAULT_LAT = -33.8884;
    static const double DEFAULT_LNG = 151.1868;

    // Timeouts and delays (ms)
    static const unsigned long HTTP_TIMEOUT_MS         = 30000;
    static const unsigned long WIFI_CONNECT_TIMEOUT_MS = 30000;
    static const unsigned long BUTTON_DEBOUNCE_MS      = 50;
    static const unsigned long COUNT_WINDOW_MS         = 5000;

    // Places radius scaling (metres per button press)
    static const int PLACES_RADIUS_PER_PRESS_M = 250;

    // GPS (for simple GPS_Coords demo)
    static const int           GPS_RX_PIN         = 16;
    static const int           GPS_TX_PIN         = 17;
    static const unsigned long GPS_BAUD           = 9600;
    static const unsigned long GPS_FIX_TIMEOUT_MS = 2000;
} // namespace Config
