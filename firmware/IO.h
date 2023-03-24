#pragma once

#include "Arduino.h"

class IO {

	// IO Pin Assignments
	const int encoderButtonPin = 10;
	const int encoderPin1 = 9;
	const int encoderPin2 = 20;

	// Track Encoder and Button Inputs
	volatile int lastEncoded = 0;
	volatile long encoderValue = 0;
	volatile int lastButton = 0;
	volatile long buttonValue = 0;
	volatile int buttonHeld = 0;
	long lastEncoderValue = 0;
	long lastButtonCount = 0;
	int lastMSB = 0;
	int lastLSB = 0;

	// Private constructor for singleton
	IO();

public:
	// Get the single instance
	static IO& get_instance();

	// Called from the main loop function
	void loop();

private:
	// Interrupt Service Routines
	static void isrUpdateEncoder();
	static void isrUpdateButton();

	// Interrupt logic, called by static ISRs
	void IRAM_ATTR updateEncoder();
	void IRAM_ATTR updateButton();

	// Parse and run serial commands
	void serialCommandParser();
	// TODO: Disable in production?

};
