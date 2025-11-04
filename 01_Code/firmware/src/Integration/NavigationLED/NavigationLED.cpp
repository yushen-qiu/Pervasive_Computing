#include "NavigationLED.h"

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

void showGreen() {
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));
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
