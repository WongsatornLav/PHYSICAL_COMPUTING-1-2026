#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[] = "iPhone Lavendis";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "aon12344321";  // CHANGE TO YOUR WIFI PASSWORD

const char MQTT_BROKER_ADRRESS[] = "phycom.it.kmitl.ac.th";  // CHANGE TO MQTT BROKER'S ADDRESS
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "arduino-uno-r4-Lavendis";  // CHANGE IT AS YOU DESIRE
const char MQTT_USERNAME[] = "";              // CHANGE IT IF REQUIRED
const char MQTT_PASSWORD[] = "";              // CHANGE IT IF REQUIRED

// The MQTT topics that Arduino should publish/subscribe
const char PUBLISH_TOPIC[] = "68070162/light";       // CHANGE IT AS YOU DESIRE
const char SUBSCRIBE_TOPIC[] = "68070162/venus";  // CHANGE IT AS YOU DESIRE

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

const int rows[4] = {7, 6, 5, 4};
const int cols[4] = {3, 2, 1, 0};
const char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(rows[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i], HIGH);
  }

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Arduino UNO R4 - Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(10000);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  connectToMQTT(); 
}

void loop() {
  mqtt.loop();

  // ตรวจสอบและ Reconnect หากการเชื่อมต่อ MQTT หลุด
  if (!mqtt.connected()) {
    connectToMQTT();
  }

  // อ่านค่าจาก Keypad ตลอดเวลาใน loop
  char key = getKey();
  if (key != '\0') { 
    sendToMQTT(key); // ส่งค่าทันทีที่มีการกดปุ่ม
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
  Serial.println();

  if (!mqtt.connected()) {
    Serial.println("Arduino UNO R4 - MQTT broker Timeout!");
    return;
  }

  if (mqtt.subscribe(SUBSCRIBE_TOPIC))
    Serial.print("Arduino UNO R4 - Subscribed to the topic: ");
  else
    Serial.print("Arduino UNO R4 - Failed to subscribe to the topic: ");

  Serial.println(SUBSCRIBE_TOPIC);
  Serial.println("Arduino UNO R4 - MQTT broker Connected!");
}

void sendToMQTT(char key) {
  // แปลงตัวอักษรที่กด ให้เป็นข้อความ (Null-terminated String)
  char messageBuffer[2] = {key, '\0'};

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);
  
  Serial.println("Arduino UNO R4 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload: ");
  Serial.println(messageBuffer);
}

void messageReceived(String &topic, String &payload) {
  // ประมวลผลข้อความที่รับเข้ามาจาก MQTT
}



char getKey() {
  char pressedKey = '\0';
  for (int c = 0; c < 4; c++) {
    digitalWrite(cols[c], LOW);
    
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rows[r]) == LOW) {
        pressedKey = keys[r][c];
        
        delay(50); // Debounce
        while (digitalRead(rows[r]) == LOW) {
          mqtt.loop(); // รักษาการเชื่อมต่อ MQTT ขณะรอกดค้าง
        }
        delay(50);
      }
    }
    digitalWrite(cols[c], HIGH);
  }
  return pressedKey;
}