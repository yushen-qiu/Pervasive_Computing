#include <Arduino.h>
#include "./GPS/GPSModule.h"
#include "./LED/LEDModule.h"

void setup()
{
    Serial.begin(115200);
    initGPS();
    initLED();

    Serial.println("GPS + LED Compass started. Move to open sky...");

    xTaskCreatePinnedToCore(taskGPS, "taskGPS", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskLED, "taskLED", 4096, NULL, 1, NULL, 0);
}

void loop()
{
}