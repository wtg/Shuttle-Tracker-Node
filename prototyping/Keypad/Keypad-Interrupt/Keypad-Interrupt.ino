const int cols[4] = {3,2,1,0};
const int rows[3] = {6,5,4};

const int bounce_interval = 200;
const char button_vals[3][4] = {
  {'1','2','3','#'},
  {'4','5','6','*'},
  {'7','8','9','0'}
};

volatile unsigned long bounceing[3][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};

volatile char last = ' ';
volatile int changed = 0;
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
        Serial.println(button_vals[i][j]);
      }
      pinMode(cols[i], OUTPUT);
      digitalWrite(cols[i], LOW);
    }
    pinMode(rows[i], INPUT_PULLUP);
  }
}

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
  /* attachInterrupt(pin_row_1, updateKeypad, CHANGE); */
  /* attachInterrupt(pin_row_2, updateKeypad, CHANGE); */
  /* attachInterrupt(pin_row_3, updateKeypad, CHANGE); */
  /* attachInterrupt(pin_row_4, updateKeypad, CHANGE); */

  attachInterrupt(rows[0], updateKeypad, LOW);
  attachInterrupt(rows[1], updateKeypad, LOW);
  attachInterrupt(rows[2], updateKeypad, LOW);

}

void loop() {
  if(changed == 1) {
    Serial.print(last);
    Serial.print('\0');
    Serial.println();
  }
  else{
    char buffer[100];
    sprintf(buffer, "no: %d", millis());
    /* Serial.println(buffer); */
  }
  delay(100);
}
