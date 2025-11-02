// Copy this file to Secrets.h and fill in your values.
#pragma once

namespace Config {
    // WiFi credentials
    static const char* WIFI_SSID     = "YOUR_WIFI_SSID";
    static const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

    // API keys
    static const char* GEMINI_API_KEY  = "YOUR_GEMINI_API_KEY";
    static const char* PLACES_API_KEY  = "YOUR_PLACES_API_KEY";
    static const char* WEATHER_API_KEY = "YOUR_WEATHER_API_KEY";
} // namespace Config

#define CONFIG_SECRETS_INCLUDED 1
