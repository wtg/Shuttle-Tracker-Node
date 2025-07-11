#ifndef OTA_UPDATE_MANAGER_H
#define OTA_UPDATE_MANAGER_H

#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>

class OTAUpdateManager {
public:
    /**
     * @param firmwareUrl  URL to the .bin firmware file
     * @param versionUrl   URL to the version.txt file
     * @param rootCACert   PEM-format root CA certificate
     */
    OTAUpdateManager(const char* firmwareUrl,
                     const char* versionUrl,
                     const char* rootCACert);

    /**
     * Call in setup() to initialize TLS and state
     */
    void begin();

    /**
     * Call in loop() to check once for new version and perform OTA
     */
    void checkAndUpdate();

private:
    const char* _firmwareUrl;
    const char* _versionUrl;
    const char* _rootCACert;

    char _currentVersion[16];
    bool _hasChecked;
    WiFiClientSecure _client;

    /**
     * Fetches latest version string; returns true if newer
     */
    bool fetchLatestVersion();

    /**
     * Runs the HTTPUpdate
     */
    void performOTA();
};

#endif // OTA_UPDATE_MANAGER_H
