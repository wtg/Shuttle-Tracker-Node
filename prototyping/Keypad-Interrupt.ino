const int pin_row_1 = 3;
const int pin_row_2 = 2;
const int pin_row_3 = 1;
const int pin_row_4 = 0;
const int pin_col_1 = 6;
const int pin_col_2 = 5;
const int pin_col_3 = 4;

const int bounce_interval = 100;
const char button_vals[3][4] = {
  {'1','2','3','#'},
  {'4','5','6','*'},
  {'7','8','9','0'}
};

volatile int bounceing[3][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};
volatile char last = ' ';
volatile int changed = 0;
void IRAM_ATTR updateKeypad(){
  int rows[4] = {digitalRead(pin_row_1), digitalRead(pin_row_2), digitalRead(pin_row_3), digitalRead(pin_row_4)};
  int cols[3] = {digitalRead(pin_col_1), digitalRead(pin_col_2), digitalRead(pin_col_3)};
  int time = millis();

  for(int i = 0; i < 4; ++i){
    for(int j = 0; j < 3; ++j){
      if(rows[i] == HIGH && cols[j] == LOW && time - bounceing[j][i] > bounce_interval){
        last = Serial.println(button_vals[j][i]);
        changed = 1;
        bounceing[j][i] = time;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pin_row_1, INPUT_PULLDOWN);
  pinMode(pin_row_2, INPUT_PULLDOWN);
  pinMode(pin_row_3, INPUT_PULLDOWN);
  pinMode(pin_row_4, INPUT_PULLDOWN);
  pinMode(pin_col_1, INPUT_PULLUP);
  pinMode(pin_col_2, INPUT_PULLUP);
  pinMode(pin_col_3, INPUT_PULLUP);

  attachInterrupt(pin_row_1, updateKeypad, CHANGE);
  attachInterrupt(pin_row_2, updateKeypad, CHANGE);
  attachInterrupt(pin_row_3, updateKeypad, CHANGE);
  attachInterrupt(pin_row_4, updateKeypad, CHANGE);
  
  attachInterrupt(pin_col_1, updateKeypad, CHANGE);
  attachInterrupt(pin_row_2, updateKeypad, CHANGE);
  attachInterrupt(pin_row_3, updateKeypad, CHANGE);
  
}

void loop() {
  if(changed == 1) {
    Serial.print(last);
    Serial.print('\0');
    Serial.println();
  }
  else{
    Serial.println("no");
  }
  delay(400);
}
