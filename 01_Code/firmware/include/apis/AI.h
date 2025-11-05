/*
 * AI API (Gemini) - Interface
 */
#pragma once
#include <Arduino.h>

// Returns JSON-like string of selected place types
String queryGemini(String weatherCondition, String localTime);
