/*
 * Weather API - Interface
 */
#pragma once
#include <Arduino.h>

// Populates condition and localTime for given coordinates
void fetchWeather(String& condition, String& localTime, double lat, double lng);
