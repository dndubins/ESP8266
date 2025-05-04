//SMTP2GO.ino
//Written for NodeMCU 1.0
//Author: D. Dubins
//Last Edited: 04-May-25
//Report value by email, using https://www.smtp2go.com
#include <ESP8266WiFi.h>
// WiFi setup:
const char* ssid = "yourSSID";       // replace with your WiFi network ssid
const char* password = "yourPASSWORD";  // replace with your WiFi passward
WiFiClient client;

//sign up for SMTP2GO for a free SMTP email service (www.smtp2go.com)
//username and password for SMTP2GO need to be base 64 encoded. Free encoding here: https://www.base64encode.org/
#define SMTP2GO_USER "yourUserName" // change to your SMTP2GO username, base64 encoded
#define SMTP2GO_PASS "yourPassword" // change to your SMTP2GO password, base64 encoded
#define SMTP2GO_SERVER "mail.smtp2go.com"
#define SMTP2GO_PORT 2525
//email contact info
# define NUMCONTACTS 1 // number of contacts to send email to
const String EMAIL_FROM = "email1@wherever.com"; // enter your email here
const String EMAIL_TO[NUMCONTACTS]={
  //"email2@wherever.com", // list email addresses as needed, separated by commas (# should match NUMCONTACTS)
  //"email3@wherever.com", 
  "email4@wherever.com"};

//email information (edit sensor output line in email subroutine)
String EMAIL_SUBJ = "ESP3266 Alert";
String EMAIL_LINE1 = "This email message is being sent from a microcontroller."; // don't put any colons in the message.

void setup() {
  Serial.begin(115200);  //start Serial Monitor
  delay(5000);           //wait a bit
  WiFi.begin(ssid, password);
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  int data = analogRead(A0);          // take an analog reading
  byte ret = emailSend(data,EMAIL_LINE1); // send email alert
  if(ret){
    Serial.println("Email sent.");
  }else{
    Serial.println("Error: Email not sent.");
  }
}

void loop() {
}

byte emailSend(int reading1, String line1){
  byte thisByte=0;
  byte respCode;
  if (client.connect(SMTP2GO_SERVER, SMTP2GO_PORT) == 1) {
    #ifdef SERIALDEBUG
      Serial.println(F("connected"));
    #endif
  }else{
    #ifdef SERIALDEBUG
      Serial.println(F("connection failed"));
    #endif
    return 0;
  }
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending EHLO"));
  #endif
  client.println("EHLO 192.168.1.123"); // the IP address doesn't matter
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending auth login"));
  #endif
  client.println("AUTH LOGIN");
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending User"));
  #endif
  client.println(SMTP2GO_USER);
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending Password"));
  #endif
  client.println(SMTP2GO_PASS);//  SMTP Password
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending From"));  
  #endif
  client.println("MAIL From:<"+EMAIL_FROM+">");
  if (!eRcv()) return 0; 
  #ifdef SERIALDEBUG
    Serial.println(F("Sending To"));
  #endif
  for(int i=0;i<NUMCONTACTS;i++){
    client.println("RCPT To:<"+EMAIL_TO[i]+">");
    if (!eRcv()) return 0;
  }
  #ifdef SERIALDEBUG
    Serial.println(F("Sending DATA"));
  #endif
  client.println(F("DATA"));
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending email"));
  #endif
  for(int i=0;i<NUMCONTACTS;i++){ 
    client.println("To:<"+EMAIL_TO[i]+">"); 
  }
  client.println("From:<"+EMAIL_FROM+">");
  client.println("Subject:"+EMAIL_SUBJ);
  client.println("Importance:low\r\n");   // Email importance flag. Set "high" if important.
  client.println(line1);
  client.print(F("\r\nCurrent reading: "));
  client.print(reading1);
  client.println(F("."));
  client.println(F("."));
  if (!eRcv()) return 0;
  #ifdef SERIALDEBUG
    Serial.println(F("Sending QUIT"));
  #endif
  client.println(F("QUIT"));
  if (!eRcv()) return 0;
  client.stop();
  #ifdef SERIALDEBUG
    Serial.println(F("disconnected"));
  #endif
  return 1;
}

byte eRcv(){
    byte respCode;
    byte thisByte;
    int loopCount = 0;
    while (!client.available())
  {
      delay(1);
      loopCount++;     // if nothing received for 10 seconds, timeout
      if (loopCount > 10000) {
      client.stop();
      #ifdef SERIALDEBUG
        Serial.println(F("\r\nTimeout"));
      #endif
      return 0;
    }
  }
  respCode = client.peek();
  while (client.available())
  {
    thisByte = client.read();
    #ifdef SERIALDEBUG
      Serial.write(thisByte);
    #endif
  }
  if (respCode >= '4')
  {
    //  efail();
    return 0;
  }
  return 1;
}
