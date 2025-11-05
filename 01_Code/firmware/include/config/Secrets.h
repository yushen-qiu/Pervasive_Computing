/*
 * Secrets (Local)
 * ---------------------------------
 * WiFi credentials and API keys. Do NOT commit real values.
 */
#pragma once

namespace Config {
    // Fill these with your actual credentials
    static const char* WIFI_SSID     = ""; // e.g., "MyWiFi"
    static const char* WIFI_PASSWORD = ""; // e.g., "SuperSecretPass"

    static const char* GEMINI_API_KEY  = "";
    static const char* PLACES_API_KEY  = "";
    static const char* WEATHER_API_KEY = "";
} // namespace Config

#define CONFIG_SECRETS_INCLUDED 1
