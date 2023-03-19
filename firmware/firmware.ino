#include "Display.h"
#include "IO.h"

IO io = IO::get_instance();

void setup(){
	Serial.begin(115200);
	Display::get_instance().init();
}

void loop(){
	io.loop();
}
