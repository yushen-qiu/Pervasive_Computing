#include <led/LEDModule.h>
#include <math.h>
#include <sensors/Magnetometer.h>

// CRGB leds[NUM_LEDS];
const int NORTH_OFFSET = 4;

// Target bearing (absolute, from true north)
static bool   g_hasTargetBearing = false;
static double g_targetBearingDeg = 0.0;

void setTargetBearing(double bearingDeg) {
    double b = fmod(bearingDeg, 360.0);
    if (b < 0) b += 360.0;
    g_targetBearingDeg = b;
    g_hasTargetBearing = true;
}

static int bearingToIndex(double bearingDeg) {
    int idx = (int)round((bearingDeg / 360.0) * NUM_LEDS) % NUM_LEDS;
    return idx;
}

// Using magnetometer heading directly; no GPS movement logic needed

void initLED() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

void taskLED(void* pvParameters) {
    (void)pvParameters;
    uint8_t    hue        = 0;
    static int lastCenter = -1;

    for (;;) {
        FastLED.clear();

        float heading = Magnetometer::headingDeg();
        if (!isnan(heading) && g_hasTargetBearing) {
            double rel = g_targetBearingDeg - (double)heading;
            rel        = fmod(rel, 360.0);
            if (rel < 0) rel += 360.0;

            int center                                = (bearingToIndex(rel) + NORTH_OFFSET) % NUM_LEDS;
            leds[center]                              = CHSV((uint8_t)(rel / 2), 255, 255);
            leds[(center + 1) % NUM_LEDS]            = CHSV((uint8_t)(rel / 2), 255, 100);
            leds[(center + NUM_LEDS - 1) % NUM_LEDS] = CHSV((uint8_t)(rel / 2), 255, 100);
            if (center != lastCenter) {
                Serial.printf("[LED] Target rel %.1f° -> LED #%d (offset %d)\n", rel, center, NORTH_OFFSET);
                lastCenter = center;
            }
        } else {
            fill_rainbow(leds, NUM_LEDS, hue++, 8);
            Serial.println("[LED] Missing target/heading... showing rainbow.");
        }

        FastLED.show();
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
