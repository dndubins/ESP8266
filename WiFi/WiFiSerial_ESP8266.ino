// Part 4: Wireless serial over WiFi (ESP8266 / NodeMCU 1.0)
// The NodeMCU creates its own WiFi network and runs a TCP server.
// Whatever you type on your phone shows up in the Serial Monitor,
// and whatever you type in the Serial Monitor shows up on your phone.

#include <ESP8266WiFi.h>

const char *ssid = "my_ESP8266_device";  // change this to something unique!
const char *password = "12345678";       // must be at least 8 characters

WiFiServer server(8888);  // TCP port 8888
WiFiClient client;        // the phone/laptop that is connected to us

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.softAP(ssid, password);  // start our own WiFi network
  Serial.print("Network name: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());  // normally 192.168.4.1

  server.begin();
  Serial.println("Server started on port 8888. Ready to connect.");
}

void loop() {
  // A new phone/laptop is trying to connect: drop the old one, accept the new one
  if (server.hasClient()) {
    client.stop();
    client = server.available();
    Serial.println("Client connected!");
    client.println("Hello from the NodeMCU!");
  }

  if (client.connected()) {
    // phone -> Serial Monitor
    while (client.available()) {
      Serial.write(client.read());
    }
    // Serial Monitor -> phone
    while (Serial.available()) {
      client.write(Serial.read());
    }
  }
}
