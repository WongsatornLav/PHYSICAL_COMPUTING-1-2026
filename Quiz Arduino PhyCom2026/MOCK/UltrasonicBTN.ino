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
const char PUBLISH_TOPIC[] = "68070162/door";       // CHANGE IT AS YOU DESIRE
const char SUBSCRIBE_TOPIC[] = "68070162/";  // CHANGE IT AS YOU DESIRE

const int PUBLISH_INTERVAL = 5000;  // 5 seconds

WiFiClient network;
MQTTClient mqtt = MQTTClient(256);

unsigned long lastPublishTime = 0;
int lastButtonState = HIGH;  // ประกาศตัวแปรเก็บสถานะปุ่มกดครั้งก่อนหน้า

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(4, INPUT);
  pinMode(0, INPUT_PULLUP);

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

  int buttonState = digitalRead(0);
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button Pressed!");
    delay(50); // Debounce + ต่อต้านทานด้วย
  }
  lastButtonState = buttonState;

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
  // 1. อ่านค่าระยะทางจาก Ultrasonic
  digitalWrite(2, LOW);
  delayMicroseconds(2);
  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);
  
  long pulseWidth = pulseIn(4, HIGH, 30000); // Timeout 30ms
  long distance = pulseWidth / 29 / 2;

  // 2. อ่านสถานะปุ่มกด ณ ขณะที่ทำการวัดค่า (INPUT_PULLUP: กด = LOW)
  int currentButtonState = digitalRead(0);

  // 3. เงื่อนไข: ถ้ามากกว่าหรือเท่ากับ 20 cm "และ" กดปุ่มอยู่ (LOW) ให้ส่ง "on"
  String text = "";
  if (distance >= 20 && currentButtonState == LOW) {
    text = "on";
  } else {
    text = "off";
  }

  // 2. แปลงค่าตัวเลขเป็นข้อความ
  String val_str = String(text);
  char messageBuffer[10];
  val_str.toCharArray(messageBuffer, 10);

  // 3. ส่งข้อมูลขึ้น MQTT Broker
  mqtt.publish(PUBLISH_TOPIC, messageBuffer);

  // แสดงผลสถานะบน Serial Monitor
  Serial.println("------------------------------------");
  Serial.println("Arduino UNO R4 - Sent to MQTT:");
  Serial.print("- Topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("- Button: ");
  Serial.println(currentButtonState == LOW ? "PRESSED" : "RELEASED");
  Serial.print("- Payload: ");
  Serial.println(text);
  Serial.println("------------------------------------");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Arduino UNO R4 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);
}

