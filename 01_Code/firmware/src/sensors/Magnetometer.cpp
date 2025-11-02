// Magnetometer module implementation for MMC5603
#include <Arduino.h>
#include <Adafruit_MMC56x3.h>
#include <Wire.h>
#include <sensors/Magnetometer.h>

namespace Magnetometer {

    static Adafruit_MMC5603 mag(12345);

    // Site-specific declination (Sydney ~12.82° East)
    static float declinationDeg = 12.82f;
    static float northOffsetDeg = 0.0f;

    // Calibration state
    static bool  calibrating = false;
    static float magMin[3]   = {10000, 10000, 10000};
    static float magMax[3]   = {-10000, -10000, -10000};
    static float offset[3]   = {0, 0, 0};
    static float scale[3]    = {1, 1, 1};

    // Last computed heading
    static float heading_deg = NAN;

    // One-time auto calibration window
    static bool          autoCalibrating   = false;
    static unsigned long autoCalibStartMs  = 0;
    static const unsigned long autoCalibDurationMs = 10000; // 10s

    static void computeCalibration_() {
        for (int i = 0; i < 3; i++) {
            offset[i] = (magMax[i] + magMin[i]) / 2.0f;
        }
        const float avg_range =
                ((magMax[0] - magMin[0]) + (magMax[1] - magMin[1]) + (magMax[2] - magMin[2])) /
                6.0f;
        for (int i = 0; i < 3; i++) {
            const float range = (magMax[i] - magMin[i]) / 2.0f;
            scale[i]          = (range > 0.0001f) ? (avg_range / range) : 1.0f;
        }
    }

    static const char* directionFromHeading_(float h) {
        if (isnan(h)) return "Unknown";
        int sector = (int)((h + 45.0f) / 90.0f) & 3; // 0..3
        switch (sector) {
            case 0: return "North";
            case 1: return "East";
            case 2: return "South";
            case 3: return "West";
        }
        return "Unknown";
    }

    static void taskMag_(void* pv) {
        (void)pv;
        uint32_t lastPrint = 0;
        for (;;) {
            update();

            // Handle one-time auto-calibration
            if (autoCalibrating && (millis() - autoCalibStartMs >= autoCalibDurationMs)) {
                stopCalibration();
                autoCalibrating = false;
                Serial.println("[Mag] Calibration complete. Using calibrated heading.");
            }

            // Optional serial controls: 'n' to set current as north, 'r' to reset offset
            if (Serial.available() > 0) {
                char c = (char)Serial.read();
                if (c == 'n' || c == 'N') {
                    float h = headingDeg();
                    if (!isnan(h)) {
                        setNorthOffset(-h);
                        Serial.printf("[Mag] North set to current heading. Offset=%.2f°\n", -h);
                    } else {
                        Serial.println("[Mag] Heading not available yet; cannot set north.");
                    }
                } else if (c == 'r' || c == 'R') {
                    setNorthOffset(0.0f);
                    Serial.println("[Mag] North offset reset to 0°");
                }
            }

            if (!isCalibrating()) {
                if (millis() - lastPrint > 1000) {
                    float h = headingDeg();
                    if (!isnan(h)) {
                        const char* dir = directionFromHeading_(h);
                        Serial.printf("[Mag] heading=%.1f° (%s)\n", h, dir);
                    }
                    lastPrint = millis();
                }
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    bool begin(uint8_t i2c_addr) {
        if (!mag.begin(i2c_addr, &Wire)) {
            return false;
        }
        // Start background task to update and print heading
        xTaskCreate(taskMag_, "Mag", 4096, nullptr, 1, nullptr);
        // Begin one-time auto calibration
        Serial.println("[Mag] Calibrating... rotate sensor slowly for 10s");
        startCalibration();
        autoCalibrating  = true;
        autoCalibStartMs = millis();
        return true;
    }

    void update() {
        sensors_event_t magEvent;
        mag.getEvent(&magEvent);

        const float mx = magEvent.magnetic.x;
        const float my = magEvent.magnetic.y;
        const float mz = magEvent.magnetic.z;

        if (calibrating) {
            if (mx < magMin[0]) magMin[0] = mx;
            if (my < magMin[1]) magMin[1] = my;
            if (mz < magMin[2]) magMin[2] = mz;
            if (mx > magMax[0]) magMax[0] = mx;
            if (my > magMax[1]) magMax[1] = my;
            if (mz > magMax[2]) magMax[2] = mz;
            return;
        }

        // Apply calibration
        const float mx_c = (mx - offset[0]) * scale[0];
        const float my_c = (my - offset[1]) * scale[1];
        (void)mz; // heading uses X/Y on flat plane

        float h = atan2f(mx_c, my_c) * 180.0f / PI;
        if (h < 0) h += 360.0f;

        // Declination + manual north offset
        h += declinationDeg;
        while (h >= 360.0f) h -= 360.0f;
        while (h < 0.0f)    h += 360.0f;

        h += northOffsetDeg;
        while (h >= 360.0f) h -= 360.0f;
        while (h < 0.0f)    h += 360.0f;

        heading_deg = h;
    }

    float headingDeg() { return heading_deg; }

    void setDeclination(float deg) { declinationDeg = deg; }

    void setNorthOffset(float deg) { northOffsetDeg = deg; }

    void startCalibration() {
        calibrating = true;
        magMin[0] = magMin[1] = magMin[2] = 10000.0f;
        magMax[0] = magMax[1] = magMax[2] = -10000.0f;
    }

    void stopCalibration() {
        calibrating = false;
        computeCalibration_();
    }

    bool isCalibrating() { return calibrating; }

} // namespace Magnetometer
