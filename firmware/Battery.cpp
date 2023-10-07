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
			if (abs(percentage - lastPercentage) >= 1) { 
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
	total -= readings[readIndex]; // Subtract the last reading
	readings[readIndex] = reading * 3.3 / 4096 * (R1 + R2) / R2 * calibrationFactor;  // Replace it with the new reading and convert to voltage
	total += readings[readIndex];  // Add it to the total
	readIndex = (readIndex + 1) % N_READINGS;  // Increment the index
	averageVoltage = total / N_READINGS;  // Calculate the new average
	percentage = 100 * (averageVoltage - batteryMin) / (batteryMax - batteryMin);
  
  percentage = percentage - 28;

	if (percentage < 0) {
		percentage = 0;
	} else if (percentage > 100) {
		percentage = 100;
	}
}

void Battery::alertError() {
	Serial.println("ERROR: Invalid battery percentage reading!");
}
