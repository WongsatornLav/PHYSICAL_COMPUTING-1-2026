#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int rows[4] = {7, 6, 5, 4};
const int cols[4] = {3, 2, 1, 0};
const char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Press Key:");
  lcd.setCursor(0, 1);

  for (int i = 0; i < 4; i++) {
    pinMode(rows[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i], HIGH);
  }

}

void loop() {
  char key = getKey(); // เรียกใช้ฟังก์ชันอ่านปุ่ม
  
  if (key != '\0') {   // ถ้ามีการกดปุ่ม (ค่าไม่ใช่ค่าว่าง)
    lcd.print(key);    // พิมพ์ตัวอักษรนั้นออกจอ LCD
  }
}

char getKey() {
  char pressedKey = '\0';
  for (int c = 0; c < 4; c++) {
    digitalWrite(cols[c], LOW); // จ่ายไฟ LOW ไปที่คอลัมน์เพื่อเช็ค
    
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rows[r]) == LOW) { // ถ้าแถวไหนอ่านค่า LOW ได้ แปลว่าปุ่มนั้นถูกกด
        pressedKey = keys[r][c];
        
        delay(50); // กันการเด้งของสวิตช์ (Debounce)
        while (digitalRead(rows[r]) == LOW) {} // วนลูปรอจนกว่าผู้ใช้จะปล่อยนิ้วออกจากปุ่ม
        delay(50);
      }
    }
    digitalWrite(cols[c], HIGH); // คืนค่าคอลัมน์กลับเป็น HIGH ก่อนไปเช็คคอลัมน์ถัดไป
  }
  return pressedKey;
}