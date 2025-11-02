#pragma once
#include <Arduino.h>

// Lightweight snapshot of latest GPS state
struct GpsFix {
    bool   valid      = false;  // true if we have a valid location
    double lat        = 0.0;    // degrees
    double lng        = 0.0;    // degrees
    double speed_mps  = 0.0;    // metres per second
    double course_deg = 0.0;    // degrees (0..360)
};

// Initialise GPS serial (UART2) with pins/baud from Config
void initGPS();

// Background FreeRTOS task that reads GPS and updates the latest fix
void taskGPS(void* pvParameters);

// Thread-safe accessor to fetch a copy of the latest fix
bool getLatestFix(GpsFix& out);

