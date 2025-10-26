// #include <Arduino.h>
// #include <FastLED.h>
// #include <TinyGPSPlus.h>
// #include <HardwareSerial.h>

// // ====================== LED config ======================
// #define LED_PIN 4
// #define NUM_LEDS 16
// #define LED_TYPE WS2812
// #define COLOR_ORDER GRB
// #define BRIGHTNESS 150
// CRGB leds[NUM_LEDS];

// // ====================== GPS config ======================
// HardwareSerial GPS(2); // 使用 UART2
// TinyGPSPlus gps;
// volatile double currentCourse = 0; // Current heading angle (shared variable)
// volatile double currentSpeed = 0;  // Current speed (m/s)
// volatile double currentLat = 0;    // Current latitude
// volatile double currentLng = 0;    // Current longitude

// // Northern shift
// const int NORTH_OFFSET = 4; // Clockwise rotation of 4 lights = 90°

// // Map heading angle to LED position (0–15)// Map heading angle to LED position (0–15)
// int bearingToIndex(double bearingDeg)
// {
//     int idx = (int)round((bearingDeg / 360.0) * NUM_LEDS) % NUM_LEDS;
//     return idx;
// }

// // ====================== GPS  ======================
// void taskGPS(void *pvParameters)
// {
//     (void)pvParameters;
//     for (;;)
//     {
//         while (GPS.available())
//         {
//             gps.encode(GPS.read());

//             if (gps.location.isValid())
//             {
//                 currentLat = gps.location.lat();
//                 currentLng = gps.location.lng();
//             }

//             if (gps.course.isUpdated() && gps.course.deg() > 0)
//             {
//                 currentCourse = gps.course.deg(); // Update heading angle
//             }

//             if (gps.speed.isUpdated())
//             {
//                 currentSpeed = gps.speed.mps(); // Update speed
//             }
//         }

//         // Print status once every second
//         static uint32_t lastPrint = 0;
//         if (millis() - lastPrint > 1000)
//         {
//             Serial.printf("[GPS] fix=%s, sats=%u, hdop=%.1f, speed=%.2f m/s, course=%.1f°,
//             lat=%.5f, lon=%.5f\n",
//                           gps.location.isValid() ? "OK" : "NO",
//                           gps.satellites.value(),
//                           gps.hdop.hdop(),
//                           gps.speed.mps(),
//                           gps.course.deg(),
//                           gps.location.lat(),
//                           gps.location.lng());
//             lastPrint = millis();
//         }

//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
// }

// // ====================== LED ======================
// void taskLED(void *pvParameters)
// {
//     (void)pvParameters;
//     uint8_t hue = 0;
//     static int lastCenter = -1; // The last light to be switched on

//     for (;;)
//     {
//         FastLED.clear();

//         if (gps.location.isValid() && gps.course.isValid() && currentCourse > 0)
//         {
//             int center = (bearingToIndex(currentCourse) + NORTH_OFFSET) % NUM_LEDS;

//             // Set the main light and two auxiliary lights on either side.
//             leds[center] = CHSV((uint8_t)(currentCourse / 2), 255, 255);
//             leds[(center + 1) % NUM_LEDS] = CHSV((uint8_t)(currentCourse / 2), 255, 100);
//             leds[(center + NUM_LEDS - 1) % NUM_LEDS] = CHSV((uint8_t)(currentCourse / 2), 255,
//             100);

//             // If the direction changes, print
//             if (center != lastCenter)
//             {
//                 Serial.printf("[LED] Heading %.1f° -> LED #%d (North offset %d)\n",
//                               currentCourse, center, NORTH_OFFSET);
//                 lastCenter = center;
//             }
//         }
//         else
//         {
//             // Display rainbow animation when no positioning or direction is specified
//             fill_rainbow(leds, NUM_LEDS, hue++, 8);
//             Serial.println("[LED] No valid GPS direction yet... showing rainbow.");
//         }

//         FastLED.show();
//         vTaskDelay(500 / portTICK_PERIOD_MS); // Refreshes every 0.5 seconds
//     }
// }

// // ====================== Initialisation ======================
// void setup()
// {
//     Serial.begin(115200);
//     GPS.begin(9600, SERIAL_8N1, 16, 17); // GPS.TX → GPIO16, GPS.RX → GPIO17 (may be left
//     unconnected)

//     FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
//     FastLED.setBrightness(BRIGHTNESS);
//     FastLED.clear();
//     FastLED.show();

//     Serial.println(" GPS + LED Compass started. Move to open sky...");

//     // Create a FreeRTOS task
//     xTaskCreatePinnedToCore(taskGPS, "taskGPS", 4096, NULL, 2, NULL, 1); // Core 1
//     xTaskCreatePinnedToCore(taskLED, "taskLED", 4096, NULL, 1, NULL, 0); // Core 0
// }

// void loop()
// {
//     // Main loop idle, tasks executed in parallel
// }
