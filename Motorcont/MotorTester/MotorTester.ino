#include<Servo.h>
Servo motor,motor2,motor3,motor4;
int pot=A2;
int gnd=7;
void setup() {
  // put your setup code here, to run once:
  motor.attach(3);
  motor2.attach(5);
  motor3.attach(6);
  motor4.attach(9);
  pinMode(gnd,INPUT_PULLUP);
  delay(2000);
  Serial.begin(9600);
}
long bl,bll;
void loop() {
  // put your main code here, to run repeatedly:
  bl=analogRead(pot);
  bll=map(bl,0,1023,1000,2200);
  if(digitalRead(gnd)){
    motor.write(90);
    motor2.write(90);
    motor3.write(90);
    motor4.write(90);
  }
  else{
    motor.writeMicroseconds(bll);
    motor2.writeMicroseconds(bll);
    motor3.writeMicroseconds(bll);
    motor4.writeMicroseconds(bll);
  }
  Serial.println();
  Serial.println(bl);
  Serial.println(bll);
  delay(200);
}
