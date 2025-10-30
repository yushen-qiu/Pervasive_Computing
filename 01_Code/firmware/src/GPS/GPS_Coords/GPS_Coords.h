#ifndef GPS_COORDS_H
#define GPS_COORDS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPS_Coords
{
public:
    static void begin(int rxPin = 16, int txPin = 17);

    static String getCoordinates();

private:
    static HardwareSerial GPS;
    static TinyGPSPlus gps;
    static int rxPin_;
    static int txPin_;
};

#endif
