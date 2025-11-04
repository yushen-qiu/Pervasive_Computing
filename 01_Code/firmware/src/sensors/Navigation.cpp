#include <math.h>
#include <sensors/Navigation.h>

NavigationData computeNavigation(double currentLat, double currentLng, double destLat,
                                 double destLng) {
    NavigationData nav{};

    // ---- Constants ----
    const double R          = 6371000.0; // Earth radius in meters
    const double WALK_SPEED = 1.4;       // average walking speed (m/s)

    // ---- Convert degrees to radians ----
    double lat1 = radians(currentLat);
    double lon1 = radians(currentLng);
    double lat2 = radians(destLat);
    double lon2 = radians(destLng);

    // ---- Haversine formula (distance) ----
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a =
            sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);

    double c           = 2 * atan2(sqrt(a), sqrt(1 - a));
    nav.distanceMeters = R * c;

    // ---- Bearing calculation ----
    double y          = sin(dLon) * cos(lat2);
    double x          = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    nav.bearingToDest = fmod((degrees(atan2(y, x)) + 360.0), 360.0);

    // ---- Estimated walking time ----
    nav.etaMinutes = (nav.distanceMeters / WALK_SPEED) / 60.0;

    return nav;
}
