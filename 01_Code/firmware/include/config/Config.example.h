// Copy this file to Config.h and fill in your values.
#pragma once

namespace Config {
// WiFi
static const char* WIFI_SSID     = "YOUR_WIFI_SSID";
static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// API keys
static const char* GEMINI_API_KEY = "YOUR_GEMINI_API_KEY";
static const char* PLACES_API_KEY = "YOUR_PLACES_API_KEY";
static const char* WEATHER_API_KEY = "YOUR_WEATHER_API_KEY";

// App pins
static const int BUTTON_PIN = 14;  // Default button pin

// Defaults (optional)
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
static const int      GPS_RX_PIN          = 16;
static const int      GPS_TX_PIN          = 17;
static const uint32_t GPS_BAUD            = 9600;
static const unsigned long GPS_FIX_TIMEOUT_MS = 2000;
}
