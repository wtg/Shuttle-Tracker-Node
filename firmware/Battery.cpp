#include "Battery.h"
#include "Arduino.h"
#include "Display.h"

Battery::Battery(){
  for (int i =0; i< N_READINGS; i++){
    readings[i] = 0;
  }
}

Battery& Battery::get_instance(){
	static Battery instance;
	return instance;
}

void Battery::loop(){

	unsigned long now = millis();
  
	if(now - lastReadingTime > pollFrequency || lastReadingTime > now){
		lastReadingTime = millis();
		measure();  
    
		// If the percentage changed, update the display
		if (percentage >= 0 && percentage <= 100) {  // Sanity check
			if (abs(percentage - lastPercentage) >= 1) {  // Event-driven update, change the threshold as needed
				Display::get_instance().render();
				lastPercentage = percentage;
			}
		} else {
			alertError();  // Alert if the reading is outside the expected range
		}
	}
}

int Battery::getPercentage() const{
	return percentage;
}

void Battery::measure(){

	// Get raw analog reading (0 - 4095)
	int reading = analogRead(batteryPin);

	// Convert to raw input voltage (0 - 3.3 V)
	float voltage = reading * 3.3 / 4096;

	// Calculate battery voltage before voltage divider
	float batteryVoltage = voltage * (R1+R2) / R2;

	// Calculate percentage based on min and max charge voltage of battery
	percentage = 100 * (batteryVoltage - batteryMin) / (batteryMax - batteryMin);

	if(percentage < 0) percentage = 0;
	if(percentage > 100) percentage = 100;

}

void Battery::alertError() {
	// Implement error handling logic. 
}
