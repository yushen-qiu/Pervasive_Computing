#pragma once
#include <Arduino.h>

// Structure to hold navigation results
struct NavigationData {
    double distanceMeters; // Distance to destination (m)
    double bearingToDest;  // Bearing angle (degrees, 0° = North)
    double etaMinutes;     // Estimated walking time (minutes)
};

/*
 * Compute navigation metrics (distance, bearing, ETA)
 * between current GPS location and a target destination.
 *
 * @param currentLat   Current latitude (degrees)
 * @param currentLng   Current longitude (degrees)
 * @param destLat      Destination latitude (degrees)
 * @param destLng      Destination longitude (degrees)
 * @return             NavigationData containing distance, bearing, and ETA
 */
NavigationData computeNavigation(double currentLat, double currentLng, double destLat,
                                 double destLng);
