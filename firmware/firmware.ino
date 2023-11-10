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
#include <WiFiClientSecure.h>

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEOTCCAyGgAwIBAgISAzrOlfnc+7MQcQaKLLFjt3TXMA0GCSqGSIb3DQEBCwUA\n" \
"MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD\n" \
"EwJSMzAeFw0yMzEwMDMxMzU1MjRaFw0yNDAxMDExMzU1MjNaMB0xGzAZBgNVBAMT\n" \
"EnNodXR0bGV0cmFja2VyLmFwcDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABOGZ\n" \
"bexQ4FQ7trLmfVUVY7EDNEgpp+tPLheoyF+CItBc5zq5CwWcLIHnqOgdjJLudQhm\n" \
"Tq8KEyVHlGhjumgAOfKjggInMIICIzAOBgNVHQ8BAf8EBAMCB4AwHQYDVR0lBBYw\n" \
"FAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFBOz\n" \
"3n06ZorkWAE+jiKhrtwulh2kMB8GA1UdIwQYMBaAFBQusxe3WFbLrlAJQOYfr52L\n" \
"FMLGMFUGCCsGAQUFBwEBBEkwRzAhBggrBgEFBQcwAYYVaHR0cDovL3IzLm8ubGVu\n" \
"Y3Iub3JnMCIGCCsGAQUFBzAChhZodHRwOi8vcjMuaS5sZW5jci5vcmcvMC8GA1Ud\n" \
"EQQoMCaCEHNodXR0bGVzLnJwaS5lZHWCEnNodXR0bGV0cmFja2VyLmFwcDATBgNV\n" \
"HSAEDDAKMAgGBmeBDAECATCCAQUGCisGAQQB1nkCBAIEgfYEgfMA8QB2AHb/iD8K\n" \
"tvuVUcJhzPWHujS0pM27KdxoQgqf5mdMWjp0AAABivYJYIQAAAQDAEcwRQIhALyH\n" \
"s6FMxKLm63hO0CXJKK4nQc9/+HFu6W41QZmFq/8HAiA+GFfuiyj26yXZkzgNwtIS\n" \
"W91zps+V5aDKDELiga3AJwB3ANq2v2s/tbYin5vCu1xr6HCRcWy7UYSFNL2kPTBI\n" \
"1/urAAABivYJYgUAAAQDAEgwRgIhALtPHhwPbqm28ouDUKtQf2KRUTfhpUyyd0p+\n" \
"8enXV5TYAiEA+oXYZX2qfL+ulAefaHVaZGUd+KAn8BC9EkUk9PFJVPswDQYJKoZI\n" \
"hvcNAQELBQADggEBABonzSlNDuB5Vmx/FaTtyCzqE0fUl2DYYPBGMjF8fMLc1BQV\n" \
"PAkUc9uEm4kGC2N5WjRssHvuPUGdBs+laFSo4xYBnmAASRjiI6K8HcA1MsE+HmXM\n" \
"IsI5S/yL3dItDI22OwT+75FbfgG7Er4DyB4BuCBwU0VwAmKLrIUCo4NJHeuq+y4r\n" \
"LNKZhcXy7j6o9ahwFD6QVRORTDBKQw8rhkCzmEZBHPEGJTZey/8qqyOXqDryhurC\n" \
"Z7zedjBIcLYAFFeW/V/RkU9EBedchhYnVsuUZKj+g2Mlepicd1QiQjqIMN9/mj9Z\n" \
"tSjGMZR2NHlTywUQ6pxaVK+phEYw/+DqVYyg2eg=\n" \
"-----END CERTIFICATE-----\n";

const char* firmwareURL = "http://staging.shuttletracker.app/node/firmware.ino.bin";  // repo link, should be ending with .bin to fit the HTTPUpdate.h library

// char* ssid = "Quy Hoang";
// char* password = "quyhoang";

char* ssid = "Link10Laptop";
char* password = "password";

WiFiClientSecure client;
WifiManager wifiManager(ssid, password);

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
    //ESP.getEfuseMac();//TODO: SETUP THE MINOR FIELD
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
  if(WiFi.status() == WL_CONNECTED){
    //checkforUpdate();
  }
  //Serial.println(WiFi.localIP());
}

