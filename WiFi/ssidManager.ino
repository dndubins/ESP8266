// ssidManager.ino 
// Connects to an expected (programmed) SSID and if that doesn't work, starts the WiFi manager
// to set up a local SSID, so you can connect to the MCU with your phone, and connect it wireless
// from there.
// Library: https://github.com/tzapu/WiFiManager 2.0.17 (available through Library Manager)

#include <ESP8266WiFi.h>

// For WiFi manager:
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  //include WiFiManager.h by tzapu  
WiFiClient client; // for WiFi client
WiFiManager wifiManager; // for WiFi manager
const char* device_ssid = "ESP8266"; // ssid of MCU
const char* ssid = "ssid"; // router ssid
const char* password = "ssidPassword"; // router password
unsigned long timer=0L;

void setup() {
  Serial.begin(115200); //start Serial Monitor
  delay(5000); //wait a bit
  WiFi.begin(ssid, password);  // for default ssid
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  timer=millis(); // reset the timer
  while (WiFi.status()!=WL_CONNECTED && millis()-timer<60000) { // 1 min timeout
    delay(500);
    Serial.print(".");
  }
  //Start WiFi Manager if unable to connect to saved ssid:
  if(WiFi.status()!=WL_CONNECTED){  // load WiFi manager if connection to default fails.
    Serial.println("\nFailed to connect to default ssid. Trying WiFi manager.");
    Serial.print("Connect to: ");
    Serial.print(device_ssid);
    Serial.println(" to connect MCU to WiFi.");
    wifiManager.autoConnect(device_ssid);
  }
  Serial.println("\nWiFi connected.");
  timer=millis(); // reset the timer
}

void loop() {
}
