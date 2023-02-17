#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"


#define GPIO_DEEP_SLEEP_DURATION     1  // sleep x seconds and then wake up
//RTC SLOW MEME SECTION
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory
RTC_DATA_ATTR static uint32_t busID = 0x0010;     // 16 bit major id


BLEAdvertising *pAdvertising;   // BLE Advertisement type
struct timeval now;

// https://www.uuidgenerator.net/
#define BEACON_UUID "278a19f0-4056-11ed-b878-0242ac120002" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)


void setBeacon() {
  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(busID);
  Serial.printf("busID:%x\n", busID);
  oBeacon.setMinor(0x3);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04, meaning not classic bluetooth
  std::string strServiceData = "";
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  ////Customize message here
  strServiceData += oBeacon.getData(); 
  ////Customize message end
  ////concatinate customized message into servicedata
  oAdvertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void init(){
  //init things here, including GPIO
  Serial.begin(115200);
  pinMode (2, OUTPUT);//LED
}


void bluetoothboardcasting(){
  digitalWrite(2, HIGH);
  gettimeofday(&now, NULL);
  Serial.printf("start ESP32 %d\n", bootcount++);
  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);
  last = now.tv_sec;
  // Create the BLE Device
  BLEDevice::init("tracker");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  setBeacon();
  // Start advertising
  pAdvertising->start();
  Serial.println("Advertizing started...");
  delay(1000);
  pAdvertising->stop();
  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  Serial.printf("in deep sleep\n");
}
void setup() {
  init();
  if (1) {//start boardcasting or 
    bluetoothboardcasting();
  }else{}
}


void loop() {
  //set up bus id, uint32_t busID, code here interrupt function
  //

}
