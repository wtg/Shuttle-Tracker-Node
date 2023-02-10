#include "Adafruit_LiquidCrystal.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"

Adafruit_LiquidCrystal lcd(0x20);
BLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);

  Wire.begin(4, 5);
  if (!lcd.begin(16, 2)) {
    Serial.println("Could not init backpack. Check wiring.");
    while(1);
  }
  lcd.print("Bus ID #452");

  BLEDevice::init("iBeacon");
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::setPower(ESP_PWR_LVL_P3); //Set the power level as +3dB 
  BLEDevice::startAdvertising();
  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID("9C3F95DC-7A90-4C5E-84CB-3D406D87B73B"));
  oBeacon.setMajor(452);
  oBeacon.setMinor(0);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  std::string strServiceData = "";
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);

  lcd.setBacklight(LOW);
}

void loop() {
  pAdvertising->start();
  delay(100);
  pAdvertising->stop();
  delay(10000);
}
