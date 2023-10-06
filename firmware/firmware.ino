#include "Battery.h"
#include "Beacon.h"
#include "Display.h"
#include "IO.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "ArduinoOTA.h"
#include "HTTPUpdate.h"
#include "esp_wifi.h"
#include "WiFiClient.h"

const char* host = ""; 
String url = "";  //path to PHP
char* ssid = "";
char* password = "";
WiFiClient client;

void setup(){
    Serial.begin(115200);
    Display::get_instance().init();
    esp_wifi_set_max_tx_power(10);
    WiFi.begin(ssid,password);
    

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println(F("Connecting to WiFi..."));
    }
    Serial.println(F("Connected to WiFi"));
    // Setup OTA handlers
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";  
        }
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println(F("\nEnd"));
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
        else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    });
    ArduinoOTA.begin();
    if (WiFi.status() == WL_CONNECTED) {
        checkforUpdate();
    }
    Serial.println(F("Ready"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
}

void checkforUpdate() {
  t_httpUpdate_return ret = httpUpdate.update(client, host, 80, url);

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

void loop(){
	IO::get_instance().loop();
	Display::get_instance().loop();
	Beacon::get_instance().loop();
	Battery::get_instance().loop();
  ArduinoOTA.handle();
}


