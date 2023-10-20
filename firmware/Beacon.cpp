#include "Beacon.h"
#include "Arduino.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"

Beacon::Beacon(){

	// Beacon setup
	BLEDevice::init("Shuttle Tracker Node");
	pAdvertising = BLEDevice::getAdvertising();
	BLEDevice::startAdvertising();
	BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
	pAdvertising->setScanResponseData(oScanResponseData);
	setBeaconData();
	pAdvertising->stop();// Don't broadcast right away

	// Initialize memory access object and retrieve previous busID.
	datastore.begin("main", false);
	busID = datastore.getInt("busId", 0);

}

Beacon& Beacon::get_instance(){
	static Beacon instance;
	return instance;
}

void Beacon::loop(){

	if(beaconEnabled){//TODO: CHANGE THIS TO ALLOW FOR BOARDCASTING UPON BOOT

		unsigned long now = millis();

		// Handle millis() overflow
		if(lastBroadcastTime > now) lastBroadcastTime = 0;

		unsigned long elapsed = now - lastBroadcastTime;

		// Broadcast briefly for broadcastDuration then wait for rest of interval
		if(!broadcasting && elapsed < broadcastDuration){
			start();
		}
		if(broadcasting && elapsed > broadcastDuration){
			stop();
		}

		// Wait full interval before resetting timer
		if(!broadcasting && elapsed > broadcastInterval){
			lastBroadcastTime = millis();
		}

	}

}

void Beacon::setBusID(int busID){

	this->busID = busID;
	datastore.putInt("busID", busID);

	// Update beacon data with new bus ID
	setBeaconData();

	// Automatically turn on after entering the bus ID for the first time
	if(!initialBusIDSet){
		on();
		initialBusIDSet = true;
	}

}

int Beacon::getBusID() const{
	return busID;
}

void Beacon::on(){
	beaconEnabled = true;
}

void Beacon::off(){
	stop();
	beaconEnabled = false;
}

void Beacon::toggle(){
	if(beaconEnabled) off();
	else if(!beaconEnabled) on();
}

bool Beacon::enabled() const{
	return beaconEnabled;
}

void Beacon::start(){
	pAdvertising->start();
	broadcasting = true;
}

void Beacon::stop(){
	pAdvertising->stop();
	broadcasting = false;
}

void Beacon::setBeaconData(){

	BLEBeacon oBeacon = BLEBeacon();
	oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
	oBeacon.setProximityUUID(BLEUUID("9C3F95DC-7A90-4C5E-84CB-3D406D87B73B"));
	oBeacon.setMajor(busID);
	oBeacon.setMinor(deviceID);//TODO: SETUP THE MINOR FIELD
	BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
	oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
	std::string strServiceData = "";
	strServiceData += (char)26;     // Len
	strServiceData += (char)0xFF;   // Type
	strServiceData += oBeacon.getData();
	oAdvertisementData.addData(strServiceData);
	pAdvertising->setAdvertisementData(oAdvertisementData);

}

