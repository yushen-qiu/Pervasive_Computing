#pragma once
#include "GPS/GPS_Module/GPSModule.h"

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
