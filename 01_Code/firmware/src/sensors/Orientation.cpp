/*
 * Orientation
 * ---------------------------------
 * Combines magnetometer heading and navigation bearing to compute
 * the relative angle to target.
 */
#include <Arduino.h>
#include <math.h>
#include <sensors/Magnetometer.h>
#include <sensors/Navigation.h> // for computeNavigation()
#include <sensors/Orientation.h>

OrientationResult computeOrientation(double currentLat, double currentLng, double destLat,
                                     double destLng, double referenceHeading) {
    OrientationResult res{};

    float heading = Magnetometer::headingDeg();
    if (isnan(heading)) {
        Serial.println("No valid magnetometer data!");
        return res;
    }

    NavigationData nav = computeNavigation(currentLat, currentLng, destLat, destLng);

    double facingBearing = fmod((heading - referenceHeading + 360.0), 360.0);

    double relative = nav.bearingToDest - heading;
    relative        = fmod(relative + 540.0, 360.0) - 180.0; // normalize to [-180, +180]

    res.currentHeading = heading;
    res.targetBearing  = nav.bearingToDest;
    res.relativeAngle  = relative;

    return res;
}
