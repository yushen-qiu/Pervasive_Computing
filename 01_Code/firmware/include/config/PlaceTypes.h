// Centralized list of supported Google Places types
#pragma once

#include <Arduino.h>

namespace Config {
    // External declarations for the place types list and its count.
    // Defined in src/config/PlaceTypes.cpp
    extern const char* const PLACE_TYPES[];
    extern const size_t      PLACE_TYPES_COUNT;
} // namespace Config

