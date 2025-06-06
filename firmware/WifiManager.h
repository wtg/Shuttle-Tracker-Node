#ifndef WifiManager_h
#define WifiManager_h

#include "WiFi.h"

class WifiManager {
public:
    WifiManager(const char* ssid, const char* password);
    void attemptConnect(); // Check and try to connect if not connected
    bool isConnected();
private:
    const char* _ssid;
    const char* _password;

        unsigned long _lastAttempt;
    
    enum {
        WIFI_STATE_INIT,
        WIFI_STATE_CONNECTING,
        WIFI_STATE_CONNECTED,
        WIFI_STATE_TIMEOUT
    } _state;
};

#endif // WIFI_MANAGER_H
