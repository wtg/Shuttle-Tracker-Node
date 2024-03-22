#pragma once

#include "Adafruit_LiquidCrystal.h"

class Display {

    // IO Pin Assignments
    const int displayClockPin = 18;
    const int displayDataPin = 19;

	// Menu States (The different screens to navigate between)
	enum menuState {
		HOME,
		BUS_ENTRY,
		WIFI_STATUS,
    FIRMWARE_VERSION
	};

	// Navigation States (The different options to scroll between)
	#define N_NAV_STATES 5
	#define NAV_STATE_NONE 0
	#define NAV_STATE_SET_BUS 1
	#define NAV_STATE_TOGGLE_BT 2
	#define NAV_STATE_WIFI_STATUS 3
  #define NAV_STATE_FIRMWARE_VERSION 4

	// Navigation text to display (corresponds with indices above)
	//  Max 10 characters to fit between << and >>
	const char* navStateStrings[N_NAV_STATES] = {
		"",
		"Set Bus ID",
		"Toggle BT",
		"Wi-Fi Stat",
    "Version"
	};

	// The current menu state (Initialized to the startup state)
	menuState currentMenu = BUS_ENTRY;

	// The currently shown navigation option (Initialized to the startup selection)
	int currentNavOption = NAV_STATE_NONE;

	// The Adafruit LiquidCrystal object to interface with display hardware
	Adafruit_LiquidCrystal lcd = Adafruit_LiquidCrystal(0x20);

	// Keypad entry state
	#define BUS_ID_SIZE 3
	char busEntryID[BUS_ID_SIZE+2] = "_  ";
	int busEntryPos = 0;

	// Backlight timeout
	const int backlightTimeout = 30000;
	unsigned long backlightTimer = millis();
	bool backlightState = true;

	// Private constructor for singleton
	Display();

public:
	// Get the single instance
	static Display& get_instance();

	// Initialize Display
	void init();

	// Input operations
	void rotaryRight();
	void rotaryLeft();
	void rotarySelect();
	void keypadPress(char digit);
	void backspace();
	void back();

	// Called from the main loop function
	void loop();

	// Build the string that should currently be shown
	void render();

private:
	// Control operations
	void setBusID(const char* id);
	void toggleBT();

	// Switch to a different menu state
	void navigateTo(menuState menuState);

	// Display a string on the actual hardware display
	void setDisplay(const char* string);

	// Backlight control
	void backlightOn();
	void backlightOff();

	// Turn on backlight in response to input
	bool startBacklight();

};
