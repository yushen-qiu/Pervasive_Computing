#include <config/Config.h>
#include <led/LEDModule.h>
#include <math.h>
#include <sensors/GPSModule.h>
#include <sensors/Magnetometer.h>

CRGB leds[Config::NUM_LEDS];

// Target bearing (absolute, from true north)
static bool   g_hasTargetBearing = false;
static double g_targetBearingDeg = 0.0;
static double g_targetLatDeg     = NAN;
static double g_targetLngDeg     = NAN;

void setTargetBearing(double bearingDeg) {
    double b = fmod(bearingDeg, 360.0);
    if (b < 0)
        b += 360.0;
    g_targetBearingDeg = b;
    g_hasTargetBearing = true;
}

void setTargetCoords(double latDeg, double lngDeg) {
    g_targetLatDeg = latDeg;
    g_targetLngDeg = lngDeg;
}

static int bearingToIndex(double bearingDeg) {
    int idx = (int)round((bearingDeg / 360.0) * Config::NUM_LEDS) % Config::NUM_LEDS;
    return idx;
}

// Using magnetometer heading directly; no GPS movement logic needed
static double deg2rad(double d) {
    return d * 0.017453292519943295;
}
static double haversineMeters(double lat1, double lon1, double lat2, double lon2) {
    static const double R    = 6371000.0; // metres
    const double        dLat = deg2rad(lat2 - lat1);
    const double        dLon = deg2rad(lon2 - lon1);
    const double        a    = sin(dLat / 2) * sin(dLat / 2) +
                     cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(dLon / 2) * sin(dLon / 2);
    const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void initLED() {
    FastLED.addLeds<LED_TYPE, Config::LED_PIN, COLOR_ORDER>(leds, Config::NUM_LEDS);
    FastLED.setBrightness(Config::LED_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

void taskLED(void* pvParameters) {
    (void)pvParameters;
    uint8_t              hue         = 0;
    static int           lastCenter  = -1;
    static bool          blinkOn     = false;
    static unsigned long lastBlinkMs = 0;

    for (;;) {
        FastLED.clear();

        // Distance check if we have a target and a GPS fix
        bool within10m = false;
        if (!isnan(g_targetLatDeg) && !isnan(g_targetLngDeg)) {
            GpsFix fix;
            if (getLatestFix(fix) && fix.valid) {
                double dist = haversineMeters(fix.lat, fix.lng, g_targetLatDeg, g_targetLngDeg);
                within10m   = (dist < 10.0);
            }
        }

        float heading = Magnetometer::headingDeg();
        if (within10m) {
            // Blink green when very close to the target
            unsigned long now = millis();
            if (now - lastBlinkMs > 300) {
                blinkOn     = !blinkOn;
                lastBlinkMs = now;
            }
            if (blinkOn) {
                fill_solid(leds, Config::NUM_LEDS, CRGB::Green);
            } else {
                FastLED.clear();
            }
        } else if (!isnan(heading) && g_hasTargetBearing) {
            double rel = g_targetBearingDeg - (double)heading;
            rel        = fmod(rel, 360.0);
            if (rel < 0)
                rel += 360.0;

            int center   = (bearingToIndex(rel) + Config::LED_NORTH_OFFSET) % Config::NUM_LEDS;
            leds[center] = CRGB::Blue;
            leds[(center + 1) % Config::NUM_LEDS]                    = CRGB(0, 0, 64);
            leds[(center + Config::NUM_LEDS - 1) % Config::NUM_LEDS] = CRGB(0, 0, 64);
            if (center != lastCenter) {
                Serial.printf("[LED] Target rel %.1f° -> LED #%d (offset %d)\n", rel, center,
                              Config::LED_NORTH_OFFSET);
                lastCenter = center;
            }
        } else {
            fill_solid(leds, Config::NUM_LEDS, CRGB::Blue);
            Serial.println("[LED] Missing target/heading... showing solid blue.");
        }

        FastLED.show();
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
