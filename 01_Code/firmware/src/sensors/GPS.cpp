// GPS: init UART and background read task; exposes latest fix
#include <config/Pins.h>
#include <sensors/GPS.h>

// Demo mode flag declared in main.cpp
extern bool demoFixStart;

// UART and TinyGPS++ parser
HardwareSerial GPS(2);
TinyGPSPlus    gps;

// Latest GPS-derived state (written by task)
volatile double currentCourse = 0; // deg
volatile double currentSpeed  = 0; // m/s
volatile double currentLat    = 0; // deg
volatile double currentLng    = 0; // deg

void initGPS() {
    GPS.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);
}

void taskGPS(void* pvParameters) {
    (void)pvParameters;
    for (;;) {
        // In demo mode, fix the GPS position and skip UART parsing
        if (!GPS.available() && demoFixStart) {
            currentLat = -33.89019462116911;
            currentLng = 151.19298444298389;
            vTaskDelay(200 / portTICK_PERIOD_MS);
            continue;
        }

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

        // Periodic debug output (every 5s)
        static uint32_t lastPrint = 0;
        if (millis() - lastPrint > 5000) {
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
