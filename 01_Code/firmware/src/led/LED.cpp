#include <led/LEDModule.h>
#include <math.h>
#include <sensors/GPS_Coords.h>

// CRGB leds[NUM_LEDS];
const int NORTH_OFFSET = 4;

static int bearingToIndex(double bearingDeg) {
    int idx = (int)round((bearingDeg / 360.0) * NUM_LEDS) % NUM_LEDS;
    return idx;
}

static double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}
static double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
}

static double bearingBetween(double lat1, double lon1, double lat2, double lon2) {
    double phi1 = deg2rad(lat1);
    double phi2 = deg2rad(lat2);
    double dLon = deg2rad(lon2 - lon1);
    double y    = sin(dLon) * cos(phi2);
    double x    = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(dLon);
    double brng = atan2(y, x);
    double deg  = fmod(rad2deg(brng) + 360.0, 360.0);
    return deg;
}

static double haversineMeters(double lat1, double lon1, double lat2, double lon2) {
    static const double R    = 6371000.0; // metres
    double              dLat = deg2rad(lat2 - lat1);
    double              dLon = deg2rad(lon2 - lon1);
    double              a    = sin(dLat / 2) * sin(dLat / 2) +
               cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void initLED() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

void taskLED(void* pvParameters) {
    (void)pvParameters;
    uint8_t       hue        = 0;
    static int    lastCenter = -1;
    static bool   havePrev   = false;
    static double prevLat    = 0.0;
    static double prevLng    = 0.0;
    static double lastCourse = -1.0; // degrees

    for (;;) {
        FastLED.clear();

        String coords = GPS_Coords();
        if (coords != "No Fix") {
            int comma = coords.indexOf(',');
            if (comma > 0) {
                String latStr = coords.substring(0, comma);
                String lngStr = coords.substring(comma + 1);
                latStr.trim();
                lngStr.trim();
                double lat = latStr.toDouble();
                double lon = lngStr.toDouble();

                if (!isnan(lat) && !isnan(lon)) {
                    if (havePrev) {
                        double dist = haversineMeters(prevLat, prevLng, lat, lon);
                        if (dist >= 3.0) { // update heading only on meaningful movement
                            lastCourse = bearingBetween(prevLat, prevLng, lat, lon);
                            prevLat    = lat;
                            prevLng    = lon;
                        }
                    } else {
                        prevLat  = lat;
                        prevLng  = lon;
                        havePrev = true;
                    }
                }
            }
        }

        if (lastCourse >= 0.0) {
            int center                    = (bearingToIndex(lastCourse) + NORTH_OFFSET) % NUM_LEDS;
            leds[center]                  = CHSV((uint8_t)(lastCourse / 2), 255, 255);
            leds[(center + 1) % NUM_LEDS] = CHSV((uint8_t)(lastCourse / 2), 255, 100);
            leds[(center + NUM_LEDS - 1) % NUM_LEDS] = CHSV((uint8_t)(lastCourse / 2), 255, 100);
            if (center != lastCenter) {
                Serial.printf("[LED] Heading %.1f° -> LED #%d (North offset %d)\n", lastCourse,
                              center, NORTH_OFFSET);
                lastCenter = center;
            }
        } else {
            fill_rainbow(leds, NUM_LEDS, hue++, 8);
            Serial.println("[LED] No valid GPS heading yet... showing rainbow.");
        }

        FastLED.show();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
