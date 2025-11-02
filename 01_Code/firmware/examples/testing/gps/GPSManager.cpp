// moved to examples/testing/gps
#include "GPSManager.h"

HardwareSerial GPS2(2);
int            baudList[] = {9600, 38400, 57600, 115200, 4800};

GpsAutoBaud autoBaud(GPS2, 16, 17, baudList, sizeof(baudList) / sizeof(baudList[0]));
GpsTracker  tracker(GPS2, 10000);

namespace GPSManager {
    void begin() {
        Serial.begin(115200);
        Serial.println("\n[GPSManager] Initialising GPS...");
        autoBaud.begin();
        tracker.begin(autoBaud.currentBaud(), 16, 17);
        Serial.println("[GPSManager] GPS ready!");
    }

    void update() {
        autoBaud.update(Serial);

        if (tracker.update()) {
            const MoveReport& r = tracker.lastReport();
            if (!r.moved) {
                Serial.printf("[10s] moved ~%.1fm (below threshold), hdop=%.1f, sats=%u\n",
                              r.dist_m, r.hdop, r.sats);
            } else {
                Serial.printf("[10s] moved %.1fm, bearing %.1f° (%s), speed~%.2fm/s, hdop=%.1f, "
                              "sats=%u\n",
                              r.dist_m, r.bearing_deg, r.bearing_txt, r.speed_mps, r.hdop, r.sats);
            }
        }

        tracker.heartbeat(Serial, 2000);
    }
} // namespace GPSManager
