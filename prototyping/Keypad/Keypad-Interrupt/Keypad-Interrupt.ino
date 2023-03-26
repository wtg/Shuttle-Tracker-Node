const int cols[4] = {3,2,1,0};
const int rows[3] = {5,6,7};

const int bounce_interval = 200;
const char button_vals[3][4] = {
    {'1','2','3','#'},
    {'4','5','6','0'},
    {'7','8','9','*'}
};

bool bouncing[3][4] = {
    {true, true, true, true},
    {true, true, true, true},
    {true, true, true, true},
};

/* OLD Interrupt Keypad testing code
void IRAM_ATTR updateKeypad(){
  unsigned long time = millis();
  if (time - bounceing[0][0] < bounce_interval) return;
  char buffer[100];
  sprintf(buffer, "%d | %d | %d", time - bounceing[0][0], bounceing[0][0], time);
  Serial.println(buffer);
  bounceing[0][0] = time;
  for(int i = 0; i < 3; ++i){
    if (digitalRead(cols[i]) == HIGH) continue;
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);
    for(int j = 0; j < 4; ++j){
      pinMode(cols[j], INPUT_PULLUP);
      if(digitalRead(cols[j]) == LOW){
        char newbuf[100];
        sprintf(newbuf, "%c, R: %d | C:%d", button_vals[i][j], rows[i], cols[j]);
        Serial.println(newbuf);
      }
      pinMode(cols[i], OUTPUT);
      digitalWrite(cols[i], LOW);
    }
    pinMode(rows[i], INPUT_PULLUP);
  }
}*/

void setup() {
  Serial.begin(115200);

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


void loop() {
    unsigned long time = millis();
    for(int i = 0; i < 3; ++i){
        if (digitalRead(cols[i]) == HIGH) continue;
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], LOW);
        for(int j = 0; j < 4; ++j){
            pinMode(cols[j], INPUT_PULLUP);
            /* if(digitalRead(cols[j]) == LOW && time - bouncing[i][j] > bounce_interval){ */
            if(digitalRead(cols[j]) == LOW && bouncing[i][j]){
                #ifndef PROD
                Serial.println("Running in Debug mode");
                #endif
                bouncing[i][j] = false;
                char newbuf[100];
                sprintf(newbuf, "%c, R: %d | C:%d", button_vals[i][j], rows[i], cols[j]);
                Serial.println(newbuf);
            }
            if(digitalRead(cols[j]) == HIGH){
              bouncing[i][j] = true;
            }
            pinMode(cols[j], OUTPUT);
            digitalWrite(cols[j], LOW);
        }
        pinMode(rows[i], INPUT_PULLUP);
    }

    delay(100);
}
