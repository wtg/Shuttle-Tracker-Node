#pragma once

class Battery {

	// IO Pin Assignment
	const int batteryPin = 4;

	// Frequency to measure battery voltage in ms
	const int pollFrequency = 10000;
	unsigned long lastReadingTime = 0;

	// Resistor values for voltage divider
	const int R1 = 51000;// R1 between battery and ESP
	const int R2 = 51000;// R2 between ESP and ground

	// Battery Parameters
	const float batteryMax = 4.2;// Full charge voltage
	const float batteryMin = 3.0;// Full discharge voltage

	// Battery State
	int percentage = 0;
	int lastPercentage = 0;

	// Private constructor for singleton
	Battery();

public:
	// Get the single instance
	static Battery& get_instance();

	// Called from the main loop function
	void loop();

	// Get battery percentage
	int getPercentage() const;

private:
	// Take a reading and calculate percentage
	void measure();

};
