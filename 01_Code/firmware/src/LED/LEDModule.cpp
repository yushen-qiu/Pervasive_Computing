#include "LEDModule.h"

CRGB leds[NUM_LEDS];

// init
void initLED() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

// clear ring
void clearLED() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void showDirection(double bearingDeg) {
    FastLED.clear();

    // -------------------------------
    // angle determine（centre ±45°）
    // -------------------------------
    // north 315–360 / 0–45
    // east 45–135
    // south 135–225
    // west 225–315
    int direction = -1; // 0=N, 1=E, 2=S, 3=W

    if (bearingDeg >= 315 || bearingDeg < 45)
        direction = 0; // n
    else if (bearingDeg >= 45 && bearingDeg < 135)
        direction = 1; // e
    else if (bearingDeg >= 135 && bearingDeg < 225)
        direction = 2; // s
    else if (bearingDeg >= 225 && bearingDeg < 315)
        direction = 3; // w

    // -------------------------------
    // corr led index
    // -------------------------------
    int startIndex = direction * 4; // 4 ea
    for (int i = 0; i < 4; i++) {
        leds[(startIndex + i) % NUM_LEDS] = CRGB::Blue; // change colour
    }

    FastLED.show();

    const char* dirName[] = {"North", "East", "South", "West"};
    if (direction >= 0) {
        Serial.printf("[LED] Direction: %s (%.1f°)\n", dirName[direction], bearingDeg);
    } else {
        Serial.printf("[LED] Unknown direction: %.1f°\n", bearingDeg);
    }
}
