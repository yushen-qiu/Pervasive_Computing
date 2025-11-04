#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <sensors/GPS.h>
#include <sensors/Orientation.h>

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
#include "LED/LEDModule.h"
#include "sensors/Navigation.h"

#include <Arduino.h>
#include <FastLED.h>

void handleNavigationLED(const NavigationData& nav);
void breatheGreen();
void turnOff();
void showGreen();
void showColor(int R, int G, int B);
