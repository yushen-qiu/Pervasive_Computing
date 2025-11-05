#include "OrientationLED.h"

static const int DIRECTION_MAP[16] = {
        0,  1,  15, 14, // North (Forward)
        13, 12, 11, 10, // East (Right)
        9,  8,  7,  6,  // South (Back)
        5,  4,  3,  2   // West (Left)
};

static uint8_t breathPhase = 0;
static bool    increasing  = true;

static void showBreathingBlue() {
    FastLED.clear();
    uint8_t brightness = increasing ? breathPhase++ : breathPhase--;
    if (breathPhase == 0 || breathPhase == 255)
        increasing = !increasing;
    CRGB color = CHSV(160, 255, brightness);
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
}

/*
 * displayOrientationLED()
 * -----------------------
 * Display target direction:
 * - When stationary (GPS speed < 0.5m/s) → Pulsing blue light
 * - When moving → Display four directions (front, rear, left, right) based on relative angle
 */
void displayOrientationLED(const OrientationResult& o) {
    if (o.isStationary) {
        showBreathingBlue();
        return;
    }

    FastLED.clear();

    double rel = o.relativeAngle - o.headingCorrection;
    rel        = fmod(rel + 540.0, 360.0) - 180.0; // Normalised to the range [-180, 180)

    int dir = -1;
    if (rel >= -45 && rel < 45)
        dir = 0; // Forward (North)
    else if (rel >= 45 && rel < 135)
        dir = 1; // Right (East)
    else if (rel <= -45 && rel > -135)
        dir = 3; // Left (West)
    else
        dir = 2; // Back (South)

    CRGB color      = CRGB::Blue;
    int  startIndex = dir * 4;
    for (int i = 0; i < 4; i++) {
        int ledIndex   = DIRECTION_MAP[startIndex + i];
        leds[ledIndex] = color;
    }

    FastLED.show();

    const char* dirName[] = {"Forward", "Right", "Back", "Left"};
    Serial.printf("[LED] Dir: %s | Rel=%.1f° | Move=%.1f° | Target=%.1f° | Corr=%.1f°\n",
                  dirName[dir], rel, o.movementBearing, o.targetBearing, o.headingCorrection);
}
