#pragma once
#include <Arduino.h>
#include "GpsTracker.h"
#include "GPSAutoBaud.h"

namespace GPSManager
{
    void begin();
    void update();
}
