//base64.ino
//Encode a char array in base 64
//This is needed for SMTP2GO.
//This routine was written with the assistance of Perplexity.AI and ChatGPT4.0 on 04-May-25
//and 
//To encode online, visit: https://www.base64encode.org/
//To decode online, visit: https://www.base64decode.org/

void setup() {
  Serial.begin(115200); //start the Serial Monitor
  delay(5000);
  char input[] = "Hello world!";  // input string to encode
  char* encoded = base64_encode(input);  // Encode the string
  Serial.println(encoded);  // Outputs "SGVsbG8gd29ybGQh"
  free(encoded); // Free the dynamically allocated memory after use
}

void loop() {
}

// Function to encode input in Base64, now handling length internally
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
