#include <Arduino.h>
#include <GPS/GPS_Coords/GPS_Coords.h>
#include <GPS/Navigation/Navigation.h> // for computeNavigation()
#include <Magnetometer/Magnetometer.h>
#include <math.h>

struct OrientationResult {
    double currentHeading; // current facing direction (absolute from north)
    double targetBearing;  // absolute bearing to destination
    double relativeAngle;  // difference between facing and target (0° = facing target)
};

/*
 * Compute the orientation and relative angle between
 * the device's current facing direction and the destination.
 *
 * @param currentLat   Current latitude in degrees
 * @param currentLng   Current longitude in degrees
 * @param destLat      Destination latitude in degrees
 * @param destLng      Destination longitude in degrees
 * @param referenceHeading  Reference direction recorded after calibration (degrees)
 *
 * @return OrientationResult containing current heading, target bearing, and relative angle
 */
OrientationResult computeOrientation(double currentLat, double currentLng, double destLat,
                                     double destLng, double referenceHeading) {
    OrientationResult res{};

    // 1️ Get the current magnetometer heading (absolute from north)
    float heading = Magnetometer::headingDeg();
    if (isnan(heading)) {
        Serial.println("No valid magnetometer data!");
        return res;
    }

    // 2️ Compute the target bearing using haversine (from Navigation.cpp)
    NavigationData nav = computeNavigation(currentLat, currentLng, destLat, destLng);

    // 3️Adjust current heading relative to reference direction (front of device)
    //    For example: if referenceHeading = 63.5°, and current heading = 83.5°,
    //    then we have rotated +20° from initial "forward" direction.
    double facingBearing = fmod((heading - referenceHeading + 360.0), 360.0);

    // 4️ Compute relative angle between facing direction and destination direction
    //    Positive = target on the right; Negative = target on the left
    double relative = nav.bearingToDest - heading;
    relative        = fmod(relative + 540.0, 360.0) - 180.0; // normalize to [-180, +180]

    // 5️Fill result structure
    res.currentHeading = heading;
    res.targetBearing  = nav.bearingToDest;
    res.relativeAngle  = relative;

    return res;
}
