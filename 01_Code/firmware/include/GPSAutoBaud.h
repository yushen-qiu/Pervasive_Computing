#pragma once
#include <Arduino.h>

/**
 * Auto-baud GPS helper:
 *  - Probes a list of baud rates until NMEA ('$') is detected
 *  - Streams NMEA as full lines (LF-delimited, CR filtered)
 *  - If silent for `silenceMs`, it re-probes automatically
 */
class GpsAutoBaud
{
public:
    // pins: RX is the ESP32 pin that receives GPS TX; TX may be -1 if unused
    GpsAutoBaud(HardwareSerial &port, int rxPin, int txPin,
                const int *baudList, size_t baudCount,
                uint32_t silenceMs = 5000, size_t maxLineLen = 120);

    // Start the USB debug serial first (e.g., Serial.begin(115200)), then call begin()
    void begin();

    // Call in loop(). Returns true if at least one full line got flushed to `out`.
    bool update(Stream &out);

    // Current locked baud, or -1 if not locked yet
    int currentBaud() const { return currentBaud_; }

    // Force a re-probe now
    void reprobe();

private:
    // --- internals ---
    void startGPS_(int baud);
    bool hasNMEA_(unsigned long ms);
    void lockBaud_();

    HardwareSerial &gps_;
    const int *baudList_;
    size_t baudCount_;
    int rxPin_;
    int txPin_;
    int currentBaud_ = -1;

    uint32_t silenceMs_;
    size_t maxLineLen_;
    uint32_t lastDollarMs_ = 0;

    String line_;
};

// To use this code, follow the calling fucntion below:

// #include <Arduino.h>
// #include "GPSAutoBaud.h"

// // UART2: GPS.TX -> GPIO16 (RX2), GPS.RX -> GPIO17 (TX2, optional)
// HardwareSerial GPS2(2);
// static const int PIN_RX2 = 16;
// static const int PIN_TX2 = 17;

// // Candidate baud rates for u-blox NEO series (you can add/remove here)
// static int kBauds[] = {9600, 38400, 57600, 115200, 4800};

// GpsAutoBaud gpsAuto(GPS2, PIN_RX2, PIN_TX2, kBauds, sizeof(kBauds)/sizeof(kBauds[0]),
//                     /*silenceMs=*/5000, /*maxLineLen=*/120);

// void setup() {
//   Serial.begin(115200);                    // USB logging
//   Serial.println("\nAuto-bauding GPS on UART2...");
//   gpsAuto.begin();                         // probe & lock
// }

// void loop() {
//   // Flush full NMEA lines to Serial; auto re-probes if silent for too long
//   gpsAuto.update(Serial);

//   // (Optional) You can access the locked baud like this:
//   // int cur = gpsAuto.currentBaud();
// }
