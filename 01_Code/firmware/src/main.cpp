#include <Arduino.h>
#include <GPS/GPS_Module/GPSModule.h>
#include <Integration/NavigationLED/NavigationLED.h>
#include <Integration/Orientation/Orientation.h>
#include <Integration/OrientationLED/OrientationLED.h>
#include <LED/LEDModule.h>
#include <Magnetometer/Magnetometer.h>

double refHeading = NAN;

void setup() {
    Serial.begin(115200);
    initLED();
    initGPS();
    xTaskCreate(taskGPS, "GPS", 4096, nullptr, 1, nullptr);

    if (!Magnetometer::begin()) {
        Serial.println("Magnetometer init failed!");
        while (1)
            delay(100);
    }
    Magnetometer::startCalibration();
    delay(10000);
    Magnetometer::stopCalibration();
    refHeading = Magnetometer::headingDeg();

    Serial.printf("%.2f°\n", refHeading);
}

double prevLat = NAN, prevLng = NAN;

void loop() {
    static unsigned long lastUpdate = 0;
    double               destLat    = -33.889938;
    double               destLng    = 151.192437;

    GpsFix fix;
    if (!getLatestFix(fix) || !fix.valid) {
        delay(500);
        return;
    }

    if (isnan(prevLat)) {
        prevLat = fix.lat;
        prevLng = fix.lng;
        delay(500);
        return;
    }

    double speedMps = fix.speed_mps;

    OrientationResult o = computeOrientation(prevLat, prevLng, fix.lat, fix.lng, destLat, destLng,
                                             refHeading, speedMps);

    NavigationData nav = computeNavigation(fix.lat, fix.lng, destLat, destLng);

    Serial.printf("[INFO] Relative Angle: %.2f° | Distance: %.2f m\n", o.relativeAngle,
                  nav.distanceMeters);

    displayOrientationLED(o);

    prevLat = fix.lat;
    prevLng = fix.lng;

    delay(500);
}
