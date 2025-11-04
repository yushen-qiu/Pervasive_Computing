#include <LED/LEDModule.h>

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

static bool triggered100m = false;
static bool triggered50m  = false;

//----------------------------------------------
// 100m: Breathe twice during the yellow light
//----------------------------------------------
static void breatheYellowTwice() {
    for (int cycle = 0; cycle < 2; cycle++) {
        for (int b = 0; b <= 255; b += 5) {
            fill_solid(leds, NUM_LEDS, CRGB(b, b, 0)); // Yellow
            FastLED.show();
            delay(8);
        }
        for (int b = 255; b >= 0; b -= 5) {
            fill_solid(leds, NUM_LEDS, CRGB(b, b, 0));
            FastLED.show();
            delay(8);
        }
    }
    FastLED.clear();
    FastLED.show();
}

void turnOff() {
    FastLED.clear();
}

void showColor(int R, int G, int B) {
    fill_solid(leds, NUM_LEDS, CRGB(R, G, B));
    FastLED.show();
}

void breatheGreen() {
    for (int cycle = 0; cycle < 2; cycle++) {
        for (int b = 0; b <= 255; b += 5) {
            // Green channel only: R=0, G=b, B=0
            fill_solid(leds, NUM_LEDS, CRGB(0, b, 0));
            FastLED.show();
            delay(8);
        }
        for (int b = 255; b >= 0; b -= 5) {
            // Green channel only: R=0, G=b, B=0
            fill_solid(leds, NUM_LEDS, CRGB(0, b, 0));
            FastLED.show();
            delay(8);
        }
    }
    FastLED.clear();
    FastLED.show();
}

//----------------------------------------------
// 50m Note: Rainbow effect for 5 seconds
//----------------------------------------------
static void rainbowEffect5s() {
    unsigned long start = millis();
    uint8_t       hue   = 0;
    while (millis() - start < 5000) {
        fill_rainbow(leds, NUM_LEDS, hue++, 7);
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
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        Serial.println("[NAV LED] System halted (near destination)");
        while (true)
            delay(1000);
    }
}
