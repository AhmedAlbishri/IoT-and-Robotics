//Libraries
#include <DHT.h>
#include <SoftwareSerial.h>
#define DEBUG true
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>
LiquidCrystal_I2C lcd(0x27,16,2);

String ssid = "A-iPhone";     // "SSID-WiFiname" 
String pass = "a1234567";       // "password"
String aioKey = "7fc06c92e3dd45a082929b57812e6ac6";

String fieldBarometer = "barometer";
String fieldPulseMonitor = "pulse-monitor";
String fieldTemp = "temp";
String fieldHum = "hum";
String fieldDust = "dust";
String fieldUV = "uv";
String fieldLight = "light";

SoftwareSerial esp8266(9,10);
//
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
//
String Q(String value) {
	String quote = String('"');
	return quote + value + quote;
}

boolean connectWiFi(){
	Serial.println("AT+CWMODE=1");
	esp8266.println("AT+CWMODE=1");
	delay(2000);
	String cmd="AT+CWJAP=" + Q(ssid) + "," + Q(pass);
	Serial.println(cmd);
	esp8266.println(cmd);
	delay(5000);
	if(esp8266.find((char *)"OK")){
		Serial.println("OK");
		return true;
	}else{
		return false;
	}
}

void sendData(String feed, String value){
  String start = "AT+CIPSTART=" + Q("TCP") + "," + Q("io.adafruit.com") + ",80";
  Serial.println(start);
  esp8266.println(start);
  
  delay(2000);
  if(esp8266.find((char *)"Error")){
    return;
  }
  
  String body = "{" +Q("value") + ":" + Q(value) + "}";
  String path = "/api/feeds/" + feed + "/data?X-AIO-Key=" + aioKey;
  String data = "POST " + path + " HTTP/1.0\n"
	  + "Host: io.adafruit.com\n"
	  + "Content-Type: application/json\n"
	  + "Content-Length: " + body.length() + "\n\n"
	  + body + "\n\n";

  String send = String("AT+CIPSEND=") + data.length();
  Serial.println(send);
  esp8266.println(send);

  if(esp8266.find((char *)">")){
    Serial.print(data);
    esp8266.print(data);
  }
  else{
   Serial.println("AT+CIPCLOSE");
   esp8266.println("AT+CIPCLOSE");
  }
}

void setup()
{
	Serial.begin(9600);
	esp8266.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  //Serial.begin(115200);
  
	Serial.println("AT");
	esp8266.println("AT");

	delay(5000);
	if(esp8266.find((char *)"OK")){
		connectWiFi();
	}
   pinMode(A1,INPUT);
  
  pinMode(pin,INPUT);
  starttime = millis(); 
  //
  dht.begin();
  Serial.println(F("BMP280 test"));
}

// Dust
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

void loop(){
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
  //
	delay(2000);
	sendData(fieldTemp, String(temp));
  //sendData(fieldBarometer, String(concentration));
	//sendData(fieldPulseMonitor, bmp.readAltitude(1013.25);
  sendData(fieldHum, String(hum));
  sendData(fieldDust, String(concentration));
  sendData(fieldLight, String(light));
}
