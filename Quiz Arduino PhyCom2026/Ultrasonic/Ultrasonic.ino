// C++ code
//
void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT); //Trig
  pinMode(4, INPUT); //Echo
}

void loop()
{
  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);
  
  int pulseWidth = pulseIn(4, HIGH);
  
  Serial.print("Pulse Width: ");
  Serial.println(pulseWidth);
  long distance = pulseWidth/29/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);
}