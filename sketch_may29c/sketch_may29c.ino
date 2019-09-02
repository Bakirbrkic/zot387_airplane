void setup() {
  // put your setup code here, to run once:
  pinMode(5,OUTPUT);
  pinMode(8,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(8)){
    digitalWrite(5,1);
    delayMicroseconds(400);
    digitalWrite(5,0);
    delayMicroseconds(100);
  }
  else{
    digitalWrite(5,1);
    delayMicroseconds(200);
    digitalWrite(5,0);
    delayMicroseconds(200);
  }
}
