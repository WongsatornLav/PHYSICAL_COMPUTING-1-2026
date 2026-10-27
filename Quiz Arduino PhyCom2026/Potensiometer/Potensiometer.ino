void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  int potValue = analogRead(A0);
  Serial.print("Potentiometer: ");
  Serial.println(potValue);

  delay(500);
}
