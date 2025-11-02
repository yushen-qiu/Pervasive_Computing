#include <GPS/GPS_Coords/GPS_Coords.h>

#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include <config/Config.h>

static const int      RXPin   = Config::GPS_RX_PIN; // GPS TX -> ESP32 RX2
static const int      TXPin   = Config::GPS_TX_PIN; // GPS RX -> ESP32 TX2
static const uint32_t GPSBaud = Config::GPS_BAUD;

HardwareSerial GPS_Serial(2);
TinyGPSPlus    gps;
bool           gpsInitialized = false;

String GPS_Coords() {
    if (!gpsInitialized) {
        GPS_Serial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
        gpsInitialized = true;
    }

    unsigned long       start   = millis();
    const unsigned long timeout = Config::GPS_FIX_TIMEOUT_MS;

    while (millis() - start < timeout) {
        while (GPS_Serial.available() > 0) {
            gps.encode(GPS_Serial.read());
        }

        if (gps.location.isUpdated() && gps.location.isValid()) {
            double lat = gps.location.lat();
            double lng = gps.location.lng();

            char buffer[50];
            snprintf(buffer, sizeof(buffer), "%.6f, %.6f", lat, lng);
            return String(buffer);
        }
    }

    return String("No Fix");
}
