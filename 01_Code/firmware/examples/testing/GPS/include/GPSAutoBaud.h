// moved to examples/testing
#pragma once
#include <Arduino.h>

/**
 * Auto-baud GPS helper:
 *  - Probes a list of baud rates until NMEA ('$') is detected
 *  - Streams NMEA as full lines (LF-delimited, CR filtered)
 *  - If silent for `silenceMs`, it re-probes automatically
 */
class GpsAutoBaud {
public:
    // pins: RX is the ESP32 pin that receives GPS TX; TX may be -1 if unused
    GpsAutoBaud(HardwareSerial& port, int rxPin, int txPin, const int* baudList, size_t baudCount,
                uint32_t silenceMs = 5000, size_t maxLineLen = 120);

    // Start the USB debug serial first (e.g., Serial.begin(115200)), then call begin()
    void begin();

    // Call in loop(). Returns true if at least one full line got flushed to `out`.
    bool update(Stream& out);

    // Current locked baud, or -1 if not locked yet
    int currentBaud() const {
        return currentBaud_;
    }

    // Force a re-probe now
    void reprobe();

private:
    // --- internals ---
    void startGPS_(int baud);
    bool hasNMEA_(unsigned long ms);
    void lockBaud_();

    HardwareSerial& gps_;
    const int*      baudList_;
    size_t          baudCount_;
    int             rxPin_;
    int             txPin_;
    int             currentBaud_ = -1;

    uint32_t silenceMs_;
    size_t   maxLineLen_;
    uint32_t lastDollarMs_ = 0;

    String line_;
};
