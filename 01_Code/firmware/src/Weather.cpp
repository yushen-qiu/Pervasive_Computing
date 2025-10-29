#include "Weather.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* weatherApiKey = "";

void fetchWeather(String &condition, String &localTime, double lat, double lng) {
	if (WiFi.status() != WL_CONNECTED) return;

	HTTPClient http;
	String url = "http://api.weatherapi.com/v1/current.json?key=" + String(weatherApiKey) +
				 "&q=" + String(lat, 6) + "," + String(lng, 6);
	http.begin(url);
	int httpCode = http.GET();

	if (httpCode > 0) {
		String payload = http.getString();
		JsonDocument doc;
		deserializeJson(doc, payload);

		condition = String(doc["current"]["condition"]["text"].as<const char*>());
		localTime = String(doc["location"]["localtime"].as<const char*>());

		Serial.print("Weather: "); Serial.println(condition);
		Serial.print("Local time: "); Serial.println(localTime);
	} else {
		Serial.println("Error fetching weather");
	}

	http.end();
}
