#pragma once
#include <Arduino.h>
#include <TinyGPSPlus.h>

struct Fix {
    double   lat   = 0;
    double   lon   = 0;
    double   alt   = 0;
    uint32_t t_ms  = 0;
    bool     valid = false;
};

struct MoveReport {
    bool        moved       = false; // determine if moving during this period
    double      dist_m      = 0;     // straight distance
    double      bearing_deg = 0;     // start-> end, engle of the beginning
    const char* bearing_txt = "";    // 16 Wind Direction Text
    double      speed_mps   = 0;     // avg spd
    double      hdop        = 0;     // Horizontal accuracy
    uint16_t    sats        = 0;     // Number of visible satellites
};

class GpsTracker {
public:
    // Construction: Pass in the hardware serial port instance and the comparison interval
    // (milliseconds)
    explicit GpsTracker(HardwareSerial& serial, uint32_t compareIntervalMs = 10000);

    // Initialise the UART with the serial port pins
    void begin(uint32_t baud, int8_t rxPin, int8_t txPin);

    // Called each time loop() executes; Returns: true if a new 10-second report has been generated.
    bool update();

    // Retrieve the most recent 10-second report (read after update() returns true)
    const MoveReport& lastReport() const {
        return report_;
    }

    // Print heartbeat status (optional)
    void heartbeat(Stream& out, uint32_t everyMs = 2000);

    // Expose TinyGPSPlus directly to facilitate access to additional fields when required
    // (optional)
    TinyGPSPlus& gps() {
        return gps_;
    }

private:
    // Mathematical Tools
    static double deg2rad(double d) {
        return d * PI / 180.0;
    }
    static double rad2deg(double r) {
        return r * 180.0 / PI;
    }
    static double      haversine(double lat1, double lon1, double lat2, double lon2);
    static double      initial_bearing(double lat1, double lon1, double lat2, double lon2);
    static const char* compass16(double deg);

    // Internal state
    HardwareSerial& serial_;
    TinyGPSPlus     gps_;
    Fix             lastFix_;
    uint32_t        compareIntervalMs_;
    uint32_t        lastHeartbeatMs_ = 0;
    MoveReport      report_;
};
