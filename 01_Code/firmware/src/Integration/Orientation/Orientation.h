#pragma once
#include <Arduino.h>

/*
 * OrientationResult
 * -----------------
 * Holds the orientation and movement state computed from GPS + magnetometer data.
 *
 * - movementBearing:   Bearing derived from recent GPS movement (degrees from North)
 * - targetBearing:     Bearing from current position to destination
 * - relativeAngle:     Signed angle difference (target relative to movement)
 *                      (0° = moving directly toward target; + = target on right)
 * - headingCorrection: Magnetometer-based correction for LED orientation (degrees)
 * - isStationary:      True if GPS movement speed is too low (device stationary)
 */
struct OrientationResult {
    double movementBearing;
    double targetBearing;
    double relativeAngle;
    double headingCorrection;
    bool   isStationary;
};

/*
 * computeOrientation()
 * --------------------
 * Computes GPS-based movement bearing, target bearing, and relative angle.
 * Magnetometer heading is used only for LED display correction.
 *
 * @param prevLat          Previous GPS latitude (degrees)
 * @param prevLng          Previous GPS longitude (degrees)
 * @param currentLat       Current GPS latitude (degrees)
 * @param currentLng       Current GPS longitude (degrees)
 * @param destLat          Destination latitude (degrees)
 * @param destLng          Destination longitude (degrees)
 * @param referenceHeading Baseline heading recorded after calibration
 * @param speedMps         Current speed (m/s), used for motion detection
 *
 * @return OrientationResult containing full movement + LED correction data
 */
OrientationResult computeOrientation(double prevLat, double prevLng, double currentLat,
                                     double currentLng, double destLat, double destLng,
                                     double referenceHeading, double speedMps);
