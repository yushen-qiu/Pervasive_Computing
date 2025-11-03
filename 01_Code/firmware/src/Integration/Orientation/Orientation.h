#pragma once
#include <Arduino.h>

/*
 * OrientationResult
 * -----------------
 * Holds the orientation information computed from GPS and magnetometer data.
 *
 * - currentHeading:   current facing direction of the device (absolute from magnetic north)
 * - targetBearing:    absolute bearing from current location to destination
 * - relativeAngle:    signed angle difference between facing and target direction
 *                     (0° = facing the target, positive = target on the right)
 */
struct OrientationResult {
    double currentHeading;
    double targetBearing;
    double relativeAngle;
};

/*
 * computeOrientation()
 * --------------------
 * Computes the current facing direction and the relative angle between the device’s
 * forward direction and the target location.
 *
 * @param currentLat        Current latitude in degrees
 * @param currentLng        Current longitude in degrees
 * @param destLat           Destination latitude in degrees
 * @param destLng           Destination longitude in degrees
 * @param referenceHeading  Heading recorded after calibration (forward baseline)
 *
 * @return OrientationResult containing:
 *         - currentHeading (absolute heading)
 *         - targetBearing (bearing to destination)
 *         - relativeAngle (difference between facing and target)
 *
 *
 */
OrientationResult computeOrientation(double currentLat, double currentLng, double destLat,
                                     double destLng, double referenceHeading);
