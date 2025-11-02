#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <config/Config.h>
#include <net/PlacesClient.h>

void fetchNearbyPlace(String includedTypes, int pressCount, double lat, double lng,
                      double& destinationLatitude, double& destinationLongitude) {
    if (WiFi.status() != WL_CONNECTED)
        return;

    HTTPClient http;
    http.setTimeout(Config::HTTP_TIMEOUT_MS);
    String includedTypesJson = includedTypes;

    String requestBody;
    requestBody.reserve(256);
    requestBody = "{";
    requestBody += "\"locationRestriction\":{\"circle\":{";
    requestBody += "\"center\":{\"latitude\":" + String(lat, 6) +
                   ",\"longitude\":" + String(lng, 6) + "},";
    requestBody += "\"radius\":" + String(pressCount * Config::PLACES_RADIUS_PER_PRESS_M);
    requestBody += "}},";
    requestBody += "\"includedTypes\":" + includedTypesJson + ",";
    requestBody += "\"rankPreference\": \"DISTANCE\","
    requestBody += "\"maxResultCount\":3";
    requestBody += "}";

    Serial.println(requestBody);

    http.begin(Config::URL_PLACES_SEARCH_NEARBY);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Goog-Api-Key", Config::PLACES_API_KEY);
    http.addHeader("X-Goog-FieldMask",
                   "places.displayName,places.formattedAddress,places.types,places.location");

    int httpCode = http.POST(requestBody);

    if (httpCode > 0) {
        String payload = http.getString();
        DynamicJsonDocument  doc(4096);
        DeserializationError err = deserializeJson(doc, payload);

        if (!err && !doc["places"][0]["location"].isNull()) {
            int randomIndex = random(0, 3);
			destinationLatitude = doc["places"][randomIndex]["location"]["latitude"];
			destinationLongitude = doc["places"][randomIndex]["location"]["longitude"];
            Serial.printf("Nearby place: %.6f, %.6f\n", destinationLatitude, destinationLongitude);
        } else {
            Serial.println("Error parsing Places API response");
        }
    } else {
        Serial.println("Error fetching Places API");
    }

    http.end();
}
