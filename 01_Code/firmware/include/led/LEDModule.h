#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <Arduino.h>
#include <FastLED.h>
#include <config/Config.h>

// Keep LED_TYPE and COLOR_ORDER here for FastLED template parameters
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

extern CRGB leds[Config::NUM_LEDS];

void initLED();
void taskLED(void* pvParameters);

// Alternatively, set the absolute bearing to the target (degrees from true north)
void setTargetBearing(double bearingDeg);

// Provide target coordinates so LED can compute distance to target
void setTargetCoords(double latDeg, double lngDeg);

#endif
