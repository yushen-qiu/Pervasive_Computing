// Gemini call to select place types from weather/time
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <apis/AI.h>
#include <config/Secrets.h>

const char* allPlaceTypes[] = {"beach",
                               "cultural_landmark",
                               "historical_place",
                               "monument",
                               "sculpture",
                               "amphitheatre",
                               "botanical_garden",
                               "cultural_center",
                               "garden",
                               "hiking_area",
                               "historical_landmark",
                               "national_park",
                               "park",
                               "picnic_ground",
                               "plaza",
                               "playground"
                               "tourist_attraction",
                                };

String queryGemini(String weatherCondition, String localTime) {
    if (WiFi.status() != WL_CONNECTED)
        return "";

    HTTPClient http;
    http.setTimeout(30000);
    String url = "https://generativelanguage.googleapis.com/v1beta/models/"
                 "gemini-2.5-flash:generateContent?key=" +
                 String(Config::GOOGLE_API_KEY);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String prompt = "Based on the weather '" + weatherCondition + "' and local time '" + localTime +
                    "', select the most suitable subset of the following place types. "
                    "Return strictly as an array of strings, e.g., [\"beach\",\"park\"], "
                    "with no additional text or explanation: ";
    for (int i = 0; i < sizeof(allPlaceTypes) / sizeof(allPlaceTypes[0]); i++) {
        prompt += String(allPlaceTypes[i]);
        if (i < (sizeof(allPlaceTypes) / sizeof(allPlaceTypes[0]) - 1))
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
