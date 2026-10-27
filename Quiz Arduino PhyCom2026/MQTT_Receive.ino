#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[] = "iPhone Lavendis";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "aon12344321";  // CHANGE TO YOUR WIFI PASSWORD

const char MQTT_BROKER_ADRRESS[] = "mqtt-dashboard.com";  // CHANGE TO MQTT BROKER'S ADDRESS
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "arduino-uno-r4-Lavendis";  // CHANGE IT AS YOU DESIRE
const char MQTT_USERNAME[] = "";              // CHANGE IT IF REQUIRED
const char MQTT_PASSWORD[] = "";              // CHANGE IT IF REQUIRED

// กำหนดเฉพาะ Topic ที่ต้องการรับข้อมูล
const char SUBSCRIBE_TOPIC[] = "68070162/venus"; 

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

void setup() {
  Serial.begin(9600);

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
  // ทำหน้าที่เช็คและรับข้อมูลเข้าจาก MQTT Broker ตลอดเวลา
  mqtt.loop();

  // เช็คเผื่อการเชื่อมต่อหลุด ให้ทำการต่อใหม่
  if (!mqtt.connected()) {
    connectToMQTT();
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

// เมื่อมีคนส่งข้อความมายัง SUBSCRIBE_TOPIC ฟังก์ชันนี้จะทำงานอัตโนมัติ
void messageReceived(String &topic, String &payload) {
  Serial.println("Arduino UNO R4 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload: " + payload);

  // นำค่า payload ที่รับได้ไปสั่งงานอุปกรณ์ต่อที่นี่
}