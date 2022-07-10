/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "SSID";
const char* password = "PASS";
const char* mqtt_server = "192.168.1.22";

const int LeftExtendPin = 16;
const int LeftRetractPin = 5;
const int RightExtendPin = 4;
const int RightRetractPin = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Get MQTT Message (Interrupt)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Left Couch Handler
  if (strcmp(topic, "CouchRelays/Left") == 0) {
    Serial.println("Got to left");
    if ((char)payload[0] == '1') {
      digitalWrite(LeftExtendPin, LOW);
      digitalWrite(LeftRetractPin, HIGH);
    } else {
      digitalWrite(LeftExtendPin, HIGH);
      digitalWrite(LeftRetractPin, LOW);
    }
  }

  // Right Couch Handler
  if (strcmp(topic, "CouchRelays/Right") == 0) {
    Serial.println("Got to right");
    if ((char)payload[0] == '1') {
      digitalWrite(RightExtendPin, LOW);
      digitalWrite(RightRetractPin, HIGH);
    } else {
      digitalWrite(RightExtendPin, HIGH);
      digitalWrite(RightRetractPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("CouchRelays/Right");
      client.subscribe("CouchRelays/Left");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(LeftExtendPin, OUTPUT);
  pinMode(LeftRetractPin, OUTPUT);
  pinMode(RightExtendPin, OUTPUT);
  pinMode(RightRetractPin, OUTPUT);

  digitalWrite(LeftExtendPin, HIGH);
  digitalWrite(LeftRetractPin, HIGH);
  digitalWrite(RightExtendPin, HIGH);
  digitalWrite(RightRetractPin, HIGH);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
