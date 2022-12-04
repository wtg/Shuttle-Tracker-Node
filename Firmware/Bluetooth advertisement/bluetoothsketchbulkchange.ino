//Include blocks
#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"


//Global Variable
#define BEACON_UUID "278a19f0-4056-11ed-b878-0242ac120002"
uint16_t busID = 0x0000;     // 16 bit major id
BLEAdvertising *pAdvertising;   // BLE Advertisement type
BLEBeacon oBeacon = BLEBeacon();


//FUNCTIONS OVERVIEW
void BLE_Adv(uint16_t durationInMs);                   // Just Advertise
void BLE_Adv(uint16_t durationInMs,uint16_t newBusID); // Set new BL package before Advertise


//
void BLE_Adv(uint16_t durationInMs){
  digitalWrite(2, HIGH);
  pAdvertising->start();
  Serial.println("qAdvertizing started...");
  delay(durationInMs);
  pAdvertising->stop();
  Serial.println("qAdvertizing stopped...");
  digitalWrite(2, LOW);
}

void BLE_Adv(uint16_t durationInMs,uint16_t newBusID){
  //USE WHEN TRY TO CHANGE THE BUS ID AND ADVERTISE
  BLEDevice::init("tracker");
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(newBusID);
  oBeacon.setMinor(0x3);//Minor
  BLEAdvertisementData BLPackage = BLEAdvertisementData();
  BLPackage.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04, meaning not classic bluetooth
  std::string strServiceData = "";
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  ////Customize message here
  strServiceData += oBeacon.getData(); 
  ////Customize message end
  ////concatinate customized message into service data
  BLPackage.addData(strServiceData);
  pAdvertising->setAdvertisementData(BLPackage);
  pAdvertising->setScanResponseData(BLPackage);
  BLE_Adv(durationInMs);
}

void setup(){
  //General setpu
  Serial.begin(115200);
  pinMode (2, OUTPUT);//LED
}


void loop() {
  //set up bus id, uint32_t busID, code here interrupt function
  //funtion calling 
  busID++;
  BLE_Adv(1000,busID);
  delay(1000);
  BLE_Adv(1000);
  delay(1000);
}
