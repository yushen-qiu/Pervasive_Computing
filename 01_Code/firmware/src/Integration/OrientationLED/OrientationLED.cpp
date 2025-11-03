#include "OrientationLED.h"

static const int DIRECTION_MAP[16] = {
        0,  1,  15, 14, // N
        13, 12, 11, 10, // E
        9,  8,  7,  6,  // S
        5,  4,  3,  2   // W
};

void displayOrientationLED(const OrientationResult& o) {
    if (isnan(o.relativeAngle)) {
        Serial.println("[LED] Invalid orientation data.");
        return;
    }

    FastLED.clear();

    // Normalize angle [-180, 180)
    double rel = fmod(o.relativeAngle + 540.0, 360.0) - 180.0;
    int    dir = -1;

    // -------------------------------
    // Determine which direction to light
    // -------------------------------
    if (rel >= -45 && rel < 45)
        dir = 0; // Forward (N)
    else if (rel >= 45 && rel < 135)
        dir = 1; // Right (E)
    else if (rel <= -45 && rel > -135)
        dir = 3; // Left (W)
    else
        dir = 2; // Back (S)

    // -------------------------------
    // Select LED colour
    // -------------------------------
    CRGB color = CRGB::Blue;

    // -------------------------------
    // Light up LEDs according to DIRECTION_MAP
    // -------------------------------
    int startIndex = dir * 4;
    for (int i = 0; i < 4; i++) {
        int ledIndex   = DIRECTION_MAP[startIndex + i];
        leds[ledIndex] = color;
    }

    FastLED.show();

    const char* dirName[] = {"Forward", "Right", "Back", "Left"};
    Serial.printf("[LED] Dir: %s | Rel=%.1f° | Head=%.1f° | Target=%.1f°\n", dirName[dir], rel,
                  o.currentHeading, o.targetBearing);
}
