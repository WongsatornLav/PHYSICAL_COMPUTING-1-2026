#include <WiFiS3.h>
#include <MQTTClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char WIFI_SSID[] = "iPhone Lavendis";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "aon12344321";  // CHANGE TO YOUR WIFI PASSWORD

const char MQTT_BROKER_ADRRESS[] = "phycom.it.kmitl.ac.th";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "arduino-uno-r4-Lavendis";
const char MQTT_USERNAME[] = "";
const char MQTT_PASSWORD[] = "";

// Topics สำหรับ Publish และ Subscribe
const char PUBLISH_TOPIC[]   = "68070162/sentosa_choice";
const String TOPIC_CHOTIPHAT = "68070162/queue_chotiphat"; // Universal Studios
const String TOPIC_LAPAS     = "68070162/queue_lapas";     // S.E.A. Aquarium
const String TOPIC_SUMET     = "68070162/queue_sumet";     // Skyline Luge

// --- อุปกรณ์ LCD และ RGB LED ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PIN_RED   = 9;
const int PIN_GREEN = 10;
const int PIN_BLUE  = 11;

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

// กำหนดค่าเริ่มต้นเป็น -1 เพื่อรอเช็คว่าได้รับข้อมูลครบทั้ง 3 ที่แล้วหรือยัง
int queue_u = -1; // Universal Studios
int queue_a = -1; // S.E.A. Aquarium
int queue_l = -1; // Skyline Luge

bool isDataUpdated = false; // ตัวแปรแจ้งเตือนเมื่อมีข้อมูลใหม่เข้ามา

void setup() {
  Serial.begin(9600);

  // ตั้งค่าขา RGB LED
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  // เริ่มต้นทำงาน LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("WiFi Connected");

  connectToMQTT();
}

void loop() {
  mqtt.loop();

  // ตรวจสอบการเชื่อมต่อ MQTT และ Reconnect หากหลุด
  if (!mqtt.connected()) {
    connectToMQTT();
  }

  // ประมวลผลเมื่อมีข้อมูลอัปเดตเข้ามา
  if (isDataUpdated) {
    isDataUpdated = false;
    processQueueData();
  }
}

void connectToMQTT() {
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);

  Serial.print("Arduino UNO R4 - Connecting to MQTT broker");

  while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nMQTT Connected!");

  // Subscribe รับข้อมูลพร้อมกันทั้ง 3 Topics
  mqtt.subscribe(TOPIC_CHOTIPHAT);
  mqtt.subscribe(TOPIC_LAPAS);
  mqtt.subscribe(TOPIC_SUMET);

  Serial.println("Subscribed to all queue topics!");
}

// ฟังก์ชัน Callback: รับค่าและอัปเดตลงตัวแปรอย่างเดียว (ทำงานเร็ว ไม่บล็อกสาย)
void messageReceived(String &topic, String &payload) {
  int value = payload.toInt();

  if (topic == TOPIC_CHOTIPHAT) {
    queue_u = value;
  } else if (topic == TOPIC_LAPAS) {
    queue_a = value;
  } else if (topic == TOPIC_SUMET) {
    queue_l = value;
  }

  isDataUpdated = true; // ตั้ง Flag ว่ามีข้อมูลใหม่เข้ามาแล้ว
}

void processQueueData() {
  // รอนั้นจนกว่าจะได้รับข้อมูลครบจากทั้ง 3 สถานที่อย่างน้อย 1 ครั้ง
  if (queue_u == -1 || queue_a == -1 || queue_l == -1) {
    lcd.setCursor(0, 0);
    lcd.print("Waiting data... ");
    return;
  }

  // 1. เปรียบเทียบหาสถานที่ที่มีเวลารอคิวน้อยที่สุด
  int minQueue = queue_u;
  String bestChoice = "Universal";

  if (queue_a < minQueue) {
    minQueue = queue_a;
    bestChoice = "Aquarium";
  }
  if (queue_l < minQueue) {
    minQueue = queue_l;
    bestChoice = "Luge";
  }

  // 2. Publish ชื่อสถานที่ที่มีคิวน้อยที่สุดไปยัง topic: sentosa_choice
  mqtt.publish(PUBLISH_TOPIC, bestChoice);

  // 3. คำนวณค่าเฉลี่ยเวลารอคิว
  int avgQueue = (queue_u + queue_a + queue_l) / 3;

  // 4. แสดงผลบนหน้าจอ LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(bestChoice + " (" + String(minQueue) + "m)");

  lcd.setCursor(0, 1);
  lcd.print("Avg: " + String(avgQueue) + "m");

  // 5. ควบคุมไฟ RGB LED (สูตรสำหรับ Common Anode)
  if (avgQueue > 60) {
    // มากกว่า 60 นาที -> สีแดง (Red)
    setRGBColor(255, 0, 0);
  } else if (avgQueue >= 30 && avgQueue <= 60) {
    // ช่วง 30 - 60 นาที -> สีส้ม (Orange = Red + Green ปริมาณน้อย)
    setRGBColor(255, 40, 0);
  } else { 
    // น้อยกว่า 30 นาที -> สีเขียวอมฟ้า (Cyan = Green + Blue)
    setRGBColor(0, 255, 255);
  }

  // แสดงผลบน Serial Monitor
  Serial.println("------------------------------------");
  Serial.print("Queues [U/A/L]: ");
  Serial.print(queue_u); Serial.print(" / ");
  Serial.print(queue_a); Serial.print(" / ");
  Serial.println(queue_l);
  Serial.print("Choice Sent: "); Serial.println(bestChoice);
  Serial.print("Average Queue: "); Serial.print(avgQueue); Serial.println("m");
  Serial.println("------------------------------------");
}

// ฟังก์ชันผสมสี RGB (สำหรับหลอด Common Anode)
void setRGBColor(int r, int g, int b) {
  analogWrite(PIN_RED, 255 - r);
  analogWrite(PIN_GREEN, 255 - g);
  analogWrite(PIN_BLUE, 255 - b);
}