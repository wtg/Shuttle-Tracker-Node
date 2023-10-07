#include "Display.h"
#include "Battery.h"
#include "Beacon.h"
#include "WiFi.h"

Display::Display(){

	Wire.begin(displayDataPin, displayClockPin);
	if(!lcd.begin(16, 2)){
		Serial.println("Error: Failed to init backpack. Check wiring.");
		// TODO: Flash RGB LED a certain color to indicate error state?
	}
	backlightOn();

}

Display& Display::get_instance(){
	static Display instance;
	return instance;
}

void Display::init(){

	// Render on boot to display initial state and overwrite anything displayed from previous boot
	render();

}

void Display::rotaryRight(){

	if(startBacklight()) return;

	if(currentMenu == HOME){
		if(currentNavOption < N_NAV_STATES - 1){
			++currentNavOption;
			render();
		}
	}

}

void Display::rotaryLeft(){

	if(startBacklight()) return;

	if(currentMenu == HOME){
		if(currentNavOption > 0){
			--currentNavOption;
			render();
		}
	}

}

void Display::rotarySelect(){

	if(startBacklight()) return;

	switch(currentMenu){
		case HOME:
			switch(currentNavOption){
				case NAV_STATE_SET_BUS:
					strcpy(busEntryID, "_  ");
					busEntryPos = 0;
					navigateTo(BUS_ENTRY);
					break;
				case NAV_STATE_TOGGLE_BT:
					toggleBT();
					break;
				case NAV_STATE_WIFI_STATUS:
					navigateTo(WIFI_STATUS);
					break;
			}
			break;
		case BUS_ENTRY:
			setBusID(busEntryID);
			currentNavOption = NAV_STATE_NONE;
			navigateTo(HOME);
			break;
		case WIFI_STATUS:
			navigateTo(HOME);
			break;
	}

}

void Display::keypadPress(char digit){

	if(startBacklight()) return;

	if(currentMenu == BUS_ENTRY){

		if(busEntryPos < BUS_ID_SIZE){
			busEntryID[busEntryPos] = digit;
			++busEntryPos;
		}
		busEntryID[busEntryPos] = '_';

		render();

	}

}

void Display::backspace(){

	if(startBacklight()) return;

	if(currentMenu == BUS_ENTRY){

		if(busEntryPos > 0){
			if(busEntryPos <= BUS_ID_SIZE){
				busEntryID[busEntryPos] = ' ';
			}
			--busEntryPos;
			busEntryID[busEntryPos] = '_';
		}

		render();

	}

}

void Display::back(){

	if(startBacklight()) return;

	if(currentMenu == HOME){
		currentNavOption = NAV_STATE_NONE;
		render();
	}else{
		navigateTo(HOME);
	}

}

void Display::loop(){

	// Shut off backlight if past timeout
	if(millis() - backlightTimer > backlightTimeout){
		backlightOff();
	}

}

void Display::setBusID(const char *id){
	Beacon::get_instance().setBusID(atoi(id));
}

void Display::toggleBT(){
	Beacon::get_instance().toggle();
	render();
}

void Display::navigateTo(menuState menuState){
	currentMenu = menuState;
	render();
}

void Display::render(){

	// Macro to help deal with string formatting without null terminators
	char buffer[34];
	#define SNPRINTF_NO_TERM(output, maxlen, format, args...) snprintf(buffer, maxlen+1, format, ##args);memcpy(output, buffer, strlen(buffer));

	// The string to send to the display (16x2 + newline + terminator)
	char output[34] = "                \n                ";

	// Display bus ID
	if(!Beacon::get_instance().enabled()){
		SNPRINTF_NO_TERM(output, 11, "#%-3d BT Off", Beacon::get_instance().getBusID())
	}else{
		SNPRINTF_NO_TERM(output, 9, "Bus #%-3d", Beacon::get_instance().getBusID())
	}

	// Display battery percentage
	SNPRINTF_NO_TERM(&output[12], 4, "%3d%%", Battery::get_instance().getPercentage())

	// Display stuff depending on current menu state
	switch(currentMenu){
		case HOME:
			if(currentNavOption > 0){
				SNPRINTF_NO_TERM(&output[17], 2, "<<")
			}
			if(currentNavOption < N_NAV_STATES - 1){
				SNPRINTF_NO_TERM(&output[31], 2, ">>")
			}
			SNPRINTF_NO_TERM(&output[20], 10, "%-.10s", navStateStrings[currentNavOption])
			break;
		case BUS_ENTRY:
			SNPRINTF_NO_TERM(output, 12, "Enter Bus   ")
			SNPRINTF_NO_TERM(&output[17], 1 + BUS_ID_SIZE + 1, "#%s", busEntryID)
			break;
		case WIFI_STATUS:
			  if (WiFi.status() == WL_CONNECTED) {
          SNPRINTF_NO_TERM(&output[17], 15, "%s", WiFi.localIP().toString().c_str());
        } else {
        SNPRINTF_NO_TERM(&output[17], 15, "Not connect");
      }
      break;
	}

	setDisplay(output);

}

void Display::setDisplay(const char* string){

	lcd.setCursor(0, 0);
	lcd.print(string);
	lcd.setCursor(0, 1);
	char second_line[17];
	memcpy(second_line, &string[17], 17);
	lcd.print(second_line);

}

void Display::backlightOn(){
	lcd.setBacklight(HIGH);
	backlightState = true;
}

void Display::backlightOff(){
	lcd.setBacklight(LOW);
	backlightState = false;
}

bool Display::startBacklight(){

	backlightTimer = millis();

	// Turn on backlight if input is received, but do not respond until next input within timeout
	if(!backlightState){
		backlightOn();
		return true;
	}else{
		return false;
	}

}
