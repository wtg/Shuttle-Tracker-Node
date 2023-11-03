#include "WifiManager.h"

WifiManager::WifiManager(const char* ssid, const char* password)
  : _ssid(ssid), _password(password), _lastAttempt(0), _state(WIFI_STATE_INIT) {}

void WifiManager::attemptConnect() {
    unsigned long now = millis();
    switch(_state) {
        case WIFI_STATE_INIT:
            if(WiFi.status() != WL_CONNECTED) {
                _lastAttempt = now; 
                WiFi.begin(_ssid, _password);
                Serial.print("Attempting to connect to WiFi");
                _state = WIFI_STATE_CONNECTING;
            } else {
                Serial.println("Already connected to WiFi");
            }
            break;
            
        case WIFI_STATE_CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("\nConnected to WiFi");
                Serial.println(WiFi.localIP());
                _state = WIFI_STATE_CONNECTED;
            } else if (now - _lastAttempt > 5000) { // 5 seconds has passed
                Serial.println("\nConnection attempt timed out");
                WiFi.disconnect();
                _lastAttempt = now; // Mark the time when we entered the timeout
                _state = WIFI_STATE_TIMEOUT;
            } else {
                Serial.print(".");
            }
            break;

        case WIFI_STATE_CONNECTED:
            break;
            
        case WIFI_STATE_TIMEOUT:
            if (now - _lastAttempt > 15000) { // 15s hour 
                _state = WIFI_STATE_INIT; // Go back to the initial state to retry the connection
            }
            break;
    }
}

bool WifiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
