/*
 * Places API - Interface
 */
#pragma once
#include <Arduino.h>

// Fetches one nearby place and outputs destination coordinates
void fetchNearbyPlace(String includedTypes, int pressCount, double lat, double lng,
                      double& destinationLatitude, double& destinationLongitude);
