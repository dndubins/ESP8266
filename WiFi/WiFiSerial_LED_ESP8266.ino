// Part 5: Control an LED and read an analog value over WiFi serial
// ESP8266 / NodeMCU 1.0
//
// Wiring:
//   NodeMCU D4 (GPIO2) -- LED -- 1K resistor -- GND
//   NodeMCU D2 (GPIO4) -- switch -- GND
//   NodeMCU A0         -- analog input (e.g. potentiometer wiper)
//
// Send '1' from your phone to turn the LED on, '0' to turn it off.
// Press the button to send an analog reading (0-1023) to your phone.

#include <ESP8266WiFi.h>

const char *ssid = "my_ESP8266_device";  // use the same name as in Part 4
const char *password = "12345678";

#define ledPin 2      // LED connected to GPIO2 (D4 on board)
#define swPin 4       // momentary switch connected to GPIO4 (D2 on board)
#define analogPin A0  // analog reading (0-1023 = 0-3.3V)

WiFiServer server(8888);
WiFiClient client;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  Serial.println("Device started. Ready to connect.");
}

void loop() {
  // accept a new connection (newest one wins)
  if (server.hasClient()) {
    client.stop();
    client = server.available();
    Serial.println("Client connected!");
    client.println("Connected! Send 1 = LED ON, 0 = LED OFF.");
  }

  if (client.connected()) {
    // Serial Monitor -> phone
    while (Serial.available()) {
      client.write(Serial.read());
    }

    // phone -> LED
    while (client.available()) {
      char cmd = client.read();
      if (cmd == '1') {
        digitalWrite(ledPin, HIGH);
        client.println("LED ON");
        Serial.println("LED ON");
      } else if (cmd == '0') {
        digitalWrite(ledPin, LOW);
        client.println("LED OFF");
        Serial.println("LED OFF");
      }
      // anything else (including newline characters) is ignored
    }

    // button -> phone
    if (digitalRead(swPin) == LOW) {  // if button pushed
      int reading = analogRead(analogPin);
      client.println(reading);
      Serial.println(reading);
      delay(500);  // simple debounce / rate limit
    }
  }

  delay(20);
}
