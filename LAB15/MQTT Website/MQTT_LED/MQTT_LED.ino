#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[] = "iPhone Lavendis";     
const char WIFI_PASSWORD[] = "aon12344321"; 

const char MQTT_BROKER_ADRRESS[] = "mqtt-dashboard.com";
const int MQTT_PORT = 1883;

String client_id_str = "arduino-LAVENDIS";

const char MQTT_USERNAME[] = "";
const char MQTT_PASSWORD[] = "";

// บังคับ Topic ตามโจทย์
const char SUBSCRIBE_TOPIC[] = "wongsatorn/68070162";
 
const int PIN_RED = 4;    // ขาไฟสีแดง
const int PIN_GREEN = 3;  // ขาไฟสีเขียว
const int PIN_BLUE = 2;   // ขาไฟสีน้ำเงิน

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

void setup() {
  Serial.begin(9600);

  // ตั้งค่าพินเป็น OUTPUT
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  // สำหรับ Common Anode: สั่ง HIGH เพื่อปิดไฟเริ่มต้นทุกสี
  digitalWrite(PIN_RED, HIGH);
  digitalWrite(PIN_GREEN, HIGH);
  digitalWrite(PIN_BLUE, HIGH);

  // เชื่อมต่อ Wi-Fi
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  connectToMQTT();
}

void loop() {
  mqtt.loop();

  if (!mqtt.connected()) {
    connectToMQTT();
  }
}

void connectToMQTT() {
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);

  Serial.print("Connecting to MQTT broker...");

  while (!mqtt.connect(client_id_str.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nMQTT Broker Connected!");

  if (mqtt.subscribe(SUBSCRIBE_TOPIC)) {
    Serial.print("Subscribed to Topic: ");
    Serial.println(SUBSCRIBE_TOPIC);
  }
}

// เมื่อมีข้อความส่งมาจาก MQTT
void messageReceived(String &topic, String &payload) {
  Serial.println("Received -> Topic: " + topic + " | Payload: " + payload);

  if (payload == "RED_ON") {
    digitalWrite(PIN_RED, LOW);
  } else if (payload == "RED_OFF") {
    digitalWrite(PIN_RED, HIGH);
  } else if (payload == "GREEN_ON") {
    digitalWrite(PIN_GREEN, LOW);
  } else if (payload == "GREEN_OFF") {
    digitalWrite(PIN_GREEN, HIGH);
  } else if (payload == "BLUE_ON") {
    digitalWrite(PIN_BLUE, LOW);
  } else if (payload == "BLUE_OFF") {
    digitalWrite(PIN_BLUE, HIGH);
  }
}