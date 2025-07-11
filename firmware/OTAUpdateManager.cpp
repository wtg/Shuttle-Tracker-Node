#include "OTAUpdateManager.h"
#include <Arduino.h>

OTAUpdateManager::OTAUpdateManager(const char* firmwareUrl,
                                   const char* versionUrl,
                                   const char* rootCACert)
    : _firmwareUrl(firmwareUrl),
      _versionUrl(versionUrl),
      _rootCACert(rootCACert),
      _hasChecked(false)
{
    // initialize current version to 0.0.0
    strncpy(_currentVersion, "0.0.0", sizeof(_currentVersion));
    _client.setCACert(_rootCACert);
}

void OTAUpdateManager::begin() {
    // Ensure TLS is configured before any requests
    _client.setCACert(_rootCACert);
}

void OTAUpdateManager::checkAndUpdate() {
    if (_hasChecked) return;
    Serial.println("OTA: Checking for update...");
    if (fetchLatestVersion()) {
        Serial.println("OTA: New version available, starting update.");
        performOTA();
    } else {
        Serial.println("OTA: Firmware up to date.");
    }
    _hasChecked = true;
}

bool OTAUpdateManager::fetchLatestVersion() {
    HTTPClient http;
    _client.setCACert(_rootCACert);
    http.begin(_client, _versionUrl);
    int code = http.GET();
    if (code == HTTP_CODE_OK) {
        String latest = http.getString();
        latest.trim();
        Serial.printf("OTA: Current v=%s, Latest v=%s\n", _currentVersion, latest.c_str());
        if (latest != String(_currentVersion)) {
            // update stored version
            latest.toCharArray(_currentVersion, sizeof(_currentVersion));
            http.end();
            return true;
        }
    } else {
        Serial.printf("OTA: Version check failed, HTTP %d\n", code);
    }
    http.end();
    return false;
}

void OTAUpdateManager::performOTA() {
    _client.setCACert(_rootCACert);
    t_httpUpdate_return ret = httpUpdate.update(_client, _firmwareUrl);
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("OTA: Update failed, err=%d, str=%s\n",
                      httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("OTA: No update available.");
        break;
      case HTTP_UPDATE_OK:
        Serial.println("OTA: Update OK, rebooting...");
        break;
    }
}