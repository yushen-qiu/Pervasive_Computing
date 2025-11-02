#pragma once
#include <Arduino.h>

namespace Magnetometer {

    // Initialise the MMC5603 over I2C. Returns true on success.
    bool begin(uint8_t i2c_addr = 0x30);

    // Polls the sensor and updates internal heading state. Call regularly.
    void update();

    // Latest heading in degrees (0..360). Returns NAN until first valid reading.
    float headingDeg();

    // Optional helpers
    void setDeclination(float deg); // Magnetic declination compensation (degrees)
    void setNorthOffset(float deg); // Manual north offset (degrees)

    // Calibration control: collect min/max then compute hard/soft iron compensation.
    void startCalibration();
    void stopCalibration();
    bool isCalibrating();

} // namespace Magnetometer
