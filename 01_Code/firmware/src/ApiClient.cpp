// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>

// const char* ssid =
// const char* password =
// const char* apiKey =   // Replace with your Places API key

// // Hardcoded coordinates (33.8884° S, 151.1868° E)
// double latitude = -33.8884;
// double longitude = 151.1868;

// const char* placeTypes[] = {
//   "beach",
//   "art_gallery",
//   "cultural_landmark",
//   "historical_place",
//   "monument",
//   "museum",
//   "sculpture",
//   "amphitheatre",
//   "amusement_center",
//   "amusement_park",
//   "botanical_garden",
//   "casino",
//   "concert_hall",
//   "convention_center",
//   "cultural_center",
//   "cycling_park",
//   "dog_park",
//   "ferris_wheel",
//   "garden",
//   "hiking_area",
//   "historical_landmark",
//   "marina",
//   "national_park",
//   "observation_deck",
//   "opera_house",
//   "park",
//   "picnic_ground",
//   "planetarium",
//   "plaza",
//   "skateboard_park",
//   "state_park",
//   "tourist_attraction",
//   "video_arcade",
//   "wildlife_park",
//   "wildlife_refuge",
//   "zoo"
// };

// void setup() {
//   Serial.begin(115200);

//   // Connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected!");

//   // Build string for types to search for
//   String includedTypesJson = "[";
//   for (int i = 0; i < sizeof(placeTypes) / sizeof(placeTypes[0]); i++) {
//     includedTypesJson += "\"" + String(placeTypes[i]) + "\"";
//     if (i < (sizeof(placeTypes) / sizeof(placeTypes[0]) - 1)) {
//       includedTypesJson += ",";
//     }
//   }
//   includedTypesJson += "]";

//   // Build request body
//   String requestBody = "{"
//     "\"locationRestriction\": {"
//       "\"circle\": {"
//         "\"center\": {\"latitude\": " + String(latitude, 6) + ", \"longitude\": " + String(longitude, 6) + "},"
//         "\"radius\": 1000"
//       "}"
//     "},"
//     "\"includedTypes\": " + includedTypesJson + ","
//     "\"maxResultCount\": 1"
//   "}";

//   // Send POST request
//   HTTPClient http;
//   http.begin("https://places.googleapis.com/v1/places:searchNearby");
//   http.addHeader("Content-Type", "application/json");
//   http.addHeader("X-Goog-Api-Key", apiKey);
//   http.addHeader("X-Goog-FieldMask", "places.displayName,places.formattedAddress,places.types,places.location");

//   int httpCode = http.POST(requestBody);
//   if (httpCode > 0) {
//     JsonDocument doc;
//     deserializeJson(doc, http.getString());
//     double placeLat = doc["places"][0]["location"]["latitude"];
//     double placeLng = doc["places"][0]["location"]["longitude"];
//     Serial.printf("Nearby place: %.6f, %.6f\n", placeLat, placeLng);
//   } else {
//     Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
//   }

//   http.end();
// }

// void loop() {
//   // Nothing here
// };
