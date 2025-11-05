#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <Integration/Orientation/Orientation.h>
#include <LED/LEDModule.h>
#include <math.h>

/**
 * displayOrientationLED()
 * -----------------------
 * Displays the target direction on the LED ring according to OrientationResult.
 * - If stationary: shows breathing blue effect.
 * - If moving:     shows 4 LEDs pointing toward target relative to movement.
 *
 * @param o OrientationResult computed from GPS + magnetometer
 */
void displayOrientationLED(const OrientationResult& o);
