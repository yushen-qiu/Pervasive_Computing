#include <Arduino.h>
#include "GpsTracker.h"

// ESP32 UART2: GPS.TX -> GPIO16, GPS.RX -> GPIO17
HardwareSerial GPS2(2);
GpsTracker tracker(GPS2, /*compareIntervalMs=*/10000);

void setup()
{
    Serial.begin(115200);
    tracker.begin(/*baud=*/9600, /*rxPin=*/16, /*txPin=*/17);
    Serial.println("Collecting GPS... (go outdoor or near a window)");
}

void loop()
{
    // Feed and process NMEA; issue a report every 10 seconds.
    if (tracker.update())
    {
        const MoveReport &r = tracker.lastReport();
        if (!r.moved)
        {
            Serial.printf("[10s] moved ~%.1fm (below threshold), hdop=%.1f, sats=%u\n",
                          r.dist_m, r.hdop, r.sats);
        }
        else
        {
            Serial.printf("[10s] moved %.1fm, bearing %.1f° (%s), speed~%.2fm/s, hdop=%.1f, sats=%u\n",
                          r.dist_m, r.bearing_deg, r.bearing_txt, r.speed_mps, r.hdop, r.sats);
        }
    }

    // Print the heartbeat status every 2 seconds
    tracker.heartbeat(Serial, 2000);
}
