#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Adafruit_MMC56x3.h>
#include <Arduino.h>

struct HeadingData {
    float  heading;
    String direction;
};

void setupMagnetometer();
void startCalibration();

HeadingData getMagnetometerReading();

extern float northOffset;
extern bool calibrating;

#endif

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