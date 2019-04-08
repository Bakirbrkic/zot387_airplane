#include <Arduino.h>
#include<Servo.h>

Servo motor;
int pot=A2;
int gnd=7;
void setup() {
  // put your setup code here, to run once:
  motor.attach(3);
  pinMode(gnd,INPUT_PULLUP);
  delay(2000);
//  Serial.begin(9600);
}
long bl,bll;
void loop() {
  // put your main code here, to run repeatedly:
  bl=analogRead(pot);
  bll=map(bl,0,1023,600,2200);
  if(digitalRead(gnd)){
    motor.write(90);
  }
  else{
    motor.writeMicroseconds(bll);
  }
//  Serial.println();
//  Serial.println(bl);
//  Serial.println(bll);
  //delay(200);
}