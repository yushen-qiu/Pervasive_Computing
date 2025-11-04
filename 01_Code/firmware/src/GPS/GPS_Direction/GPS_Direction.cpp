#include <GPS/GPS_Direction/GPS_Direction.h>
#include <GPS/GPS_Module/GPSModule.h>
#include <LED/LEDModule.h>

void GPS_UpdateDirection() {
    if (gps.location.isValid() && gps.course.isValid()) {
        double bearing = currentCourse;
        showDirection(bearing); // imp led func
    } else {
        clearLED(); // off if not direction
        Serial.println("[GPS_Direction] No valid course, clearing LED.");
    }
}
