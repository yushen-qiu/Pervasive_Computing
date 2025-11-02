#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <config/Config.h>
#include <net/WeatherClient.h>

void fetchWeather(String& condition, String& localTime, double lat, double lng) {
    if (WiFi.status() != WL_CONNECTED)
        return;

    HTTPClient http;
    http.setTimeout(Config::HTTP_TIMEOUT_MS);
    String url = String(Config::URL_WEATHER_CURRENT) + "?key=" + String(Config::WEATHER_API_KEY) +
                 "&q=" + String(lat, 6) + "," + String(lng, 6);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String              payload = http.getString();
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, payload);

        condition = String(doc["current"]["condition"]["text"].as<const char*>());
        localTime = String(doc["location"]["localtime"].as<const char*>());

        Serial.print("Weather: ");
        Serial.println(condition);
        Serial.print("Local time: ");
        Serial.println(localTime);
    } else {
        Serial.println("Error fetching weather");
    }

    http.end();
}
