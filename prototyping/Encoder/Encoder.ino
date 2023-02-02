/*
This is a mockup of how we can use the encoder to control our display interface
with the global encoderValue variable. Clockwise turns increase, while Counter
Clockwise turns decrease encoderValue. At the moment, the amount by which the
variable changes for each indexed turn on the encoder is not consistent. It will
change by anywhere from 1 to 4 for each indexed rotation.
*/

int encoderPin1 = 4;
int encoderPin2 = 5;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

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

void setup() {
  Serial.begin(115200);
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  attachInterrupt(encoderPin1, updateEncoder, CHANGE);
  attachInterrupt(encoderPin2, updateEncoder, CHANGE);
}

void loop() {
  Serial.println(encoderValue);
  delay(1000);
}
