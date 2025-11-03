#pragma once
#include <Arduino.h>

namespace Magnetometer {

    // Initialise the MMC5603 over I2C. Returns true on success.
    bool begin(uint8_t i2c_addr = 0x30);

    // Polls the sensor and updates internal heading state. Call regularly (module also
    // runs a background task, so this is optional for basic usage).
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

// String getDirection(float heading);

// Initialize the magnetometer. Returns true on success.
// bool initMagnetometer();

// // Start/stop calibration mode. While calibrating, the module will update
// // internal min/max values from calls to getBearing(), but getBearing() will
// // return a negative value to indicate no valid heading yet.
// void startCalibration();
// void stopCalibration();

// // Read the current bearing in degrees [0..360). Returns -1.0 if no valid
// // reading (for example while calibrating or on error).
// float getBearing();

// // Convert a bearing (degrees) to a simple cardinal string like "North",
// // "East", "South", "West".
// String getCardinal(float heading);

// // Set/get user-adjustable parameters
// void setNorthOffset(float degrees);
// void setDeclination(float degrees);

// // Retrieve calibration data (offsets and scales). Arrays must be size 3.
// void getCalibration(float outOffset[3], float outScale[3]);