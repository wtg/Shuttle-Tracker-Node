/*
    An older prototype for encoder control. Has repeated outputs for
    a single rotation index and gets directions wrong most of the time.
*/

bool cw_pressed = false;
bool ccw_pressed = false;

void IRAM_ATTR cw(){
  cw_pressed = true;
  // Serial.println("Clockwise");
}

void IRAM_ATTR ccw(){
 ccw_pressed = true;
//  Serial.println("Counter-clockwise");
}

void setup() {
  Serial.begin(115200);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  attachInterrupt(4, cw, FALLING);
  attachInterrupt(5, ccw, FALLING);
}

void loop() {
  if(cw_pressed){
    Serial.printf("Clockwise\n");
    cw_pressed = false;
  }
  if(ccw_pressed){
    Serial.printf("Counter-clockwise\n");
    ccw_pressed = false;
  }
}
