#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

extern HardwareSerial GPS;
extern TinyGPSPlus    gps;

extern volatile double currentCourse;
extern volatile double currentSpeed;
extern volatile double currentLat;
extern volatile double currentLng;

void initGPS();
void taskGPS(void* pvParameters);
