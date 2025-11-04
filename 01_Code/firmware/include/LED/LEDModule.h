#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <sensors/GPS.h>
#include <sensors/Orientation.h>
#include <sensors/Navigation.h>

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

// Navigation LED helpers
void handleNavigationLED(const NavigationData& nav);
void turnOffLED();
void showColorLED(int R, int G, int B);
// Breathing effect in the given RGB color. Blocks forever.
void breatheColorForever(int R, int G, int B);
