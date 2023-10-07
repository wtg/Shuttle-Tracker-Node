#include "WifiManager.h"

WifiManager::WifiManager(const char* ssid, const char* password)
  : _ssid(ssid), _password(password) {}

void WifiManager::attemptConnect() {
    if(WiFi.status() != WL_CONNECTED) {
        WiFi.begin(_ssid, _password);
        Serial.print("Attempting to connect to WiFi");
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
        Serial.println("\nConnected to WiFi");
        Serial.println(WiFi.localIP());
    }
}

bool WifiManager:: isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
