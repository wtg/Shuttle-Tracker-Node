#include "Battery.h"
#include "Beacon.h"
#include "Display.h"
#include "IO.h"
#include "WiFiClient.h"
#include "ArduinoOTA.h"
#include "HTTPUpdate.h"
#include "esp_wifi.h"
#include "WiFiClient.h"
#include "WifiManager.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <string.h>

char firmwareVersion[] = "0.0.0";
bool hasCheckedForUpdate = false; 

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n" \
"WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n" \
"RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n" \
"AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n" \
"R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n" \
"sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n" \
"NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n" \
"Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n" \
"/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n" \
"Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n" \
"FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n" \
"AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n" \
"Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n" \
"gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n" \
"PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n" \
"ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n" \
"CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n" \
"lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n" \
"avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n" \
"yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n" \
"yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n" \
"hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n" \
"HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n" \
"MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n" \
"nLRbwHOoq7hHwg==\n" \
"-----END CERTIFICATE-----\n";



const char* firmwareURL = "https://staging.shuttletracker.app/node/firmware.ino.bin";  // repo link, should be ending with .bin to fit the HTTPUpdate.h library
const char* versionURL = "https://staging.shuttletracker.app/node/version.txt"; // version link

char* ssid = "City Station";
char* password = "CityStation2022";

WiFiClientSecure client;
WifiManager wifiManager(ssid, password);

void reportSystemStatus() {
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi is not connected. Cannot report status.");
        return;
    }

    int batteryPercentage = Battery::get_instance().getPercentage();
    const char* wifiState = (WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected";

    // Format data as text
    String dataText;
    dataText += "WiFi State: ";
    dataText += wifiState;
    dataText += "\nFirmware Version: ";
    dataText += firmwareVersion;
    dataText += "\nBattery Percentage: ";
    dataText += batteryPercentage;
    dataText += "%";

    // HTTP POST over HTTPS
    WiFiClientSecure secureClient;
    secureClient.setCACert(rootCACertificate); 
    HTTPClient http;
    http.begin(secureClient, "https://server.com/report"); // Use HTTPS URL
    http.addHeader("Content-Type", "text/plain"); // Specify content-type as text/plain

    int httpResponseCode = http.POST(dataText);

    if(httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        // Optionally, print the response
        String response = http.getString();
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end(); 
}

bool checkForVersionUpdate() {
    WiFiClientSecure secureClient;
    secureClient.setCACert(rootCACertificate);

    HTTPClient http;
    bool check = false;
    http.begin(secureClient ,versionURL);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String latestVersion = http.getString();
        latestVersion.trim(); 

        Serial.print("Current firmware version: ");
        Serial.println(firmwareVersion);
        Serial.print("Latest firmware version: ");
        Serial.println(latestVersion);

        if (latestVersion != firmwareVersion) {
            strncpy(firmwareVersion, latestVersion.c_str(), sizeof(firmwareVersion) - 1);
            firmwareVersion[sizeof(firmwareVersion) - 1] = '\0'; 
            Serial.println("A new firmware version is available, updating...");
            check = true;
        } else {
            Serial.println("Firmware is up to date.");
        }
    } else {
        Serial.print("Failed to check for firmware version. HTTP error: ");
        Serial.println(httpCode);
    }
    http.end();
    return check;
}

void checkforUpdate() {
  Serial.println("Checking for firmware updates...");
  client.setCACert(rootCACertificate);  
  t_httpUpdate_return ret = httpUpdate.update(client, firmwareURL);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
    }
}

void setup(){
    Serial.begin(115200);
    Display::get_instance().init();
    esp_wifi_set_max_tx_power(10);
    WiFi.begin(ssid,password);
    //setupOTA();
    client.setCACert(rootCACertificate);
}

void loop(){
  Beacon::get_instance().loop();
	IO::get_instance().loop();
	Display::get_instance().loop();
	Battery::get_instance().loop();
  wifiManager.attemptConnect();
  if (WiFi.status() == WL_CONNECTED) {
    if (!hasCheckedForUpdate) { 
      if (!checkForVersionUpdate()) {
      }
      hasCheckedForUpdate = true; // Mark that the check has been done
    }
  } else {
    hasCheckedForUpdate = false; // Reset the flag if disconnected, to check again next time it connects
  }
}

