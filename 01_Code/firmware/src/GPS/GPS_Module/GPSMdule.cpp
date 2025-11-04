#include <GPS/GPS_Module/GPSModule.h>

HardwareSerial GPS(2);
TinyGPSPlus    gps;

volatile double currentCourse = 0;
volatile double currentSpeed  = 0;
volatile double currentLat    = 0;
volatile double currentLng    = 0;

void initGPS() {
    GPS.begin(9600, SERIAL_8N1, 16, 17);
}

void taskGPS(void* pvParameters) {
    (void)pvParameters;
    for (;;) {
        while (GPS.available()) {
            gps.encode(GPS.read());

            if (gps.location.isValid()) {
                currentLat = gps.location.lat();
                currentLng = gps.location.lng();
            }

            if (gps.course.isUpdated() && gps.course.deg() > 0) {
                currentCourse = gps.course.deg();
            }

            if (gps.speed.isUpdated()) {
                currentSpeed = gps.speed.mps();
            }
        }

        static uint32_t lastPrint = 0;
        if (millis() - lastPrint > 1000) {
            Serial.printf("[GPS] fix=%s, sats=%u, hdop=%.1f, speed=%.2f m/s, course=%.1f°, "
                          "lat=%.5f, lon=%.5f\n",
                          gps.location.isValid() ? "OK" : "NO", gps.satellites.value(),
                          gps.hdop.hdop(), gps.speed.mps(), gps.course.deg(), gps.location.lat(),
                          gps.location.lng());
            lastPrint = millis();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

double getLat() {
    return currentLat;
}

double getLng() {
    return currentLng;
}
