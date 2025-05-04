// WiFiAccessPoint.ino: WiFi Access Point
// Set the NodeMCU 1.0 v3 up as a WiFi access point, and start a simple web server. 
// Connect to the ESP8266 directly, then go to http://192.168.4.1 in a web browser
// connected to this access point.
// Written by: ChatGPT 4.0
// Requested by: D. Dubins
// Date: 03-Mar-25
// User toggles the on-board LED on and off.

#include <ESP8266WiFi.h> // Include the WiFi library for ESP8266
#include <WiFiClient.h>  // Used for creating TCP connections
#include <ESP8266WebServer.h> // Web server library

#ifndef APSSID
#define APSSID "ESP8266"   //enter SSID name here
#define APPSK "ESP123456"   //enter password, at least 8 chars long
#endif

const char *ssid = APSSID;  //store APSSID to ssid
const char *password = APPSK; //stor APPSK to password

ESP8266WebServer server(80);  //Create a web server object that listens on port 80 (HTTP)

const int ledPin = 2; // Variable to keep track of LED state (off by default)
bool ledState = false;

void handleRoot() {
  float volts=analogRead(A0)*3.3/1023.0; // read divs on pin A0 and convert to volts
  // Create a simple HTML page with a button to toggle the LED
  String ledString=String(ledState ? "<font color=\"green\">ON</font>" : "<font color=\"red\">OFF</font>");
  String html = "<!DOCTYPE html><html><head><title>LED Control</title></head><body>";
  html += "<h1>You are connected</h1>";
  html += "Voltage on A0: <b><font color=\"blue\">" + (String)volts + "V</font></b>";
  html += "<p>LED is currently: <strong>" + ledString + "</strong></p>";
  html += "<form action=\"toggle\" method=\"POST\">";
  html += "<button type=\"submit\">Toggle LED</button>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html); // Send the HTML to the client
}

void handleToggle() {
  ledState = !ledState; // Flip the LED state (on to off or off to on)
  digitalWrite(ledPin,ledState?LOW:HIGH); // Turn LED on (LOW) or off (HIGH)
  handleRoot();  // Reload the main page to show updated state
}

void setup() {
  Serial.begin(115200); // start Serial Monitor
  delay(5000);  // Short delay to allow everything to settle
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
  digitalWrite(ledPin, HIGH); // Turn LED off (active LOW)
  // Start setting up the access point
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.mode(WIFI_AP);
  bool result = WiFi.softAP(ssid, password);
  if (result) {
    Serial.println("Access Point Started Successfully");
  } else {
    Serial.println("Access Point Failed to Start");
  }
  Serial.print("AP SSID: ");
  Serial.println(WiFi.softAPSSID());

  // Print the IP address of the AP to the Serial Monitor
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Define URL routes and their handler functions
  server.on("/", handleRoot);  // When user goes to 192.168.4.1/
  server.on("/toggle", HTTP_POST, handleToggle); // When user presses the button
  server.begin();  // Start the web server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();  // Listen for incoming client requests
}
