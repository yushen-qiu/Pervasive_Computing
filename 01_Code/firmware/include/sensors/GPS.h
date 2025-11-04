#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

extern HardwareSerial GPS;
extern TinyGPSPlus    gps;

struct GpsFix {
    bool   valid      = false;
    double lat        = 0.0;
    double lng        = 0.0;
    double speed_mps  = 0.0;
    double course_deg = 0.0;
};

inline bool getLatestFix(GpsFix& out) {
    extern volatile double currentLat;
    extern volatile double currentLng;
    extern volatile double currentSpeed;
    extern volatile double currentCourse;

    out.lat        = currentLat;
    out.lng        = currentLng;
    out.speed_mps  = currentSpeed;
    out.course_deg = currentCourse;

    out.valid = !(out.lat == 0.0 && out.lng == 0.0);
    return out.valid;
}

extern volatile double currentCourse;
extern volatile double currentSpeed;
extern volatile double currentLat;
extern volatile double currentLng;

void   initGPS();
void   taskGPS(void* pvParameters);
double getLng();
double getLat();
