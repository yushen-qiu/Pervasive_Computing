#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 16
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS 150

extern CRGB      leds[NUM_LEDS];
extern const int NORTH_OFFSET;

void initLED();
void taskLED(void* pvParameters);

// Set the target GPS destination for the LED heading indicator
void setTarget(double lat, double lng);

// Alternatively, set the absolute bearing to the target (degrees from true north)
void setTargetBearing(double bearingDeg);

#endif
