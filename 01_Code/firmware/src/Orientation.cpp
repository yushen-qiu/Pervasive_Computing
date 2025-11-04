#include <Arduino.h>
#include <GPS/Navigation/Navigation.h> // for computeNavigation()
#include <Magnetometer/Magnetometer.h>
#include <Orientation.h>
#include <math.h>

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
