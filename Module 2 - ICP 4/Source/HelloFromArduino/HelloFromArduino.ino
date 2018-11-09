#include <SoftwareSerial.h>
#include "Sync.h"

#define ssid "A-iPhone"
#define pass "a1234567"
#define host "cs5590-m2l4.herokuapp.com"
#define projectId "firstapp-87082"
//#define projectId "my-project-1537125901149"

#define BUFFER_SIZE 50
bool isJoined = false;
bool isConnected = false;

// AT Commands -- store in flash
const char atCommand[] PROGMEM = "AT";
const char echoOffCommand[] PROGMEM = "ATE0";
const char baudCommand[] PROGMEM = "AT+UART_CUR=9600,8,1,0,0";
const char modeCommand[] PROGMEM = "AT+CWMODE=1";
const char joinCommand[] PROGMEM = "AT+CWJAP=\"" ssid "\",\"" pass "\"";
const char startCommand[] PROGMEM = "AT+CIPSTART=\"TCP\",\"" host "\",80";
const char closeCommand[] PROGMEM = "AT+CIPCLOSE";
const char sendDataCommand[] PROGMEM = "AT+CIPSENDEX=2048";

Sync updateSync;

// Variables
#define USE_SOFTWARE_SERIAL 0

#if USE_SOFTWARE_SERIAL
SoftwareSerial esp8266(2,3);
#define BAUD_RATE 9600
#else
#define esp8266 Serial
#define BAUD_RATE 115200
#endif

void setup() {

  while(!esp8266){}

  #if USE_SOFTWARE_SERIAL
    Serial.begin(BAUD_RATE);
    setupSoftwareSerial();
  #else
    esp8266.begin(BAUD_RATE);
    sendCommand(atCommand);
    while(!readline().equals("OK")) {}
  #endif

  sendCommand(echoOffCommand);
  while(!readline().equals("OK")) {}
  
  sendCommand(modeCommand);
  while(!readline().equals("OK")) {}

  sendCommand(joinCommand);

  while(!isJoined) {
    String result = readline();
    if (result.equals("OK")) {
      isJoined = true;
    } else if (result.indexOf("FAIL")  != -1 || result.indexOf("ERROR") != -1) {
      return;
    }
  }

  updateData();
}

void loop() {
}

String readline(char until) {
  char buff[BUFFER_SIZE] = {0};
  int n = 0;
  
  while(1) {
    int in = esp8266.read();
    if (in >= 0) {
      #if USE_SOFTWARE_SERIAL
      Serial.write(in);
      #endif
      if (in == until) {
        buff[n] = in;
        buff[n+1] = 0;
        return String(buff);
      }
      if (in == '\n' && n > 0 && buff[n - 1] == '\r') {
        //\r\n
        if (n > 1) {
          buff[n - 1] = 0;
          return String(buff);
        }
        n = 0;
        continue;
      }
      if (in == '\n') {
        n = 0;
        continue;
      }
      buff[n] = in; n++;
      if (n == BUFFER_SIZE - 2) {
        buff[n + 1] = 0;
        return String(buff);
      }
    }
  }
}

String readline(void) {
  return readline(0);
}

void sendCommand(const char *msg) {
  esp8266.println((__FlashStringHelper*)msg);
  #if USE_SOFTWARE_SERIAL
  Serial.println((__FlashStringHelper*)msg);
  #endif
}

void updateData() {
  if (!isConnected) {
    sendCommand(startCommand);
    while(1) {
      String result = readline();
      if (result.equals("OK") || result.equals("ALREADY CONNECTED")) {
        isConnected = true;
        break;
      } else if (result.equals("FAIL")|| result.equals("ERROR")) {
        delay(1000);
        break;
      }
    }
    if (!isConnected) {
      return;
    }
  }

  sendCommand(sendDataCommand);

  bool responded = false;
  while(!responded) {
    String result = readline('>');
    if (result.equals(">")) {
      responded = true;
    } else if (result.equals("FAIL") || result.equals("ERROR")) {
      sendCommand(closeCommand);
      isConnected = false;
      delay(1000);
      return;
    }
  }

  esp8266.print("GET /");
  esp8266.print(projectId);
  esp8266.print("/%22Hello%20from%20Arduino%22");

  esp8266.println(" HTTP/1.0");
  esp8266.print("Host: ");
  esp8266.println(host);
  esp8266.println();
  esp8266.print("\\0"); // EOT

#if USE_SOFTWARE_SERIAL

  Serial.print("GET /");
  Serial.print(projectId);
  Serial.print("/\"Hello from Arduino\"");

  Serial.println(" HTTP/1.0");
  Serial.print("Host: ");
  Serial.println(host);
  Serial.println();
  Serial.print("\\0"); // EOT

#endif
      
  responded = false;
  while(!responded) {
    String result = readline();
    if (result.equals("SEND OK")) {
      responded = true;
    } else if (result.equals("SEND FAIL") || result.equals("ERROR")) {
      responded = true;
      sendCommand(closeCommand);
      isConnected = false;
      delay(1000);
    }
  }
  
  bool closed = false;
  while(!closed) {
    String result = readline();
    if (result.equals("CLOSED")) {
      closed = true;
    }
  }
  isConnected = false;
}

#if USE_SOFTWARE_SERIAL

String getResultIfAvailable() {
  while(esp8266.available()) {
    String result = esp8266.readStringUntil('\n');
    result.trim();
    if (result[0] != 0) {
      return result;
    }
  }
  return "";
}

String waitForResult() {
  // Wait for data
  int _startMillis = millis();
  while(esp8266.available() == 0 && millis() - _startMillis < 60000);
  return getResultIfAvailable();
}

bool setupSoftwareSerial() {
  unsigned long current = 0;
  for (int i = 0; i < 2; i++) {
    for (int attempt = 0; attempt < 5; attempt++) {
      unsigned long baud = i ? 115200 : 9600;
      esp8266.begin(baud);
      sendCommand(atCommand);
      delay(1000);
      if (getResultIfAvailable().indexOf("OK") != -1) {
          current = baud;
          break;
      }
    }
    if (current) { break; }
  }

  Serial.print(F("Current baud is "));
  Serial.println(current);
  esp8266.setTimeout(5000);
  if (current == 0) {
    return false;
  }
  if (current == BAUD_RATE) {
    return true;
  }
  for (int attempt = 0; attempt < 5; attempt++) {
    sendCommand(baudCommand);
//    delay(50);
    String result = getResultIfAvailable();
    if (result.indexOf("OK") != -1 || result.indexOf("AT") != -1) {
      esp8266.begin(BAUD_RATE);
//      delay(100);
      return true;
    }
  }
  return false;
}

#endif
