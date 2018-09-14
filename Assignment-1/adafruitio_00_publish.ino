// Adafruit IO Publish Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
//
#include <DHT.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>
//
SoftwareSerial esp8266(9,10); 
LiquidCrystal_I2C lcd(0x27,16,2);
//
//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
Adafruit_BMP280 bmp; // I2C

int error;

float temp; //Stores temperature value


/************************ Example Starts Here *******************************/

// this int will hold the current count for our sketch
//int count = 0;

// set up the 'Temp' feed
AdafruitIO_Feed *Temp = io.feed("Temp");

void setup() {
  // LCD
  lcd.init();
  lcd.backlight();
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  //Temp
  temp = dht.readTemperature();
  lcd.print("Temp: ");
  lcd.print(temp);
  
  // save count to the 'counter' feed on Adafruit IO
  Serial.print("sending -> ");
  Serial.println(temp);
  Barometer->save(temp);

  // increment the count by 1
  count++;

  // Adafruit IO is rate limited for publishing, so a delay is required in
  // between feed->save events. In this example, we will wait three seconds
  // (1000 milliseconds == 1 second) during each loop.
  delay(3000);

}
