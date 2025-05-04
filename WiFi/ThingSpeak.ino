//ThingSpeak.ino
//Report one or more values to thingspeak.mathworks.com
#include <ESP8266WiFi.h>
// WiFi setup:
const char* ssid = "mySSID";       // replace with your WiFi network ssid
const char* password = "myPASSWORD";  // replace with your WiFi passward
WiFiClient client;

// Thingspeak setup:
// Create an account and log in to https://www.thingspeak.mathworks.com, create a channel, and obtain the Write API key.
String apiKey = "myWriteAPI";         // replace with your channel’s ThingSpeak API key
const char* server = "api.thingspeak.com";  // thingspeak server

void setup() {
  Serial.begin(115200);  //start Serial Monitor
  delay(5000);           // Wait a bit
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
  float t = analogRead(A0);          // take an analog reading
  //float h = analogRead(A0);          // placeholder for another value
  if (client.connect(server, 80)) {  // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    //postStr +="&field2="; // revise as necessary for number of fields used
    //postStr += String(h); //
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.print("Measure 1 sent: ");  // revise as necessary for the number of fields used
    Serial.println(t);
    //Serial.print(", Measure 2: ");
    //Serial.println(h);

    // Wait for server response
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read and print the response
    while (client.available()) {
      String line = client.readStringUntil('\n');
      if (line.startsWith("HTTP/1.1")) {
        Serial.print("HTTP Response: ");
        Serial.println(line);
        if (line.indexOf("200 OK") >= 0) {
          Serial.println("ThingSpeak update successful!");
        } else {
          Serial.println("ThingSpeak update failed!");
        }
      }
    }
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  client.stop();
  delay(60000);  // 60 sec delay. thingspeak needs min 15 sec delay betw updates
}
