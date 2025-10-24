#include "LEDModule.h"

// CRGB leds[NUM_LEDS];
const int NORTH_OFFSET = 4;

int bearingToIndex(double bearingDeg)
{
    int idx = (int)round((bearingDeg / 360.0) * NUM_LEDS) % NUM_LEDS;
    return idx;
}

void initLED()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

void taskLED(void *pvParameters)
{
    (void)pvParameters;
    uint8_t hue = 0;
    static int lastCenter = -1;

    for (;;)
    {
        FastLED.clear();

        if (gps.location.isValid() && gps.course.isValid() && currentCourse > 0)
        {
            int center = (bearingToIndex(currentCourse) + NORTH_OFFSET) % NUM_LEDS;

            leds[center] = CHSV((uint8_t)(currentCourse / 2), 255, 255);
            leds[(center + 1) % NUM_LEDS] = CHSV((uint8_t)(currentCourse / 2), 255, 100);
            leds[(center + NUM_LEDS - 1) % NUM_LEDS] = CHSV((uint8_t)(currentCourse / 2), 255, 100);

            if (center != lastCenter)
            {
                Serial.printf("[LED] Heading %.1f° -> LED #%d (North offset %d)\n",
                              currentCourse, center, NORTH_OFFSET);
                lastCenter = center;
            }
        }
        else
        {
            fill_rainbow(leds, NUM_LEDS, hue++, 8);
            Serial.println("[LED] No valid GPS direction yet... showing rainbow.");
        }

        FastLED.show();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
