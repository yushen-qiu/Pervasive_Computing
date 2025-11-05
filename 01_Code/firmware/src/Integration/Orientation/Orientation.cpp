#include <Arduino.h>
#include <GPS/GPS_Coords/GPS_Coords.h>
#include <GPS/Navigation/Navigation.h>
#include <Magnetometer/Magnetometer.h>
#include <math.h>

struct OrientationResult {
    double movementBearing;
    double targetBearing;
    double relativeAngle;
    double headingCorrection;
    bool   isStationary;
};

static double bearingBetween(double lat1, double lng1, double lat2, double lng2) {
    double lat1r = radians(lat1);
    double lat2r = radians(lat2);
    double dLon  = radians(lng2 - lng1);
    double y     = sin(dLon) * cos(lat2r);
    double x     = cos(lat1r) * sin(lat2r) - sin(lat1r) * cos(lat2r) * cos(dLon);
    double brng  = atan2(y, x);
    brng         = degrees(brng);
    return fmod(brng + 360.0, 360.0);
}

OrientationResult computeOrientation(double prevLat, double prevLng, double currentLat,
                                     double currentLng, double destLat, double destLng,
                                     double referenceHeading, double speedMps) {
    OrientationResult res{};

    if (speedMps < 0.5) { // Less than 0.5 m/s is considered stationary
        res.isStationary = true;
        return res;
    }

    res.movementBearing = bearingBetween(prevLat, prevLng, currentLat, currentLng);

    NavigationData nav = computeNavigation(currentLat, currentLng, destLat, destLng);
    res.targetBearing  = nav.bearingToDest;

    double diff       = nav.bearingToDest - res.movementBearing;
    res.relativeAngle = fmod(diff + 540.0, 360.0) - 180.0;

    // float heading = Magnetometer::headingDeg();
    // if (!isnan(heading)) {
    //     res.headingCorrection = fmod((heading - referenceHeading + 360.0), 360.0);
    // } else {
    //     res.headingCorrection = 0.0;
    // }

    res.isStationary = false;
    return res;
}
