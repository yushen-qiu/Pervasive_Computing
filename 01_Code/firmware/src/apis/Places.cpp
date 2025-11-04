#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <apis/Places.h>
#include <config/Secrets.h>
static int pressMultiplier = 200;

void fetchNearbyPlace(String includedTypes, int pressCount, double lat, double lng,
                      double& destinationLatitude, double& destinationLongitude) {
    if (WiFi.status() != WL_CONNECTED)
        return;

    HTTPClient http;
    String     includedTypesJson = includedTypes;

    String requestBody = "{"
                         "\"locationRestriction\": {"
                         "\"circle\": {"
                         "\"center\": {\"latitude\": " +
                         String(lat, 6) + ", \"longitude\": " + String(lng, 6) +
                         "},"
                         "\"radius\": " +
                         String(pressCount * pressMultiplier) +
                         "}"
                         "},"
                         "\"includedTypes\": " +
                         includedTypesJson +
                         ","
                         "\"rankPreference\": \"DISTANCE\","
                         "\"maxResultCount\": 3"
                         "}";

    Serial.println("Request Body:");
    Serial.println(requestBody);

    http.begin("https://places.googleapis.com/v1/places:searchNearby");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Goog-Api-Key", Config::PLACES_API_KEY);
    http.addHeader("X-Goog-FieldMask",
                   "places.displayName,places.formattedAddress,places.types,places.location");

    int httpCode = http.POST(requestBody);

    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("API Response:");
        Serial.println(payload);

        JsonDocument         doc;
        DeserializationError err = deserializeJson(doc, payload);

        if (!err && doc["places"].size() > 0) {
            // Randomly pick one of the results
            int randomIndex = random(0, 3);

            destinationLatitude  = doc["places"][randomIndex]["location"]["latitude"];
            destinationLongitude = doc["places"][randomIndex]["location"]["longitude"];

            Serial.printf("Selected place %d: %.6f, %.6f\n", randomIndex, destinationLatitude,
                          destinationLongitude);
        } else {
            Serial.println("Error parsing Places API response or no places found.");
        }
    } else {
        Serial.printf("HTTP request failed. Code: %d\n", httpCode);
    }

    http.end();
}
