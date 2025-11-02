// Lightweight WiFi helper for connecting with a timeout
#pragma once
#include <Arduino.h>

namespace WiFiUtil {
// Attempts to connect to WiFi within the given timeoutMs.
// Returns true on success, false on timeout/failure. Prints progress to Serial.
bool connect(const char* ssid, const char* password, unsigned long timeoutMs);
}

