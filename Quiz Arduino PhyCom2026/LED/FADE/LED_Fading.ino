const int led1 = 3;
const int led2 = 5;
const int led3 = 6;
const int led4 = 9;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void loop() {
  for (int i = 0; i <= 255; i++) {
    int val1 = i;
    int val2 = (i * 2) % 256;
    int val3 = 255 - i;
    int val4 = map(i, 0, 255, 127, 255);

    analogWrite(led1, val1);
    analogWrite(led2, val2);
    analogWrite(led3, val3);
    analogWrite(led4, val4);

    delay(10);
  }
}