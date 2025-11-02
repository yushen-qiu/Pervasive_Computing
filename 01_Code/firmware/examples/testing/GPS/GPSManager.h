// moved to examples/testing
#pragma once
#include "GPSAutoBaud.h"
#include "GpsTracker.h"

#include <Arduino.h>

namespace GPSManager {
    void begin();
    void update();
} // namespace GPSManager
