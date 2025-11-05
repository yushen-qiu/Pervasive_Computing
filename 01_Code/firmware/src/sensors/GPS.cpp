/*
 * GPS module
 * ---------------------------------
 * Provides initialization and a background task to read NMEA data
 * from the GPS module and expose the latest fix through volatile globals
 * and small accessors.
 *
 * Hardware
 * - Uses `HardwareSerial` (Serial2 on ESP32) with 9600 baud.
 * - RX/TX pins are centralized in `Pins::GPS_RX` and `Pins::GPS_TX`.
 *
 * Data exposed
 * - `currentLat`, `currentLng`: latest latitude/longitude (degrees).
 * - `currentSpeed`: latest speed (m/s).
 * - `currentCourse`: latest course over ground (degrees).
 */
#include <config/Pins.h>
#include <sensors/GPS.h>

// Underlying UART and TinyGPS++ parser
HardwareSerial GPS(2);
TinyGPSPlus    gps;

// Latest GPS-derived state (written by the GPS task)
volatile double currentCourse = 0; // degrees
volatile double currentSpeed  = 0; // meters/second
volatile double currentLat    = 0; // degrees
volatile double currentLng    = 0; // degrees

/**
 * Initialize the GPS UART with the configured RX/TX pins.
 * Baud: 9600, framing: SERIAL_8N1
 */
void initGPS() {
    GPS.begin(9600, SERIAL_8N1, Pins::GPS_RX, Pins::GPS_TX);
}

/**
 * FreeRTOS task: continuously read from GPS serial, feed TinyGPS++,
 * and update the latest location/speed/course globals.
 *
 * @param pvParameters Unused task parameter.
 */
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

        // Periodic debug output every 5s
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

/** Return latest latitude (degrees). */
double getLat() {
    return currentLat;
}

/** Return latest longitude (degrees). */
double getLng() {
    return currentLng;
}
