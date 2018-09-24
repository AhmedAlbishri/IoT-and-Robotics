int switchState = 0;
int delayTime = 100;
int numberOfLoops = 3;
int loopCount = 0;

bool trafficToggle = false;

void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, INPUT);
}

void origToggle() {
  switchState = digitalRead(2);
  if (switchState == LOW) {
    // the button is not pressed
    digitalWrite(3, HIGH); //green LED
    digitalWrite(4, LOW); //Red LED
    digitalWrite(5, LOW); //red LED
  } else {
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);

    delay(delayTime); // wait for a quarter seconds
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    delay(delayTime);
  }
}


void traffic() {
   digitalWrite(3, LOW);
   digitalWrite(4, LOW);
   digitalWrite(5, LOW);
   
   digitalWrite(3, HIGH); //green LED
   delay(1000);
   digitalWrite(3, LOW); //green LED
   digitalWrite(4, HIGH); //Red LED
   delay(250);
   digitalWrite(4, LOW); //Red LED
   digitalWrite(5, HIGH); //red LED
   delay(1000);
   
   digitalWrite(3, LOW);
   digitalWrite(4, LOW);
   digitalWrite(5, LOW);
}


void loop() {

  switchState = digitalRead(2);
  if (switchState == HIGH) {
    trafficToggle = true;
  }
  
  if (trafficToggle) {
    loopCount = loopCount + 1;
    if (loopCount > numberOfLoops - 1) {
      trafficToggle = false;
      loopCount = 0;
    }
    traffic();
  }

}
