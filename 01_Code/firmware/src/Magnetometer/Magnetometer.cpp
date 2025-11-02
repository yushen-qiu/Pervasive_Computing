// #include <Adafruit_MMC56x3.h>
// #include <Arduino.h>
#include <Wire.h>
#include "Magnetometer.h"

Adafruit_MMC5603 mag = Adafruit_MMC5603(12345);

// Magnetic declination (Sydney ~12.82° East)
float declinationAngle = 12.82;

// Calibration data
float mag_min[3] = {10000, 10000, 10000};
float mag_max[3] = {-10000, -10000, -10000};
float offset[3]  = {0, 0, 0};
float scale[3]   = {1, 1, 1};

// Heading offset for manual north correction
float northOffset = 0.0; // degrees

// Modes
bool          calibrating = true;
unsigned long lastPrint   = 0;

void computeCalibration();

// Generalize conversion of bearings to simple cardinal directions (e.g. N,S,E,W)
String getDirection(float heading) {
    int sector = (int)((heading + 45) / 90);
    sector     = sector % 4;
    switch (sector) {
        case 0: return "North";
        case 1: return "East";
        case 2: return "South";
        case 3: return "West";
        default: return "Unknown";
    }
}

// Checks connection with Magnetometer
void setupMagnetometer() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("MMC5603 Magnetometer Calibration & Heading with North Offset");

    if (!mag.begin(MMC56X3_DEFAULT_ADDRESS, &Wire)) {
        Serial.println("No MMC5603 detected!");
        while (1);
    }

    Serial.println("\nRotate the sensor slowly in all directions to calibrate.");
    Serial.println("Press any key in Serial Monitor when done to lock calibration.\n");
    Serial.println("After calibration, point sensor to actual north and press 'n' to set north offset.\n");
}

HeadingData getMagnetometerReading() {
    sensors_event_t magEvent;
    mag.getEvent(&magEvent);

    // Init data
    HeadingData data = {0.0, "Unknown"};

    float mx = magEvent.magnetic.x;
    float my = magEvent.magnetic.y;
    float mz = magEvent.magnetic.z;

    // Update minmax calibration values
    if (calibrating) {
        if (mx < mag_min[0]) mag_min[0] = mx;
        if (my < mag_min[1]) mag_min[1] = my;
        if (mz < mag_min[2]) mag_min[2] = mz;

        if (mx > mag_max[0]) mag_max[0] = mx;
        if (my > mag_max[1]) mag_max[1] = my;
        if (mz > mag_max[2]) mag_max[2] = mz;

        // Calibration Mode: Print live values every 500 ms
        if (millis() - lastPrint > 500) {
            lastPrint = millis();
            Serial.print("Raw X:"); Serial.print(mx, 2);
            Serial.print(" Y:");    Serial.print(my, 2);
            Serial.print(" Z:");    Serial.println(mz, 2);
            Serial.println("Keep rotating...");
        }
        // End calibration: Upon any input in Serial Monitor
        if (Serial.available()) {
            calibrating = false;
            Serial.read(); // Buffer clear
            computeCalibration();
        }
        return data;
    } 
    // Apply calibration
    float mx_c = (mx - offset[0]) * scale[0];
    float my_c = (my - offset[1]) * scale[1];
    float mz_c = (mz - offset[2]) * scale[2];

    // Compute heading
    float heading = atan2(mx_c, my_c) * 180.0 / PI;
    if (heading < 0) heading += 360.0;

    // Apply declination & north offset
    heading += declinationAngle + northOffset;
    while (heading < 0)      heading += 360.0;
    while (heading >= 360.0) heading -= 360.0;

    // Update return data:
    data.heading    = heading;
    data.direction  = getDirection(heading);

    // Check Input ('n'): Manual north calibration
    if (Serial.available()) {
        char c = Serial.read();

        // Set current direction as north
        if (c == 'n') { 
            northOffset = 0.0 - heading;
            while (northOffset > 180.0)     northOffset -= 360.0;
            while (northOffset <= -180.0)   northOffset += 360.0;
            Serial.print("Manual north set. Heading offset = ");
            Serial.println(northOffset, 2);
        }

        // Reset offset
        if (c == 'r') { 
            northOffset = 0.0;
            Serial.println("North Heading offset reset to 0°");
        }
    }

    return data;
}

// Calibration Phase
void startCalibration() {
    calibrating = true;
}

// Compute calibration offsets and scales
void computeCalibration() {
    // Normalize scaling
    float avg_range = ((mag_max[0] - mag_min[0]) + (mag_max[1] - mag_min[1]) + (mag_max[2] - mag_min[2])) / 6.0;
    
    for (int i = 0; i < 3; i++) {
        // Hard-iron correction
        offset[i]   = (mag_max[i] + mag_min[i]) / 2.0;

        // Soft-iron correction
        float range = (mag_max[i] - mag_min[i]) / 2.0;
        scale[i]    = avg_range / range;
    }

    Serial.println("\nCalibration complete!");
    Serial.println("Offsets (µT):");
    Serial.print("X: ");    Serial.print(offset[0], 2);
    Serial.print("  Y: ");  Serial.print(offset[1], 2);
    Serial.print("  Z: ");  Serial.println(offset[2], 2);

    Serial.println("Scales:");
    Serial.print("X: ");    Serial.print(scale[0], 3);
    Serial.print("  Y: ");  Serial.print(scale[1], 3);
    Serial.print("  Z: ");  Serial.println(scale[2], 3);

    Serial.println("\nNow showing calibrated heading...");
}