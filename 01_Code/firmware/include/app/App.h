#pragma once
#include <Arduino.h>

class App {
public:
    void begin(); // init serial, button, WiFi
    void tick();  // debounce + 5s window + trigger flow

private:
    // flow helpers
    void runFlow(); // Weather -> AI -> Places

    // state for button logic
    int           buttonState_      = HIGH;
    int           lastButtonState_  = HIGH;
    unsigned long lastDebounceTime_ = 0;
    int           pressCount_       = 0;
    bool          countingActive_   = false;
    unsigned long startTime_        = 0;
    bool          windowFinished_   = false;

    // last destination (for logging)
    double destLat_ = 0;
    double destLng_ = 0;
};
