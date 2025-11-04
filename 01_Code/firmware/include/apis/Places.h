#pragma once
#include <Arduino.h>

void fetchNearbyPlace(String includedTypes, int pressCount, double lat, double lng,
                      double& destinationLatitude, double& destinationLongitude);
