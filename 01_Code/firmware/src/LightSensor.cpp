// // Ambient light sensor implementation using ADC with auto-calibration
// #include <Arduino.h>
// #include <config/Config.h>
// #include <sensors/LightSensor.h>

// namespace {
//     int           g_minVal         = 1023;
//     int           g_maxVal         = 0;
//     int           g_lastRaw        = 0;
//     int           g_lastPercent    = 0;
//     bool          g_calibrated     = false;
//     unsigned long g_calibStartMs   = 0;
//     unsigned long g_calibrationLen = 5000; // default 5s
// } // namespace

// namespace LightSensor {

//     void begin(unsigned long calibrationMs) {
//         g_minVal         = 1023;
//         g_maxVal         = 0;
//         g_lastRaw        = 0;
//         g_lastPercent    = 0;
//         g_calibrated     = false;
//         g_calibStartMs   = millis();
//         g_calibrationLen = calibrationMs;

//         pinMode(Config::LIGHT_SENSOR_PIN, INPUT);
//     }

//     void tick() {
//         int v     = analogRead(Config::LIGHT_SENSOR_PIN);
//         g_lastRaw = v;

//         if (!g_calibrated) {
//             if (v > g_maxVal)
//                 g_maxVal = v;
//             if (v < g_minVal)
//                 g_minVal = v;
//             if (millis() - g_calibStartMs >= g_calibrationLen) {
//                 // handle degenerate case
//                 if (g_maxVal <= g_minVal) {
//                     g_minVal = 0;
//                     g_maxVal = 1023;
//                 }
//                 g_calibrated = true;
//                 Serial.printf("[Light] Calibrated min=%d max=%d\n", g_minVal, g_maxVal);
//             }
//         }

//         int clamped = v;
//         if (g_calibrated) {
//             if (clamped < g_minVal)
//                 clamped = g_minVal;
//             if (clamped > g_maxVal)
//                 clamped = g_maxVal;
//         }
//         g_lastPercent = map(clamped, g_minVal, g_maxVal, 0, 100);
//         if (g_lastPercent < 0)
//             g_lastPercent = 0;
//         if (g_lastPercent > 100)
//             g_lastPercent = 100;
//     }

//     bool isCalibrated() {
//         return g_calibrated;
//     }

//     int raw() {
//         return g_lastRaw;
//     }

//     int percent() {
//         return g_lastPercent;
//     }

//     int minValue() {
//         return g_minVal;
//     }

//     int maxValue() {
//         return g_maxVal;
//     }

// } // namespace LightSensor
