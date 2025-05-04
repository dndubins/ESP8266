//ThingSpeak.ino
//Report one or more values to thingspeak.mathworks.com
#include <ESP8266WiFi.h>
// WiFi setup:
const char* ssid = "mySSID"; // WiFi network ssid
const char* password = "myPASS"; // WiFi passward
WiFiClient client;

// ThingSpeak Account and Channel setup:
// Create an account and log in to www.thingspeak.mathworks.com, create a channel, and obtain an API key.
String apiKey = "myWriteAPIkey"; // replace with your channel’s ThingSpeak Write API Key
const char* server = "api.thingspeak.com"; // thingspeak server

void setup() {
  Serial.begin(115200); //start Serial Monitor
  delay(5000); // Wait a bit
  WiFi.begin(ssid, password);
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  float t=analogRead(A0); // take an analog reading
  if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(t);
    //postStr +="&field2="; // revise as necessary for number of fields used
    //postStr += String(h);
    //postStr += "\r\n\r\n";  // this puts two hard returns at the end of the data string
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.print("Measure 1: "); // revise as necessary for the number of fields used
    Serial.print(t);
    //Serial.print(", Measure 2: ");
    //Serial.println(h);
  }
  client.stop();
  delay(60000); // 60 sec delay. thingspeak needs min 15 sec delay betw updates
}
