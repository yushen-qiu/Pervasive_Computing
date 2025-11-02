#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>

// Snapshot of the latest GPS state (thread-safe accessor provided below)
struct GpsFix {
    bool   valid      = false;  // true if we have a valid location fix
    double lat        = 0.0;    // degrees
    double lng        = 0.0;    // degrees
    double speed_mps  = 0.0;    // metres per second
    double course_deg = 0.0;    // degrees (0..360)
};

// Initialize GPS serial and hardware
void initGPS();

// FreeRTOS task that continuously reads and parses GPS data
void taskGPS(void* pvParameters);

// Thread-safe accessor: copies the latest fix into 'out'.
// Returns 'true' if out.valid is true (valid location available).
bool getLatestFix(GpsFix& out);

#endif
