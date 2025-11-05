/*
 * LED Module (API)
 * ---------------------------------
 * Public interface for LED ring/strip control using FastLED.
 */
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

// Init and basic control
void initLED(); // initialize FastLED and buffers

// Orientation display (moved from OrientationLED.cpp)
void displayOrientationLED(const OrientationResult& o);

// Navigation LED helpers
void handleNavigationLED(const NavigationData& nav);
void turnOffLED();
void showColorLED(int R, int G, int B);
