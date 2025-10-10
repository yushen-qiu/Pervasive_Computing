#include "GpsTracker.h"
#include <math.h>

static const double R_EARTH = 6371000.0; // Earth's radius (metres)

GpsTracker::GpsTracker(HardwareSerial &serial, uint32_t compareIntervalMs)
    : serial_(serial), compareIntervalMs_(compareIntervalMs) {}

void GpsTracker::begin(uint32_t baud, int8_t rxPin, int8_t txPin)
{
    serial_.begin(baud, SERIAL_8N1, rxPin, txPin);
}

bool GpsTracker::update()
{
    // feeding NMEA
    while (serial_.available())
        gps_.encode(serial_.read());

    const uint32_t nowMs = millis();
    if (!gps_.location.isUpdated())
        return false;

    const double lat = gps_.location.lat();
    const double lon = gps_.location.lng();

    if (!lastFix_.valid)
    {
        lastFix_.lat = lat;
        lastFix_.lon = lon;
        lastFix_.alt = gps_.altitude.meters();
        lastFix_.t_ms = nowMs;
        lastFix_.valid = true;
        return false; // Initial positioning, no report generated
    }

    if (nowMs - lastFix_.t_ms < compareIntervalMs_)
        return false;

    // Satisfy comparison window, calculate displacement/azimuth
    const double dist = haversine(lastFix_.lat, lastFix_.lon, lat, lon);
    const double brng = initial_bearing(lastFix_.lat, lastFix_.lon, lat, lon);

    // Simple noise threshold (<3m deemed stationary)
    const bool moved = dist >= 3.0 && gps_.location.isValid();
    const double speed = dist / ((nowMs - lastFix_.t_ms) / 1000.0);

    report_.moved = moved;
    report_.dist_m = dist;
    report_.bearing_deg = brng;
    report_.bearing_txt = compass16(brng);
    report_.speed_mps = speed;
    report_.hdop = gps_.hdop.hdop();
    report_.sats = gps_.satellites.value();

    // Sliding window
    lastFix_.lat = lat;
    lastFix_.lon = lon;
    lastFix_.alt = gps_.altitude.meters();
    lastFix_.t_ms = nowMs;

    return true; // A new report has been produced.
}

void GpsTracker::heartbeat(Stream &out, uint32_t everyMs)
{
    const uint32_t now = millis();
    if (now - lastHeartbeatMs_ < everyMs)
        return;
    lastHeartbeatMs_ = now;

    out.printf("[status] fix=%s, sats=%u, hdop=%.1f, speed=%.2f m/s, course=%.1f°\n",
               gps_.location.isValid() ? "OK" : "NO",
               gps_.satellites.value(), gps_.hdop.hdop(),
               gps_.speed.mps(), gps_.course.deg());
}

// ====== Static utility functions ======
double GpsTracker::haversine(double lat1, double lon1, double lat2, double lon2)
{
    const double phi1 = deg2rad(lat1);
    const double phi2 = deg2rad(lat2);
    const double dphi = deg2rad(lat2 - lat1);
    const double dlambda = deg2rad(lon2 - lon1);

    const double a = sin(dphi / 2.0) * sin(dphi / 2.0) +
                     cos(phi1) * cos(phi2) * sin(dlambda / 2.0) * sin(dlambda / 2.0);
    const double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R_EARTH * c;
}

double GpsTracker::initial_bearing(double lat1, double lon1, double lat2, double lon2)
{
    const double phi1 = deg2rad(lat1);
    const double phi2 = deg2rad(lat2);
    const double dLambda = deg2rad(lon2 - lon1);

    const double y = sin(dLambda) * cos(phi2);
    const double x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(dLambda);
    const double theta = atan2(y, x); // -pi..pi
    const double brng = fmod(rad2deg(theta) + 360.0, 360.0);
    return brng;
}

const char *GpsTracker::compass16(double deg)
{
    static const char *dirs[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
        "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    const int idx = static_cast<int>(round(deg / 22.5)) % 16;
    return dirs[idx];
}

// #include <Arduino.h>

// // UART2: GPS.TX -> GPIO16 (RX2), GPS.RX -> GPIO17 (TX2, optional)
// HardwareSerial GPS(2);
// static const int PIN_RX2 = 16; // ESP32 receive（with GPS  TX）
// static const int PIN_TX2 = 17; // ESP32 sending（with GPS  RX, optional）

// // NEO series rate
// int candidates[] = {9600, 38400, 57600, 115200, 4800};
// int currentBaud = -1;

// void startGPS(int baud)
// {
//   GPS.end();
//   delay(30);
//   GPS.begin(baud, SERIAL_8N1, PIN_RX2, PIN_TX2);
//   currentBaud = baud;
//   Serial.printf("\n[Probe] Trying GPS baud = %d ...\n", baud);
// }

// // return if detect NMEA start with '$'
// bool hasNMEA(unsigned long ms = 1200)
// {
//   unsigned long t0 = millis();
//   while (millis() - t0 < ms)
//   {
//     while (GPS.available())
//     {
//       char c = GPS.read();
//       if (c == '$')
//         return true;
//     }

//     delay(1);
//   }
//   return false;
// }

// // keep searching if not locked
// void lockBaud()
// {
//   while (true)
//   {
//     for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
//     {
//       startGPS(candidates[i]);
//       if (hasNMEA())
//       {
//         Serial.printf("[OK] NMEA detected at %d baud.\n", currentBaud);
//         Serial.println("-------- NMEA stream (lines) --------");
//         return;
//       }
//     }
//     Serial.println("[Retry] No NMEA yet. Re-trying in 1s...");
//     delay(1000);
//   }
// }

// void setup()
// {
//   Serial.begin(115200); // USB testing port（115200）
//   Serial.println("\nAuto-bauding GPS on UART2 (RX2=GPIO16, TX2=GPIO17)...");
//   lockBaud();
// }

// void loop()
// {
//   static String line;
//   static unsigned long lastDollarMs = millis();

//   //  GPS data show up
//   while (GPS.available())
//   {
//     char c = GPS.read();
//     if (c == '$')
//       lastDollarMs = millis(); // record time
//     if (c == '\r')
//       continue; // filter CR
//     if (c == '\n')
//     {
//       if (line.length())
//       {
//         Serial.println(line);
//         line = "";
//       }
//     }
//     else
//     {
//       line += c;
//       if (line.length() > 120)
//       {
//         Serial.println(line);
//         line = "";
//       }
//     }
//   }

//   // over 5 sec not meet '$'，Indicates serial port silence or baud rate mismatch; please retry detection.
//   if (millis() - lastDollarMs > 5000)
//   {
//     Serial.println("\n[Lost] No NMEA for 5s. Re-probing baud...");
//     lockBaud();
//     lastDollarMs = millis();
//   }
// }
