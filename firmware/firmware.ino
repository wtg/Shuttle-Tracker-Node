#include "Display.h"
#include "IO.h"

void setup(){
	Serial.begin(115200);
	Display::get_instance().init();
}

void loop(){
	IO::get_instance().loop();
}
