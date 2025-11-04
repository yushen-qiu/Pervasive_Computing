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
