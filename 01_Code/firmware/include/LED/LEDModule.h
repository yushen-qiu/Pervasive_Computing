// LED ring/strip control (FastLED)
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <config/Pins.h>
#include <sensors/GPS.h>
#include <sensors/Navigation.h>
#include <sensors/Orientation.h>

// LED hardware configuration
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS 150

void initLED();

void displayOrientationLED(const OrientationResult& o);

void handleNavigationLED(const NavigationData& nav);
void turnOffLED();
void showColorLED(int R, int G, int B);
