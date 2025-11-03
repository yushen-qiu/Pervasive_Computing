#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <Integration/Orientation.h> // for OrientationResult
#include <led/LEDModule.h>           // for leds[], NUM_LEDS
#include <math.h>

/**
 * displayOrientationLED()
 * -----------------------
 * Given an OrientationResult (computed separately),
 * lights up the LED ring to indicate the direction of the destination.
 *
 * @param o  OrientationResult containing currentHeading, targetBearing, relativeAngle
 */
void displayOrientationLED(const OrientationResult& o);
