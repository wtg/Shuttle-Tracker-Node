const int pin_row_1 = 3;
const int pin_row_2 = 2;
const int pin_row_3 = 1;
const int pin_row_4 = 0;
const int pin_col_1 = 6;
const int pin_col_2 = 5;
const int pin_col_3 = 4;

void setup() {
  Serial.begin(115200);
  pinMode(pin_row_1, INPUT_PULLDOWN);
  pinMode(pin_row_2, INPUT_PULLDOWN);
  pinMode(pin_row_3, INPUT_PULLDOWN);
  pinMode(pin_row_4, INPUT_PULLDOWN);
  pinMode(pin_col_1, INPUT_PULLUP);
  pinMode(pin_col_2, INPUT_PULLUP);
  pinMode(pin_col_3, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(pin_row_1) == HIGH && digitalRead(pin_col_1) == LOW) Serial.println("1");
  if(digitalRead(pin_row_1) == HIGH && digitalRead(pin_col_2) == LOW) Serial.println("2");
  if(digitalRead(pin_row_1) == HIGH && digitalRead(pin_col_3) == LOW) Serial.println("3");
  if(digitalRead(pin_row_2) == HIGH && digitalRead(pin_col_1) == LOW) Serial.println("4");
  if(digitalRead(pin_row_2) == HIGH && digitalRead(pin_col_2) == LOW) Serial.println("5");
  if(digitalRead(pin_row_2) == HIGH && digitalRead(pin_col_2) == LOW) Serial.println("6");
  if(digitalRead(pin_row_3) == HIGH && digitalRead(pin_col_1) == LOW) Serial.println("7");
  if(digitalRead(pin_row_3) == HIGH && digitalRead(pin_col_2) == LOW) Serial.println("8");
  if(digitalRead(pin_row_3) == HIGH && digitalRead(pin_col_3) == LOW) Serial.println("9");
  if(digitalRead(pin_row_4) == HIGH && digitalRead(pin_col_1) == LOW) Serial.println("*");
  if(digitalRead(pin_row_4) == HIGH && digitalRead(pin_col_2) == LOW) Serial.println("0");
  if(digitalRead(pin_row_4) == HIGH && digitalRead(pin_col_3) == LOW) Serial.println("#");
}