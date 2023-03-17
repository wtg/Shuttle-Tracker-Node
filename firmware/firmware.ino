#include "IO.h"

IO io = IO::get_instance();

void setup(){
	Serial.begin(115200);
}

void loop(){
	io.loop();
}
