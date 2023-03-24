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


	//Key Pad Setup
	pinMode(cols[0], OUTPUT);
	pinMode(cols[1], OUTPUT);
	pinMode(cols[2], OUTPUT);
	pinMode(cols[3], OUTPUT);

	digitalWrite(cols[0], LOW);
	digitalWrite(cols[1], LOW);
	digitalWrite(cols[2], LOW);
	digitalWrite(cols[3], LOW);

	pinMode(rows[0], INPUT_PULLUP);
	pinMode(rows[1], INPUT_PULLUP);
	pinMode(rows[2], INPUT_PULLUP);

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


    char buffer[100];
    unsigned long time = millis();
    for(int i = 0; i < 3; ++i){
        if (digitalRead(cols[i]) == HIGH) continue;
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], LOW);
        for(int j = 0; j < 4; ++j){
            pinMode(cols[j], INPUT_PULLUP);
            /* if(digitalRead(cols[j]) == LOW && time - bouncing[i][j] > bounce_interval){ */
            if(digitalRead(cols[j]) == LOW && bouncing[i][j]){
                bouncing[i][j] = false;
                char newbuf[100];
                sprintf(newbuf, "%c, R: %d | C:%d", button_vals[i][j], rows[i], cols[j]);
                Serial.println(newbuf);
				if(button_vals[i][j] == '*') {Display::get_instance().backspace();}
				else{ Display::get_instance().keypadPress(button_vals[i][j]);}
            }
            if(digitalRead(cols[j]) == HIGH){
              bouncing[i][j] = true;
            }
            pinMode(cols[j], OUTPUT);
            digitalWrite(cols[j], LOW);
        }
        pinMode(rows[i], INPUT_PULLUP);
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
