#include "RainbowLED.h"

#define LED_PIN 4
#define NUM_LEDS 16
#define BRIGHTNESS 255
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

CRGB    leds[NUM_LEDS];
uint8_t gHue = 0;

namespace RainbowLED {
    void begin() {
        FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.setBrightness(BRIGHTNESS);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
    }

    void rainbow(uint8_t delayMs) {
        fill_rainbow(leds, NUM_LEDS, gHue, 7);
        FastLED.show();
        gHue++;
        delay(delayMs);
    }

    void solidColor(CRGB color) {
        fill_solid(leds, NUM_LEDS, color);
        FastLED.show();
    }

    void setBrightness(uint8_t brightness) {
        FastLED.setBrightness(brightness);
        FastLED.show();
    }
} // namespace RainbowLED
