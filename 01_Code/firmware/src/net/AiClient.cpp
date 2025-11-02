#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <config/Config.h>
#include <config/PlaceTypes.h>
#include <net/AiClient.h>

String queryGemini(String weatherCondition, String localTime) {
    if (WiFi.status() != WL_CONNECTED)
        return "";

    HTTPClient http;
    http.setTimeout(Config::HTTP_TIMEOUT_MS);
    String url = String(Config::URL_GEMINI_BASE) + "/models/" + Config::GEMINI_MODEL +
                 ":generateContent?key=" + String(Config::GEMINI_API_KEY);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String prompt = "Based on the weather '" + weatherCondition + "' and local time '" + localTime +
                    "', select the most suitable subset of the following place types. "
                    "Return strictly as an array of strings, e.g., [\"beach\",\"park\"], "
                    "with no additional text or explanation: ";
    for (size_t i = 0; i < Config::PLACE_TYPES_COUNT; i++) {
        prompt += String(Config::PLACE_TYPES[i]);
        if (i < (Config::PLACE_TYPES_COUNT - 1))
            prompt += ", ";
    }
    prompt.replace("\"", "\\\"");

    String requestBody = "{\"contents\":[{\"parts\":[{\"text\":\"" + prompt + "\"}]}]}";
    int    httpCode    = http.POST(requestBody);

    String responseStr = "";
    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Raw Gemini response: " + payload);

        JsonDocument         doc;
        DeserializationError err = deserializeJson(doc, payload);

        if (!err && !doc["candidates"][0]["content"]["parts"][0]["text"].isNull()) {
            responseStr =
                    String(doc["candidates"][0]["content"]["parts"][0]["text"].as<const char*>());
            responseStr.replace("```json", "");
            responseStr.replace("```", "");
            responseStr.replace("\n", "");
            responseStr.trim();

            Serial.println("Gemini selected place types: " + responseStr);
        } else {
            Serial.println("Error parsing Gemini response");
        }
    } else {
        Serial.println("Error querying Gemini, HTTP code: " + String(httpCode));
    }

    http.end();
    return responseStr;
}
