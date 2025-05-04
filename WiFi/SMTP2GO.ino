//SMTP2GO.ino
//Written for NodeMCU 1.0
//Author: D. Dubins
//Last Edited: 04-May-25
//Report value by email, using https://www.smtp2go.com

#include <ESP8266WiFi.h>
// WiFi setup:
const char* ssid = "yourSSID";       // replace with your WiFi network ssid
const char* password = "yourPASS";   // replace with your WiFi passward
WiFiClient client;

//sign up for SMTP2GO for a free SMTP email service (www.smtp2go.com)
#define SMTP2GO_USER "you@whereyouare.com"  // change to your SMTP Users Username (not your account username)
#define SMTP2GO_PASS "yourPASS"             // change to your SMTP Users password (not your account password)
#define SMTP2GO_SERVER "mail.smtp2go.com"
#define SMTP2GO_PORT 2525

//email contact info
#define NUMCONTACTS 1                              // number of contacts to send email to
const String EMAIL_FROM = "you@whereyouare.com";   // enter your email here
const String EMAIL_TO[NUMCONTACTS] = {
  //"email1@wherever.com", // list email addresses as needed separated by commas (# should match NUMCONTACTS)
  //"email2@wherever.com",
  "email3@wherever.com"
};

//email information (edit sensor output line in email subroutine)
String EMAIL_SUBJ = "ESP3266 Alert";
String EMAIL_LINE1 = "This email message is being sent from a microcontroller.";  // don't put any colons in the message.

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
  int data = analogRead(A0);                // take an analog reading
  byte ret = emailSend(data, EMAIL_LINE1);  // send email alert
  if (ret) {
    Serial.println("Email sent.");
  } else {
    Serial.println("Error: Email not sent.");
  }
}

void loop() {
}

byte emailSend(int reading1, String line1) {
  //Username and password for SMTP2GO need to be base 64 encoded. Free encoding here: https://www.base64encode.org/
  //Encode username to base64 (required for SMTP2GO)
  char usr[] = SMTP2GO_USER;
  int usr_len = strlen(usr);
  int usr_encoded_len = 4 * ((usr_len + 2) / 3);
  char* usr_encoded=base64_encode(usr);  //encode username
  //Encode password to base64 (required for SMTP2GO)
  char pass[] = SMTP2GO_PASS;
  char* pass_encoded=base64_encode(pass);  //encode password
  //Serial.print("User encoded: ");  //for debugging
  //Serial.println(usr_encoded);
  //Serial.print("Pass encoded: ");
  //Serial.println(pass_encoded);

  //Start the email connection
  byte thisByte = 0;
  byte respCode;
  if (client.connect(SMTP2GO_SERVER, SMTP2GO_PORT) == 1) {
#ifdef SERIALDEBUG
    Serial.println(F("connected"));
#endif
  } else {
#ifdef SERIALDEBUG
    Serial.println(F("connection failed"));
#endif
    return 0;
  }
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending EHLO"));
#endif
  client.println("EHLO 192.168.1.123");  // the IP address doesn't matter
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending auth login"));
#endif
  client.println("AUTH LOGIN");
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending User"));
#endif
  client.println(usr_encoded);  // SMTP base 64 encoded Username
  free(usr_encoded);
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending Password"));
#endif
  client.println(pass_encoded);  //  SMTP base 64 encoded Password
  free(pass_encoded);
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending From"));  // change to your email address (sender)
#endif
  client.println("MAIL From:<" + EMAIL_FROM + ">");  // not important
  if (!eRcv()) return 0;                             // change to recipient address
#ifdef SERIALDEBUG
  Serial.println(F("Sending To"));
#endif
  for (int i = 0; i < NUMCONTACTS; i++) {  // email TO contact info goes here
    client.println("RCPT To:<" + EMAIL_TO[i] + ">");
    if (!eRcv()) return 0;
  }
#ifdef SERIALDEBUG
  Serial.println(F("Sending DATA"));
#endif
  client.println(F("DATA"));
  if (!eRcv()) return 0;
#ifdef SERIALDEBUG
  Serial.println(F("Sending email"));  // change to recipient address
#endif
  for (int i = 0; i < NUMCONTACTS; i++) {        // email TO contact info goes here
    client.println("To:<" + EMAIL_TO[i] + ">");  // change to your address
  }
  client.println("From:<" + EMAIL_FROM + ">");
  client.println("Subject:" + EMAIL_SUBJ);
  client.println("Importance:low\r\n");  // Email importance flag. Set "high" if important.
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

byte eRcv() {
  byte respCode;
  byte thisByte;
  int loopCount = 0;
  while (!client.available()) {
    delay(1);
    loopCount++;  // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
#ifdef SERIALDEBUG
      Serial.println(F("\r\nTimeout"));
#endif
      return 0;
    }
  }
  respCode = client.peek();
  while (client.available()) {
    thisByte = client.read();
#ifdef SERIALDEBUG
    Serial.write(thisByte);
#endif
  }
  if (respCode >= '4') {
    //  efail();
    return 0;
  }
  return 1;
}

//The following function was written with the assistance of Perplexity.AI and ChatGPT 4.0 to encode a string into base 64:
char* base64_encode(const char* input) {
  const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
  int input_len = strlen(input);
  int encoded_len = 4 * ((input_len + 2) / 3);  // Calculate encoded string length
  char* output = (char*) malloc(encoded_len + 1);  // +1 for null terminator

  if (output == NULL) {  // Check if memory allocation succeeded
    Serial.println("Memory allocation failed!");
    return NULL;
  }

  int i = 0, j = 0;
  uint8_t byte3[3], byte4[4];

  while (input_len--) {
    byte3[i++] = *(input++);
    if (i == 3) {
      byte4[0] = (byte3[0] & 0xfc) >> 2;
      byte4[1] = ((byte3[0] & 0x03) << 4) | ((byte3[1] & 0xf0) >> 4);
      byte4[2] = ((byte3[1] & 0x0f) << 2) | ((byte3[2] & 0xc0) >> 6);
      byte4[3] = byte3[2] & 0x3f;

      for (i = 0; i < 4; i++) {
        output[j++] = base64_chars[byte4[i]];
      }
      i = 0;
    }
  }

  // Handle padding for the last few bytes
  if (i) {
    for (int k = i; k < 3; k++)
      byte3[k] = '\0';

    byte4[0] = (byte3[0] & 0xfc) >> 2;
    byte4[1] = ((byte3[0] & 0x03) << 4) | ((byte3[1] & 0xf0) >> 4);
    byte4[2] = ((byte3[1] & 0x0f) << 2) | ((byte3[2] & 0xc0) >> 6);

    for (int k = 0; k < i + 1; k++)
      output[j++] = base64_chars[byte4[k]];

    while (i++ < 3)
      output[j++] = '=';
  }

  output[j] = '\0';  // Null-terminate the output string
  return output;     // Return the Base64-encoded string
}
