/*
 * LED Module
 * ---------------------------------
 * FastLED-based helpers to initialize and display
 * orientation/navigation cues on an LED ring/strip.
 */
#include <LED/LEDModule.h>
#include <math.h>
#include <sensors/Orientation.h>

static CRGB leds[Pins::LED_COUNT];
static bool triggered100m = false;
static bool triggered50m  = false;

// init
void initLED() {
    FastLED.addLeds<LED_TYPE, Pins::LED, COLOR_ORDER>(leds, Pins::LED_COUNT);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
}

// (Removed unused helpers: clearLED, showDirection)

// Orientation LED display moved from src/OrientationLED.cpp
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

    // Determine which direction to light
    if (rel >= -45 && rel < 45)
        dir = 0; // Forward (N)
    else if (rel >= 45 && rel < 135)
        dir = 1; // Right (E)
    else if (rel <= -45 && rel > -135)
        dir = 3; // Left (W)
    else
        dir = 2; // Back (S)

    // Select LED colour
    CRGB color = CRGB::Blue;

    // Light up LEDs according to DIRECTION_MAP
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

//----------------------------------------------
// 100m: Breathe twice during the yellow light
//----------------------------------------------
static void breatheYellowTwice() {
    for (int cycle = 0; cycle < 2; cycle++) {
        for (int b = 0; b <= 255; b += 5) {
            fill_solid(leds, Pins::LED_COUNT, CRGB(b, b, 0)); // Yellow
            FastLED.show();
            delay(8);
        }
        for (int b = 255; b >= 0; b -= 5) {
            fill_solid(leds, Pins::LED_COUNT, CRGB(b, b, 0));
            FastLED.show();
            delay(8);
        }
    }
    FastLED.clear();
    FastLED.show();
}

void turnOffLED() {
    FastLED.clear();
    FastLED.show();
}

void showColorLED(int R, int G, int B) {
    fill_solid(leds, Pins::LED_COUNT, CRGB(R, G, B));
    FastLED.show();
}

// (Removed unused helper: breatheGreen)

// (Removed unused helper: breatheColorForever)

//----------------------------------------------
// 50m Note: Rainbow effect for 5 seconds
//----------------------------------------------
static void rainbowEffect5s() {
    unsigned long start = millis();
    uint8_t       hue   = 0;
    while (millis() - start < 5000) {
        fill_rainbow(leds, Pins::LED_COUNT, hue++, 7);
        FastLED.show();
        delay(30);
    }
    FastLED.clear();
    FastLED.show();
}

void handleNavigationLED(const NavigationData& nav) {
    // --- First time entering the 100 metres ---
    if (!triggered100m && nav.distanceMeters <= 100 && nav.distanceMeters > 50) {
        Serial.println("[NAV LED] Within 100 m → Yellow breathing twice");
        triggered100m = true;
        breatheYellowTwice();
    }

    // --- First time entering 50 metres ---
    if (!triggered50m && nav.distanceMeters <= 50) {
        Serial.println("[NAV LED] Within 50 m → Rainbow then shutdown");
        triggered50m = true;
        rainbowEffect5s();

        // Shut down the system
        fill_solid(leds, Pins::LED_COUNT, CRGB::Black);
        FastLED.show();
        Serial.println("[NAV LED] System halted (near destination)");
        while (true)
            delay(1000);
    }
}
