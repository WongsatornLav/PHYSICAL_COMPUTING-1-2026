#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[] = "iPhone Lavendis";     // CHANGE TO YOUR WIFI SSID
const char WIFI_PASSWORD[] = "aon12344321";  // CHANGE TO YOUR WIFI PASSWORD

const char MQTT_BROKER_ADRRESS[] = "phycom.it.kmitl.ac.th";  // CHANGE TO MQTT BROKER'S ADDRESS
//const char MQTT_BROKER_ADRRESS[] = "192.168.0.11";  // CHANGE TO MQTT BROKER'S IP ADDRESS
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "arduino-uno-r4-Lavendis";  // CHANGE IT AS YOU DESIRE
const char MQTT_USERNAME[] = "";              // CHANGE IT IF REQUIRED, empty if not required
const char MQTT_PASSWORD[] = "";              // CHANGE IT IF REQUIRED, empty if not required

// The MQTT topics that Arduino should publish/subscribe
const char PUBLISH_TOPIC[] = "68070162/";       // CHANGE IT AS YOU DESIRE
const char SUBSCRIBE_TOPIC[] = "68070162/venus";  // CHANGE IT AS YOU DESIRE

const int PUBLISH_INTERVAL = 5000;  // 5 seconds

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

unsigned long lastPublishTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.print("Arduino UNO R4 - Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  connectToMQTT();
}

void loop() {
  mqtt.loop();

  // ตรวจสอบการเชื่อมต่อ MQTT และ Reconnect หากหลุด
  if (!mqtt.connected()) {
    connectToMQTT();
  }

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }
}

void connectToMQTT() {
  // Connect to the MQTT broker
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  // Create a handler for incoming messages
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

  // Subscribe to a topic, the incoming messages are processed by messageHandler() function
  if (mqtt.subscribe(SUBSCRIBE_TOPIC))
    Serial.print("Arduino UNO R4 - Subscribed to the topic: ");
  else
    Serial.print("Arduino UNO R4 - Failed to subscribe to the topic: ");

  Serial.println(SUBSCRIBE_TOPIC);
  Serial.println("Arduino UNO R4 - MQTT broker Connected!");
}

void sendToMQTT() {

  // int val = millis()/1000;
  // //int val = analogRead(A0);
  // String val_str = String(val);
  // char messageBuffer[10];
  // val_str.toCharArray(messageBuffer, 10);
  // mqtt.publish(PUBLISH_TOPIC, messageBuffer);
  
  // Serial.println("Arduino UNO R4 - sent to MQTT:");
  // Serial.print("- topic: ");
  // Serial.println(PUBLISH_TOPIC);
  // Serial.print("- payload:");
  // Serial.println(messageBuffer);
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Arduino UNO R4 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);

  int num = payload.toInt();
  
  if (num >= 36 && num <= 50) {
    digitalWrite(2, LOW);   // Red ON
    digitalWrite(3, HIGH);  // Blue OFF
    digitalWrite(4, HIGH);  // Green OFF
  }
  // 26 - 35 -> LED สีฟ้า
  else if (num >= 26 && num <= 35) {
    digitalWrite(2, HIGH);  // Red OFF
    digitalWrite(3, LOW);   // Blue ON
    digitalWrite(4, HIGH);  // Green OFF
  }
  // 10 - 25 -> LED สีเขียว
  else if (num >= 10 && num <= 25) {
    digitalWrite(2, HIGH);  // Red OFF
    digitalWrite(3, HIGH);  // Blue OFF
    digitalWrite(4, LOW);   // Green ON
  }
  // นอกเหนือจากช่วง 10-50 ให้ปิดไฟทั้งหมด
  else {
    digitalWrite(2, HIGH);  // Red OFF
    digitalWrite(3, HIGH);  // Blue OFF
    digitalWrite(4, HIGH);  // Green OFF
  }
}

