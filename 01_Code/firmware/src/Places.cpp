#include "Places.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* placesApiKey = "";

void fetchNearbyPlace(String includedTypes, int pressCount, double lat, double lng, double &destinationLatitude, double &destinationLongitude) {
	if (WiFi.status() != WL_CONNECTED) return;

	HTTPClient http;
	String includedTypesJson = includedTypes;

	String requestBody = "{"
		"\"locationRestriction\": {"
			"\"circle\": {"
				"\"center\": {\"latitude\": " + String(lat, 6) + ", \"longitude\": " + String(lng, 6) + "},"
				"\"radius\": " + String(pressCount * 250) +
			"}"
		"},"
		"\"includedTypes\": " + includedTypesJson + ","
		"\"maxResultCount\": 1"
	"}";

	Serial.println(requestBody);

	http.begin("https://places.googleapis.com/v1/places:searchNearby");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("X-Goog-Api-Key", placesApiKey);
	http.addHeader("X-Goog-FieldMask", "places.displayName,places.formattedAddress,places.types,places.location");

	int httpCode = http.POST(requestBody);

	if (httpCode > 0) {
		String payload = http.getString();
		JsonDocument doc;
		DeserializationError err = deserializeJson(doc, payload);

		if (!err && !doc["places"][0]["location"].isNull()) {
			destinationLatitude = doc["places"][0]["location"]["latitude"];
			destinationLongitude = doc["places"][0]["location"]["longitude"];
			Serial.printf("Nearby place: %.6f, %.6f\n", destinationLatitude, destinationLongitude);
		} else {
			Serial.println("Error parsing Places API response");
		}
	} else {
		Serial.println("Error fetching Places API");
	}

	http.end();
}

