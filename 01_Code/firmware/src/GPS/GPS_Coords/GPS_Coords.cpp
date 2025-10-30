#include "GPS_Coords.h"

HardwareSerial GPS_Coords::GPS(2);
TinyGPSPlus GPS_Coords::gps;
int GPS_Coords::rxPin_ = 16;
int GPS_Coords::txPin_ = 17;

// ====================== initialise ======================
void GPS_Coords::begin(int rxPin, int txPin)
{
    rxPin_ = rxPin;
    txPin_ = txPin;
    GPS.begin(115200, SERIAL_8N1, rxPin_, txPin_);
    Serial.println("[GPS] Module initialised at 9600 baud.");
}

// ====================== get coords ======================
String GPS_Coords::getCoordinates()
{
    while (GPS.available() > 0)
    {
        gps.encode(GPS.read());
    }

    if (gps.location.isValid())
    {
        double lat = gps.location.lat();
        double lon = gps.location.lng();

        char buffer[40];
        snprintf(buffer, sizeof(buffer), "%.6f, %.6f", lat, lon);
        return String(buffer);
    }
    else
    {
        return String("NO FIX");
    }
}
