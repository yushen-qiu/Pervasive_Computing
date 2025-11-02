#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include <config/Config.h>
#include <sensors/GPSModule.h>

// Internal GPS objects (not exposed)
static HardwareSerial GPS(2);
static TinyGPSPlus    gps;

// Latest fix snapshot guarded by a critical section
static portMUX_TYPE g_gpsMux = portMUX_INITIALIZER_UNLOCKED;
static GpsFix       g_lastFix;

void initGPS() {
    GPS.begin(Config::GPS_BAUD, SERIAL_8N1, Config::GPS_RX_PIN, Config::GPS_TX_PIN);
}

bool getLatestFix(GpsFix& out) {
    portENTER_CRITICAL(&g_gpsMux);
    out = g_lastFix;
    portEXIT_CRITICAL(&g_gpsMux);
    return out.valid;
}

void taskGPS(void* pvParameters) {
    (void)pvParameters;

    // Local copies that we update when data is fresh
    double lat = 0.0, lng = 0.0, speed = 0.0, course = 0.0;
    bool   valid = false;

    for (;;) {
        while (GPS.available()) {
            gps.encode(GPS.read());

            if (gps.location.isValid()) {
                lat   = gps.location.lat();
                lng   = gps.location.lng();
                valid = true;
            }

            if (gps.course.isUpdated() && gps.course.deg() > 0) {
                course = gps.course.deg();
            }

            if (gps.speed.isUpdated()) {
                speed = gps.speed.mps();
            }
        }

        // Publish snapshot once per loop
        portENTER_CRITICAL(&g_gpsMux);
        g_lastFix.valid      = valid;
        g_lastFix.lat        = lat;
        g_lastFix.lng        = lng;
        g_lastFix.speed_mps  = speed;
        g_lastFix.course_deg = course;
        portEXIT_CRITICAL(&g_gpsMux);

        static uint32_t lastPrint = 0;
        if (millis() - lastPrint > 1000) {
            Serial.printf("[GPS] fix=%s, sats=%u, hdop=%.1f, speed=%.2f m/s, course=%.1f°, "
                          "lat=%.5f, lon=%.5f\n",
                          valid ? "OK" : "NO", gps.satellites.value(), gps.hdop.hdop(), speed,
                          course, lat, lng);
            lastPrint = millis();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
