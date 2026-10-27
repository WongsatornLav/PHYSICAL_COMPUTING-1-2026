const int sensorPinTMP = A1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int readingTMP = analogRead(sensorPinTMP);
  
  float temperatureC = ((readingTMP * (5.0 / 1024.0)) - 0.5) * 100.0;

  Serial.print("Temp: ");
  Serial.print(temperatureC);
  Serial.println(" C");

  delay(1000);
}