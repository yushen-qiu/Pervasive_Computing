#pragma once

#include <Arduino.h>

namespace LightSensor {
    // Initialize sensor; starts an auto-calibration window.
    void begin(unsigned long calibrationMs = 5000);

    // Call periodically to update readings and finish calibration.
    void tick();

    // True once calibration window has elapsed.
    bool isCalibrated();

    // Returns last raw ADC value.
    int raw();

    // Returns mapped percentage [0..100] based on calibrated min/max.
    int percent();

    // Current calibrated bounds.
    int minValue();
    int maxValue();
} // namespace LightSensor
