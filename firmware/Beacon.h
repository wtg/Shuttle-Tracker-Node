#pragma once

#include "Preferences.h"
#include "BLEAdvertising.h"

class Beacon {

	// Broadcast settings in ms
	const int broadcastInterval = 10000;
	const int broadcastDuration = 100;
	unsigned long lastBroadcastTime = 0;

	// Beacon status
	bool beaconEnabled = false;
	bool broadcasting = false;

	// Bus ID
	int busID = 0;
	bool initialBusIDSet = false;

	// The beacon
	BLEAdvertising *pAdvertising;

	// Private constructor for singleton
	Beacon();

	// Object for accessing persistant storage.
	Preferences datastore;

public:
	// Get the single instance
	static Beacon& get_instance();

	// Called from the main loop function
	void loop();

	// Bus ID
	void setBusID(int busID);
	int getBusID() const;

	// Beacon control
	void on();
	void off();
	void toggle();
	bool enabled() const;
	void start();
	void stop();

private:
	void setBeaconData();

};
