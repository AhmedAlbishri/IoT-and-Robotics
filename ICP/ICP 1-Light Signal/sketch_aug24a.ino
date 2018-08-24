int switchState = 0;
void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, INPUT);
}

void loop(){
  switchState = digitalRead(2);
  if(switchState == LOW){
  // the button is not pressed
   
  digitalWrite(3, HIGH); //green LED
  
  digitalWrite(4, LOW); //Red LED
  
  digitalWrite(5, LOW); //red LED
  //
  digitalWrite(3, HIGH); //green LED
  
  digitalWrite(4, LOW); //Red LED
  
  digitalWrite(5, LOW); //red LED
  //
  digitalWrite(3, HIGH); //green LED
  
  digitalWrite(4, LOW); //Red LED
  
  digitalWrite(5, LOW); //red LED
  }

  delay(250); 
  digitalWrite(3, LOW);
  delay(200); 
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  //  
  delay(350); // wait for a quarter seconds
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  delay(450);
  //
  delay(350); // wait for a quarter seconds
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  //delay(350); // wait for a quarter seconds
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);

}
