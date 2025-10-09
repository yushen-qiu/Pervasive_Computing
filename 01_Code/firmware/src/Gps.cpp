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
#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <math.h>

HardwareSerial GPS(2); // UART2: GPS.TX -> GPIO16, GPS.RX -> GPIO17(可不接)
TinyGPSPlus gps;

static const double R_EARTH = 6371000.0; // Earth radius (meters)

// degree <-> radian
static inline double deg2rad(double d) { return d * PI / 180.0; }
static inline double rad2deg(double r) { return r * 180.0 / PI; }

// Haversine distance (meters)
double haversine(double lat1, double lon1, double lat2, double lon2)
{
    double phi1 = deg2rad(lat1);
    double phi2 = deg2rad(lat2);
    double dphi = deg2rad(lat2 - lat1);
    double dlambda = deg2rad(lon2 - lon1);

    double a = sin(dphi / 2.0) * sin(dphi / 2.0) +
               cos(phi1) * cos(phi2) * sin(dlambda / 2.0) * sin(dlambda / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return R_EARTH * c;
}

// Initial bearing (0~360°, from point1 to point2, relative to true north)
double initial_bearing(double lat1, double lon1, double lat2, double lon2)
{
    double phi1 = deg2rad(lat1);
    double phi2 = deg2rad(lat2);
    double dLambda = deg2rad(lon2 - lon1);

    double y = sin(dLambda) * cos(phi2);
    double x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(dLambda);
    double theta = atan2(y, x); // -pi..pi
    double brng = fmod(rad2deg(theta) + 360.0, 360.0);
    return brng;
}

// 16-wind compass text
const char *compass16(double deg)
{
    static const char *dirs[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
        "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    int idx = (int)round(deg / 22.5) % 16;
    return dirs[idx];
}

// previous fix saved for 10s comparison
struct Fix
{
    double lat = 0;
    double lon = 0;
    double alt = 0;
    uint32_t t_ms = 0;
    bool valid = false;
};
Fix lastFix;

void setup()
{
    Serial.begin(115200);
    GPS.begin(9600, SERIAL_8N1, 16, 17); // NEO-6M 9600
    Serial.println("Collecting GPS... (go outdoor or near a window)");
}

void loop()
{
    while (GPS.available())
        gps.encode(GPS.read());

    if (gps.location.isUpdated())
    {
        double lat = gps.location.lat();
        double lon = gps.location.lng();
        double hdop = gps.hdop.hdop();
        uint32_t nowMs = millis();

        if (!lastFix.valid)
        {
            lastFix.lat = lat;
            lastFix.lon = lon;
            lastFix.alt = gps.altitude.meters();
            lastFix.t_ms = nowMs;
            lastFix.valid = true;
            Serial.println("[init] got first fix.");
            return;
        }

        if (nowMs - lastFix.t_ms >= 10000)
        { // compare every 10s
            double dist = haversine(lastFix.lat, lastFix.lon, lat, lon);
            double brng = initial_bearing(lastFix.lat, lastFix.lon, lat, lon);

            // simple noise guard
            if (dist < 3.0 || !gps.location.isValid())
            {
                Serial.printf("[10s] moved ~%.1fm (below threshold), hdop=%.1f, sats=%u\n",
                              dist, hdop, gps.satellites.value());
            }
            else
            {
                double speed_mps = dist / ((nowMs - lastFix.t_ms) / 1000.0);
                Serial.printf("[10s] moved %.1fm, bearing %.1f° (%s), speed~%.2fm/s, hdop=%.1f, sats=%u\n",
                              dist, brng, compass16(brng), speed_mps, hdop, gps.satellites.value());
            }

            // slide window
            lastFix.lat = lat;
            lastFix.lon = lon;
            lastFix.alt = gps.altitude.meters();
            lastFix.t_ms = nowMs;
            lastFix.valid = true;
        }
    }

    // heartbeat
    static uint32_t t = 0;
    if (millis() - t > 2000)
    {
        Serial.printf("[status] fix=%s, sats=%u, hdop=%.1f, speed=%.2f m/s, course=%.1f°\n",
                      gps.location.isValid() ? "OK" : "NO",
                      gps.satellites.value(), gps.hdop.hdop(),
                      gps.speed.mps(), gps.course.deg());
        t = millis();
    }
}
