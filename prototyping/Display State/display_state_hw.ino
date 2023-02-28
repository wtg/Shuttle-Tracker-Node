#include "Adafruit_LiquidCrystal.h"

#include "DisplayState.h"

Adafruit_LiquidCrystal lcd(0x20);

DisplayState state(&lcd);


int buttonPin=6;
int encoderPin1 = 7;
int encoderPin2 = 9;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
volatile int lastButton = 0;
volatile long buttonValue = 0;
volatile int buttonHeld = 0;

long lastencoderValue = 0;
long lastButtonCount = 0;

int lastMSB = 0;
int lastLSB = 0;

void IRAM_ATTR updateEncoder(){
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

void IRAM_ATTR updateButton(){
  static unsigned long last_interrupt_time = 0;
  const int debounce_interval = 200;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > debounce_interval ){
    int cvalue = digitalRead(buttonPin);
    if(cvalue != lastButton){
      buttonValue++;
      lastButton = cvalue;
      last_interrupt_time = interrupt_time;
    }
  }
}




void setup() {

  Serial.begin(115200);

  Wire.begin(18, 19);
  if (!lcd.begin(16, 2)) {
    Serial.println("Could not init backpack. Check wiring.");
    while(1);
  }

  lcd.setBacklight(HIGH);

  lcd.print("hello");

  state.init();

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  attachInterrupt(encoderPin1, updateEncoder, CHANGE);
  attachInterrupt(encoderPin2, updateEncoder, CHANGE);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), updateButton, CHANGE);

}

void loop() {

  //Serial.println("------------------");
  //Serial.println(encoderValue);
  //Serial.println(buttonValue);

  if(encoderValue > lastencoderValue){
    state.rotaryRight();
    lastencoderValue = encoderValue;
  }
  else if(encoderValue < lastencoderValue){
    state.rotaryLeft();
    lastencoderValue = encoderValue;
  }

  if(buttonValue > lastButtonCount){
    state.rotarySelect();
    lastButtonCount = buttonValue;
  }

  //delay(100);

}
