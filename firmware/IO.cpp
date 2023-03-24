#include "IO.h"
#include "Display.h"

IO::IO(){

	// GPIO Setup
	pinMode(encoderPin1, INPUT_PULLUP);
	pinMode(encoderPin2, INPUT_PULLUP);
	pinMode(encoderButtonPin, INPUT_PULLUP);

	// Interrupt Setup
	attachInterrupt(encoderPin1, isrUpdateEncoder, CHANGE);
	attachInterrupt(encoderPin2, isrUpdateEncoder, CHANGE);
	attachInterrupt(encoderButtonPin, isrUpdateButton, CHANGE);

}

IO& IO::get_instance(){
	static IO instance;
	return instance;
}

void IO::loop(){

	// Check if encoder was moved right or left since last loop
	if(encoderValue > lastEncoderValue){
		Display::get_instance().rotaryRight();
		lastEncoderValue = encoderValue;
	}
	else if(encoderValue < lastEncoderValue){
		Display::get_instance().rotaryLeft();
		lastEncoderValue = encoderValue;
	}

	// Check if button has been pressed since last loop
	if(buttonValue > lastButtonCount){
		Display::get_instance().rotarySelect();
		lastButtonCount = buttonValue;
	}

	// Parse and run serial commands
	serialCommandParser();

}

void IO::isrUpdateEncoder(){
	IO::get_instance().updateEncoder();
}

void IO::isrUpdateButton(){
	IO::get_instance().updateButton();
}

void IRAM_ATTR IO::updateEncoder(){

	int MSB = digitalRead(encoderPin1); //MSB = most significant bit
	int LSB = digitalRead(encoderPin2); //LSB = least significant bit

	int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
	int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value


	// Overcounting issue might be solved by adjusting these if statements.
	// It might be that not all clauses are necessary
	if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
		encoderValue ++;
	}
	if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
		encoderValue --;
	}

	lastEncoded = encoded; //store this value for next time

}

void IRAM_ATTR IO::updateButton(){

	static unsigned long last_interrupt_time = 0;
	const int debounce_interval = 200;
	unsigned long interrupt_time = millis();

	if(interrupt_time - last_interrupt_time > debounce_interval ){
		int cvalue = digitalRead(encoderButtonPin);
		if(cvalue != lastButton){
			buttonValue++;
			lastButton = cvalue;
			last_interrupt_time = interrupt_time;
		}
	}

}

void IO::serialCommandParser(){

	if(Serial.available() > 0){

		char buffer[16] = {};
		Serial.readBytesUntil('\n', buffer, 16);

		if(!strcmp("right", buffer)){
			Serial.println("Serial Command: right");
			Display::get_instance().rotaryRight();
		}else if(!strcmp("left", buffer)){
			Serial.println("Serial Command: left");
			Display::get_instance().rotaryLeft();
		}else if(!strcmp("select", buffer)){
			Serial.println("Serial Command: select");
			Display::get_instance().rotarySelect();
		}else if(!strcmp("back", buffer)){
			Serial.println("Serial Command: back");
			Display::get_instance().back();
		}else if(!strcmp("backspace", buffer)){
			Serial.println("Serial Command: backspace");
			Display::get_instance().backspace();
		}

		if(isdigit(buffer[0])){
			Serial.print("Serial Command: digit ");
			Serial.println(buffer[0]);
			Display::get_instance().keypadPress(buffer[0]);
		}

	}

}
