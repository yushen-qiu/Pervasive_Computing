
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

#include "GPSAutoBaud.h"

GpsAutoBaud::GpsAutoBaud(HardwareSerial &port, int rxPin, int txPin,
                         const int *baudList, size_t baudCount,
                         uint32_t silenceMs, size_t maxLineLen)
    : gps_(port),
      baudList_(baudList),
      baudCount_(baudCount),
      rxPin_(rxPin),
      txPin_(txPin),
      silenceMs_(silenceMs),
      maxLineLen_(maxLineLen) {}

void GpsAutoBaud::begin()
{
    // First probe and lock baud
    lockBaud_();
}

void GpsAutoBaud::reprobe()
{
    lockBaud_();
}

bool GpsAutoBaud::update(Stream &out)
{
    bool flushedLine = false;

    // Read all available bytes, build LF-delimited lines (strip CR)
    while (gps_.available())
    {
        char c = gps_.read();
        if (c == '$')
            lastDollarMs_ = millis();
        if (c == '\r')
            continue;

        if (c == '\n')
        {
            if (line_.length())
            {
                out.println(line_);
                line_ = "";
                flushedLine = true;
            }
        }
        else
        {
            line_ += c;
            if (line_.length() > maxLineLen_)
            {
                out.println(line_);
                line_ = "";
                flushedLine = true;
            }
        }
    }

    // Silence watchdog: no '$' seen for a while -> re-probe
    if (millis() - lastDollarMs_ > silenceMs_)
    {
        out.println("\n[Lost] No NMEA for a while. Re-probing baud...");
        lockBaud_();
        lastDollarMs_ = millis();
    }

    return flushedLine;
}

void GpsAutoBaud::startGPS_(int baud)
{
    gps_.end();
    delay(30);
    gps_.begin(baud, SERIAL_8N1, rxPin_, txPin_);
    currentBaud_ = baud;
    Serial.printf("\n[Probe] Trying GPS baud = %d ...\n", baud);
}

bool GpsAutoBaud::hasNMEA_(unsigned long ms)
{
    unsigned long t0 = millis();
    while (millis() - t0 < ms)
    {
        while (gps_.available())
        {
            char c = gps_.read();
            if (c == '$')
                return true;
        }
        delay(1);
    }
    return false;
}

void GpsAutoBaud::lockBaud_()
{
    while (true)
    {
        for (size_t i = 0; i < baudCount_; ++i)
        {
            startGPS_(baudList_[i]);
            if (hasNMEA_(1200))
            {
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
