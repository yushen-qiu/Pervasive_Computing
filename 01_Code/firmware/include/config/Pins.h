// Centralized hardware pin assignments
#pragma once

#include <Arduino.h>

namespace Pins {
    // Buttons, switches, inputs
    constexpr uint8_t BUTTON = 14; // User button

    // LED strip / ring
    constexpr uint8_t LED       = 4;  // Data pin for addressable LEDs
    constexpr uint8_t LED_COUNT = 16; // Number of LEDs in the ring/strip

    // GPS UART (ESP32 example: Serial2)
    constexpr int GPS_RX = 16; // GPS module TX -> MCU RX
    constexpr int GPS_TX = 17; // GPS module RX -> MCU TX
} // namespace Pins
