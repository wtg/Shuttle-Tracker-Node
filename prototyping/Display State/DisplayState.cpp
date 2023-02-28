#include <iostream>
#include <cstring>
#include <Arduino.h>
#include "Adafruit_LiquidCrystal.h"

#include "DisplayState.h"

DisplayState::DisplayState(Adafruit_LiquidCrystal* lc_lcd){
  lcd = lc_lcd;
  //render();
}

void DisplayState::init(){

  render();

}

void DisplayState::rotaryRight(){

	if(currentMenu == HOME){
		if(currentNavOption < N_NAV_STATES - 1){
			++currentNavOption;
			render();
		}
	}

}

void DisplayState::rotaryLeft(){

	if(currentMenu == HOME){
		if(currentNavOption > 0){
			--currentNavOption;
			render();
		}
	}

}

void DisplayState::rotarySelect(){

	switch(currentMenu){
		case HOME:
			switch(currentNavOption){
				case NAV_STATE_SET_BUS:
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

void DisplayState::keypadPress(char digit){

	if(currentMenu == BUS_ENTRY){

		if(busEntryPos < BUS_ID_SIZE){
			busEntryID[busEntryPos] = digit;
			++busEntryPos;
		}
		busEntryID[busEntryPos] = '_';

		render();

	}

}

void DisplayState::backspace(){

	if(currentMenu == BUS_ENTRY){

		if(busEntryPos > 0){
			if(busEntryPos < BUS_ID_SIZE){
				busEntryID[busEntryPos] = ' ';
			}
			--busEntryPos;
			busEntryID[busEntryPos] = '_';
		}

		render();

	}

}

void DisplayState::back(){

	if(currentMenu == HOME){
		currentNavOption = NAV_STATE_NONE;
	}else{
		navigateTo(HOME);
	}

}

void DisplayState::setBusID(const char* id){

	busID = atoi(id);

}

void DisplayState::toggleBT(){

	btEnable = !btEnable;
	render();

}

void DisplayState::navigateTo(menuState menuState){
	currentMenu = menuState;
	render();
}

void DisplayState::render(){

	// Macro to help deal with string formatting without null terminators
	char buffer[34];
	#define SNPRINTF_NO_TERM(output, maxlen, format, args...) snprintf(buffer, maxlen+1, format, ##args);memcpy(output, buffer, strlen(buffer));

	// The string to send to the display (16x2 + newline + terminator)
	char output[34] = "                \n                ";

	// Display bus ID
	if(!btEnable){
		SNPRINTF_NO_TERM(output, 10, "#%-3d  NoBT", busID)
	}else{
		SNPRINTF_NO_TERM(output, 9, "Bus #%-3d", busID)
	}

	// Display battery percentage
	SNPRINTF_NO_TERM(&output[12], 4, "%3d%%", battery)

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
                  SNPRINTF_NO_TERM(&output[17], 15, "129.161.000.000")
                  break;
                }

                setDisplay(output);
}

void DisplayState::setDisplay(const char *string){
  // std::cout << "----------------" << std::endl;
  //std::cout << string << std::endl;
  //std::cout << "----------------" << std::endl/* << std::endl*/

  //Serial.println(string);

  lcd->setCursor(0, 0);
  lcd->print(string);
  lcd->setCursor(0, 1);
  char second_line[17];
  memcpy(second_line, &string[17], 17);
  lcd->print(second_line);
}
