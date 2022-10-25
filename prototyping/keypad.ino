void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLDOWN);
  pinMode(3, INPUT_PULLDOWN);
  pinMode(10, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(1, INPUT_PULLDOWN);
  pinMode(9, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);
  pinMode(7, INPUT_PULLDOWN);
  pinMode(6, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
}

void loop() {
    if(digitalRead(2) == HIGH) Serial.println("2");
    if(digitalRead(3) == HIGH) Serial.println("3");
    if(digitalRead(10) == HIGH) Serial.println("10");
    if(digitalRead(18) == HIGH) Serial.println("18");
    if(digitalRead(1) == HIGH) Serial.println("1");
    if(digitalRead(9) == HIGH) Serial.println("9");
    if(digitalRead(4) == HIGH) Serial.println("4");
    if(digitalRead(7) == HIGH) Serial.println("7");
    if(digitalRead(6) == HIGH) Serial.println("6");
    if(digitalRead(5) == HIGH) Serial.println("5");
}
