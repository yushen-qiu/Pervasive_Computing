#pragma once
#include <FastLED.h>

namespace RainbowLED {
    void begin();

    void rainbow(uint8_t delayMs = 20);

    void solidColor(CRGB color);

    void setBrightness(uint8_t brightness);
} // namespace RainbowLED
