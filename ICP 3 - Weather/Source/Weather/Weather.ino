/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(9,10); 
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>
LiquidCrystal_I2C lcd(0x27,16,2);
//
//Thingspeak//
#define SSID "A-iPhone"     // "SSID-WiFiname" 
#define PASS "a1234567"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=FIRB0M2PQDU2AAPA"; //change it with your api key like "GET /update?key=Your Api Key"

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
Adafruit_BMP280 bmp; // I2C

int error;

//Variables
// Heartbeat monitor A2
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int light;

//dust//
int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float Vsig;

void setup()
{
  //
  
  lcd.init();
  lcd.backlight();
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  Serial.begin(115200);
  // wifi //
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(2000);
  //if(esp8266.find("OK")){
  connectWiFi();
  //}

  pinMode(A1,INPUT);
  
  pinMode(pin,INPUT);
  starttime = millis(); 
  //
  dht.begin();
  Serial.println(F("BMP280 test"));
//  if (!bmp.begin()) {  
//    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
//    while (1);
//}
}

void getDust() {
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
    lcd.print("Concentration = ");
    lcd.print(concentration);
    lcd.println(" pcs/0.01cf");
    lcd.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
  }
}

void loop()
{
    //
    lcd.clear();
    lcd.setCursor(0, 0);
    temp = dht.readTemperature();
    hum = dht.readHumidity();

    
    lcd.print("Humidity: ");
    lcd.print(hum);
    
    lcd.setCursor(0, 1);
    lcd.print(" %, Temp: ");
    lcd.print(temp);
    lcd.println(" Celsius");


   light=analogRead(A1);

  int sensorValue;
  long  sum=0;
  for(int i=0;i<1024;i++)
   {  
      sensorValue=analogRead(A0);
      sum=sensorValue+sum;
      delay(2);
   }   
 sum = sum >> 10;
 Vsig = sum*4980.0/1023.0; // Vsig is the value of voltage measured from the SIG pin of the Grove interface

    ///////////
    sendData();

     delay(10000); //Delay 2 sec.

  
}
boolean connectWiFi(){
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(5000);
  if(esp8266.find("OK")){
    Serial.println("OK");
    return true;    
  }else{
    return false;
  }
}

void sendData(){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  if(esp8266.find("Error")){
    tone(12, 1000);
    return;
  }
  cmd = msg ;
  cmd += "&field4=";
  cmd += temp;
  cmd += "&field7=";
  cmd += hum;
  cmd += "&field6=";
  cmd += light;
  cmd += "&field2=";
  cmd += concentration;
  cmd += "&field5=";
  cmd += Vsig;
  cmd += "&field3=";
  cmd += bmp.readAltitude(1013.25);
  cmd += "\r\n";

  
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
 
  if(esp8266.find(">")){
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else{
   Serial.println("AT+CIPCLOSE");
   esp8266.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}
   
