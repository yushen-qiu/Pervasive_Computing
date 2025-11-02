#include <WiFi.h>
#include <net/WiFiUtil.h>

namespace WiFiUtil {

    bool connect(const char* ssid, const char* password, unsigned long timeoutMs) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("Connecting to WiFi");
        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print('.');
            if (millis() - start > timeoutMs) {
                Serial.println("\nWiFi connect timeout");
                return false;
            }
        }
        Serial.println("\nWiFi Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }

} // namespace WiFiUtil
