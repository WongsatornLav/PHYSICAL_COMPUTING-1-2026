#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int tempPin = A0;

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};
byte celsius[8] = {
  B00000,
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000
};

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, heart);
  lcd.createChar(2, celsius);
  lcd.setCursor(6, 0);
  lcd.print("I");
  lcd.setCursor(7, 0);
  lcd.write(byte(1));
  lcd.setCursor(8,0);
  lcd.print("IT");
  
}

void loop()
{
  int rawValue = analogRead(tempPin);
  float voltage = rawValue * (5.0 / 1023.0);
  float tempC = (voltage - 0.5) * 100.0;
  
  lcd.setCursor(0,1);
  lcd.print(68070162);
  lcd.setCursor(9,1);
  lcd.print("       ");
  lcd.setCursor(9,1);
  lcd.print(tempC);
  lcd.write(byte(2));
  delay(500);

}