// moved to examples/testing
#include "GPSAutoBaud.h"

GpsAutoBaud::GpsAutoBaud(HardwareSerial& port, int rxPin, int txPin, const int* baudList,
                         size_t baudCount, uint32_t silenceMs, size_t maxLineLen)
    : gps_(port), baudList_(baudList), baudCount_(baudCount), rxPin_(rxPin), txPin_(txPin),
      silenceMs_(silenceMs), maxLineLen_(maxLineLen) {}

void GpsAutoBaud::begin() {
    // First probe and lock baud
    lockBaud_();
}

void GpsAutoBaud::reprobe() {
    lockBaud_();
}

bool GpsAutoBaud::update(Stream& out) {
    bool flushedLine = false;

    // Read all available bytes, build LF-delimited lines (strip CR)
    while (gps_.available()) {
        char c = gps_.read();
        if (c == '$')
            lastDollarMs_ = millis();
        if (c == '\r')
            continue;

        if (c == '\n') {
            if (line_.length()) {
                out.println(line_);
                line_       = "";
                flushedLine = true;
            }
        } else {
            line_ += c;
            if (line_.length() > maxLineLen_) {
                out.println(line_);
                line_       = "";
                flushedLine = true;
            }
        }
    }

    // Silence watchdog: no '$' seen for a while -> re-probe
    if (millis() - lastDollarMs_ > silenceMs_) {
        out.println("\n[Lost] No NMEA for a while. Re-probing baud...");
        lockBaud_();
        lastDollarMs_ = millis();
    }

    return flushedLine;
}

void GpsAutoBaud::startGPS_(int baud) {
    gps_.end();
    delay(30);
    gps_.begin(baud, SERIAL_8N1, rxPin_, txPin_);
    currentBaud_ = baud;
    Serial.printf("\n[Probe] Trying GPS baud = %d ...\n", baud);
}

bool GpsAutoBaud::hasNMEA_(unsigned long ms) {
    unsigned long t0 = millis();
    while (millis() - t0 < ms) {
        while (gps_.available()) {
            char c = gps_.read();
            if (c == '$')
                return true;
        }
        delay(1);
    }
    return false;
}

void GpsAutoBaud::lockBaud_() {
    while (true) {
        for (size_t i = 0; i < baudCount_; ++i) {
            startGPS_(baudList_[i]);
            if (hasNMEA_(1200)) {
                Serial.printf("[OK] NMEA detected at %d baud.\n", currentBaud_);
                Serial.println("-------- NMEA stream (lines) --------");
                lastDollarMs_ = millis();
                return;
            }
        }
        Serial.println("[Retry] No NMEA yet. Re-trying in 1s...");
        delay(1000);
    }
}
