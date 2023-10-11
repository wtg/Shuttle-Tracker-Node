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

const char* firmwareURL = "https://github.com/USERNAME/REPO_NAME/raw/main/path_to_file/firmware.bin";  // repo link, should be ending with .bin to fit the HTTPUpdate.h library

char* ssid = "City Station";
char* password = "CityStation2022";
WiFiClient client;
WifiManager wifiManager(ssid, password);

void checkforUpdate() {
  Serial.println("Checking for firmware updates...");
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
}

void loop(){
	IO::get_instance().loop();
	Display::get_instance().loop();
	Beacon::get_instance().loop();
	Battery::get_instance().loop();
  wifiManager.attemptConnect();
  //Serial.println(WiFi.localIP());
}

