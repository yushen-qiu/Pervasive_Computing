#pragma once
#include <Arduino.h>

namespace WiFiUtil {
    bool connect(const char* ssid, const char* password, unsigned long timeoutMs);
}
