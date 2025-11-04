#pragma once
#include "GPS/GPS_Module/GPSModule.h"
#include <Orientation.h>

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 16
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS 150

extern CRGB leds[NUM_LEDS];

void initLED();
void clearLED();
void showDirection(double bearingDeg);

// Orientation display (moved from OrientationLED.cpp)
void displayOrientationLED(const OrientationResult& o);

#pragma once
#include "GPS/Navigation/Navigation.h"
#include "LED/LEDModule.h"

#include <Arduino.h>
#include <FastLED.h>

void handleNavigationLED(const NavigationData& nav);
void breatheGreen();
void turnOff();
void showGreen();
void showColor(int R, int G, int B);
